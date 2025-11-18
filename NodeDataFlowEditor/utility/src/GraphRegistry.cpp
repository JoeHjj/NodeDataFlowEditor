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

#include "utility/GraphRegistry.hpp"
#include "utility/GroupDescriptor.hpp"
#include "utility/NodeDescriptor.hpp"
#include "view/ConnectionItem.hpp"
#include "view/GroupItem.hpp"
#include "view/NodeItem.hpp"
#include "view/PortLabel.hpp"

#include <QDebug>
#include <algorithm>

qint64
GraphRegistry::registerNode(NodeItem* n)
{
    QMutexLocker lock(&m_mutex);
    if (dynamic_cast<GroupItem*>(n))
        return -1;
    if (m_nodes.contains(n))
        return m_nodes[n]->uid;

    auto* d = new NodeDescriptor();
    d->uid = m_nextNodeId++;
    d->node = n;

    m_nodes[n] = d;
    return d->uid;
}

void
GraphRegistry::unregisterNode(NodeItem* n)
{
    QMutexLocker lock(&m_mutex);
    if (dynamic_cast<GroupItem*>(n))
        return;
    auto it = m_nodes.find(n);
    if (it == m_nodes.end())
        return;
    delete it.value();
    m_nodes.erase(it);
}

NodeDescriptor*
GraphRegistry::getNode(NodeItem* n)
{
    QMutexLocker lock(&m_mutex);
    return lookupNodeUnlocked(n);
}

void
GraphRegistry::registerInput(NodeItem* n, PortLabel* p)
{
    QMutexLocker lock(&m_mutex);
    if (p->isInputPort())
    {
        if (auto* d = lookupNodeUnlocked(n))
            d->inputsDescriptor[p] = {};
    }
    else
        qWarning() << "port " << p->name() << "in " << n->nodeName() << "is not an input port";
}

void
GraphRegistry::registerOutput(NodeItem* n, PortLabel* p)
{
    QMutexLocker lock(&m_mutex);
    if (p->isOutputPort())
    {
        if (auto* d = lookupNodeUnlocked(n))
            d->outputsDescriptor[p] = {};
    }
    else
        qWarning() << "port " << p->name() << " in " << n->nodeName() << "is not an output port";
}

void
GraphRegistry::registerParameter(NodeItem* n, PortLabel* p)
{
    QMutexLocker lock(&m_mutex);
    if (p->isParameterPort())
    {
        if (auto* d = lookupNodeUnlocked(n))
            d->parametersInputsDescriptor[p] = {};
    }
    else
        qWarning() << "port " << p->name() << " in " << n->nodeName() << "is not an parameter port";
}

void
GraphRegistry::unregisterInput(NodeItem* n, PortLabel* p)
{
    QMutexLocker lock(&m_mutex);
    if (p->isInputPort())
    {
        if (auto* d = lookupNodeUnlocked(n))
            d->inputsDescriptor.remove(p);
    }
}

void
GraphRegistry::unregisterOutput(NodeItem* n, PortLabel* p)
{
    QMutexLocker lock(&m_mutex);
    if (p->isOutputPort())
    {
        if (auto* d = lookupNodeUnlocked(n))
            d->outputsDescriptor.remove(p);
    }
}

void
GraphRegistry::unregisterParameter(NodeItem* n, PortLabel* p)
{
    QMutexLocker lock(&m_mutex);
    if (auto* d = lookupNodeUnlocked(n))
        d->parametersInputsDescriptor.remove(p);
}

PortLabel*
GraphRegistry::resolvePort(const QString& nodeName,
                           const QString& portName)
{
    for (NodeDescriptor* nd : std::as_const(m_nodes))
    {
        if (!nd->node || nd->node->nodeName() != nodeName)
            continue;

        auto scan = [&](const QMap<PortLabel*, QVector<ConnectionItem*>>& mp) {
            for (PortLabel* p : mp.keys())
                if (p->name() == portName &&
                    p->moduleName() == nodeName)
                    return p;
            return (PortLabel*)nullptr;
        };

        if (auto p = scan(nd->inputsDescriptor))
            return p;
        if (auto p = scan(nd->outputsDescriptor))
            return p;
        if (auto p = scan(nd->parametersInputsDescriptor))
            return p;
    }

    return nullptr;
}

void
GraphRegistry::registerConnection(PortLabel* from, PortLabel* to, ConnectionItem* c)
{
    QMutexLocker lock(&m_mutex);
    PortLabel* inPort = nullptr;
    PortLabel* outPort = nullptr;
    if (from->isAnyInputPort())
    {
        inPort = from;
        if (to->isOutputPort())
            outPort = to;
    }
    else
    {
        inPort = to;
        if (from->isOutputPort())
            outPort = from;
    }
    if (!inPort || !outPort)
    {
        qWarning() << "cant register connection to ports  [incorrect types] " << from->name() << " in " << from->moduleName() << " to " << to->name() << " in " << to->moduleName();
        return;
    }
    NodeItem* outNode = nullptr;
    NodeItem* inNode = nullptr;
    if (auto desc = findNode(outPort->moduleName()))
        outNode = desc->node;
    else if (auto gDesc = findGroup(outPort->moduleName()))
        outNode = gDesc->group;

    if (auto desc = findNode(inPort->moduleName()))
        inNode = desc->node;
    else if (auto gDesc = findGroup(inPort->moduleName()))
        inNode = gDesc->group;
    if (!inNode || !outNode)
    {
        qWarning() << "cant register connection to ports " << from->name() << " in " << from->moduleName() << " to " << to->name() << " in " << to->moduleName();

        return;
    }
    if (auto* d = lookupNodeUnlocked(outNode))
    {
        d->outputsDescriptor[outPort].push_back(c);
    }

    if (auto* d = lookupNodeUnlocked(inNode))
    {
        d->inputsDescriptor[inPort].push_back(c);
    }
}

PortLabel*
GraphRegistry::getInputPortByName(const NodeItem& node, const QString& portName)
{
    for (auto* port : node.inputs())
    {
        if (port && port->name() == portName)
            return port;
    }
    return nullptr;
}

PortLabel*
GraphRegistry::getOutputPortByName(const NodeItem& node, const QString& portName)
{
    for (auto* port : node.outputs())
    {
        if (port && port->name() == portName)
            return port;
    }
    return nullptr;
}

PortLabel*
GraphRegistry::getParameterPortByName(const NodeItem& node, const QString& portName)
{
    for (auto* port : node.parameterPorts().keys())
    {
        if (port && port->name() == portName)
            return port;
    }
    return nullptr;
}

void
GraphRegistry::unregisterConnection(ConnectionItem* c)
{
    QMutexLocker lock(&m_mutex);
    for (auto* nd : std::as_const(m_nodes))
    {
        for (auto& vec : nd->inputsDescriptor)
        {
            vec.removeAll(c);
        }

        for (auto& vec : nd->outputsDescriptor)
        {
            vec.removeAll(c);
        }

        for (auto& vec : nd->parametersInputsDescriptor)
        {
            vec.removeAll(c);
        }
    }
}

void
GraphRegistry::nodeMoved(NodeItem* node)
{
    // Lambda for NodeItem m_port
    auto refreshNodePorts = [&](const QMap<PortLabel*, QVector<ConnectionItem*>>& mp) {
        for (auto it = mp.begin(); it != mp.end(); ++it)
        {
            PortLabel const* port = it.key();
            const QVector<ConnectionItem*>& connections = it.value();

            for (ConnectionItem* connection : connections)
            {
                if (!connection)
                    continue;

                connection->onNodeMoved(port->isAnyInputPort(),
                                        port->scenePos(),
                                        port->boundingRect());
            }
        }
    };

    // Lambda for GroupItem forwarded m_port
    auto refreshGroupForwardedPorts = [&](GroupDescriptor const* gd) {
        auto refreshMap = [this](const QMap<PortLabel*, QVector<PortLabel*>>& mp) {
            for (auto it = mp.begin(); it != mp.end(); ++it)
            {
                const QVector<PortLabel*>& actualPorts = it.value();

                for (PortLabel* actual : actualPorts)
                {
                    auto conns = getConnections(actual);
                    for (ConnectionItem* c : std::as_const(conns))
                        if (c)
                        {
                            if (actual->isAnyInputPort())
                            {
                                c->onNodeMoved(actual->isAnyInputPort(),
                                               it.key()->scenePos(),
                                               actual->boundingRect());
                            }
                            else
                            {
                                c->onNodeMoved(actual->isAnyInputPort(),
                                               it.key()->scenePos(),
                                               it.key()->boundingRect());
                            }
                        }
                }
            }
        };

        refreshMap(gd->forwardInputsDescriptor);
        refreshMap(gd->forwardOutputsDescriptor);
        refreshMap(gd->forwardParametersInputsDescriptor);
    };

    // Dispatch
    if (auto g = dynamic_cast<GroupItem*>(node))
    {
        if (GroupDescriptor const* gd = lookupGroupUnlocked(g))
            refreshGroupForwardedPorts(gd);
        return;
    }

    if (NodeDescriptor const* nd = lookupNodeUnlocked(node))
    {
        if (!nd->node->isVisible())
            return;
        refreshNodePorts(nd->inputsDescriptor);
        refreshNodePorts(nd->outputsDescriptor);
        refreshNodePorts(nd->parametersInputsDescriptor);
    }
}

qint64
GraphRegistry::registerGroup(GroupItem* g)
{
    QMutexLocker lock(&m_mutex);

    if (m_groups.contains(g))
        return m_groups[g]->uid;

    auto* d = new GroupDescriptor();
    d->uid = m_nextGroupId++;
    d->group = g;

    m_groups[g] = d;

    // Workaround, remove from nodes
    auto it = m_nodes.find(g);
    if (it == m_nodes.end())
        return d->uid;
    delete it.value();
    m_nodes.erase(it);

    return d->uid;
}

void
GraphRegistry::unregisterGroup(GroupItem* g)
{
    QMutexLocker lock(&m_mutex);

    auto it = m_groups.find(g);
    if (it == m_groups.end())
        return;

    delete it.value();
    m_groups.erase(it);
}

void
GraphRegistry::addNodeToGroup(GroupItem* g, NodeItem* n)
{
    QMutexLocker lock(&m_mutex);
    GroupDescriptor* gd = lookupGroupUnlocked(g);
    NodeDescriptor* nd = lookupNodeUnlocked(n);
    if (gd && nd)
        gd->memberNodes.push_back(nd);
}

void
GraphRegistry::removeNodeFromGroup(GroupItem* g, NodeItem const* n)
{
    QMutexLocker lock(&m_mutex);
    GroupDescriptor* gd = lookupGroupUnlocked(g);
    if (!gd)
        return;

    gd->memberNodes.erase(
        std::remove_if(gd->memberNodes.begin(), gd->memberNodes.end(),
                       [&](auto const* nd) { return nd && nd->node == n; }),
        gd->memberNodes.end());
}

QVector<GroupDescriptor*>
GraphRegistry::groupsOf(NodeItem const* n)
{
    QMutexLocker lock(&m_mutex);
    QVector<GroupDescriptor*> result;

    for (GroupDescriptor* gd : std::as_const(m_groups))
    {
        for (NodeDescriptor const* nd : std::as_const(gd->memberNodes))
        {
            if (nd && nd->node == n)
            {
                result.push_back(gd);
                break;
            }
        }
    }
    return result;
}

void
GraphRegistry::registerForwardInput(GroupItem* g, PortLabel* forward, PortLabel* actual)
{
    QMutexLocker lock(&m_mutex);
    if (auto* gd = lookupGroupUnlocked(g))
    {
        gd->forwardInputsDescriptor[forward].push_back(actual);
        forward->copyTagsFrom(*actual);
    }
}

void
GraphRegistry::registerForwardOutput(GroupItem* g, PortLabel* forward, PortLabel* actual)
{
    QMutexLocker lock(&m_mutex);
    if (auto* gd = lookupGroupUnlocked(g))
    {
        gd->forwardOutputsDescriptor[forward].push_back(actual);
        forward->copyTagsFrom(*actual);
    }
}

void
GraphRegistry::registerForwardParameter(GroupItem* g, PortLabel* forward, PortLabel* actual)
{
    QMutexLocker lock(&m_mutex);
    if (auto* gd = lookupGroupUnlocked(g))
    {
        gd->forwardParametersInputsDescriptor[forward].push_back(actual);
        forward->copyTagsFrom(*actual);
    }
}

void
GraphRegistry::unregisterForwardPort(GroupItem* g, PortLabel* forward)
{
    QMutexLocker lock(&m_mutex);
    if (auto* gd = lookupGroupUnlocked(g))
    {
        gd->forwardInputsDescriptor.remove(forward);
        gd->forwardOutputsDescriptor.remove(forward);
        gd->forwardParametersInputsDescriptor.remove(forward);
    }
}

QVector<NodeDescriptor*>
GraphRegistry::allNodes() const
{
    QMutexLocker lock(&m_mutex);
    return m_nodes.values().toVector();
}

QVector<GroupDescriptor*>
GraphRegistry::allGroups() const
{
    QMutexLocker lock(&m_mutex);
    return m_groups.values().toVector();
}

GraphRegistry::GraphRegistry() = default;

GraphRegistry::~GraphRegistry() = default;

NodeDescriptor*
GraphRegistry::lookupNodeUnlocked(NodeItem* n) const
{
    return m_nodes.value(n, nullptr);
}

GroupDescriptor*
GraphRegistry::lookupGroupUnlocked(GroupItem* g) const
{
    return m_groups.value(g, nullptr);
}

NodeDescriptor*
GraphRegistry::findNode(const QString& name)
{
    QMutexLocker lock(&m_mutex);
    for (NodeDescriptor* nd : std::as_const(m_nodes))
        if (nd->node && nd->node->nodeName() == name)
            return nd;
    return nullptr;
}

GroupDescriptor*
GraphRegistry::findGroup(const QString& name)
{
    QMutexLocker lock(&m_mutex);
    for (GroupDescriptor* gd : std::as_const(m_groups))
        if (gd->group && gd->group->nodeName() == name)
            return gd;
    return nullptr;
}

NodeDescriptor*
GraphRegistry::findNodeInGroup(const QString& name, GroupItem* g)
{
    QMutexLocker lock(&m_mutex);
    GroupDescriptor const* gd = lookupGroupUnlocked(g);
    if (!gd)
        return nullptr;

    for (NodeDescriptor* nd : gd->memberNodes)
        if (nd->node && nd->node->nodeName() == name)
            return nd;

    return nullptr;
}

QVector<PortLabel*>
GraphRegistry::getAllPortsForwardedToAPort(PortLabel* actual)
{
    QMutexLocker lock(&m_mutex);
    QVector<PortLabel*> result;

    for (GroupDescriptor const* gd : std::as_const(m_groups))
    {
        auto check = [&](const QMap<PortLabel*, QVector<PortLabel*>>& mp) {
            for (auto it = mp.begin(); it != mp.end(); ++it)
            {
                PortLabel* forward = it.key();
                const QVector<PortLabel*>& targets = it.value();

                if (targets.contains(actual))
                    result.push_back(forward);
            }
        };

        check(gd->forwardInputsDescriptor);
        check(gd->forwardOutputsDescriptor);
        check(gd->forwardParametersInputsDescriptor);
    }

    return result;
}

QVector<PortLabel*>
GraphRegistry::getAllForwardedPortsFromAPort(PortLabel* forwardPort)
{
    QMutexLocker lock(&m_mutex);
    QVector<PortLabel*> result;

    for (GroupDescriptor const* gd : std::as_const(m_groups))
    {
        auto appendIfExists = [&](const QMap<PortLabel*, QVector<PortLabel*>>& mp) {
            if (mp.contains(forwardPort))
                result += mp[forwardPort];
        };

        appendIfExists(gd->forwardInputsDescriptor);
        appendIfExists(gd->forwardOutputsDescriptor);
        appendIfExists(gd->forwardParametersInputsDescriptor);
    }

    return result;
}

QVector<ConnectionItem*>
GraphRegistry::getConnections(PortLabel* port)
{
    QVector<ConnectionItem*> result;

    if (!port)
        return result;

    QMutexLocker lock(&m_mutex);

    // Find the NodeDescriptor that owns this port
    NodeItem* ownerNode = nullptr;
    if (findNode(port->moduleName()))
        ownerNode = findNode(port->moduleName())->node;

    if (!ownerNode)
    {
        // its a group
        return getConnectionsFromGroupPort(port);
    }

    NodeDescriptor* nd = lookupNodeUnlocked(ownerNode);
    if (!nd)
        return result;

    // Look in inputs
    if (nd->inputsDescriptor.contains(port))
        result += nd->inputsDescriptor[port];

    // Look in outputs
    if (nd->outputsDescriptor.contains(port))
        result += nd->outputsDescriptor[port];

    // Look in parameters
    if (nd->parametersInputsDescriptor.contains(port))
        result += nd->parametersInputsDescriptor[port];

    return result;
}

bool
GraphRegistry::hasConnection(PortLabel* port)
{
    if (auto forwardsPorts = getAllForwardedPortsFromAPort(port); !forwardsPorts.isEmpty())
    {
        for (auto p : std::as_const(forwardsPorts))
        {
            if (hasConnection(p))
            {
                return true;
            }
        }
    }
    return !getConnections(port).empty();
}

QVector<ConnectionItem*>
GraphRegistry::getConnectionsFromGroupPort(PortLabel* forwardPort)
{
    QVector<ConnectionItem*> result;

    if (!forwardPort)
        return result;

    QMutexLocker lock(&m_mutex);

    // Iterate all groups to find which group owns this forwarded port
    for (auto const* gd : std::as_const(m_groups))
    {
        auto checkMap = [&](const QMap<PortLabel*, QVector<PortLabel*>>& mp) {
            if (!mp.contains(forwardPort))
                return;

            const QVector<PortLabel*>& actualPorts = mp[forwardPort];

            for (PortLabel* actual : actualPorts)
            {
                // Get all connections from this actual port
                QVector<ConnectionItem*> conns = getConnections(actual);
                result += conns;
            }
        };

        checkMap(gd->forwardInputsDescriptor);
        checkMap(gd->forwardOutputsDescriptor);
        checkMap(gd->forwardParametersInputsDescriptor);
    }

    return result;
}

ConnectionItem*
GraphRegistry::findConnection(PortLabel& fromPort, QString portName, QString moduleName)
{
    for (ConnectionItem* conn : getConnections(&fromPort))
    {
        if (fromPort.isAnyInputPort())
        {
            if (conn->outputPort().portName == portName && conn->outputPort().moduleName == moduleName)
                return conn;
        }
        else
        {
            if (conn->inputPort().portName == portName && conn->inputPort().moduleName == moduleName)
                return conn;
        }
    }

    return nullptr;
}

bool
GraphRegistry::hasConnectionTo(PortLabel& fromPort, QString toPortName, QString toPortModuleName)
{
    return (findConnection(fromPort, toPortName, toPortModuleName) != nullptr);
}

bool
GraphRegistry::hasConnectionTo(PortLabel& fromPort, PortLabel& toPort)
{
    PortLabel* in = nullptr;
    PortLabel const* out = nullptr;
    if (fromPort.isAnyInputPort())
    {
        in = &fromPort;
    }
    else if (toPort.isAnyInputPort())
    {
        in = &toPort;
    }
    if (fromPort.isOutputPort())
    {
        out = &fromPort;
    }
    else if (toPort.isOutputPort())
    {
        out = &toPort;
    }
    if (!in || !out)
        return false;

    return hasConnectionTo(*in, out->name(), out->moduleName());
}

void
GraphRegistry::activateNode(NodeItem* node)
{
    node->setActive(true);
    for (auto port : node->outputs())
    {
        for (ConnectionItem* conn : getConnections(port))
        {
            conn->setIsActive(true);
        }
    }
}

void
GraphRegistry::deactivateNode(NodeItem* node)
{
    node->setActive(false);
    for (auto port : node->outputs())
    {
        for (ConnectionItem* conn : getConnections(port))
        {
            conn->setIsActive(false);
        }
    }
}

bool
GraphRegistry::isNodeActive(NodeItem* node)
{
    return node->isActivated();
}
