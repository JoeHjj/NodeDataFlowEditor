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
#include <QList>
#include <QRandomGenerator>
#include <functional>
#include <unordered_map>

class GraphRegistry;
class ConnectionItem;
class GraphScene;
class GroupItem;
class NodeItem;
class PortLabel;

/**
 * @brief Generate a random fully saturated, bright glowing color.
 */
QColor generate_glowing_color();

/**
 * @brief Retrieve all selected NodeItem instances from a list of graphics items.
 */
QList<NodeItem*> get_selected_nodes(const QList<QGraphicsItem*>& items);

/**
 * @brief Retrieve all selected GroupItem instances from a list of graphics items.
 */
QList<GroupItem*> get_selected_groups(const QList<QGraphicsItem*>& items);

/**
 * @brief Compute the bounding rectangle that encloses all given nodes.
 */
QRectF calculate_group_rect(const QList<NodeItem*>& nodes);

/**
 * @brief Apply a function to every selected NodeItem in the scene.
 */
void for_each_selected_node(const GraphScene* scene, const std::function<void(NodeItem*)>& fn);

/**
 * @brief Apply a function to every selected ConnectionItem in the scene.
 */
void for_each_selected_connection(const GraphScene* scene, const std::function<void(ConnectionItem*)>& fn);

/**
 * @brief Apply a function to every selected GroupItem in the scene.
 */
void for_each_selected_group(const GraphScene* scene, const std::function<void(GroupItem*)>& fn);

/**
 * @brief Safely remove and delete a temporary connection.
 */
void remove_temp_connection(GraphScene* scene, ConnectionItem*& tempConnection);

/**
 * @brief Find a PortLabel at a given scene position.
 */
PortLabel* port_at_position(const QGraphicsScene& scene, const QPointF& pos);

/**
 * @brief Retrieve both ports associated with a given connection.
 */
std::pair<PortLabel*, PortLabel*> get_ports_for_connection(
    std::unordered_map<ConnectionItem*, std::pair<PortLabel*, PortLabel*>> connectionsMap,
    ConnectionItem* connection);

/**
 * @brief Remove elements from a container if they match a predicate.
 */
template <typename Container, typename Predicate>
void
erase_if(Container& container, Predicate pred)
{
    for (auto it = container.begin(); it != container.end();)
    {
        if (pred(*it))
            it = container.erase(it);
        else
            ++it;
    }
}
