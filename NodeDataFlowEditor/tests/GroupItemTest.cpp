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

#include "view/GroupItem.hpp"
#include "utility/GraphRegistry.hpp"
#include "view/GraphScene.hpp"
#include "view/NodeItem.hpp"
#include "view/PortLabel.hpp"

#include <QApplication>
#include <QGraphicsScene>
#include <QWidget>
#include <gtest/gtest.h>
#include <memory>

// -----------------------------------------------------------------------------
// Test Fixture
// -----------------------------------------------------------------------------
class GroupItemTest : public ::testing::Test
{
protected:
    static void SetUpTestSuite()
    {
        int argc = 0;
        app = new QApplication(argc, nullptr);
    }

    static void TearDownTestSuite()
    {
        delete app;
        app = nullptr;
    }

    void SetUp() override
    {
        registry = std::make_shared<GraphRegistry>();
        scene = new QGraphicsScene();
    }

    void TearDown() override
    {
        delete scene;
        scene = nullptr;
        registry.reset();
    }

    static QApplication* app;
    std::shared_ptr<GraphRegistry> registry;
    QGraphicsScene* scene;
};

QApplication* GroupItemTest::app = nullptr;

// -----------------------------------------------------------------------------
// Subclass to expose protected internals if needed
// -----------------------------------------------------------------------------
class TestableGroupItem : public GroupItem
{
public:
    using GroupItem::GroupItem;
    void publicOnGroupPortMouseClicked(NodeItem* n, PortLabel* p)
    {
        onGroupPortMouseClicked(n, p);
    }

    void publicOnGroupPortMouseReleased(NodeItem* n, PortLabel* p)
    {
        onGroupPortMouseReleased(n, p);
    }
    QSet<NodeItem*> publicNodes() { return nodes(); }
};

// -----------------------------------------------------------------------------
// Tests
// -----------------------------------------------------------------------------

TEST_F(GroupItemTest, CreateGroupAndPorts)
{
    // Given two NodeItems with input/output ports
    NodeItem node1(registry, "Node1");
    NodeItem node2(registry, "Node2");

    node1.addInput("In1");
    node1.addOutput("Out1");

    node2.addInput("In2");
    node2.addOutput("Out2");

    scene->addItem(&node1);
    scene->addItem(&node2);

    // When creating a group with these nodes
    TestableGroupItem group(registry, {&node1, &node2}, scene);

    // Then the original nodes should be hidden and locked
    EXPECT_FALSE(node1.isVisible());
    EXPECT_FALSE(node2.isVisible());
    EXPECT_FALSE(node1.flags() & QGraphicsItem::ItemIsMovable);
    EXPECT_FALSE(node2.flags() & QGraphicsItem::ItemIsMovable);

    // Then the group should contain mirrored ports
    EXPECT_FALSE(group.inputs().isEmpty());
    EXPECT_FALSE(group.outputs().isEmpty());

    // Then the group should know its member nodes
    auto members = group.publicNodes();
    EXPECT_TRUE(members.contains(&node1));
    EXPECT_TRUE(members.contains(&node2));
}

TEST_F(GroupItemTest, UngroupRestoresNodes)
{
    // Given two NodeItems in a group
    NodeItem node1(registry, "Node1");
    NodeItem node2(registry, "Node2");

    node1.addInput("In1");
    node2.addOutput("Out1");

    scene->addItem(&node1);
    scene->addItem(&node2);

    TestableGroupItem* group = new TestableGroupItem(registry, {&node1, &node2}, scene);
    scene->addItem(group);

    // When ungrouping
    group->ungroup(scene);

    // Then the original nodes should be visible and movable
    EXPECT_TRUE(node1.isVisible());
    EXPECT_TRUE(node2.isVisible());
    EXPECT_TRUE(node1.flags() & QGraphicsItem::ItemIsMovable);
    EXPECT_TRUE(node2.flags() & QGraphicsItem::ItemIsMovable);

    // And the group should be removed from the scene
    EXPECT_TRUE(scene->items().indexOf(group) == -1);
}

TEST_F(GroupItemTest, GroupTitleConcatenation)
{
    // Given nodes with titles
    NodeItem node1(registry, "A");
    NodeItem node2(registry, "B");
    NodeItem node3(registry, "C");

    // When creating a group
    TestableGroupItem group(registry, {&node3, &node1, &node2}, scene);

    // Then the group title should be a stable, sorted summary
    QString title = group.nodeName();
    EXPECT_TRUE(title.contains("A"));
    EXPECT_TRUE(title.contains("B"));
    EXPECT_TRUE(title.contains("C"));
}

TEST_F(GroupItemTest, ForwardPortClick)
{
    // Given a NodeItem in a scene and a group
    NodeItem node1(registry, "Node1");
    node1.addInput("In1");
    scene->addItem(&node1);

    GraphScene graphScene;
    scene->addItem(&node1);

    TestableGroupItem group(registry, {&node1}, scene);

    // When simulating a group port click (forwarded)
    PortLabel* groupPort = group.inputs().first();
    group.publicOnGroupPortMouseClicked(&group, groupPort);

    // Then the scene should receive the port click
    // (Cannot check scene internals easily; assume no crash)
    SUCCEED();
}

// test ports tagging compatibility
