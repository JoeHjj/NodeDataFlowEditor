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

#include <QGraphicsView>

class GraphScene;
class QWheelEvent;
class QKeyEvent;

/**
 * @brief Custom graphics view for displaying and interacting with the node-based scene.
 *
 * GraphView provides specialized handling for zooming, panning, and keyboard shortcuts
 * within the node editor. It extends QGraphicsView to support intuitive navigation
 * and event forwarding to the Scene.
 */
class GraphView final : public QGraphicsView
{
    Q_OBJECT

public:
    /**
     * @brief Construct a new GraphView bound to a Scene.
     * @param scene The Scene instance to visualize and interact with.
     * @param parent Optional parent widget.
     *
     * The view initializes custom interaction settings and connects to the given Scene
     * for event handling and rendering.
     */
    explicit GraphView(GraphScene* scene, QWidget* parent = nullptr);

protected:
    /**
     * @brief Handle mouse wheel events for zooming in and out of the scene.
     * @param event Wheel input event providing scroll direction and delta.
     *
     * Typically used to scale the view smoothly around the cursor position.
     */
    void wheelEvent(QWheelEvent* event) override;

    /**
     * @brief Handle keyboard press events for scene navigation or shortcuts.
     * @param event Key press event.
     *
     * Allows camera panning, selection toggling, or other editor-level shortcuts.
     */
    void keyPressEvent(QKeyEvent* event) override;

    /**
     * @brief Handle keyboard release events for state cleanup after key presses.
     * @param event Key release event.
     */
    void keyReleaseEvent(QKeyEvent* event) override;
};
