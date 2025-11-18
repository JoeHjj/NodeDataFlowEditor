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

#include "view/NodeItem.hpp"
#include <QMap>
#include <QSet>

class ConnectionItem;
class GraphRegistry;

/**
 * @brief A composite node that groups multiple NodeItem instances.
 *
 * GroupItem hides the original nodes, exposes a merged set of ports/parameters,
 * forwards connections through its own ports, and keeps selection/movement in sync.
 * External connections to member nodes are re-bound to the group's ports while grouped,
 * and restored back to inner ports on ungroup.
 */
class GroupItem : public NodeItem
{
    Q_OBJECT
public:
    /**
     * @brief Create a new group around the given nodes and add it to the scene.
     * @param nodes List of nodes to include in the group. Null entries are ignored.
     * @param scene Scene to which the group will be added. Can be nullptr (caller adds later).
     *
     * The constructor hides member nodes and disables their movement. It mirrors their
     * ports/parameters on the group, rewires external connections to the group's ports,
     * and composes a title from member node names.
     */
    explicit GroupItem(std::shared_ptr<GraphRegistry> registry, const QList<NodeItem*>& nodes, QGraphicsScene* scene);

    /**
     * @brief Destructor.
     *
     * Disconnects signal forwarding and performs any local cleanup owned by the group.
     * Member nodes are not deleted here; they are only hidden/locked while grouped.
     */
    ~GroupItem() override;

    /**
     * @brief Break the group and restore original nodes to the scene.
     * @param scene Target scene to remove the group from (and where members reside).
     *
     * Rebinds any connections on the group's ports back to suitable inner ports,
     * reveals and unlocks member nodes, cleans null edges, and finally removes and deletes the group.
     */
    void ungroup(QGraphicsScene* scene);

    /**
     * @brief Get the set of member nodes contained in the group.
     * @return Set of NodeItem pointers.
     */
    QSet<NodeItem*> nodes();

    bool isAGroupNode() const override { return true; }

protected:
    /**
     * @brief Keep members in sync with the group's movement and selection.
     *
     * - Propagates ItemPositionChange: moves member nodes by the same delta.
     * - Propagates ItemSelectedHasChanged: mirrors selection state to members.
     */
    QVariant itemChange(GraphicsItemChange change, const QVariant& value) override;

private:
    // Member nodes currently contained in the group.
    QSet<NodeItem*> m_nodes;

    /**
     * @brief Build a short, stable group title from member node titles.
     */
    QString buildTitle();

    /**
     * @brief Expose a merged set of input/output ports on the group based on inner ports.
     *
     * Buckets by port name, creates one group port per name, and wires connection forwarding.
     */
    void mirrorPorts();

    /**
     * @brief Expose representative parameter widgets on the group and broadcast their changes.
     *
     * Attempts to clone a representative widget per parameter name and connect it so updates
     * are propagated to all inner widgets with the same name.
     */
    void mirrorParams();

protected slots:
    /**
     * @brief Forward a group port click to the Scene so it behaves like a normal node port.
     */
    void onGroupPortMouseClicked(NodeItem*, PortLabel* port);

    /**
     * @brief Forward a group port mouse release to the Scene.
     */
    void onGroupPortMouseReleased(NodeItem*, PortLabel* port);
};
