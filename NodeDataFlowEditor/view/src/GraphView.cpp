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

#include "view/GraphView.hpp"
#include "view/GraphScene.hpp"

#include <QKeyEvent>
#include <QWheelEvent>
#include <QtGlobal>

GraphView::GraphView(GraphScene* scene, QWidget* parent)
    : QGraphicsView(parent)
{
    setScene(scene);
    setRenderHint(QPainter::Antialiasing);
    setTransformationAnchor(AnchorUnderMouse);
    setResizeAnchor(AnchorUnderMouse);
    setViewportUpdateMode(FullViewportUpdate);
    setCacheMode(CacheNone);

    setDragMode(RubberBandDrag);

    // Hide scrollbars
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
}

void
GraphView::wheelEvent(QWheelEvent* event)
{
    constexpr qreal zoomFactor = 1.15;

    QPointF delta = event->angleDelta();

    if (delta.y() == 0)
    {
        event->ignore();
        return;
    }

    qreal factor = (delta.y() > 0) ? zoomFactor : 1.0 / zoomFactor;

    // Clamp zoom level
    if (qreal currentScale = transform().m11();
        (factor > 1.0 && currentScale < 5.0) ||
        (factor < 1.0 && currentScale > 0.1))
        scale(factor, factor);

    event->accept();
}

void
GraphView::keyPressEvent(QKeyEvent* event)
{
    if (event->key() == Qt::Key_Control)
    {
        setDragMode(ScrollHandDrag);
        setCursor(Qt::CrossCursor);
        return;
    }
    QGraphicsView::keyPressEvent(event);
}

void
GraphView::keyReleaseEvent(QKeyEvent* event)
{
    if (event->key() == Qt::Key_Control)
    {
        setDragMode(RubberBandDrag);
        unsetCursor();
        return;
    }
    QGraphicsView::keyReleaseEvent(event);
}
