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

#pragma once

#include <QMap>
#include <QMutex>
#include <QPointF>
#include <QVector>

class NodeItem;
class GroupItem;
class PortLabel;
class ConnectionItem;
struct GroupDescriptor;
struct NodeDescriptor;

/**
 * @brief Global registry for all nodes, groups, ports, and their connections.
 *
 * GraphRegistry is used to track every NodeItem,
 * GroupItem, PortLabel, and ConnectionItem participating in the graph.
 *
 * Its responsibilities include:
 *  - Assigning unique IDs to nodes and groups.
 *  - Tracking ports and connections.
 *  - Maintaining group membership and port forwarding rules.
 *  - Providing lookup utilities by name, port, or ownership.
 *
 */
class GraphRegistry
{
public:
    GraphRegistry();

    ~GraphRegistry();

    // Disable copy
    GraphRegistry(const GraphRegistry&) = delete;
    GraphRegistry& operator=(const GraphRegistry&) = delete;

    // Disable move
    GraphRegistry(GraphRegistry&&) = delete;
    GraphRegistry& operator=(GraphRegistry&&) = delete;

    /**
     * @brief Returns the descriptor for the given node, or nullptr if not registered.
     */
    NodeDescriptor* getNode(NodeItem* n);

    /**
     * @brief Returns a PortLabel matching the given node name and port name.
     */
    PortLabel* resolvePort(const QString& nodeName, const QString& portName);

    /**
     * @brief Returns all groups a given node belongs to.
     */
    QVector<GroupDescriptor*> groupsOf(NodeItem const* n);

    // -------------------------------------------------------------------------
    // Iterators
    // -------------------------------------------------------------------------

    /**
     * @brief Returns a list of all node descriptors currently in the registry.
     */
    QVector<NodeDescriptor*> allNodes() const;

    /**
     * @brief Returns a list of all groups currently in the registry.
     */
    QVector<GroupDescriptor*> allGroups() const;

    // -------------------------------------------------------------------------
    // Find helpers
    // -------------------------------------------------------------------------

    /**
     * @brief Finds a node by its display name (if available).
     */
    NodeDescriptor* findNode(const QString& name);

    /**
     * @brief Finds a group by its display name.
     */
    GroupDescriptor* findGroup(const QString& name);

    /**
     * @brief Finds a node by name, but only within the given group.
     */
    NodeDescriptor* findNodeInGroup(const QString& name, GroupItem* g);

    // -------------------------------------------------------------------------
    // Port / connection helpers
    // -------------------------------------------------------------------------

    /**
     * @brief Returns all group-forwarded ports that resolve to the given actual port.
     */
    QVector<PortLabel*> getAllPortsForwardedToAPort(PortLabel* actual);

    /**
     * @brief Returns all actual ports pointed to by the given forwarded group port.
     */
    QVector<PortLabel*> getAllForwardedPortsFromAPort(PortLabel* forwardPort);

    /**
     * @brief Returns all connections attached to this port.
     */
    QVector<ConnectionItem*> getConnections(PortLabel* port);

    /**
     * @brief Returns connections attached to a group-level forwarded port.
     */
    QVector<ConnectionItem*> getConnectionsFromGroupPort(PortLabel* forwardPort);

    /**
     * @brief Checks whether a port has any connections.
     */
    bool hasConnection(PortLabel* port);

    /**
     * @brief Finds a connection leaving fromPort and ending at a port with the given name.
     */
    ConnectionItem* findConnection(PortLabel& fromPort, QString portName, QString moduleName);

    /**
     * @brief Checks whether fromPort is connected to a target port with the given name and module.
     */
    bool hasConnectionTo(PortLabel& fromPort, QString toPortName, QString toPortModuleName);

    /**
     * @brief Checks whether fromPort is directly connected to toPort.
     */
    bool hasConnectionTo(PortLabel& fromPort, PortLabel& toPort);

    /**
     * @brief Marks a node as active (implementation-specific behavior).
     */
    void activateNode(NodeItem* node);

    /**
     * @brief Marks a node as inactive.
     */
    void deactivateNode(NodeItem* node);

    bool isNodeActive(NodeItem* node);

    /**
     * @brief Finds an input port on the node by name.
     */
    PortLabel* getInputPortByName(const NodeItem& node, const QString& portName);

    /**
     * @brief Finds an output port on the node by name.
     */
    PortLabel* getOutputPortByName(const NodeItem& node, const QString& portName);

    /**
     * @brief Finds a parameter port on the node by name.
     */
    PortLabel* getParameterPortByName(const NodeItem& node, const QString& portName);

private:
    NodeDescriptor* lookupNodeUnlocked(NodeItem* n) const;
    GroupDescriptor* lookupGroupUnlocked(GroupItem* g) const;
    // -------------------------------------------------------------------------
    // Node registration
    // -------------------------------------------------------------------------

    /**
     * @brief Registers a new node and assigns it a unique ID.
     * @param n Pointer to the NodeItem to register.
     * @return Assigned node ID.
     */
    qint64 registerNode(NodeItem* n);

    /**
     * @brief Removes a previously registered node.
     */
    void unregisterNode(NodeItem* n);
    /// Registers individual ports belonging to a node.
    void registerInput(NodeItem* n, PortLabel* p);
    void registerOutput(NodeItem* n, PortLabel* p);
    void registerParameter(NodeItem* n, PortLabel* p);

    /// Unregisters individual ports belonging to a node.
    void unregisterInput(NodeItem* n, PortLabel* p);
    void unregisterOutput(NodeItem* n, PortLabel* p);
    void unregisterParameter(NodeItem* n, PortLabel* p);
    /**
     * @brief Registers a new connection between two ports.
     */
    void registerConnection(PortLabel* from, PortLabel* to, ConnectionItem* c);

    /**
     * @brief Removes a connection from the registry.
     */
    void unregisterConnection(ConnectionItem* c);

    /**
     * @brief Should be called when a node is moved in the scene (used for updating connection paths).
     */
    void nodeMoved(NodeItem* n);

    /// Registers port forwarding rules (group port → actual node port).
    void registerForwardInput(GroupItem* g, PortLabel* forward, PortLabel* actual);
    void registerForwardOutput(GroupItem* g, PortLabel* forward, PortLabel* actual);
    void registerForwardParameter(GroupItem* g, PortLabel* forward, PortLabel* actual);

    /// Removes all forwarding rules for a given forwarded port.
    void unregisterForwardPort(GroupItem* g, PortLabel* forward);

    // -------------------------------------------------------------------------
    // Group registration
    // -------------------------------------------------------------------------

    /**
     * @brief Registers a group and assigns it a unique ID.
     */
    qint64 registerGroup(GroupItem* g);

    /**
     * @brief Removes a previously registered group.
     */
    void unregisterGroup(GroupItem* g);

    /**
     * @brief Adds a node to a group (updates descriptor).
     */
    void addNodeToGroup(GroupItem* g, NodeItem* n);

    /**
     * @brief Removes a node from a group.
     */
    void removeNodeFromGroup(GroupItem* g, NodeItem const* n);

private:
    mutable QRecursiveMutex m_mutex;             ///< Protects all registry state.
    QMap<NodeItem*, NodeDescriptor*> m_nodes;    ///< All registered nodes.
    QMap<GroupItem*, GroupDescriptor*> m_groups; ///< All registered groups.

    qint64 m_nextNodeId = 1;  ///< Auto-incrementing node ID counter.
    qint64 m_nextGroupId = 1; ///< Auto-incrementing group ID counter.
    friend class NodeItem;
    friend class GroupItem;
    friend class NodeFactory;
    friend class GraphScene;
    friend struct WidgetVisitor;
    friend class GraphRegistryTest;
};
