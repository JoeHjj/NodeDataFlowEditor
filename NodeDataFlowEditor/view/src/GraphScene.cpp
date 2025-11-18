/*
    MIT License

    Copyright (c) 2025 Joseph Al Hajjar

    Permission is hereby granted, free of charge, to any person obtaining a copy
    of this software and associated documentation files (the "Software"), to deal
    in the Software without restriction, including without limitation the rights
    to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
    copies of the Software, and to permit persons to whom the Software is
    furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included in
    all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
    AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
    OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
    THE SOFTWARE.
*/

#include "view/GraphScene.hpp"

#include "factory/NodeFactory.hpp"
#include "utility/GraphRegistry.hpp"
#include "utility/GroupDescriptor.hpp"
#include "utility/NodeDescriptor.hpp"
#include "utility/NodeHelper.hpp"
#include "view/ConnectionItem.hpp"
#include "view/GroupItem.hpp"
#include "view/NodeItem.hpp"
#include "view/PortLabel.hpp"

#include <QApplication>
#include <QDir>
#include <QGraphicsSceneContextMenuEvent>
#include <QGraphicsView>
#include <QKeyEvent>
#include <QMenu>
#include <QWidget>

GraphScene::GraphScene(QObject* parent)
    : QGraphicsScene(parent)
    , m_registry(std::make_shared<GraphRegistry>())
    , m_factory(std::make_shared<NodeFactory>(m_registry))
{
}

GraphScene::~GraphScene()
{
    for (QGraphicsItem* item : items())
        if (auto const* node = dynamic_cast<NodeItem*>(item))
            disconnectNode(node);

    std::ignore = disconnect(this);
}

std::shared_ptr<NodeFactory>
GraphScene::getNodeFactory()
{
    return m_factory;
}

std::shared_ptr<GraphRegistry>
GraphScene::getGraphRegistry()
{
    return m_registry;
}

void
GraphScene::addNodeItem(NodeItem* node)
{
    if (node == nullptr)
        throw std::runtime_error("node is null");
    addItem(node);
}

void
GraphScene::groupSelectedNodes(QList<NodeItem*> nodes)
{
    auto g = new GroupItem(m_registry, nodes, this);
    g->setSelected(true);
    m_registry->nodeMoved(g);
}

void
GraphScene::deleteNodeConnections(const NodeItem* node)
{
    for (PortLabel* port : node->inputs())
    {
        auto connections = getGraphRegistry()->getConnections(port);
        for (ConnectionItem* conn : std::as_const(connections))
            deleteConnection(conn);
    }
    for (PortLabel* port : node->outputs())
    {
        auto connections = getGraphRegistry()->getConnections(port);
        for (ConnectionItem* conn : std::as_const(connections))
            deleteConnection(conn);
    }
    for (PortLabel* port : node->paramsInputs())
    {
        auto connections = getGraphRegistry()->getConnections(port);
        for (ConnectionItem* conn : std::as_const(connections))
            deleteConnection(conn);
    }
}

void
GraphScene::deleteConnection(ConnectionItem* connection)
{
    for (auto p : items())
    {
        if (auto port = dynamic_cast<PortLabel*>(p))
        {
            auto registry = getGraphRegistry();
            registry->GraphRegistry::unregisterConnection(connection);

            if (port->isParameterPort())
            {
                if (auto group = m_registry->findGroup(port->moduleName()))
                {
                    if (group)
                    {
                        m_factory->disableWidgetOfConnectedParametersInput(group->group);
                    }
                }
                else
                {
                    auto node = m_registry->findNode(port->moduleName());
                    m_factory->disableWidgetOfConnectedParametersInput(node->node);
                }
            }
        }
    }

    removeItem(connection);
    delete connection;
}

void
GraphScene::contextMenuEvent(QGraphicsSceneContextMenuEvent* event)
{
    auto nodes = get_selected_nodes(selectedItems());
    auto groups = get_selected_groups(selectedItems());

    QMenu menu;
    QAction const* groupAction = nullptr;
    QAction const* ungroupAction = nullptr;

    if (nodes.size() >= 2 && groups.isEmpty())
        groupAction = menu.addAction("Group");

    if (!groups.isEmpty())
        ungroupAction = menu.addAction("Ungroup");

    QAction const* selected = menu.exec(event->screenPos());

    if (!selected)
        return;

    if (selected == groupAction)
        groupSelectedNodes(nodes);

    else if (selected == ungroupAction)
        for (GroupItem* g : std::as_const(groups))
        {
            for (auto n : g->inputs())
            {
                auto concretePorts = m_registry->getAllForwardedPortsFromAPort(n);
                for (auto p : concretePorts)
                {
                    if (n->displayName().startsWith(p->moduleName() + "_"))
                        n->sgnDisplayedNameChanged(n->displayName().remove(p->moduleName() + "_"));
                }

            }
            for (auto n : g->outputs())
            {
                auto concretePorts = m_registry->getAllForwardedPortsFromAPort(n);
                for (auto p : concretePorts)
                {
                    if (n->displayName().startsWith(p->moduleName() + "_"))
                        n->sgnDisplayedNameChanged(n->displayName().remove(p->moduleName() + "_"));
                }
            }
            for (auto n : g->parameterPorts().keys())
            {
                auto concretePorts = m_registry->getAllForwardedPortsFromAPort(n);
                for (auto p : concretePorts)
                {
                    if (n->displayName().startsWith(p->moduleName() + "_"))
                        n->sgnDisplayedNameChanged(n->displayName().remove(p->moduleName() + "_"));
                }
            }
            g->ungroup(this);
        }
}

void
GraphScene::setLightLinesColor(const QColor& newLightLinesColor)
{
    m_lightLinesColor = newLightLinesColor;
}

void
GraphScene::connectNode(const NodeItem* node)
{
    connect(node, &NodeItem::sgnPortMouseClicked, this, [this](NodeItem*, PortLabel* port) {
        this->onPortClicked(port);
    });

    connect(node, &NodeItem::sgnPortMouseReleased, this, [this](NodeItem*, PortLabel* port) {
        this->onPortMouseReleased(port);
    });
}

void
GraphScene::disconnectNode(const NodeItem* node) const
{
    std::ignore = disconnect(node);
}

void
GraphScene::setDarkLinesColor(const QColor& newDarkLinesColor)
{
    m_darkLinesColor = newDarkLinesColor;
}

void
GraphScene::setBackgroundColor(const QColor& newBackgroundColor)
{
    m_backgroundColor = newBackgroundColor;
}

void
GraphScene::keyPressEvent(QKeyEvent* event)
{
    // If any interactive widget inside the scene has focus — let the widget handle the key
    QGraphicsItem* item = focusItem();

    // Walk up parents to find a QGraphicsProxyWidget
    while (item)
    {
        if (auto proxy = qgraphicsitem_cast<QGraphicsProxyWidget*>(item))
        {
            QWidget* w = proxy->widget();
            if (w)
            {
                // Forward the event directly to the embedded widget
                QApplication::sendEvent(w, event);
                event->accept();
                return;
            }
        }

        item = item->parentItem();
    }
    const bool ctrlPressed = event->modifiers() & Qt::ControlModifier;

    switch (event->key())
    {
        case Qt::Key_Delete:
        {
            for_each_selected_connection(this, [this](ConnectionItem* conn) {
                deleteConnection(conn);
            });
            for_each_selected_node(this, [this](NodeItem* node) {
                deleteNodeConnections(node);
                removeItem(node);
                node->deleteLater();
            });
            for_each_selected_group(this, [this](GroupItem* group) { group->ungroup(this); });
            break;
        }

        case Qt::Key_A:
        {
            if (ctrlPressed)
            {
                // Select all selectable items
                for (QGraphicsItem* itm : items(Qt::AscendingOrder))
                    if (itm->flags() & QGraphicsItem::ItemIsSelectable)
                        itm->setSelected(true);
                return;
            }
            break;
        }

        default:
        {
            QGraphicsScene::keyPressEvent(event);
            break;
        }
    }
}

void
GraphScene::onPortClicked(PortLabel* port)
{
    m_startPort = port;

    if ((port->getOrientation() == PortLabel::Orientation::Parameter || port->getOrientation() == PortLabel::Orientation::Input) && m_registry->hasConnection(port))
    {
        m_startPort = nullptr;
        return;
    }

    ConnectionPort startPoint{port->scenePos(), port->boundingRect(), port->name(), port->moduleName(), (port->getOrientation() == PortLabel::Orientation::Parameter || port->getOrientation() == PortLabel::Orientation::Input)};
    m_tempConnection = new ConnectionItem(startPoint);
    addItem(m_tempConnection);
}

void
GraphScene::onPortMouseReleased(PortLabel* port)
{
    if (!m_tempConnection || !m_startPort)
        return;

    if (port && getNodeFactory()->PortsAreCompatible(*this->getGraphRegistry(), m_startPort, port) && port != m_startPort)
    {
        m_tempConnection->setIsCompatible(true);
        ConnectionPort endPoint{port->scenePos(), port->boundingRect(), port->name(), port->moduleName(), (port->getOrientation() == PortLabel::Orientation::Parameter || port->getOrientation() == PortLabel::Orientation::Input)};
        m_tempConnection->addPort(endPoint);
        if (auto conn = m_factory->createConnectionBetweenPorts(m_startPort, port))
        {
            addItem(conn);
        }
        {
            remove_temp_connection(this, m_tempConnection);
        }
    }
    else
    {
        remove_temp_connection(this, m_tempConnection);
    }

    m_startPort = nullptr;
}

void
GraphScene::mouseMoveEvent(QGraphicsSceneMouseEvent* event)
{
    if (m_tempConnection)
    {
        m_tempConnection->setIsCompatible(false);
        PortLabel* port = port_at_position(*this, event->scenePos());
        if (m_lastFoundPort)
            m_lastFoundPort->setHovered(false);
        m_lastFoundPort = nullptr;

        if (port)
        {
            bool compatible = getNodeFactory()->PortsAreCompatible(*this->getGraphRegistry(), m_startPort, port);
            m_tempConnection->setIsCompatible(compatible);

            if (compatible)
            {
                port->setHovered(true);
                m_lastFoundPort = port;
            }
        }

        m_tempConnection->updateEndPoint(event->scenePos());
    }

    QGraphicsScene::mouseMoveEvent(event);
}

void
GraphScene::mouseReleaseEvent(QGraphicsSceneMouseEvent* event)
{
    if (!m_tempConnection || !m_startPort)
    {
        QGraphicsScene::mouseReleaseEvent(event);
        return;
    }

    if (PortLabel* releasedPort = port_at_position(*this, event->scenePos());
        releasedPort && getNodeFactory()->PortsAreCompatible(*this->getGraphRegistry(), m_startPort, releasedPort) && releasedPort != m_startPort)
    {
        m_tempConnection->setIsCompatible(true);

        auto conn = m_factory->createConnectionBetweenPorts(m_startPort, releasedPort);
        if (conn)
            addItem(conn);
    }
    removeItem(m_tempConnection);
    delete m_tempConnection;
    m_tempConnection = nullptr;
    m_startPort = nullptr;

    QGraphicsScene::mouseReleaseEvent(event);
}

void
GraphScene::drawBackground(QPainter* painter, const QRectF& rect)
{
    constexpr int gridSize = 20;
    const QPen lightPen(m_lightLinesColor);
    const QPen darkPen(m_darkLinesColor);

    painter->fillRect(rect, m_backgroundColor);

    qreal left = std::floor(rect.left() / gridSize) * gridSize;
    qreal right = std::ceil(rect.right() / gridSize) * gridSize;
    qreal top = std::floor(rect.top() / gridSize) * gridSize;
    qreal bottom = std::ceil(rect.bottom() / gridSize) * gridSize;

    QVector<QLineF> lightLines;
    QVector<QLineF> darkLines;

    for (auto x = static_cast<int>(left); x <= right; x += gridSize)
    {
        QLineF line(x, top, x, bottom);
        if (x % (gridSize * 5) == 0)
            darkLines.append(line);
        else
            lightLines.append(line);
    }

    for (auto y = static_cast<int>(top); y <= bottom; y += gridSize)
    {
        QLineF line(left, y, right, y);
        if (y % (gridSize * 5) == 0)
            darkLines.append(line);
        else
            lightLines.append(line);
    }

    painter->setPen(lightPen);
    painter->drawLines(lightLines);

    painter->setPen(darkPen);
    painter->drawLines(darkLines);
}
