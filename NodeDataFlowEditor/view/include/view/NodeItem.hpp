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

#include "view/EditableLabelItem.hpp"
#include "view/PortLabel.hpp"

#include <QGraphicsItem>
#include <QMap>
#include <QVector>
#include <memory>

class PortLabel;
class GraphRegistry;
class QGraphicsProxyWidget;
class QWidget;

/**
 * @class NodeItem
 * @brief Visual and logical representation of a node in the graph scene.
 *
 * NodeItem encapsulates the node's title, input ports (left), output ports (right)
 * and parameter ports (center) with associated widgets. It handles layout,
 * painting, interaction events, and coordinates connections via a GraphRegistry.
 *
 * Responsibilities:
 * - Maintain lists of ports and parameter widgets.
 * - Lay out ports in three columns: inputs (left), parameters (center), outputs (right).
 * - Provide public methods to add/remove ports and parameter widgets.
 * - Emit signals for port interactions and node movement.
 */
class NodeItem : public QGraphicsObject
{
    Q_OBJECT

signals:
    /**
     * @brief Emitted when the given @p port is clicked by the user.
     * @param node Pointer to this NodeItem.
     * @param port Pointer to the PortLabel that was clicked.
     */
    void sgnPortMouseClicked(NodeItem* node, PortLabel* port);

    /**
     * @brief Emitted when the mouse is released over the given @p port.
     * @param node Pointer to this NodeItem.
     * @param port Pointer to the PortLabel that was released.
     */
    void sgnPortMouseReleased(NodeItem* node, PortLabel* port);

    /**
     * @brief Emitted when the mouse enters the visual area of @p port.
     * @param node Pointer to this NodeItem.
     * @param port Pointer to the PortLabel that received the hover.
     */
    void sgnPortMouseEntered(NodeItem* node, PortLabel* port);

    /**
     * @brief Emitted when the mouse leaves the visual area of @p port.
     * @param node Pointer to this NodeItem.
     * @param port Pointer to the PortLabel that lost hover.
     */
    void sgnPortMouseLeaved(NodeItem* node, PortLabel* port);

    /**
     * @brief Emitted whenever the node moves in the scene (position changed).
     */
    void sgnItemMoved();

    /**
     * @brief Emitted when the user edits the node's displayed name.
     * @param node Pointer to this NodeItem.
     * @param newdisplayedName New text entered by the user.
     */
    void sgnDisplayedNameChanged(NodeItem* node, const QString& newdisplayedName);

public:
    /**
     * @brief Construct a NodeItem with a custom title color.
     * @param registry Graph manager/registry that owns this node.
     * @param nodeName Internal node identifier (stable).
     * @param nodeDisplayedName User-visible title. If empty, @p nodeName is used.
     * @param titleColor Color used to draw the title bar.
     * @param parent Optional parent QGraphicsItem.
     */
    explicit NodeItem(std::shared_ptr<GraphRegistry> registry,
                      QString nodeName,
                      QString nodeDisplayedName,
                      QColor titleColor,
                      QGraphicsItem* parent = nullptr);

    /**
     * @brief Construct a NodeItem using an automatically generated title color.
     * @param registry Graph manager/registry that owns this node.
     * @param nodeName Internal node identifier (stable).
     * @param nodeDisplayedName Optional user-visible title.
     * @param parent Optional parent QGraphicsItem.
     */
    explicit NodeItem(std::shared_ptr<GraphRegistry> registry,
                      QString nodeName,
                      QString nodeDisplayedName = "",
                      QGraphicsItem* parent = nullptr);

    ~NodeItem() override;

    /**
     * @brief Query whether this node is a group/composite node.
     *
     * Override in derived classes that represent container/group nodes.
     *
     * @return true if this node acts as a group; false otherwise.
     */
    virtual bool isAGroupNode() const { return false; }

    /**
     * @brief Returns all ports (inputs + outputs + parameters) belonging to this node.
     * @return Vector of PortLabel pointers. Ownership remains with the NodeItem.
     */
    QVector<PortLabel*> getAllPorts() const;

    /**
     * @brief Add an input port with internal @p name (display uses name).
     * @param name Internal and displayed name of the port.
     * @return Pointer to the created PortLabel (owned by this NodeItem).
     */
    PortLabel* addInput(const QString& name);

    /**
     * @brief Add an output port with internal @p name (display uses name).
     * @param name Internal and displayed name of the port.
     * @return Pointer to the created PortLabel (owned by this NodeItem).
     */
    PortLabel* addOutput(const QString& name);

    /**
     * @brief Add an input port with separate internal and displayed names.
     * @param name Internal (stable) name of the port.
     * @param displayName Human-visible label for the port.
     * @return Pointer to the created PortLabel.
     */
    PortLabel* addInput(const QString& name, const QString& displayName);

    /**
     * @brief Add an output port with separate internal and displayed names.
     * @param name Internal (stable) name of the port.
     * @param displayName Human-visible label for the port.
     * @return Pointer to the created PortLabel.
     */
    PortLabel* addOutput(const QString& name, const QString& displayName);

    /**
     * @brief Remove an input port by its internal name.
     * If no port matches, the method does nothing.
     * @param name Internal name of the input port to remove.
     */
    void removeInput(const QString& name);

    /**
     * @brief Remove a parameter input port by its internal name.
     * If no port matches, the method does nothing.
     * @param name Internal name of the parameter port to remove.
     */
    void removeParamInput(const QString& name);

    /**
     * @brief Remove an output port by its internal name.
     * If no port matches, the method does nothing.
     * @param name Internal name of the output port to remove.
     */
    void removeOutput(const QString& name);

    /**
     * @brief Remove an input port by pointer.
     * Disconnects signals, unregisters from registry, and deletes the item.
     * @param input Pointer to the PortLabel to remove.
     */
    void removeInput(PortLabel* input);

    /**
     * @brief Remove a parameter port by pointer.
     * Removes both the PortLabel and its associated proxy widget (if any).
     * @param input Pointer to the PortLabel to remove.
     */
    void removeParamInput(PortLabel* input);

    /**
     * @brief Remove an output port by pointer.
     * Disconnects and deletes the given port.
     * @param output Pointer to the PortLabel to remove.
     */
    void removeOutput(PortLabel* output);

    /**
     * @brief Returns the node's bounding rectangle used for painting & hit-tests.
     * @return QRectF bounding rectangle.
     */
    QRectF boundingRect() const override;

    /**
     * @brief Paint the node (background, title bar and effects).
     * The child PortLabel / QGraphicsProxyWidget items are responsible for their own painting.
     *
     * @param painter QPainter to draw with.
     * @param option Drawing style options (unused).
     * @param widget Optional widget context (unused).
     */
    void paint(QPainter* painter,
               const QStyleOptionGraphicsItem*,
               QWidget*) override;

    /**
     * @brief Connects the given port's signals to this NodeItem's forwarding signals.
     *
     * This establishes the usual node->port wiring so clicks/hover/etc on ports
     * are forwarded as node-level signals (sgnPortMouseClicked, ...).
     *
     * @param port Pointer to the PortLabel to connect.
     */
    void connectPorts(const PortLabel* port);

    /**
     * @brief Disconnects all signal/slot connections between this node and @p port.
     *
     * Use before deleting a port to remove any lingering signal connections.
     * @param port Pointer to the PortLabel to disconnect.
     */
    void disconnectPorts(PortLabel* port);

    /**
     * @brief Query whether the node is currently active.
     * Active nodes typically appear highlighted.
     * @return true when active, false otherwise.
     */
    bool isActivated() const;

    /**
     * @brief Get a copy of the input ports vector.
     * @return QVector of PortLabel pointers.
     */
    QVector<PortLabel*> inputs() const;

    /**
     * @brief Get a copy of the output ports vector.
     * @return QVector of PortLabel pointers.
     */
    QVector<PortLabel*> outputs() const;

    /**
     * @brief Show or hide this node and all its ports and parameter widgets.
     * Also adjusts visibility of connections attached to those ports.
     * @param val If true, show everything; if false, hide.
     */
    void changeVisibility(bool val);

    /**
     * @brief Show or hide the node visuals (title/background) and its ports.
     * Slightly different from changeVisibility: intended for toggling the node body.
     * @param val Visibility value.
     */
    void changeNodeVisibility(bool val);

    /**
     * @brief Add a parameter widget to the center column and create an associated parameter port.
     *
     * The provided QWidget will be wrapped by a QGraphicsProxyWidget and positioned
     * under the parameter port label. The NodeItem takes ownership of the proxy;
     * the QWidget ownership remains with the caller unless otherwise transferred.
     *
     * @param widget The QWidget that implements the parameter UI.
     * @param name Internal name for the created parameter port.
     * @return Pointer to the new parameter PortLabel, or nullptr if widget is null.
     */
    PortLabel* addParameter(QWidget* widget, const QString& name);

    /**
     * @brief Add a parameter widget with a specific display name for the port.
     * @param widget The QWidget for the parameter UI.
     * @param name Internal name for the parameter port.
     * @param displayName Human-friendly label for the parameter port.
     * @return Pointer to the new parameter PortLabel.
     */
    PortLabel* addParameter(QWidget* widget, const QString& name, const QString& displayName);

    /**
     * @brief Returns a map of the parameter widgets (original QWidget -> proxy).
     * @return Map containing user widgets and their QGraphicsProxyWidget wrappers.
     */
    QMap<QWidget*, QGraphicsProxyWidget*> parameterWidgets() const;

    /**
     * @brief Returns the QWidget associated with a parameter PortLabel.
     * @param port Parameter port whose widget is requested.
     * @return Pointer to the QWidget, or nullptr if not found.
     */
    QWidget* getParameterWidget(PortLabel*) const;

    /**
     * @brief Returns the internal map of parameter ports to their proxy widgets.
     * @return QMap from PortLabel* to QGraphicsProxyWidget*.
     */
    QMap<PortLabel*, QGraphicsProxyWidget*> parameterPorts() const;

    /**
     * @brief Returns a QList of all parameter PortLabel pointers.
     * @return QList of parameter ports.
     */
    QList<PortLabel*> paramsInputs() const;

    /* ---------------------------
     * Title control
     * --------------------------- */

    /**
     * @brief Update the displayed node title.
     * This updates the child EditableLabelItem and triggers a relayout.
     * @param t New displayed title.
     */
    void setDisplayedNodeName(const QString& t);

    /**
     * @brief Get the currently displayed node title.
     * @return The user-visible node title.
     */
    QString displayedNodeName() const;

    /**
     * @brief Set the internal node name (identifier).
     * @param t Internal node name.
     */
    void setNodeName(const QString& t);

    /**
     * @brief Get the internal node name.
     * @return Internal identifier string.
     */
    QString nodeName() const;

    /**
     * @brief Set the color used to draw the node's title bar.
     * @param c QColor for the title bar.
     */
    void setNodeNameColor(const QColor& c);

    /**
     * @brief Remove every port from the node and disconnect associated connections.
     * This removes input, output and parameter ports and their proxies.
     */
    void disconnectAllPorts();

    /**
     * @brief Given a QGraphicsProxyWidget belonging to this node's parameters,
     * return the PortLabel associated with it, or nullptr if none found.
     * @param poxy Reference to the proxy widget.
     * @return PortLabel* or nullptr.
     */
    PortLabel* getPort(const QGraphicsProxyWidget& poxy) const;

    /**
     * @brief Recompute layout sizes and positions for all child items.
     * Must be called after adding/removing ports, changing port text, or resizing parameter widgets.
     */
    void updateLayout();

protected:
    /**
     * @brief Qt item change handler.
     * Used to detect moves and selection changes.
     * Emits sgnItemMoved when position changes and updates selection visuals.
     *
     * @param change The kind of change.
     * @param value The new value for the change.
     * @return QVariant Possibly-modified value to pass to base implementation.
     */
    QVariant itemChange(GraphicsItemChange change,
                        const QVariant& value) override;

    /**
     * @brief Handle scene-level events (hover, mouse press/release, double-click).
     * This forwards port events and updates hover/selection state.
     *
     * @param event Pointer to the QEvent to handle.
     * @return true if the event was consumed; otherwise delegates to base.
     */
    bool sceneEvent(QEvent* event) override;

    /**
     * @brief Recompute the internal bounding rect (m_rect) based on current child sizes.
     * Called internally by updateLayout().
     */
    void updateRect();

private:
    /**
     * @brief Internal helper: add a parameter-type PortLabel (no widget).
     * Use addParameter(...) to add both port and widget.
     * @param name Internal port name.
     * @return PortLabel* newly created parameter port.
     */
    PortLabel* addParamInput(const QString& name);

    /* ---------------------------
     * Drawing helpers
     * --------------------------- */

    /**
     * @brief Draw the node background (rounded rectangle).
     * @param painter Painter to draw with.
     */
    void drawBackground(QPainter& painter) const;

    /**
     * @brief Draw the title bar background and cutout for the main area.
     * Actual text is provided by m_nodeNameLabel (child QGraphicsItem).
     * @param painter Painter to draw with.
     */
    void drawTitle(QPainter& painter) const;

    /**
     * @brief Draw an outer glowing border when hovered/selected.
     * @param painter Painter to draw with.
     */
    void drawGlowingBounding(QPainter& painter);

    /**
     * @brief Set the node's active state which affects rendering (glow).
     * @param newIsActive True to mark active; false otherwise.
     */
    void setActive(bool newIsActive);

private:
    // ==================================================
    // TITLE SUPPORT
    // ==================================================

    EditableLabelItem* m_nodeNameLabel = nullptr; ///< Child item that displays and edits the node title.
    QString m_nodeName;                           ///< Stable internal node identifier.
    QString m_displayedNodeName;                  ///< User-visible title text.
    QColor m_nodeNameColor;                       ///< Color used for the title bar.

    // ==================================================
    // PORTS
    // ==================================================

    QVector<PortLabel*> m_inputs;  ///< Left column input ports (ordered top->bottom).
    QVector<PortLabel*> m_outputs; ///< Right column output ports (ordered top->bottom).

    // ==================================================
    // VISUAL CONFIGURATION
    // ==================================================

    qreal m_titleHeight = 30;   ///< Height reserved for title bar.
    int m_margin = 20;          ///< Outer margin inside the node rectangle.
    int m_spacing = 20;         ///< Vertical spacing between stacked items.
    int minWidth = 100;         ///< Minimum node width.
    qreal m_maxInputWidth = 0;  ///< Cached maximum input port width.
    qreal m_maxOutputWidth = 0; ///< Cached maximum output port width.
    qreal m_maxParamWidth = 0;  ///< Cached maximum parameter column width.

    QColor m_bgColor = QColor(30, 30, 30);     ///< Node background color.
    QColor m_borderColor = QColor(70, 70, 70); ///< Border color.

    QRectF m_rect; ///< Computed bounding rectangle for the node (includes margins).

    // ==================================================
    // PARAMETERS
    // ==================================================

    /**
     * @brief Map of raw parameter QWidget pointers to their QGraphicsProxyWrapper.
     *
     * The NodeItem does not take ownership of the original QWidget pointer
     * (caller may still own it), but the QGraphicsProxyWidget is parented
     * to this NodeItem and will be deleted with it.
     */
    QMap<QWidget*, QGraphicsProxyWidget*> m_parameterWidgets;

    /**
     * @brief Map of parameter PortLabel -> proxy widget used to display the parameter control.
     *
     * Each parameter port typically has a PortLabel (label above) and a proxy widget (control below).
     */
    QMap<PortLabel*, QGraphicsProxyWidget*> m_parameterPorts;

    QSize m_paramsRectSize; ///< Cached size of the stacked parameter controls area.

    // ==================================================
    // STATE
    // ==================================================

protected:
    bool m_hovered = false;                    ///< True while mouse is over the node.
    bool m_selected = false;                   ///< True when the node is selected.
    bool m_isActive = false;                   ///< Logical active state (may change appearance).
    std::shared_ptr<GraphRegistry> m_registry; ///< Registry managing nodes/connections.

    friend class GraphRegistry;
};
