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

#include <QPushButton>

/**
 * @brief A small custom QPushButton displaying a pencil icon.
 *
 * This button is 20x20 pixels, designed to visually represent an "edit" action.
 * The pencil is slightly rotated from bottom-right to top-left, with a red eraser,
 * yellow body, white wood tip, and black lead.
 */
class PenButton : public QPushButton
{
    Q_OBJECT
public:
    /**
     * @brief Constructs a PenButton.
     * @param parent The parent widget, default is nullptr.
     */
    explicit PenButton(QWidget* parent = nullptr);

protected:
    /**
     * @brief Paints the pencil icon.
     * @param event The paint event.
     */
    void paintEvent(QPaintEvent* event) override;
};
