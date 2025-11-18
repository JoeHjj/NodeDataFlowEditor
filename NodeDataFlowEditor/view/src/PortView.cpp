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
#include "view/PortView.hpp"
#include <QFontMetricsF>
#include <QGraphicsSceneMouseEvent>
#include <QPainter>

PortView::PortView(const QString& text, QGraphicsItem* parent)
    : QGraphicsObject(parent)
    , m_text(text)
{
    setAcceptedMouseButtons(Qt::LeftButton);

    m_labelItem = new QGraphicsTextItem(text, this);
    m_labelItem->setDefaultTextColor(Qt::white);
    m_labelItem->setFont(QFont("Arial", 10, QFont::Bold));

    m_editWidget = new QLineEdit(text);
    m_editProxy = new QGraphicsProxyWidget(this);
    m_editProxy->setWidget(m_editWidget);
    m_editProxy->setVisible(false);

    connect(m_editWidget, &QLineEdit::returnPressed, this, &PortView::finishEditing);
    connect(m_editWidget, &QLineEdit::editingFinished, this, &PortView::finishEditing);

    repositionElements();
}

void
PortView::setArrowBeforeLabel(bool value)
{
    m_arrowBeforeLabel = value;
    repositionElements();
}

void
PortView::setEditable(bool value)
{
    m_editable = value;
}

void
PortView::setText(const QString& text)
{
    m_text = text;
    m_labelItem->setPlainText(text);
    m_editWidget->setText(text);
    repositionElements();
}

void
PortView::setColor(const QColor& color)
{
    m_color = color;
    update();
}

qreal
PortView::textWidth() const
{
    return m_labelItem ? m_labelItem->boundingRect().width() : 0;
}

qreal
PortView::textHeight() const
{
    return m_labelItem ? m_labelItem->boundingRect().height() : 0;
}

QRectF
PortView::boundingRect() const
{
    qreal arrowW = 10;
    qreal spacing = 4;
    return QRectF(0, 0, arrowW + spacing + textWidth(), qMax(arrowW, textHeight()));
}

void
PortView::paint(QPainter* painter, const QStyleOptionGraphicsItem*, QWidget*)
{
    painter->setRenderHint(QPainter::Antialiasing);

    qreal arrowW = 10;
    qreal arrowH = 10;
    qreal spacing = 4;

    QPointF arrowPos = m_arrowBeforeLabel
                           ? QPointF(0, (textHeight() - arrowH) / 2)
                           : QPointF(textWidth() + spacing, (textHeight() - arrowH) / 2);

    QPolygonF arrow;
    arrow << QPointF(0, 0)
          << QPointF(arrowW, arrowH / 2)
          << QPointF(0, arrowH)
          << QPointF(0, 0);

    if (m_arrowBeforeLabel)
    {
        QTransform t;
        t.translate(arrow.boundingRect().center().x(), arrow.boundingRect().center().y());
        t.rotate(180);
        t.translate(-arrow.boundingRect().center().x(), -arrow.boundingRect().center().y());
        arrow = t.map(arrow);
    }

    arrow.translate(arrowPos);

    painter->setBrush(m_color);
    painter->setPen(Qt::NoPen);
    painter->drawPolygon(arrow);
}

void
PortView::mouseDoubleClickEvent(QGraphicsSceneMouseEvent* event)
{
    if (m_editable)
        startEditing();

    event->accept();
}

void
PortView::startEditing()
{
    m_labelItem->setVisible(false);
    m_editProxy->setVisible(true);

    m_editWidget->setText(m_text);
    m_editWidget->selectAll();
    m_editWidget->setFocus();
}

void
PortView::finishEditing()
{
    if (!m_editProxy->isVisible())
        return;

    m_text = m_editWidget->text();
    m_labelItem->setPlainText(m_text);
    m_editProxy->setVisible(false);
    m_labelItem->setVisible(true);

    repositionElements();
    emit textChanged(m_text);
}

void
PortView::repositionElements()
{
    m_labelItem->setPlainText(m_text);
    m_labelItem->adjustSize();
    m_editProxy->setGeometry(m_labelItem->boundingRect());

    qreal xOffset = m_arrowBeforeLabel ? 10 + 4 : 0;
    m_labelItem->setPos(xOffset, 0);
    m_editProxy->setPos(xOffset, 0);

    update();
}
