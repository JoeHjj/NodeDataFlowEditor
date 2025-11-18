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

#include "view/ConnectionPort.hpp"

#include <QGraphicsPathItem>
#include <QPair>
#include <QTimer>

#include <optional>

/**
 * @brief Represents a visual link (edge) between two ports in a node-based graph.
 *
 * ConnectionItem manages its geometry, path drawing, active animation, and state flags.
 * It can dynamically update its endpoints as ports move or as the user drags a pending connection.
 */
class ConnectionItem final : public QGraphicsPathItem
{
    Q_INTERFACES(QGraphicsItem)

public:
    /**
     * @brief Construct a connection from a port.
     * @param startPoint Information about the port position and rect.
     * @param parent Optional parent graphics item.
     *
     * The connection will visually follow the cursor until its endpoint is set.
     */
    explicit ConnectionItem(const ConnectionPort& port, QGraphicsItem* parent = nullptr);
    explicit ConnectionItem(const ConnectionPort& port1, const ConnectionPort& port2, QGraphicsItem* parent = nullptr);

    /**
     * @brief Destructor. Cleans up timers and visual resources.
     */
    ~ConnectionItem() override;

    /**
     * @brief add an input or output endpoint.
     * @param isInput True if setting the input side; false for output.
     * @param point New position in scene coordinates.
     * @param portRect Bounding rectangle of the port.
     */
    void addPort(const ConnectionPort& port);

    /**
     * @brief Update the connection when a connected node moves.
     * @param isInput True if the input port moved; false for output port.
     * @param newPos New position of the moved port in scene coordinates.
     * @param rect Bounding rectangle of the moved port.when forwarded port
     * use the group node rect
     */
    void onNodeMoved(bool isInput, const QPointF& newPos, const QRectF& rect);

    /**
     * @brief Update the endpoint position while the connection is being drawn interactively.
     * @param point New cursor or temporary endpoint position.
     */
    void updateEndPoint(const QPointF& point);

    /**
     * @brief Retrieve the data of the input and output ports.
     */
    ConnectionPort inputPort() const;
    ConnectionPort outputPort() const;

    /**
     * @brief Check or set the active state of the connection.
     *
     * Active connections can be drawn differently (e.g. with glow or animation)
     * to indicate data flow or selection.
     */
    bool isActivated() const;
    void setIsActive(bool newIsActive);

    /**
     * @brief Recompute the connection curve between the two endpoints.
     *
     * Typically called when a port moves or endpoint positions change.
     */
    void updatePath();

    /**
     * @brief Mark the connection as compatible or incompatible with its target.
     *
     * Used during drag-to-connect operations to provide user feedback.
     */
    void setIsCompatible(bool newIsCompatible);

    /**
     * @brief Check whether the connection is currently being destroyed.
     *
     * This is used to prevent updates during teardown animations or cleanup.
     */
    bool isDestroying() const;
    QPainterPath shape() const override;

private:
    /**
     * @brief Paint the connection curve and optional animated elements.
     */
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) override;

    /**
     * @brief Compute and store a smooth curved path between the start and end points.
     */
    void drawPath(const QPointF& startPoint, const QPointF& endPoint);

    /**
     * @brief Update internal animation state for active connection visuals.
     */
    void updateAnimationStatus();

    /**
     * @brief Compute the exact point on the input port for connection attachment.
     */
    QPointF computeInputPoint(const ConnectionPort& port) const;

    /**
     * @brief Compute the exact point on the output port for connection attachment.
     */
    QPointF computeOutputPoint(const ConnectionPort& port) const;

private:
    ConnectionPort m_inputPort;  ///< Data for input connection port.
    ConnectionPort m_outputPort; ///< Data for output connection port.

    QPointF m_endPoint; ///< Current end point of the connection.

    bool m_isCompatible = false; ///< Whether the connection is currently compatible.

    QPainterPath m_currentPath; ///< Cached connection curve.

    bool m_isActive = false;           ///< Whether this connection is active/animated.
    QTimer m_animationTimer;           ///< Timer used for animating active connections.
    QVector<double> m_circlePositions; ///< Animation positions for decorative elements (e.g. flowing dots).

    bool m_isDestroying = false; ///< Flag set during cleanup to prevent further updates.
};

inline std::optional<ConnectionPort>
otherPort(const ConnectionItem& conn, const ConnectionPort& port)
{
    ConnectionPort p1 = conn.inputPort();
    auto p2 = conn.outputPort();

    if (p1 == port)
        return p2;
    else if (p2 == port)
        return p1;

    return std::nullopt; // passed port not found in the pair
}
