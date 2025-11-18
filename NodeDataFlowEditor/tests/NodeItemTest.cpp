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

#include "view/NodeItem.hpp"
#include "utility/GraphRegistry.hpp"
#include "view/PortLabel.hpp"

#include <QApplication>
#include <QEvent>
#include <QGraphicsScene>
#include <QGraphicsSceneHoverEvent>
#include <QGraphicsSceneMouseEvent>
#include <QWidget>

#include <gtest/gtest.h>

// ----------------------------------------------------------------------------
// Test Fixture
// ----------------------------------------------------------------------------
class NodeItemTest : public ::testing::Test
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

QApplication* NodeItemTest::app = nullptr;

// ----------------------------------------------------------------------------
// Subclass NodeItem to expose protected sceneEvent
// ----------------------------------------------------------------------------
class TestableNodeItem : public NodeItem
{
public:
    using NodeItem::NodeItem;
    bool publicSceneEvent(QEvent* event) { return sceneEvent(event); }

    // Helpers to check internal states
    bool isHovered() const { return m_hovered; }
};

// ----------------------------------------------------------------------------
// Tests
// ----------------------------------------------------------------------------

TEST_F(NodeItemTest, AddRemovePorts)
{
    // Given a NodeItem in a scene
    TestableNodeItem node(registry, "TestNode");
    scene->addItem(&node);

    // When adding input, output, and parameter ports
    PortLabel* input = node.addInput("Input1");
    PortLabel* output = node.addOutput("Output1");
    QWidget paramWidget;
    PortLabel* param = node.addParameter(&paramWidget, "Param1");

    // Then all ports should exist
    EXPECT_TRUE(node.inputs().contains(input));
    EXPECT_TRUE(node.outputs().contains(output));
    EXPECT_TRUE(node.paramsInputs().contains(param));

    // When removing ports by name
    node.removeInput("Input1");
    node.removeOutput("Output1");
    node.removeParamInput("Param1");

    // Then all ports should be removed
    EXPECT_FALSE(node.inputs().contains(input));
    EXPECT_FALSE(node.outputs().contains(output));
    EXPECT_FALSE(node.paramsInputs().contains(param));
}

TEST_F(NodeItemTest, SceneEventHover)
{
    // Given: a NodeItem added to a scene
    TestableNodeItem node(registry, "NodeEvent");
    scene->addItem(&node);

    // When: sending a hover enter event
    QGraphicsSceneHoverEvent hoverEnter(QEvent::GraphicsSceneHoverEnter);
    hoverEnter.setPos(node.boundingRect().center());
    node.publicSceneEvent(&hoverEnter);

    // Then: the node should be hovered
    EXPECT_TRUE(node.isHovered());

    // When: sending a hover leave event
    QGraphicsSceneHoverEvent hoverLeave(QEvent::GraphicsSceneHoverLeave);
    hoverLeave.setPos(node.boundingRect().center());
    node.publicSceneEvent(&hoverLeave);

    // Then: node is no longer hovered
    EXPECT_FALSE(node.isHovered());
}

TEST_F(NodeItemTest, SetTitle)
{
    // Given a NodeItem with initial title
    TestableNodeItem node(registry, "OldTitle", "", QColor(Qt::red));

    // When updating nodeName
    node.setNodeName("NewTitle");

    // Then nodeName should be updated
    EXPECT_EQ(node.nodeName(), "NewTitle");
}

TEST_F(NodeItemTest, SetDisplayName)
{
    // Given a NodeItem with initial title
    TestableNodeItem node(registry, "OldTitle", "other text", QColor(Qt::red));

    // When updating nodeName
    node.setNodeName("NewTitle");

    // Then nodeName should be updated
    EXPECT_EQ(node.nodeName(), "NewTitle");
    EXPECT_EQ(node.displayedNodeName(), "other text");
}

TEST_F(NodeItemTest, Visibility)
{
    // Given a NodeItem
    TestableNodeItem node(registry, "NodeVisibility");

    // When activating and changing visibility
    node.changeVisibility(true);

    // Then NodeItem should be visible
    EXPECT_TRUE(node.isVisible());

    // When hiding the node
    node.changeVisibility(false);

    // Then NodeItem should not be visible
    EXPECT_FALSE(node.isVisible());
}
