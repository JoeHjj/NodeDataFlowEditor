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

#include <QMap>
#include <QVector>

class NodeItem;
class PortLabel;
class ConnectionItem;

/**
 * @brief Describes a single node in the graph.
 *
 * A NodeDescriptor keeps track of the node's unique ID and all of
 * its port-related metadata. Each port is mapped to the list of
 * connections that originate from or terminate at that port.
 */
struct NodeDescriptor
{
    qint64 uid = -1;          ///< Globally unique node ID assigned by GraphRegistry.
    NodeItem* node = nullptr; ///< Pointer to the actual NodeItem in the scene.

    /// Map of input ports to the connections entering the node.
    QMap<PortLabel*, QVector<ConnectionItem*>> inputsDescriptor;

    /// Map of output ports to the connections leaving the node.
    QMap<PortLabel*, QVector<ConnectionItem*>> outputsDescriptor;

    /// Map of parameter ports to connections feeding parameter inputs.
    QMap<PortLabel*, QVector<ConnectionItem*>> parametersInputsDescriptor;
};
