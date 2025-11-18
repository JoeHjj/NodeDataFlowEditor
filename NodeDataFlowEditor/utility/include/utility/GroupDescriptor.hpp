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

struct NodeDescriptor;
class GroupItem;
class PortLabel;

/**
 * @brief Describes a group of nodes.
 *
 * A GroupDescriptor tracks member nodes as well as the forwarding
 * rules used by the group (i.e., group-level ports mapped to the
 * underlying node ports).
 */
struct GroupDescriptor
{
    qint64 uid = -1;            ///< Globally unique group ID.
    GroupItem* group = nullptr; ///< Pointer to the actual GroupItem.

    QVector<NodeDescriptor*> memberNodes; ///< All nodes that belong to this group.

    /// Forwarded input ports (group port → list of actual node ports).
    QMap<PortLabel*, QVector<PortLabel*>> forwardInputsDescriptor;

    /// Forwarded output ports (group port → list of actual node ports).
    QMap<PortLabel*, QVector<PortLabel*>> forwardOutputsDescriptor;

    /// Forwarded parameter ports (group port → list of actual node ports).
    QMap<PortLabel*, QVector<PortLabel*>> forwardParametersInputsDescriptor;
};
