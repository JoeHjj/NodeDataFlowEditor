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

#include "view/GroupItem.hpp"
#include "utility/GraphRegistry.hpp"
#include "utility/WidgetVisitor.hpp"
#include "view/GraphScene.hpp"
#include "view/PortLabel.hpp"

#include <QCheckBox>
#include <QComboBox>
#include <QDateEdit>
#include <QDial>
#include <QDoubleSpinBox>
#include <QFontMetrics>
#include <QGraphicsProxyWidget>
#include <QGraphicsScene>
#include <QInputDialog>
#include <QPlainTextEdit>
#include <QTextEdit>
#include <QWidget>

namespace
{
    QString joinedNames(const QList<NodeItem*>& nodes)
    {
        QString parts = nodes.front()->nodeName();
        int i = 0;
        for (auto const* n : nodes)
        {
            if (i == 0)
            {
                ++i;
                continue;
            }

            parts += " . " + n->nodeName();
        }

        return parts;
    }
} // anonymous namespace

GroupItem::GroupItem(std::shared_ptr<GraphRegistry> registry,
                     const QList<NodeItem*>& nodes,
                     QGraphicsScene* scene)
    : NodeItem(registry, "Group", "Group", QColor(70, 130, 180)) // steel blue
{
    if (scene)
        scene->addItem(this);
    m_registry->registerGroup(this);

    QRectF nodesRect;
    bool firstNode = true;

    // Track original nodes, hide them, lock movement
    for (auto* n : nodes)
    {
        if (!n)
            continue;

        m_nodes.insert(n);
        n->changeNodeVisibility(false);
        n->setFlag(ItemIsMovable, false);

        // Update bounding rect of nodes
        if (firstNode)
        {
            nodesRect = n->sceneBoundingRect();
            firstNode = false;
        }
        else
        {
            nodesRect = nodesRect.united(n->sceneBoundingRect());
        }
    }

    setNodeName(buildTitle());
    setDisplayedNodeName(nodeName());
    // Determine group position
    // center of nodes' bounding rect
    QPointF centerPos = nodesRect.center();

    // Align group center
    setPos(centerPos - QPointF(boundingRect().width(), boundingRect().height()));

    mirrorPorts();
    mirrorParams();
    updateLayout();

    // Make group m_port behave like normal node m_port
    connect(this, &NodeItem::sgnPortMouseClicked, this, &GroupItem::onGroupPortMouseClicked);
    connect(this, &NodeItem::sgnPortMouseReleased, this, &GroupItem::onGroupPortMouseReleased);
}

GroupItem::~GroupItem()
{
    m_registry->unregisterGroup(this);
    m_nodes.clear();
}

QString
GroupItem::buildTitle()
{
    QList<NodeItem*> list = m_nodes.values();
    std::sort(list.begin(), list.end(), [](auto* a, auto* b) { return a->displayedNodeName() < b->displayedNodeName(); });
    const auto name = joinedNames(list);
    return name;
}

void
GroupItem::mirrorPorts()
{
    for (auto n : std::as_const(m_nodes))
    {
        for (auto* p : n->inputs())
        {
            if (p)
            {
                PortLabel* g = addInput(p->moduleName() + "_" + p->name());
                g->setDisplayName(p->moduleName() + "_" + p->displayName());
                connect(g, &PortLabel::sgnDisplayedNameChanged, this, [p](const QString& displayName) {
                    p->setDisplayName(displayName);
                });
                m_registry->registerForwardInput(this, g, p);
            }
        }
        for (auto* p : n->outputs())
        {
            if (p)
            {
                PortLabel* g = addOutput(p->moduleName() + "_" + p->name());
                g->setDisplayName(p->moduleName() + "_" + p->displayName());
                connect(g, &PortLabel::sgnDisplayedNameChanged, this, [p](const QString& displayName) {
                    p->setDisplayName(displayName);
                });
                m_registry->registerForwardOutput(this, g, p);
            }
        }
    }

    updateLayout();
    emit sgnItemMoved();
}

void
GroupItem::mirrorParams()
{
    QMap<QString, PortLabel*> paramBuckets;
    QMap<QString, QWidget*> paramWidgetsBuckets;
    for (auto n : std::as_const(m_nodes))
    {
        for (auto* p : n->paramsInputs())
        {
            if (!p)
                continue;
            paramBuckets[p->moduleName() + "_" + p->name()] = p;
            paramWidgetsBuckets[p->moduleName() + "_" + p->name()] = n->getParameterWidget(p);
        }
    }

    for (auto it = paramBuckets.begin(); it != paramBuckets.end(); ++it)
    {
        const QString& name = it.key();
        PortLabel* port = it.value();
        if (!port)
            continue;

        QWidget* firstWidget = paramWidgetsBuckets.value(name);

        WidgetVisitor visitor(m_registry, port, name, this);
        visitor.visit(firstWidget);
    }
}

void
GroupItem::onGroupPortMouseClicked(NodeItem*, PortLabel* port)
{
    // Forward to the Scene so it behaves like clicks on a normal node.
    if (auto* sc = qobject_cast<GraphScene*>(scene()))
        sc->onPortClicked(port);
}

void
GroupItem::onGroupPortMouseReleased(NodeItem*, PortLabel* port)
{
    // Same forwarding for mouse release.
    if (auto* sc = qobject_cast<GraphScene*>(scene()))
        sc->onPortMouseReleased(port);
}

QVariant
GroupItem::itemChange(GraphicsItemChange change, const QVariant& value)
{
    if (change == ItemPositionChange && scene())
    {
        // Move member nodes along with the group.
        const QPointF newPos = value.toPointF();
        const QPointF delta = newPos - pos();
        if (!delta.isNull())
        {
            for (NodeItem* n : std::as_const(m_nodes))
            {
                n->setPos(n->pos() + delta);
                emit n->sgnItemMoved();
            }
        }
    }
    else if (change == ItemSelectedHasChanged)
    {
        // Keep selection in sync between the group and its members.
        const bool sel = value.toBool();
        for (NodeItem* n : std::as_const(m_nodes))
            n->setSelected(sel);
    }
    return NodeItem::itemChange(change, value);
}

void
GroupItem::ungroup(QGraphicsScene* sc)
{
    disconnectAllPorts();
    // Make members visible and movable again, and tidy their connection sets.
    auto offset = pos();
    for (NodeItem* n : std::as_const(m_nodes))
    {
        if (!n)
            continue;
        n->changeNodeVisibility(true);
        n->setFlag(ItemIsMovable, true);
        emit n->sgnItemMoved();
        n->setPos(offset);
        offset += n->boundingRect().topRight() + QPointF(20, 20);

        // remove forwarding m_port
        for (auto* p : n->inputs())
        {
            m_registry->unregisterForwardPort(this, p);
        }
        for (auto* p : n->outputs())
        {
            m_registry->unregisterForwardPort(this, p);
        }

        for (auto* p : n->paramsInputs())
        {
            m_registry->unregisterForwardPort(this, p);
        }
        n->updateLayout();
    }
    m_nodes.clear();

    // Remove and delete the group item itself.
    if (sc)
        sc->removeItem(this);
    this->deleteLater();
}

QSet<NodeItem*>
GroupItem::nodes()
{
    return m_nodes;
}
