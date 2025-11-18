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

#include <QPointF>
#include <QRectF>
#include <QString>

/**
 * @brief Holds data for initializing a new connection.
 *
 * This structure records the starting port position, the port's bounding rect,
 * and whether the connection originates from an input port.
 */
struct ConnectionPort
{
    QPointF scenePos;     ///< Position of the port in scene coordinates.
    QRectF rect;          ///< Port’s bounding rectangle.
    QString portName;     ///< Port’s name.
    QString moduleName;   ///< Module’s name.
    bool isInput = false; ///< True if the connection begins at an input port.

    // --- Equality operator ---
    bool operator==(const ConnectionPort& other) const
    {
        return portName == other.portName &&
               moduleName == other.moduleName &&
               isInput == other.isInput;
    }

    // --- Copy constructor ---
    ConnectionPort(const ConnectionPort& other) = default;

    // --- Copy assignment ---
    ConnectionPort& operator=(const ConnectionPort& other) = default;

    // --- Move constructor ---
    ConnectionPort(ConnectionPort&& other) noexcept = default;

    // --- Move assignment ---
    ConnectionPort& operator=(ConnectionPort&& other) noexcept = default;

    // --- Default constructor ---
    ConnectionPort() = default;

    // If you want to allow constructing with values:
    ConnectionPort(QPointF pos, QRectF r, QString p, QString m, bool input)
        : scenePos(std::move(pos))
        , rect(std::move(r))
        , portName(std::move(p))
        , moduleName(std::move(m))
        , isInput(input)
    {}
};
