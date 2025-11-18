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

#include "factory/NodeFactory.hpp"
#include "model/NodeModel.hpp"
#include "presenter/NodePresenter.hpp"
#include "utility/GraphRegistry.hpp"
#include "utility/GroupDescriptor.hpp"
#include "utility/NodeDescriptor.hpp"
#include "view/GraphScene.hpp"
#include "view/GroupItem.hpp"
#include "view/NodeItem.hpp"
#include "view/NodeItemViewAdapter.hpp"
#include "view/PortLabel.hpp"

bool
NodeFactory::PortsAreCompatible(GraphRegistry& registry, PortLabel* port1, PortLabel* port2)
{
    if (!port1->isVisible() || !port2->isVisible())
        return false;

    if (port1->parentItem() == port2->parentItem())
        return false;

    if ((port1->isAnyInputPort() && port2->isAnyInputPort()) || (port1->isOutputPort() && port2->isOutputPort()))
        return false;

    if (port1->getTagBitMask().count() == 0 || port2->getTagBitMask().count() == 0)
        return false;

    if ((port1->getOrientation() == PortLabel::Orientation::Parameter ||
         port1->getOrientation() == PortLabel::Orientation::Input) &&
        registry.hasConnection(port1))
    {
        return false;
    }

    if ((port2->getOrientation() == PortLabel::Orientation::Parameter ||
         port2->getOrientation() == PortLabel::Orientation::Input) &&
        registry.hasConnection(port2))
    {
        return false;
    }

    auto haveSameTags = ::haveSameTags(*port1, *port2);
    return haveSameTags;
}

NodeFactory::NodeFactory(std::shared_ptr<GraphRegistry> registry)
    : m_scene(nullptr)
    , m_registry(registry)
{}

NodeFactory::Node::~Node()
{
    delete model;
    delete adapter;
    delete presenter;
    // Note: item is owned by the scene, do not delete here.
}

std::unique_ptr<NodeFactory::Node>
NodeFactory::createNode(GraphScene* scene,
                        const QString& nodeName,
                        const QString& displayedName,
                        const QColor& color,
                        const QPointF& pos)
{
    auto out = std::make_unique<Node>();
    if (!scene)
        return std::move(out);
    m_scene = scene;

    // View NodeItem
    out->item = new NodeItem(m_registry, nodeName, displayedName, color);
    out->item->setPos(pos);
    scene->addItem(out->item);

    // Model
    out->model = new NodeModel(scene);
    out->model->setNodeName(nodeName);
    out->model->setDisplayedNodeName(displayedName);
    out->model->setTitleColor(color);
    out->model->setPosition(pos);
    out->model->setVisible(true);

    // Adapter + Presenter
    out->adapter = new NodeItemViewAdapter(m_registry, out->item, scene);
    out->presenter = new NodePresenter(out->model, out->adapter, scene);
    m_scene->connectNode(out->item);

    return std::move(out);
}

std::unique_ptr<NodeFactory::Node>
NodeFactory::createNode(GraphScene* scene,
                        const QString& nodeName,
                        const QColor& color,
                        const QPointF& pos)
{
    return createNode(scene, nodeName, nodeName, color, pos);
}

void
NodeFactory::addInput(const Node& node, const QString& name, const QString& displayedName)
{
    if (node.model)
        node.model->addPort(name, displayedName, PortSpec::Kind::Input);
}

void
NodeFactory::addOutput(const Node& node, const QString& name, const QString& displayedName)
{
    if (node.model)
        node.model->addPort(name, displayedName, PortSpec::Kind::Output);
}

void
NodeFactory::addParameter(const Node& node, QWidget* widget, const QString& name, const QString& displayedName)
{
    if (node.model)
        node.model->addParam(name, displayedName, widget, PortSpec::Kind::Param);
}

void
NodeFactory::addInput(const Node& node, const QString& name)
{
    addInput(node, name, name);
}

void
NodeFactory::addOutput(const Node& node, const QString& name)
{
    addOutput(node, name, name);
}

void
NodeFactory::addParameter(const Node& node, QWidget* widget, const QString& name)
{
    addParameter(node, widget, name, name);
}

ConnectionItem*
NodeFactory::createConnection(GraphScene& scene, PortLabel& port1, PortLabel& port2, bool isActive)
{
    auto pConnection = createConnectionBetweenPorts(&port1, &port2);
    if (!pConnection)
        return nullptr;

    scene.addItem(pConnection);

    PortLabel const* inputPort = &port2;

    if (port1.isAnyInputPort())
        inputPort = &port1;

    auto node = m_registry->findNode(inputPort->moduleName());
    if (isActive)
        m_registry->activateNode(node->node);
    else
        m_registry->deactivateNode(node->node);
    return pConnection;
}

void
NodeFactory::getNodePorts(const Node& node, QVector<PortLabel*>& inputPorts, QVector<PortLabel*>& outputPorts, QList<PortLabel*>& parameterPorts)
{
    if (node.item)
    {
        inputPorts = node.item->inputs();
        outputPorts = node.item->outputs();
        parameterPorts = node.item->parameterPorts().keys();
    }
}

PortLabel*
NodeFactory::getInputPortByName(const Node& node, const QString& portName)
{
    if (node.item)
    {
        for (auto* port : node.item->inputs())
        {
            if (port && port->name() == portName)
                return port;
        }
    }
    return nullptr;
}

PortLabel*
NodeFactory::getOutputPortByName(const Node& node, const QString& portName)
{
    if (node.item)
    {
        for (auto* port : node.item->outputs())
        {
            if (port && port->name() == portName)
                return port;
        }
    }
    return nullptr;
}

PortLabel*
NodeFactory::getParameterPortByName(const Node& node, const QString& portName)
{
    if (node.item)
    {
        for (auto* port : node.item->parameterPorts().keys())
        {
            if (port && port->name() == portName)
                return port;
        }
    }
    return nullptr;
}

void
NodeFactory::disableWidgetOfConnectedParametersInput(NodeItem* item)
{
    for (auto* pPort : item->parameterPorts().keys())
    {
        if (m_registry->hasConnection(pPort))
        {
            item->parameterPorts().value(pPort)->setEnabled(false);
        }
        else
        {
            item->parameterPorts().value(pPort)->setEnabled(true);
        }
    }
}

ConnectionItem*
NodeFactory::createConnectionBetweenPorts(PortLabel* fromPort, PortLabel* toPort)
{
    if (!PortsAreCompatible(*m_registry, fromPort, toPort))
        return nullptr;

    if (!fromPort || !toPort)
        return nullptr;
    if (m_registry->hasConnectionTo(*fromPort, *toPort))
        return nullptr;

    auto concreteFrom = fromPort;
    auto concreteTo = toPort;
    auto fwdFromports = m_registry->getAllForwardedPortsFromAPort(fromPort);
    auto fwdToports = m_registry->getAllForwardedPortsFromAPort(toPort);
    if (!fwdFromports.isEmpty())
    {
        for (auto p : std::as_const(fwdFromports))
        {
            if ((p->moduleName() + "_" + p->name()) == fromPort->name())
            {
                concreteFrom = p;
                break;
            }
        }
    }
    if (!fwdToports.isEmpty())
    {
        for (auto p : std::as_const(fwdToports))
        {
            if ((p->moduleName() + "_" + p->name()) == toPort->name())
            {
                concreteTo = p;
                break;
            }
        }
    }

    auto* connection = new ConnectionItem(concreteFrom->getConnectionPortData(), concreteTo->getConnectionPortData());

    m_registry->registerConnection(concreteFrom, concreteTo, connection);

    if (auto group = m_registry->findGroup(fromPort->moduleName()))
    {
        if (group)
        {
            disableWidgetOfConnectedParametersInput(group->group);
            m_registry->nodeMoved(group->group);
        }
    }
    else
    {
        auto node = m_registry->findNode(fromPort->moduleName());
        disableWidgetOfConnectedParametersInput(node->node);
        m_registry->nodeMoved(node->node);
    }

    if (auto group = m_registry->findGroup(toPort->moduleName()))
    {
        if (group)
        {
            disableWidgetOfConnectedParametersInput(group->group);
            m_registry->nodeMoved(group->group);
        }
    }
    else
    {
        auto node = m_registry->findNode(toPort->moduleName());
        disableWidgetOfConnectedParametersInput(node->node);
        m_registry->nodeMoved(node->node);
    }

    // disable connected widget

    return connection;
}

void
NodeFactory::removeInput(const Node& node, const QString& name)
{
    if (node.model)
        node.model->removePort(name, PortSpec::Kind::Input);
}

void
NodeFactory::removeOutput(const Node& node, const QString& name)
{
    if (node.model)
        node.model->removePort(name, PortSpec::Kind::Output);
}

void
NodeFactory::removeParameter(const Node& node, const QString& name)
{
    if (node.model)
        node.model->removeParam(name, PortSpec::Kind::Param);
}
