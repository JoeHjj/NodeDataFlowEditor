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
#include <QGraphicsItem>
#include <QGraphicsProxyWidget>
#include <QGraphicsTextItem>
#include <QLineEdit>
#include <QObject>

/**
 * @brief Visual component for displaying and optionally editing a port name.
 *
 * PortView renders a label and an optional left-side arrow indicator.
 * It can enter inline editing mode via a QLineEdit hosted in a
 * QGraphicsProxyWidget. This widget is used internally by PortLabel.
 */
class PortView : public QGraphicsObject
{
    Q_OBJECT

public:
    /**
     * @brief Construct a PortView displaying the given text.
     * @param text Initial label text.
     * @param parent Optional parent graphics item.
     */
    explicit PortView(const QString& text, QGraphicsItem* parent = nullptr);

    /**
     * @brief Show the arrow before (true) or after (false) the label.
     * @param value Whether the arrow comes first.
     */
    void setArrowBeforeLabel(bool value);

    /**
     * @brief Enable or disable inline editing.
     * @param value True to allow editing, false to disable it.
     */
    void setEditable(bool value);

    /**
     * @brief Change the displayed text.
     * @param text New label text.
     */
    void setText(const QString& text);

    /**
     * @brief Set the main color used for arrow and text highlight.
     * @param color New color.
     */
    void setColor(const QColor& color);

    /**
     * @brief Width of the rendered text (not the whole bounding rect).
     * @return Width in scene units.
     */
    qreal textWidth() const;

    /**
     * @brief Height of the rendered text.
     * @return Height in scene units.
     */
    qreal textHeight() const;

    /**
     * @brief Begin editing the label if editable.
     *
     * Creates and displays the QLineEdit via a proxy widget.
     */
    void startEditing();

signals:
    /**
     * @brief Emitted whenever the text changes due to editing.
     * @param newText The updated text.
     */
    void textChanged(const QString& newText);

protected:
    /**
     * @brief Draws the label and arrow.
     * @param painter Painter to draw with.
     * @param option Style options.
     * @param widget Optional widget.
     */
    void paint(QPainter* painter,
               const QStyleOptionGraphicsItem* option,
               QWidget* widget) override;

    /**
     * @brief Bounding rectangle containing the arrow and label.
     * @return QRectF enclosing all elements.
     */
    QRectF boundingRect() const override;

    /**
     * @brief Double-click to start editing, if editable.
     * @param event Mouse event details.
     */
    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent* event) override;

private:
    /**
     * @brief Update positions of arrow, label, and editor proxy.
     *
     * Called when text changes or when arrow position flips.
     */
    void repositionElements();

    /**
     * @brief Finish editing and commit the new text.
     */
    void finishEditing();

private:
    QGraphicsTextItem* m_labelItem{};    ///< Displayed label item.
    QLineEdit* m_editWidget{};           ///< Inline editing widget.
    QGraphicsProxyWidget* m_editProxy{}; ///< Hosts m_editWidget in the scene.
    bool m_editable = true;              ///< Whether editing is allowed.
    bool m_arrowBeforeLabel = true;      ///< If true, arrow is drawn before text.
    QColor m_color = Qt::gray;           ///< Color used for visuals.
    QString m_text;                      ///< Current label text.
};
