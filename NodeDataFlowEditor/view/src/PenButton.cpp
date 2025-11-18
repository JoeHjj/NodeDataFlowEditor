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

#include "view/PenButton.hpp"
#include <QPainter>
#include <QPolygonF>
#include <QTransform>

PenButton::PenButton(QWidget* parent)
    : QPushButton(parent)
{
    setFixedSize(20, 20);
    setCursor(Qt::PointingHandCursor);
    setFlat(true);
}

void
PenButton::paintEvent(QPaintEvent* event)
{
    Q_UNUSED(event);

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    QTransform transform;
    transform.translate(width() / 2.0, height() / 2.0);
    transform.rotate(-25);
    transform.translate(-width() / 2.0, -height() / 2.0);
    painter.setTransform(transform);

    painter.setBrush(Qt::red);
    painter.setPen(Qt::NoPen);
    QRectF eraserRect(8, 2, 4, 3);
    painter.drawRoundedRect(eraserRect, 1, 1);

    painter.setBrush(Qt::yellow);
    QRectF bodyRect(8, 5, 4, 9);
    painter.drawRect(bodyRect);

    painter.setPen(QPen(Qt::black, 0.5));
    painter.drawLine(8.5, 7, 11.5, 7);
    painter.drawLine(8.5, 10, 11.5, 10);

    painter.setBrush(Qt::white);
    painter.setPen(Qt::NoPen);
    QPolygonF tip;
    tip << QPointF(8, 14) << QPointF(12, 14) << QPointF(10, 17);
    painter.drawPolygon(tip);

    painter.setBrush(Qt::black);
    QPolygonF lead;
    lead << QPointF(9.5, 15) << QPointF(10.5, 15) << QPointF(10, 17);
    painter.drawPolygon(lead);

    painter.setPen(QPen(Qt::black, 0.3));
    painter.setBrush(Qt::NoBrush);
    painter.drawRect(bodyRect);
    painter.drawRoundedRect(eraserRect, 1, 1);
}
