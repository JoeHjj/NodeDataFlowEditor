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

#include "view/PortLabel.hpp"
#include "view/PortView.hpp"

#include "view/ConnectionItem.hpp"

#include <QGraphicsSceneMouseEvent>
#include <QPainter>

PortLabel::PortLabel(QString name,
                     QString displayName,
                     QString moduleName,
                     Orientation orientation,
                     QGraphicsItem* parent)
    : QGraphicsObject(parent)
    , m_portName(std::move(name))
    , m_portDisplayName(std::move(displayName))
    , m_moduleName(std::move(moduleName))
    , m_orientation(orientation)
{
    setAcceptedMouseButtons(Qt::LeftButton);
    setAcceptHoverEvents(true);

    m_portView = new PortView(m_portDisplayName, this);
    m_portView->setArrowBeforeLabel(isAnyInputPort());
    m_portView->setEditable(true);
    m_portView->setColor(m_portColor);

    connect(m_portView, &PortView::textChanged, this, [this](const QString& t) {
        m_portDisplayName = t;
        emit sgnDisplayedNameChanged(t);
        prepareGeometryChange();
        update();
    });

    repositionLabel();
}

PortLabel::PortLabel(QString name,
                     QString moduleName,
                     Orientation orientation,
                     QGraphicsItem* parent)
    : PortLabel(name, name, moduleName, orientation, parent)
{
}

PortLabel::~PortLabel() = default;

QRectF
PortLabel::boundingRect() const
{
    if (!m_portView)
        return QRectF();

    qreal arrowWidth = 10;
    qreal spacing = 4;
    qreal labelWidth = m_portView->textWidth();
    qreal width = arrowWidth + spacing + labelWidth;
    qreal height = m_portView->textHeight();

    return QRectF(0, 0, width, height);
}

void
PortLabel::paint(QPainter* painter, const QStyleOptionGraphicsItem*, QWidget*)
{
    painter->setRenderHint(QPainter::Antialiasing);

    QRectF rect = boundingRect();

    // Draw hover highlight
    if (m_hovered)
    {
        painter->setBrush(m_hoveredColor);
        painter->setPen(Qt::NoPen);
        painter->drawRect(rect);
    }

    // Draw clicked highlight
    if (m_clicked)
    {
        painter->setBrush(m_clickedColor);
        painter->setPen(Qt::NoPen);
        painter->drawRect(rect);
    }
}

void
PortLabel::setOrientation(Orientation o)
{
    if (m_orientation == o)
        return;

    prepareGeometryChange();
    m_orientation = o;
    if (m_portView)
        m_portView->setArrowBeforeLabel(isAnyInputPort());

    repositionLabel();
    update();
}

PortLabel::Orientation
PortLabel::getOrientation() const
{
    return m_orientation;
}

bool
PortLabel::isAnyInputPort() const
{
    return m_orientation == Orientation::Input || m_orientation == Orientation::Parameter;
}

bool
PortLabel::isParameterPort() const
{
    return m_orientation == Orientation::Parameter;
}

bool
PortLabel::isInputPort() const
{
    return m_orientation == Orientation::Input;
}

bool
PortLabel::isOutputPort() const
{
    return m_orientation == Orientation::Output;
}

void
PortLabel::setDisplayName(const QString& text)
{
    if (m_portDisplayName == text)
        return;

    m_portDisplayName = text;
    if (m_portView)
        m_portView->setText(m_portDisplayName);

    prepareGeometryChange();
    update();
}

QString
PortLabel::displayName() const
{
    return m_portDisplayName;
}

void
PortLabel::setName(const QString& text)
{
    m_portName = text;
}

QString
PortLabel::name() const
{
    return m_portName;
}

void
PortLabel::setModuleName(const QString& moduleName)
{
    m_moduleName = moduleName;
}

QString
PortLabel::moduleName() const
{
    return m_moduleName;
}

ConnectionPort
PortLabel::getConnectionPortData() const
{
    return {scenePos(),
            boundingRect(),
            m_portName,
            m_moduleName,
            isAnyInputPort()};
}

void
PortLabel::setHovered(bool hovered)
{
    m_hovered = hovered;
    update();
}

void
PortLabel::setColor(const QColor& color)
{
    m_portColor = color;
    if (m_portView)
        m_portView->setColor(color);
    update();
}

bool
PortLabel::sceneEvent(QEvent* event)
{
    switch (event->type())
    {
        case QEvent::GraphicsSceneHoverEnter:
            m_hovered = true;
            emit sgnPortMouseEntered(this);
            update();
            return true;

        case QEvent::GraphicsSceneHoverLeave:
            m_hovered = false;
            emit sgnPortMouseLeaved(this);
            update();
            return true;

        case QEvent::GraphicsSceneMousePress:
            m_clicked = true;
            emit sgnPortMouseClicked(this);
            update();
            return true;

        case QEvent::GraphicsSceneMouseRelease:
            m_clicked = false;
            emit sgnPortMouseReleased(this);
            update();
            return true;

        case QEvent::GraphicsSceneMouseDoubleClick:
            m_portView->startEditing();
            return true;

        default:
            break;
    }

    return QGraphicsItem::sceneEvent(event);
}

void
PortLabel::repositionLabel()
{
    if (!m_portView)
        return;

    // Align arrow and label horizontally
    if (isAnyInputPort())
    {
        m_portView->setPos(0, 0);
    }
    else
    {
        // Output: shift arrow to the right of label
        m_portView->setPos(0, 0);
        m_portView->setArrowBeforeLabel(false);
    }
}
