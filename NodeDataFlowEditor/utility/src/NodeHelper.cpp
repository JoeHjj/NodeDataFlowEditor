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

#include "utility/NodeHelper.hpp"
#include "utility/GraphRegistry.hpp"
#include "view/GraphScene.hpp"
#include "view/GroupItem.hpp"
#include "view/NodeItem.hpp"

QColor
generate_glowing_color()
{
    int hue = QRandomGenerator::global()->bounded(360);
    int saturation = 255;
    int value = 255;

    return QColor::fromHsv(hue, saturation, value);
}

QList<NodeItem*>
get_selected_nodes(const QList<QGraphicsItem*>& items)
{
    QList<NodeItem*> selectedNodes;
    for (QGraphicsItem* item : items)
        if (auto* node = dynamic_cast<NodeItem*>(item); node && !dynamic_cast<GroupItem*>(item))
            selectedNodes.append(node);
    return selectedNodes;
}

QList<GroupItem*>
get_selected_groups(const QList<QGraphicsItem*>& items)
{
    QList<GroupItem*> selectedGroups;
    for (QGraphicsItem* item : items)
        if (auto* group = dynamic_cast<GroupItem*>(item))
            selectedGroups.append(group);
    return selectedGroups;
}

QRectF
calculate_group_rect(const QList<NodeItem*>& nodes)
{
    QRectF rect;
    for (auto const* node : nodes)
        rect = rect.united(node->sceneBoundingRect());
    return rect;
}

void
for_each_selected_node(const GraphScene* scene, const std::function<void(NodeItem*)>& fn)
{
    for (QGraphicsItem* item : scene->selectedItems())
        if (auto* node = dynamic_cast<NodeItem*>(item))
            fn(node);
}

void
for_each_selected_connection(const GraphScene* scene, const std::function<void(ConnectionItem*)>& fn)
{
    for (QGraphicsItem* item : scene->selectedItems())
        if (auto* conn = dynamic_cast<ConnectionItem*>(item))
            fn(conn);
}

void
for_each_selected_group(const GraphScene* scene, const std::function<void(GroupItem*)>& fn)
{
    for (QGraphicsItem* item : scene->selectedItems())
        if (auto* group = dynamic_cast<GroupItem*>(item))
            fn(group);
}

void
remove_temp_connection(GraphScene* scene, ConnectionItem*& tempConnection)
{
    if (tempConnection)
    {
        scene->removeItem(tempConnection);
        delete tempConnection;
        tempConnection = nullptr;
    }
}

PortLabel*
port_at_position(const QGraphicsScene& scene, const QPointF& pos)
{
    QList<QGraphicsItem*> itemsAtPos = scene.items(pos);
    for (QGraphicsItem* item : std::as_const(itemsAtPos))
        if (auto port = dynamic_cast<PortLabel*>(item))
            return port;
    return nullptr;
}

std::pair<PortLabel*, PortLabel*>
get_ports_for_connection(
    std::unordered_map<ConnectionItem*, std::pair<PortLabel*, PortLabel*>> connectionsMap,
    ConnectionItem* connection)
{
    if (auto it = connectionsMap.find(connection); it != connectionsMap.end())
        return it->second;
    return {nullptr, nullptr};
}
