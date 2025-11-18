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

#include "view/EditableLabelItem.hpp"
#include "view/PenButton.hpp"
#include <QFont>
#include <QFontMetricsF>
#include <QGraphicsSceneMouseEvent>

EditableLabelItem::EditableLabelItem(const QString& text, QGraphicsItem* parent)
    : QGraphicsObject(parent)
    , m_labelText(text)
{
    setFlag(ItemHasNoContents, false);
    setAcceptedMouseButtons(Qt::LeftButton);
    setAcceptHoverEvents(true);

    m_labelItem = new QGraphicsTextItem(text, this);
    m_labelItem->setDefaultTextColor(Qt::white);
    m_labelItem->setFont(QFont("Arial", 10, QFont::Bold));

    m_editWidget = new QLineEdit(text);

    m_editProxy = new QGraphicsProxyWidget(this);
    m_editProxy->setWidget(m_editWidget);
    m_editProxy->setVisible(false);

    m_penButton = new PenButton();
    m_penProxy = new QGraphicsProxyWidget(this);
    m_penProxy->setWidget(m_penButton);

    repositionElements();

    connect(m_penButton, &QPushButton::clicked,
            this, &EditableLabelItem::startEditing);
    connect(m_editWidget, &QLineEdit::returnPressed,
            this, &EditableLabelItem::finishEditing);
    connect(m_editWidget, &QLineEdit::textChanged,
            this, &EditableLabelItem::updateGeometryFromText);
    connect(m_editWidget, &QLineEdit::editingFinished,
            this, &EditableLabelItem::onEditWidgetLostFocus);
}

void
EditableLabelItem::setLabelText(const QString& text)
{
    m_labelText = text;
    m_labelItem->setPlainText(text);
    m_editWidget->setText(text);
    repositionElements();
    emit textChanged(text);
}

QString
EditableLabelItem::getLabelText() const
{
    return m_labelText;
}

QRectF
EditableLabelItem::boundingRect() const
{
    return QRectF(0, 0, m_totalWidth, m_labelHeight);
}

void
EditableLabelItem::setModifyButtonVisibility(bool value)
{
    m_penButton->setVisible(value);
}

void
EditableLabelItem::paint(QPainter*, const QStyleOptionGraphicsItem*, QWidget*)
{
    // No custom painting needed
}

void
EditableLabelItem::repositionElements()
{
    m_labelItem->setPlainText(m_editWidget->text());
    m_labelItem->adjustSize();
    m_editProxy->setGeometry(m_labelItem->boundingRect());

    m_labelWidth = m_labelItem->boundingRect().width();
    m_labelHeight = m_labelItem->boundingRect().height();
    qreal xOffset = 0;
    if (m_penProxy->isVisible())
    {
        m_penProxy->setPos(0, 0);
        xOffset = m_penProxy->boundingRect().width() + 2;
    }
    m_labelItem->setPos(xOffset, 0);
    m_editProxy->setPos(xOffset, 0);

    m_totalWidth = xOffset + m_labelWidth;

    prepareGeometryChange();
    update();
}

void
EditableLabelItem::startEditing()
{
    m_labelItem->setVisible(false);
    m_editProxy->setVisible(true);

    m_editWidget->setText(m_labelText);
    m_editWidget->selectAll();
    m_editWidget->setFocus();
}

void
EditableLabelItem::finishEditing()
{
    m_labelText = m_editWidget->text();
    m_labelItem->setPlainText(m_labelText);
    m_editProxy->setVisible(false);
    m_labelItem->setVisible(true);

    repositionElements();
    emit textChanged(m_labelText);
}

void
EditableLabelItem::updateGeometryFromText()
{
    m_labelItem->setPlainText(m_editWidget->text());
    repositionElements();
}

void
EditableLabelItem::mouseDoubleClickEvent(QGraphicsSceneMouseEvent* event)
{
    startEditing();
    event->accept();
}

void
EditableLabelItem::onEditWidgetLostFocus()
{
    if (!m_editProxy->isVisible())
        return;

    finishEditing();
}
