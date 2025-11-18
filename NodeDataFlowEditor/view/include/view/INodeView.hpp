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

#include <QColor>
#include <QObject>
#include <QPointF>
#include <QString>
#include <QVector>

class PortLabel;

/**
 * @brief Interface for visual node elements in a node-based editor.
 *
 * INodeView defines a common API for node view classes (e.g., NodeItem, GroupItem)
 * to control appearance, position, ports, and interaction behavior.
 *
 * It provides a standard abstraction layer so different node implementations
 * can interoperate with the same Scene or controller logic.
 */
class INodeView : public QObject
{
    Q_OBJECT

public:
    using QObject::QObject;
    ~INodeView() override = default;

    // ================================
    // Node appearance and state
    // ================================

    /**
     * @brief Set the display nodeName of the node.
     * @param t The new nodeName string.
     */
    virtual void setDisplayedNodeName(const QString& t) = 0;

    /**
     * @brief Set the nodeName bar color.
     * @param c The new nodeName color.
     */
    virtual void setTitleColor(const QColor& c) = 0;

    /**
     * @brief Enable or disable the node’s “active” state.
     * @param on True to activate, false to deactivate.
     *
     * Used to visually indicate that a node is part of an active path or computation.
     */
    virtual void setActive(bool) = 0;

    /**
     * @brief Get the node “active” state.
     */
    virtual bool active() = 0;
    /**
     * @brief Show or hide the node in the scene.
     * @param on True to make visible, false to hide.
     */
    virtual void setVisibleNode(bool on) = 0;

    // ================================
    // Geometry
    // ================================

    /**
     * @brief Get the current scene position of the node.
     * @return QPointF representing the node’s scene position.
     */
    virtual QPointF position() const = 0;

    /**
     * @brief Set the scene position of the node.
     * @param p New position.
     */
    virtual void setPosition(const QPointF& p) = 0;

    // ================================
    // Ports management
    // ================================

    /**
     * @brief Add a new input port to the node.
     * @param name Port name.
     * @return Pointer to the created PortLabel.
     */
    virtual PortLabel* addInput(const QString& name) = 0;

    /**
     * @brief Add a new parameter port to the node.
     * @param widget Associated input widget (e.g. slider, spinbox).
     * @param name Port name.
     * @return Pointer to the created PortLabel.
     */
    virtual PortLabel* addParam(QWidget* widget, const QString& name) = 0;

    /**
     * @brief Add a new output port to the node.
     * @param name Port name.
     * @return Pointer to the created PortLabel.
     */
    virtual PortLabel* addOutput(const QString& name) = 0;

    /**
     * @brief Add a new input port to the node.
     * @param name Port name.
     * @param displayName Port displayed name.
     * @return Pointer to the created PortLabel.
     */
    virtual PortLabel* addInput(const QString& name, const QString& displayName) = 0;

    /**
     * @brief Add a new parameter port to the node.
     * @param widget Associated input widget (e.g. slider, spinbox).
     * @param name Port name.
     * @param displayName Port displayed name.
     * @return Pointer to the created PortLabel.
     */
    virtual PortLabel* addParam(QWidget* widget, const QString& name, const QString& displayName) = 0;

    /**
     * @brief Add a new output port to the node.
     * @param name Port name.
     * @param displayName Port displayed name.
     * @return Pointer to the created PortLabel.
     */
    virtual PortLabel* addOutput(const QString& name, const QString& displayName) = 0;

    /**
     * @brief Remove an existing input, parameter, or output port by name.
     */
    virtual void removeInput(const QString& name) = 0;
    /**
     * @brief Remove a parameter-type input port by name.
     *
     * @param name Internal name of the parameter input to remove.
     */
    virtual void removeParamInput(const QString& name) = 0;

    /**
     * @brief Remove an output port by name.
     *
     * @param name Internal name of the output port to remove.
     */
    virtual void removeOutput(const QString& name) = 0;

    /**
     * @brief Determine whether this node represents a group container.
     *
     * Some node types are group nodes (subgraphs or composite nodes)
     * that contain other nodes internally. This method allows the
     * graph system to differentiate them from regular data-processing
     * nodes.
     *
     * @return true if this node is a group/composite node.
     * @return false if this node is a regular functional node.
     */
    virtual bool isAGroupNode() const = 0;

    /**
     * @brief Get all input ports.
     * @return Vector of input PortLabel pointers.
     */
    virtual QVector<PortLabel*> inputs() const = 0;

    /**
     * @brief Get all output ports.
     * @return Vector of output PortLabel pointers.
     */
    virtual QVector<PortLabel*> outputs() const = 0;

    /**
     * @brief Get all parameter input ports.
     * @return List of parameter PortLabel pointers.
     */
    virtual QList<PortLabel*> paramsInputs() const = 0;

signals:
    /**
     * @brief Emitted when the node moves in the scene.
     */
    void sgnItemMoved();

    /**
     * @brief Emitted when the user interacts with a port (mouse events).
     */
    void sgnPortMouseClicked(INodeView* node, PortLabel* port);
    void sgnPortMouseReleased(INodeView* node, PortLabel* port);
    void sgnPortMouseEntered(INodeView* node, PortLabel* port);
    void sgnPortMouseLeaved(INodeView* node, PortLabel* port);

    /**
     * @brief Emitted when the node’s selection state changes.
     * @param selected True if the node is now selected, false otherwise.
     */
    void sgnSelectedChanged(bool selected);
};
