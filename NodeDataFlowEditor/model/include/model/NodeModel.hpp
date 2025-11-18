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

/**
 * @brief Defines the basic metadata for a node port.
 *
 * A PortSpec describes a single port by its name and kind (input, output, or parameter).
 * It is used by NodeModel to represent the structure of a node without relying
 * on the graphical view layer.
 */
struct PortSpec
{
    /**
     * @brief Type of port connection.
     */
    enum class Kind : uint8_t
    {
        Input,  ///< Standard input port.
        Output, ///< Standard output port.
        Param   ///< Parameter or control port.
    };

    QString name;        ///< Name of the port.
    QString displayName; ///< Display name of the port.
    Kind kind;           ///< Type of the port.
};

/**
 * @brief Describes a parameter port specification with an associated widget.
 *
 * Extends PortSpec by including a QWidget pointer representing the
 * input control for this parameter.
 */
struct ParamSpec : PortSpec
{
    QWidget* widget = nullptr; ///< Pointer to the associated parameter widget.
};

/**
 * @brief Data model representing a node’s logical state and configuration.
 *
 * NodeModel stores and emits changes to a node’s properties such as:
 * - Title, color, and visibility
 * - Position in the scene
 * - Defined ports and parameters
 *
 * It does not manage any graphical rendering; instead, it provides data
 * that a presenter or view (e.g., NodeItem) can bind to.
 */
class NodeModel : public QObject
{
    Q_OBJECT

public:
    /**
     * @brief Construct a new NodeModel instance.
     * @param parent Optional QObject parent.
     */
    explicit NodeModel(QObject* parent = nullptr);

    // ================================
    // Accessors
    // ================================

    /** @brief Get the node’s name. */
    const QString& nodeName() const { return m_nodeName; }

    /** @brief Get the node’s  displayed name. */
    const QString& displayedNodeName() const { return m_displayedNodeName; }

    /** @brief Get the node’s name bar color. */
    const QColor& titleColor() const { return m_titleColor; }

    /** @brief Check whether the node is currently active. */
    bool isActive() const { return m_active; }

    /** @brief Check whether the node is visible. */
    bool isVisible() const { return m_visible; }

    /** @brief Get the node’s current position in the scene. */
    QPointF position() const { return m_position; }

    /** @brief Get the list of all non-parameter ports. */
    const QVector<PortSpec>& ports() const { return m_ports; }

    /** @brief Get the list of all parameter ports. */
    const QVector<ParamSpec>& params() const { return m_params; }

public slots:
    // ================================
    // Property setters
    // ================================

    /**
     * @brief Set the node displayed name.
     * @param t New name string.
     */
    void setDisplayedNodeName(const QString& t);

    /**
     * @brief Set the node name.
     * @param t New name string.
     */
    void setNodeName(const QString& t);

    /**
     * @brief Set the node’s name bar color.
     * @param c New color.
     */
    void setTitleColor(const QColor& c);

    /**
     * @brief Set the active state of the node.
     * @param on True to activate, false to deactivate.
     */
    void setActive(bool on);

    /**
     * @brief Set whether the node is visible.
     * @param on True to show, false to hide.
     */
    void setVisible(bool on);

    /**
     * @brief Set the position of the node in the scene.
     * @param p New position.
     */
    void setPosition(const QPointF& p);

    // ================================
    // Port management
    // ================================

    /**
     * @brief Add a new port of the given kind.
     * @param name Name of the port.
     * @param displayName Display name of the port.
     * @param kind Type of the port (Input, Output, or Param).
     */
    void addPort(const QString& name, const QString& displayName, PortSpec::Kind kind);

    /**
     * @brief Add a new parameter port with an associated widget.
     * @param name Name of the parameter.
     * @param displayName Display name of the parameter.
     * @param widget Pointer to the associated input widget.
     * @param kind Type of the port (typically Param).
     */
    void addParam(const QString& name, const QString& displayName, QWidget* widget, PortSpec::Kind kind);

    /**
     * @brief Remove a port from the node.
     * @param name Name of the port.
     * @param kind Type of the port to remove.
     */
    void removePort(const QString& name, PortSpec::Kind kind);

    /**
     * @brief Remove a parameter from the node.
     * @param name Name of the parameter.
     * @param kind Type of the parameter to remove.
     */
    void removeParam(const QString& name, PortSpec::Kind kind);

signals:
    // ================================
    // Property change signals
    // ================================

    /** @brief Emitted when the node’s name changes. */
    void titleChanged(const QString&);

    /** @brief Emitted when the node’s name color changes. */
    void titleColorChanged(const QColor&);

    /** @brief Emitted when the node’s active state changes. */
    void activeChanged(bool);

    /** @brief Emitted when visibility changes. */
    void visibilityChanged(bool);

    /** @brief Emitted when position changes. */
    void positionChanged(const QPointF&);

    // ================================
    // Port management signals
    // ================================

    /** @brief Emitted when a new port is added. */
    void portAdded(const PortSpec&);

    /** @brief Emitted when a new parameter port is added. */
    void paramAdded(const ParamSpec&);

    /** @brief Emitted when a port is removed. */
    void portRemoved(const PortSpec&);

    /** @brief Emitted when a parameter is removed. */
    void paramRemoved(const ParamSpec&);

private:
    QString m_nodeName{"Node"};          ///< Node Name.
    QString m_displayedNodeName{"Node"}; ///< Node display Name.
    QColor m_titleColor{Qt::darkCyan};   ///< Header color.
    bool m_active{false};                ///< Whether node is active.
    bool m_visible{true};                ///< Whether node is visible.
    QPointF m_position{0.0, 0.0};        ///< Scene position of the node.
    QVector<PortSpec> m_ports;           ///< List of standard input/output ports.
    QVector<ParamSpec> m_params;         ///< List of parameter port with widgets.
};
