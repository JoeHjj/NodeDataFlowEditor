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

#include "view/NodeItem.hpp"
#include "view/PortLabel.hpp"
#include <QColor>
#include <QGraphicsScene>
#include <QObject>
#include <QPointF>
#include <QString>

class NodeItemViewAdapter;
class NodeModel;
class NodePresenter;
class GraphRegistry;
class GraphScene;

/**
 * @brief Responsible for creating and managing nodes within the node-based editor.
 *
 * NodeFactory serves as a central point for:
 * - Creating new nodes (with model, view, and presenter linkage)
 * - Adding and removing ports or parameters
 * - Tagging ports with metadata
 *
 * It coordinates between the scene, model, and presentation layers to ensure
 * node creation and configuration remain consistent throughout the system.
 */
class NodeFactory
{
public:
    /**
     * @brief Construct a new NodeFactory instance.
     * @param parent Optional QObject parent.
     */
    explicit NodeFactory(std::shared_ptr<GraphRegistry> registry);

    /**
     * @brief Encapsulates the components that make up a fully defined node.
     *
     * A Node combines:
     * - The model (logic and data)
     * - The view (NodeItem for rendering)
     * - The adapter (connects view and model)
     * - The presenter (handles synchronization and interaction logic)
     */
    struct Node
    {
        NodeModel* model{nullptr};             ///< Data model representing node logic.
        NodeItem* item{nullptr};               ///< Visual representation in the scene.
        NodeItemViewAdapter* adapter{nullptr}; ///< Connects model and view.
        NodePresenter* presenter{nullptr};     ///< Handles presentation logic and synchronization.

        /**
         * @brief Destructor that cleans up the Node’s components.
         */
        ~Node();
    };

    // ================================
    // Node creation
    // ================================

    /**
     * @brief Create a new node and add it to a scene.
     * @param scene Target Scene where the node will be added.
     * @param nodeName The name of the node.
     * @param displayedName Display name of the node.
     * @param color Header color for the node (default: dark cyan).
     * @param pos Scene position for the new node (default: origin).
     * @return A unique_ptr to the created Node structure.
     *
     * The created node does not include default ports or icons.
     * It is immediately added to the provided scene.
     */
    std::unique_ptr<Node> createNode(GraphScene* scene,
                                     const QString& nodeName,
                                     const QString& displayedName,
                                     const QColor& color = Qt::darkCyan,
                                     const QPointF& pos = QPointF(0, 0));

    /**
     * @brief Create a new node and add it to a scene.
     * @param scene Target Scene where the node will be added.
     * @param nodeName Display node name of the node.
     * @param color Header color for the node (default: dark cyan).
     * @param pos Scene position for the new node (default: origin).
     * @return A unique_ptr to the created Node structure.
     *
     * The created node does not include default port or icons.
     * It is immediately added to the provided scene.
     */
    std::unique_ptr<Node> createNode(GraphScene* scene,
                                     const QString& nodeName,
                                     const QColor& color = Qt::darkCyan,
                                     const QPointF& pos = QPointF(0, 0));

    // ================================
    // Port manipulation (Model-driven)
    // ================================

    /**
     * @brief Add an input port to a node.
     * @param node Target node.
     * @param name Port name to add.
     */
    void addInput(const Node& node, const QString& name);

    /**
     * @brief Add an output port to a node.
     * @param node Target node.
     * @param name Port name to add.
     */
    void addOutput(const Node& node, const QString& name);

    /**
     * @brief Add a parameter port to a node.
     * @param node Target node.
     * @param widget Associated parameter widget (e.g., slider, combo box).
     * @param name Port name to add.
     */
    void addParameter(const Node& node, QWidget* widget, const QString& name);

    /**
     * @brief Add an input port to a node.
     * @param node Target node.
     * @param name Port name to add.
     * @param displayedName Port displayed name.
     */
    void addInput(const Node& node, const QString& name, const QString& displayedName);

    /**
     * @brief Add an output port to a node.
     * @param node Target node.
     * @param name Port name to add.
     * @param displayedName Port displayed name.
     */
    void addOutput(const Node& node, const QString& name, const QString& displayedName);

    /**
     * @brief Add a parameter port to a node.
     * @param node Target node.
     * @param widget Associated parameter widget (e.g., slider, combo box).
     * @param name Port name to add.
     * @param displayedName Port displayed name.
     */
    void addParameter(const Node& node, QWidget* widget, const QString& name, const QString& displayedName);

    /**
     * @brief Create a connection between two ports.
     * @param scene Target scene where the connection will be created.
     * @param port1 First port label.
     * @param port2 Second port label.
     * @return Pointer to the created ConnectionItem.
     */
    ConnectionItem* createConnection(GraphScene& scene, PortLabel& port1, PortLabel& port2, bool isActive);

    /**
     * @brief Check whether two ports can be connected based on orientation, tags, and state.
     * @param port1 First port.
     * @param port2 Second port.
     * @return True if the ports are compatible and can form a valid connection.
     */
    bool
    PortsAreCompatible(GraphRegistry& registry, PortLabel* port1, PortLabel* port2);

    /**
     * @brief Retrieve all ports belonging to a given node.
     *
     * This helper extracts the node’s input, output, and parameter
     * ports into the provided containers. Implementations should
     * ensure that the returned pointers refer to the actual port
     * instances owned by the node, in their current visual and
     * logical order.
     *
     * @param node The node whose ports will be retrieved.
     * @param inputPorts Output list that will receive all input ports.
     * @param outputPorts Output list that will receive all output ports.
     * @param parameterPorts Output list that will receive all parameter ports.
     */
    void getNodePorts(const Node& node,
                      QVector<PortLabel*>& inputPorts,
                      QVector<PortLabel*>& outputPorts,
                      QList<PortLabel*>& parameterPorts);
    /**
     * @brief Retrieve a input port by name from a node.
     * @param node Target node.
     * @param portName Name of the port to retrieve.
     * @return Pointer to the PortLabel if found; nullptr otherwise.
     */
    PortLabel* getInputPortByName(const Node& node, const QString& portName);

    /**
     * @brief Retrieve an output port by name from a node.
     * @param node Target node.
     * @param portName Name of the port to retrieve.
     * @return Pointer to the PortLabel if found; nullptr otherwise.
     */
    PortLabel* getOutputPortByName(const Node& node, const QString& portName);

    /**
     * @brief Retrieve a parameter port by name from a node.
     * @param node Target node.
     * @param portName Name of the port to retrieve.
     * @return Pointer to the PortLabel if found; nullptr otherwise.
     */
    PortLabel* getParameterPortByName(const Node& node, const QString& portName);

    void disableWidgetOfConnectedParametersInput(NodeItem* item);

    /**
     * @brief Create a new connection between two ports.
     *
     * This function creates and registers a ConnectionItem linking
     * @p fromPort to @p toPort. Implementations should enforce
     * graph rules, such as type compatibility, direction constraints,
     * and whether multiple connections are allowed. If the connection
     * cannot be created, the function should return nullptr.
     *
     * Ownership of the created ConnectionItem is typically transferred
     * to the scene or graph manager.
     *
     * @param fromPort The output or parameter port acting as the source.
     * @param toPort The input port acting as the target.
     *
     * @return A pointer to the created ConnectionItem, or nullptr on failure.
     */
    ConnectionItem*
    createConnectionBetweenPorts(PortLabel* fromPort, PortLabel* toPort);
    // ================================
    // Tag helpers (templated)
    // ================================

    /**
     * @brief Attach a compile-time tag type to an input port.
     * @tparam TAG Tag type to assign.
     * @param node Target node.
     * @param portName Name of the port to tag.
     */
    template <typename TAG>
    void addInputTag(Node& node, const QString& portName)
    {
        for (auto port : node.item->inputs())
        {
            if (port->name() == portName)
            {
                port->template addTag<TAG>();
                break;
            }
        }
    }
    /**
     * @brief Attach a compile-time tag type to an output port.
     * @tparam TAG Tag type to assign.
     * @param node Target node.
     * @param portName Name of the port to tag.
     */
    template <typename TAG>
    void addOutputTag(Node& node, const QString& portName)
    {
        for (auto port : node.item->outputs())
        {
            if (port->name() == portName)
            {
                port->template addTag<TAG>();
                break;
            }
        }
    }

    /**
     * @brief Attach a compile-time tag type to a parameter port.
     * @tparam TAG Tag type to assign.
     * @param node Target node.
     * @param portName Name of the parameter port to tag.
     */
    template <typename TAG>
    void addParamTag(Node& node, const QString& portName)
    {
        for (auto port : node.item->paramsInputs())
        {
            if (port->name() == portName)
            {
                port->template addTag<TAG>();
                break;
            }
        }
    }

    // ================================
    // Port removal helpers
    // ================================

    /**
     * @brief Remove an input port from a node.
     * @param node Target node.
     * @param name Name of the input port to remove.
     */
    void removeInput(const Node& node, const QString& name);

    /**
     * @brief Remove an output port from a node.
     * @param node Target node.
     * @param name Name of the output port to remove.
     */
    void removeOutput(const Node& node, const QString& name);

    /**
     * @brief Remove a parameter port from a node.
     * @param node Target node.
     * @param name Name of the parameter port to remove.
     */
    void removeParameter(const Node& node, const QString& name);

private:
    GraphScene* m_scene{nullptr}; ///< Cached pointer to the active scene.
    std::shared_ptr<GraphRegistry> m_registry;
};
