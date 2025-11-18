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

#include "view/ConnectionItem.hpp"

#include <QPainter>
#include <QPainterPath>
#include <QPen>
#include <QtMath>
#include <QStyleOption>
#include <QDebug>

ConnectionItem::ConnectionItem(const ConnectionPort& port, QGraphicsItem* parent)
    : QGraphicsPathItem(parent)
{
    setFlag(ItemIsSelectable, true);
    QPen pen(Qt::red, 2);
    setPen(pen);
    setZValue(1);

    addPort(port);

    m_circlePositions = {0.0, 0.2, 0.4, 0.6, 0.8};

    QObject::connect(&m_animationTimer, &QTimer::timeout, [this]() {
        for (auto& position : m_circlePositions)
        {
            if (!m_inputPort.isInput)
            {
                // forward
                position = std::fmod(position + 0.01, 1.0);
            }
            else
            {
                // backward
                position -= 0.01;
                if (position < 0.0)
                    position += 1.0;
            }
        }
        update();

    });
}

ConnectionItem::ConnectionItem(const ConnectionPort& port1, const ConnectionPort& port2, QGraphicsItem* parent)
    : ConnectionItem(port1, parent)
{
    addPort(port2);
}

void
ConnectionItem::addPort(const ConnectionPort& port)
{
    if (port.isInput)
    {
        m_inputPort = port;
    }
    else
    {
        m_outputPort = port;
    }
    updatePath();
}

ConnectionItem::~ConnectionItem()
{
    m_isDestroying = true;
}

void
ConnectionItem::onNodeMoved(bool isInput, const QPointF& newPos, const QRectF& rect)
{
    if (isInput)
    {
        m_inputPort.scenePos = newPos;
    }
    else
    {
        m_outputPort.scenePos = newPos;
        if (rect.isValid() && !rect.isNull())
            m_outputPort.rect = rect;
    }
    updatePath();
}

void
ConnectionItem::updateEndPoint(const QPointF& point)
{
    m_endPoint = point;
    updatePath();
}

ConnectionPort
ConnectionItem::inputPort() const
{
    return m_inputPort;
}

ConnectionPort
ConnectionItem::outputPort() const
{
    return m_outputPort;
}

void
ConnectionItem::updateAnimationStatus()
{
    if (m_isActive)
        m_animationTimer.start(30);
    else
        m_animationTimer.stop();
}

QPointF
ConnectionItem::computeInputPoint(const ConnectionPort& port) const
{
    return {port.scenePos.x() + port.rect.width(), port.scenePos.y() + port.rect.height() / 2 - 3};
}

QPointF
ConnectionItem::computeOutputPoint(const ConnectionPort& port) const
{
    return {port.scenePos.x(), port.scenePos.y() + port.rect.height() / 2 - 3};
}

void
ConnectionItem::drawPath(const QPointF& startPoint, const QPointF& endPoint)
{
    prepareGeometryChange();

    if (startPoint == endPoint)
        return;
    if (startPoint.isNull() || endPoint.isNull())
        return;

    if (m_isCompatible || (!m_inputPort.portName.isEmpty() && !m_outputPort.portName.isEmpty()))
        setPen(QPen(Qt::green, 2));
    else if (!m_isCompatible)
        setPen(QPen(Qt::red, 2));
    QPainterPath path(startPoint);
    qreal dx = endPoint.x() - startPoint.x();
    QPointF ctrl1 = startPoint + QPointF(dx * 0.25, 0);
    QPointF ctrl2 = endPoint - QPointF(dx * 0.25, 0);
    path.cubicTo(ctrl1, ctrl2, endPoint);

    m_currentPath = path;
    setPath(path);

    updateAnimationStatus();
}

void
ConnectionItem::setIsCompatible(bool newIsCompatible)
{
    m_isCompatible = newIsCompatible;
    QPen pen(Qt::red, 2);

    if (m_isCompatible)
        pen = QPen(Qt::green, 2);

    setPen(pen);
}

bool
ConnectionItem::isDestroying() const
{
    return m_isDestroying;
}

bool
ConnectionItem::isActivated() const
{
    return m_isActive;
}

void
ConnectionItem::setIsActive(bool newIsActive)
{
    m_isActive = newIsActive;
    updateAnimationStatus();
    update();
}

void
ConnectionItem::updatePath()
{
    QPointF startPoint;
    QPointF endPoint;

    // Start from input
    if (!m_inputPort.scenePos.isNull())
    {
        startPoint = computeOutputPoint(m_inputPort);
    }

    if (!m_outputPort.scenePos.isNull()) // start from output
    {
        endPoint = computeInputPoint(m_outputPort);
    }

    if (!startPoint.isNull() && !endPoint.isNull())
        drawPath(startPoint, endPoint);
    else if (!endPoint.isNull()) // no output yet
        drawPath(endPoint, m_endPoint);
    else if (!startPoint.isNull())
        drawPath(startPoint, m_endPoint);
}

QPainterPath
ConnectionItem::shape() const
{
    QPainterPathStroker stroker;
    stroker.setWidth(10); // thickness for selection hit area
    return stroker.createStroke(m_currentPath);
}

void
ConnectionItem::paint(QPainter* painter,
                      const QStyleOptionGraphicsItem* option,
                      QWidget*)
{
    painter->setRenderHint(QPainter::Antialiasing, true);

    if (option->state & QStyle::State_Selected)
    {
        QPen glowPen(QColor(0, 150, 255, 180)); // bright blue, semi-transparent
        glowPen.setWidth(10);                   // outer glow width
        glowPen.setCapStyle(Qt::RoundCap);
        glowPen.setJoinStyle(Qt::RoundJoin);
        painter->setPen(glowPen);
        painter->drawPath(m_currentPath);

        QPen innerGlow(QColor(0, 180, 255, 255)); // stronger blue
        innerGlow.setWidth(6);
        innerGlow.setCapStyle(Qt::RoundCap);
        innerGlow.setJoinStyle(Qt::RoundJoin);
        painter->setPen(innerGlow);
        painter->drawPath(m_currentPath);
    }
    QPen normalPen = pen();
    normalPen.setCapStyle(Qt::RoundCap);
    normalPen.setJoinStyle(Qt::RoundJoin);
    painter->setPen(normalPen);
    painter->setBrush(Qt::NoBrush);
    painter->drawPath(m_currentPath);
    if (m_isActive)
    {
        if (m_isCompatible || (!m_inputPort.portName.isEmpty() && !m_outputPort.portName.isEmpty()))
            painter->setBrush(Qt::green);
        else
            painter->setBrush(Qt::red);
        painter->setPen(Qt::NoPen);

        for (qreal t : std::as_const(m_circlePositions))
        {
            QPointF pt = m_currentPath.pointAtPercent(t);
            painter->drawEllipse(pt, 5, 5);
        }
    }
}
