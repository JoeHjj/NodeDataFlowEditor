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

#include <QGraphicsItem>
#include <QGraphicsProxyWidget>
#include <QGraphicsTextItem>
#include <QLineEdit>
#include <QObject>

class PenButton;

/**
 * @brief A QGraphicsItem that displays a label which can be edited in-place.
 *
 * The item contains a text label, a QLineEdit editor, and an edit button
 * (PenButton). Clicking the button switches the label to edit mode. The
 * item automatically resizes to fit the text.
 */
class EditableLabelItem : public QGraphicsObject
{
    Q_OBJECT

public:
    /**
     * @brief Constructs an EditableLabelItem with initial text.
     * @param text The initial label text.
     * @param parent Optional parent QGraphicsItem.
     */
    explicit EditableLabelItem(const QString& text, QGraphicsItem* parent = nullptr);

    /**
     * @brief Sets the label text and updates the display.
     * @param text The new text.
     */
    void setLabelText(const QString& text);

    /**
     * @brief Returns the current label text.
     * @return The label text.
     */
    QString getLabelText() const;

    // QGraphicsItem interface
    QRectF boundingRect() const override;

    void setModifyButtonVisibility(bool value);

protected:
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) override;
    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent* event) override;

private slots:
    void onEditWidgetLostFocus();

signals:
    /**
     * @brief Emitted when the text is changed.
     * @param newText The new text.
     */
    void textChanged(const QString& newText);

private:
    QGraphicsTextItem* m_labelItem = nullptr;
    QGraphicsProxyWidget* m_editProxy = nullptr;
    QGraphicsProxyWidget* m_penProxy = nullptr;
    QLineEdit* m_editWidget = nullptr;
    PenButton* m_penButton = nullptr;
    QString m_labelText;

    qreal m_totalWidth = 0;
    qreal m_labelWidth = 0;
    qreal m_labelHeight = 20;

    // Layout helpers
    void repositionElements();

    // Editing handlers
    void startEditing();
    void finishEditing();
    void updateGeometryFromText();
};
