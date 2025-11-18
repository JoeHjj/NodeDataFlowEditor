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

#include "factory/NodeFactory.hpp"
#include "model/NodeModel.hpp"
#include "utility/GraphRegistry.hpp"
#include "view/GraphScene.hpp"
#include "view/NodeItemViewAdapter.hpp"
#include "view/PortLabel.hpp"
#include <QApplication>
#include <QThread>
#include <QWidget>
#include <gtest/gtest.h>
class NodeFactoryTest : public ::testing::Test
{
public:
    template <typename T>
    struct ValueHolder
    {};

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

    static QApplication* app;
};

QApplication* NodeFactoryTest::app = nullptr;

TEST_F(NodeFactoryTest, CreateNodeAndPorts)
{
    auto scene = std::make_unique<GraphScene>();
    auto factory = scene->getNodeFactory();
    auto registry = scene->getGraphRegistry();
    // GIVEN a NodeFactory
    // WHEN creating a Node in the scene
    auto node = factory->createNode(scene.get(), "TestNode", "DisplayedNameOfTestNode", QColor(Qt::red), QPointF(10, 20));

    // THEN the node item should exist in the scene
    ASSERT_NE(node->item, nullptr);
    EXPECT_EQ(node->item->nodeName(), "TestNode");
    EXPECT_EQ(node->item->displayedNodeName(), "DisplayedNameOfTestNode");
    EXPECT_EQ(node->item->pos(), QPointF(10, 20));

    // WHEN adding input, output, and parameter ports
    factory->addInput(*node, "Input1", "displayedNameIn1");
    factory->addOutput(*node, "Output1", "displayedNameOut1");

    QWidget paramWidget;
    factory->addParameter(*node, &paramWidget, "Param1", "displayedNameParam1");

    QVector<PortLabel*> inputPorts;
    QVector<PortLabel*> outputPorts;
    QList<PortLabel*> parameterPorts;
    factory->getNodePorts(*node, inputPorts, outputPorts, parameterPorts);

    // THEN all ports should exist
    EXPECT_EQ(inputPorts.size(), 1);
    EXPECT_EQ(outputPorts.size(), 1);
    EXPECT_EQ(parameterPorts.size(), 1);

    EXPECT_EQ(inputPorts.front()->name(), "Input1");
    EXPECT_EQ(outputPorts.front()->name(), "Output1");
    EXPECT_EQ(parameterPorts.front()->name(), "Param1");

    EXPECT_EQ(inputPorts.front()->displayName(), "displayedNameIn1");
    EXPECT_EQ(outputPorts.front()->displayName(), "displayedNameOut1");
    EXPECT_EQ(parameterPorts.front()->displayName(), "displayedNameParam1");

    // WHEN removing ports
    factory->removeInput(*node, "Input1");
    factory->removeOutput(*node, "Output1");
    factory->removeParameter(*node, "Param1");

    factory->getNodePorts(*node, inputPorts, outputPorts, parameterPorts);

    // THEN ports should be removed
    EXPECT_TRUE(inputPorts.empty());
    EXPECT_TRUE(outputPorts.empty());
    EXPECT_TRUE(parameterPorts.empty());
}

TEST_F(NodeFactoryTest, AddTagsToInputPort)
{
    auto scene = std::make_unique<GraphScene>();
    auto factory = scene->getNodeFactory();
    auto registry = scene->getGraphRegistry();
    // GIVEN a node with an input port
    auto node = factory->createNode(scene.get(), "Node1", QColor(Qt::blue), QPointF(0, 0));
    factory->addInput(*node, "Input1");
    PortLabel* in1 = factory->getInputPortByName(*node, "Input1");

    // WHEN adding a ValueHolder<int> tag to the port
    in1->addTag<ValueHolder<int>>();

    // THEN the port should have the tag
    EXPECT_TRUE(in1->hasTag<ValueHolder<int>>());
}

TEST_F(NodeFactoryTest, AddTagsToOutputPort)
{
    auto scene = std::make_unique<GraphScene>();
    auto factory = scene->getNodeFactory();
    auto registry = scene->getGraphRegistry();
    // GIVEN a node with an output port
    auto node = factory->createNode(scene.get(), "Node2", QColor(Qt::green), QPointF(100, 0));
    factory->addOutput(*node, "Output1");
    PortLabel* out2 = factory->getOutputPortByName(*node, "Output1");

    // WHEN adding a ValueHolder<int> tag to the port
    out2->addTag<ValueHolder<int>>();

    // THEN the port should have the tag
    EXPECT_TRUE(out2->hasTag<ValueHolder<int>>());
}

TEST_F(NodeFactoryTest, CreateConnectionBetweenTaggedPorts)
{
    auto scene = std::make_unique<GraphScene>();
    auto factory = scene->getNodeFactory();
    auto registry = scene->getGraphRegistry();
    // GIVEN two nodes with tagged ports
    auto node1 = factory->createNode(scene.get(), "Node1", QColor(Qt::blue), QPointF(0, 0));
    auto node2 = factory->createNode(scene.get(), "Node2", QColor(Qt::green), QPointF(100, 0));

    factory->addInput(*node1, "Input1");
    factory->addOutput(*node2, "Output1");

    factory->addInputTag<ValueHolder<int>>(*node1, "Input1");
    factory->addOutputTag<ValueHolder<int>>(*node2, "Output1");

    // WHEN creating a connection
    PortLabel* in1 = factory->getInputPortByName(*node1, "Input1");
    PortLabel* out2 = factory->getOutputPortByName(*node2, "Output1");
    ConnectionItem* conn = factory->createConnectionBetweenPorts(in1, out2);

    // THEN the connection should exist
    ASSERT_NE(conn, nullptr);
    EXPECT_EQ(in1->name(), "Input1");
    EXPECT_EQ(in1->moduleName(), "Node1");
    EXPECT_EQ(out2->name(), "Output1");
    EXPECT_EQ(out2->moduleName(), "Node2");

    EXPECT_TRUE(registry->hasConnectionTo(*in1, *out2));
}

TEST_F(NodeFactoryTest, FailConnectionBetweenUntaggedPorts)
{
    auto scene = std::make_unique<GraphScene>();
    auto factory = scene->getNodeFactory();
    auto registry = scene->getGraphRegistry();
    // GIVEN two nodes with ports but without matching tags
    auto node1 = factory->createNode(scene.get(), "Node3", QColor(Qt::blue), QPointF(0, 0));
    auto node2 = factory->createNode(scene.get(), "Node4", QColor(Qt::green), QPointF(100, 0));

    factory->addInput(*node1, "Input1");
    factory->addOutput(*node2, "Output1");

    PortLabel* in1 = factory->getInputPortByName(*node1, "Input1");
    PortLabel* out2 = factory->getOutputPortByName(*node2, "Output1");

    // WHEN creating a connection between ports with mismatched types
    std::ignore = factory->createConnectionBetweenPorts(in1, out2);

    // THEN the connection should not exist
    EXPECT_FALSE(registry->hasConnectionTo(*in1, *out2));
}

TEST_F(NodeFactoryTest, FailConnectionBetweenDifferentTaggedPorts)
{
    auto scene = std::make_unique<GraphScene>();
    auto factory = scene->getNodeFactory();
    auto registry = scene->getGraphRegistry();
    // GIVEN two nodes with ports but without matching tags
    auto node1 = factory->createNode(scene.get(), "Node3", QColor(Qt::blue), QPointF(0, 0));
    auto node2 = factory->createNode(scene.get(), "Node4", QColor(Qt::green), QPointF(100, 0));

    factory->addInput(*node1, "Input1");
    factory->addOutput(*node2, "Output1");
    factory->addInputTag<ValueHolder<double>>(*node1, "Input1");
    factory->addOutputTag<ValueHolder<int>>(*node2, "Output1");
    PortLabel* in1 = factory->getInputPortByName(*node1, "Input1");
    PortLabel* out2 = factory->getOutputPortByName(*node2, "Output1");

    // WHEN creating a connection between ports with mismatched types
    std::ignore = factory->createConnectionBetweenPorts(in1, out2);

    // THEN the connection should not exist
    EXPECT_FALSE(registry->hasConnectionTo(*in1, *out2));
}

TEST_F(NodeFactoryTest, GetPortByName)
{
    auto scene = std::make_unique<GraphScene>();
    auto factory = scene->getNodeFactory();
    auto registry = scene->getGraphRegistry();
    // GIVEN a node with ports
    auto node = factory->createNode(scene.get(), "NodeX", QColor(Qt::yellow), QPointF(50, 50));
    factory->addInput(*node, "MyInput");
    factory->addOutput(*node, "MyOutput");

    // WHEN querying ports by name
    PortLabel* inputPort = factory->getInputPortByName(*node, "MyInput");
    PortLabel* outputPort = factory->getOutputPortByName(*node, "MyOutput");
    PortLabel* missingPort = factory->getInputPortByName(*node, "Nope");

    // THEN the correct ports are returned
    ASSERT_NE(inputPort, nullptr);
    EXPECT_EQ(inputPort->name(), "MyInput");

    ASSERT_NE(outputPort, nullptr);
    EXPECT_EQ(outputPort->name(), "MyOutput");

    EXPECT_EQ(missingPort, nullptr);
}

TEST_F(NodeFactoryTest, PortRegistrationUpdatesRegistry)
{
    auto scene = std::make_unique<GraphScene>();
    auto factory = scene->getNodeFactory();
    auto registry = scene->getGraphRegistry();

    auto node = factory->createNode(scene.get(), "NodeMVP", Qt::cyan, QPointF(10, 20));
    NodeItem* item = node->item;

    // Model: node is registered
    EXPECT_NE(registry->findNode("NodeMVP"), nullptr);

    // View: node is in the scene
    EXPECT_TRUE(scene->items().contains(item));

    // Node position reflects creation
    EXPECT_EQ(item->pos(), QPointF(10, 20));
}

TEST_F(NodeFactoryTest, changesFromModelRecievedInView)
{
    auto scene = std::make_unique<GraphScene>();
    auto factory = scene->getNodeFactory();
    auto registry = scene->getGraphRegistry();

    auto node = factory->createNode(scene.get(), "NodeMVP", Qt::cyan, QPointF(10, 20));

    node->model->setActive(true);
    EXPECT_TRUE(node->adapter->active());
}
