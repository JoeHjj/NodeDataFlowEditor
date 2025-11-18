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

#include <QGraphicsScene>
#include <memory>

class ConnectionItem;
class GraphRegistry;
class NodeItem;
class NodeFactory;
class PortLabel;

/**
 * @brief Custom QGraphicsScene implementation for the node editor environment.
 *
 * Scene manages the placement, grouping, and interconnection of NodeItems via
 * PortLabels and ConnectionItems. It also handles background drawing,
 * mouse interaction, and temporary connection logic during user operations.
 *
 * Responsibilities include:
 * - Creating and tracking node connections.
 * - Handling mouse events for connection creation.
 * - Rendering a grid background.
 * - Managing selection grouping and visual feedback.
 */
class GraphScene final : public QGraphicsScene
{
    Q_OBJECT

public:
    /**
     * @brief Construct a new Scene instance.
     * @param parent Optional QObject parent.
     */
    explicit GraphScene(QObject* parent = nullptr);

    /**
     * @brief Destructor. Cleans up all remaining connections and nodes.
     */
    ~GraphScene() override;

    std::shared_ptr<NodeFactory> getNodeFactory();

    std::shared_ptr<GraphRegistry> getGraphRegistry();
    /**
     * @brief Add a NodeItem to the scene.
     * @param node The NodeItem to add.
     */
    void addNodeItem(NodeItem* node);

    /**
     * @brief Group all currently selected nodes into a GroupItem.
     *
     * The resulting GroupItem replaces the selected nodes and inherits
     * their connections where applicable.
     */
    void groupSelectedNodes(QList<NodeItem*> nodes);

    // ================================
    // Appearance
    // ================================

    /**
     * @brief Set the background color of the scene.
     * @param newBackgroundColor The new background color.
     */
    void setBackgroundColor(const QColor& newBackgroundColor);

    /**
     * @brief Set the color for darker grid lines.
     * @param newDarkLinesColor The new dark line color.
     */
    void setDarkLinesColor(const QColor& newDarkLinesColor);

    /**
     * @brief Set the color for lighter grid lines.
     * @param newLightLinesColor The new light line color.
     */
    void setLightLinesColor(const QColor& newLightLinesColor);

    // ================================
    // Connection management
    // ================================

    /**
     * @brief Connect all ports of a given node to their corresponding targets.
     * @param node Node to connect.
     *
     * Typically called when restoring saved graphs or reloading scenes.
     */
    void connectNode(const NodeItem* node);

    /**
     * @brief Disconnect all connections related to a node.
     * @param node Node to disconnect.
     */
    void disconnectNode(const NodeItem* node) const;

public slots:
    /**
     * @brief Handle user click on a port.
     * @param port The clicked port.
     *
     * Starts or extends a connection, depending on current interaction state.
     */
    void onPortClicked(PortLabel* port);

    /**
     * @brief Handle mouse release on a port.
     * @param port The released port.
     *
     * Finalizes or cancels a pending connection.
     */
    void onPortMouseReleased(PortLabel* port);

protected:
    // ================================
    // Event handling
    // ================================

    /**
     * @brief Handle mouse movement for dragging and connection drawing.
     */
    void mouseMoveEvent(QGraphicsSceneMouseEvent* event) override;

    /**
     * @brief Handle mouse release for finalizing node placement or connections.
     */
    void mouseReleaseEvent(QGraphicsSceneMouseEvent* event) override;

    /**
     * @brief Draw a grid background for the scene.
     * @param painter Painter used to draw the background.
     * @param rect The visible rectangle to be painted.
     */
    void drawBackground(QPainter* painter, const QRectF& rect) override;

    /**
     * @brief Handle keyboard shortcuts or navigation events.
     * @param event The key press event.
     */
    void keyPressEvent(QKeyEvent* event) override;

    /**
     * @brief Handle context menu (right-click) events.
     * @param event The context menu event.
     */
    void contextMenuEvent(QGraphicsSceneContextMenuEvent* event) override;

private:
    void deleteNodeConnections(const NodeItem* node);
    void deleteConnection(ConnectionItem* connection);

private:
    ConnectionItem* m_tempConnection = nullptr; ///< Temporary connection being created.
    PortLabel* m_startPort = nullptr;           ///< Port where a connection drag started.
    PortLabel* m_lastFoundPort = nullptr;       ///< Most recently hovered compatible port.

    QColor m_backgroundColor = Qt::darkGray; ///< Scene background color.
    QColor m_lightLinesColor = Qt::gray;     ///< Color for lighter grid lines.
    QColor m_darkLinesColor = Qt::black;     ///< Color for darker grid lines.
    std::shared_ptr<GraphRegistry> m_registry;
    std::shared_ptr<NodeFactory> m_factory;
};
