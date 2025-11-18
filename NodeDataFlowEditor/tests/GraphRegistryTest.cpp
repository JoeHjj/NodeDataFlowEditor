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

#include <QApplication>
#include <QGraphicsScene>
#include <QWidget>
#include <gtest/gtest.h>

#include "factory/NodeFactory.hpp"
#include "utility/GraphRegistry.hpp"
#include "utility/GroupDescriptor.hpp"
#include "utility/NodeDescriptor.hpp"
#include "view/ConnectionItem.hpp"
#include "view/GraphScene.hpp"
#include "view/GroupItem.hpp"
#include "view/NodeItem.hpp"
#include "view/PortLabel.hpp"

#include <memory>

// -----------------------------------------------------------------------------
// Fixture
// -----------------------------------------------------------------------------
class GraphRegistryTest : public ::testing::Test
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

    void unregisterNode(GraphRegistry* registry, NodeItem* item)
    {
        registry->unregisterNode(item);
    }

    void unregisterGroup(GraphRegistry* registry, GroupItem* item)
    {
        registry->unregisterGroup(item);
    }

    void unregisterInput(GraphRegistry* registry, NodeItem* item, PortLabel* in)
    {
        registry->unregisterInput(item, in);
    }

    void unregisterOutput(GraphRegistry* registry, NodeItem* item, PortLabel* out)
    {
        registry->unregisterOutput(item, out);
    }

    void unregisterParameterInput(GraphRegistry* registry, NodeItem* item, PortLabel* param)
    {
        registry->unregisterParameter(item, param);
    }

    void unregisterConnection(GraphRegistry* registry, ConnectionItem* conn)
    {
        registry->unregisterConnection(conn);
    }

    void addNodeToGroup(GraphRegistry* registry, GroupItem* group, NodeItem* node)
    {
        registry->addNodeToGroup(group, node);
    }

    void removeNodeFromGroup(GraphRegistry* registry, GroupItem* group, NodeItem* node)
    {
        registry->removeNodeFromGroup(group, node);
    }

    void nodeMoved(GraphRegistry* registry, NodeItem* node)
    {
        registry->nodeMoved(node);
    }

    static QApplication* app;
};

QApplication* GraphRegistryTest::app = nullptr;

// Helper: create node via factory
static std::unique_ptr<NodeFactory::Node>
makeNode(NodeFactory* factory,
         GraphScene* scene,
         const QString& title,
         const QColor& color = QColor(Qt::gray),
         const QPointF& pos = {})
{
    return factory->createNode(scene, title, color, pos);
}

// -----------------------------------------------------------------------------
// 1. Node registration
// -----------------------------------------------------------------------------
TEST_F(GraphRegistryTest, RegisterNode)
{
    auto scene = std::make_unique<GraphScene>();
    auto factory = scene->getNodeFactory();
    auto registry = scene->getGraphRegistry();

    auto node = makeNode(factory.get(), scene.get(), "NodeReg", Qt::red, QPointF(1, 2));
    NodeItem* item = node->item;

    ASSERT_NE(registry->findNode(item->nodeName()), nullptr);
    EXPECT_NE(registry->getNode(item), nullptr);
}

TEST_F(GraphRegistryTest, UnregisterNode)
{
    auto scene = std::make_unique<GraphScene>();
    auto factory = scene->getNodeFactory();
    auto registry = scene->getGraphRegistry();

    auto node = makeNode(factory.get(), scene.get(), "ToRemove");
    NodeItem* item = node->item;
    ASSERT_NE(registry->findNode(item->nodeName()), nullptr);

    unregisterNode(registry.get(), item);
    EXPECT_EQ(registry->findNode(item->nodeName()), nullptr);
    EXPECT_EQ(registry->getNode(item), nullptr);
}

TEST_F(GraphRegistryTest, GetNodeAndFindNode)
{
    auto scene = std::make_unique<GraphScene>();
    auto factory = scene->getNodeFactory();
    auto registry = scene->getGraphRegistry();

    auto node = makeNode(factory.get(), scene.get(), "Finder");
    NodeItem* item = node->item;

    auto descByGet = registry->getNode(item);
    auto descByFind = registry->findNode(item->nodeName());

    ASSERT_NE(descByGet, nullptr);
    ASSERT_NE(descByFind, nullptr);
    EXPECT_EQ(descByGet->node, item);
    EXPECT_EQ(descByFind->node, item);
}

// -----------------------------------------------------------------------------
// 2. Port registration
// -----------------------------------------------------------------------------
TEST_F(GraphRegistryTest, RegisterAndResolvePorts)
{
    auto scene = std::make_unique<GraphScene>();
    auto factory = scene->getNodeFactory();
    auto registry = scene->getGraphRegistry();

    auto node = makeNode(factory.get(), scene.get(), "PortNode");
    factory->addInput(*node, "inA");
    factory->addOutput(*node, "outA");
    // proxy will delete the widget
    auto w = new QWidget();
    factory->addParameter(*node, w, "paramA");

    PortLabel* pIn = registry->resolvePort(node->item->nodeName(), "inA");
    PortLabel* pOut = registry->resolvePort(node->item->nodeName(), "outA");
    PortLabel* pParam = registry->resolvePort(node->item->nodeName(), "paramA");

    ASSERT_NE(pIn, nullptr);
    ASSERT_NE(pOut, nullptr);
    ASSERT_NE(pParam, nullptr);
}

TEST_F(GraphRegistryTest, UnregisterPorts)
{
    auto scene = std::make_unique<GraphScene>();
    auto factory = scene->getNodeFactory();
    auto registry = scene->getGraphRegistry();

    auto node = makeNode(factory.get(), scene.get(), "UnregPorts");
    factory->addInput(*node, "i1");
    factory->addOutput(*node, "o1");
    auto w = new QWidget();
    factory->addParameter(*node, w, "p1");

    PortLabel* pIn = registry->resolvePort(node->item->nodeName(), "i1");
    PortLabel* pOut = registry->resolvePort(node->item->nodeName(), "o1");
    PortLabel* pParam = registry->resolvePort(node->item->nodeName(), "p1");

    ASSERT_NE(pIn, nullptr);
    ASSERT_NE(pOut, nullptr);
    ASSERT_NE(pParam, nullptr);

    unregisterInput(registry.get(), node->item, pIn);
    unregisterOutput(registry.get(), node->item, pOut);
    unregisterParameterInput(registry.get(), node->item, pParam);

    EXPECT_EQ(registry->resolvePort(node->item->nodeName(), "i1"), nullptr);
    EXPECT_EQ(registry->resolvePort(node->item->nodeName(), "o1"), nullptr);
    EXPECT_EQ(registry->resolvePort(node->item->nodeName(), "p1"), nullptr);
}

// -----------------------------------------------------------------------------
// 3. Connections
// -----------------------------------------------------------------------------
TEST_F(GraphRegistryTest, RegisterAndQueryConnection)
{
    auto scene = std::make_unique<GraphScene>();
    auto factory = scene->getNodeFactory();
    auto registry = scene->getGraphRegistry();

    auto n1 = makeNode(factory.get(), scene.get(), "C1");
    auto n2 = makeNode(factory.get(), scene.get(), "C2");
    factory->addInput(*n1, "in1");
    factory->addOutput(*n2, "out1");

    PortLabel* in1 = factory->getInputPortByName(*n1, "in1");
    PortLabel* out1 = factory->getOutputPortByName(*n2, "out1");

    factory->addInputTag<ValueHolder<int>>(*n1, "in1");
    factory->addOutputTag<ValueHolder<int>>(*n2, "out1");
    ASSERT_NE(in1, nullptr);
    ASSERT_NE(out1, nullptr);

    ConnectionItem* conn = factory->createConnection(*scene, *in1, *out1, false);

    EXPECT_TRUE(registry->hasConnectionTo(*in1, *out1));
    auto conns = registry->getConnections(in1);
    EXPECT_TRUE(conns.contains(conn));

    ConnectionItem* found = registry->findConnection(*in1, out1->name(), out1->moduleName());
    EXPECT_EQ(found, conn);

    unregisterConnection(registry.get(), conn);
    delete conn;
}

TEST_F(GraphRegistryTest, UnregisterConnection)
{
    auto scene = std::make_unique<GraphScene>();
    auto factory = scene->getNodeFactory();
    auto registry = scene->getGraphRegistry();

    auto n1 = makeNode(factory.get(), scene.get(), "U1");
    auto n2 = makeNode(factory.get(), scene.get(), "U2");
    factory->addInput(*n1, "in");
    factory->addOutput(*n2, "out");
    PortLabel* in = factory->getInputPortByName(*n1, "in");
    PortLabel* out = factory->getOutputPortByName(*n2, "out");
    factory->addInputTag<ValueHolder<int>>(*n1, "in");
    factory->addOutputTag<ValueHolder<int>>(*n2, "out");
    ConnectionItem* conn = factory->createConnection(*scene, *in, *out, false);

    ASSERT_TRUE(registry->hasConnectionTo(*in, *out));

    unregisterConnection(registry.get(), conn);
    EXPECT_FALSE(registry->hasConnectionTo(*in, *out));

    delete conn;
}

// -----------------------------------------------------------------------------
// 4. Groups + forward ports
// -----------------------------------------------------------------------------
TEST_F(GraphRegistryTest, RegisterAndUnregisterGroup)
{
    auto scene = std::make_unique<GraphScene>();
    auto factory = scene->getNodeFactory();
    auto registry = scene->getGraphRegistry();

    auto n1 = makeNode(factory.get(), scene.get(), "GNode1");
    auto n2 = makeNode(factory.get(), scene.get(), "GNode2");
    factory->addInput(*n1, "i1");
    factory->addOutput(*n2, "o1");

    QList<NodeItem*> nodes{n1->item, n2->item};
    GroupItem* group = new GroupItem(registry, nodes, scene.get());

    auto gDesc = registry->findGroup(group->nodeName());
    ASSERT_NE(gDesc, nullptr);

    unregisterGroup(registry.get(), group);
    EXPECT_EQ(registry->findGroup(group->nodeName()), nullptr);

    delete group;
}

TEST_F(GraphRegistryTest, AddRemoveNodeFromGroupAndGroupsOf)
{
    auto scene = std::make_unique<GraphScene>();
    auto factory = scene->getNodeFactory();
    auto registry = scene->getGraphRegistry();

    auto n = makeNode(factory.get(), scene.get(), "GSingle");
    factory->addInput(*n, "ix");
    QList<NodeItem*> nodes{n->item};
    GroupItem* group = new GroupItem(registry, nodes, scene.get());

    auto extra = makeNode(factory.get(), scene.get(), "Extra");
    addNodeToGroup(registry.get(), group, extra->item);

    auto groups = registry->groupsOf(extra->item);
    EXPECT_FALSE(groups.empty());

    removeNodeFromGroup(registry.get(), group, extra->item);
    auto groupsAfter = registry->groupsOf(extra->item);
    EXPECT_TRUE(std::none_of(groupsAfter.begin(), groupsAfter.end(),
                             [&](auto* gd) { return gd && gd->group == group; }));

    delete group;
}

// -----------------------------------------------------------------------------
// 5. nodeMoved propagation
// -----------------------------------------------------------------------------
TEST_F(GraphRegistryTest, NodeMovedUpdatesConnectionPath)
{
    auto scene = std::make_unique<GraphScene>();
    auto factory = scene->getNodeFactory();
    auto registry = scene->getGraphRegistry();

    auto nOut = makeNode(factory.get(), scene.get(), "MoveOut");
    auto nIn = makeNode(factory.get(), scene.get(), "MoveIn");

    factory->addOutput(*nOut, "o");
    factory->addInput(*nIn, "i");
    factory->addOutputTag<ValueHolder<int>>(*nOut, "o");
    factory->addInputTag<ValueHolder<int>>(*nIn, "i");
    PortLabel* out = factory->getOutputPortByName(*nOut, "o");
    PortLabel* in = factory->getInputPortByName(*nIn, "i");

    ConnectionItem* conn = factory->createConnection(*scene, *in, *out, false);
    ASSERT_NE(conn, nullptr);

    QPainterPath beforePath = conn->path();
    QRectF beforeBounds = beforePath.boundingRect();

    nOut->item->setPos(nOut->item->pos() + QPointF(50, 30));
    nodeMoved(registry.get(), nOut->item);

    QPainterPath afterPath = conn->path();
    QRectF afterBounds = afterPath.boundingRect();

    EXPECT_TRUE(beforeBounds != afterBounds || beforePath.elementCount() != afterPath.elementCount());

    unregisterConnection(registry.get(), conn);
    delete conn;
}

TEST_F(GraphRegistryTest, NodeMovedForGroupPropagates)
{
    auto scene = std::make_unique<GraphScene>();
    auto factory = scene->getNodeFactory();
    auto registry = scene->getGraphRegistry();

    auto n1 = makeNode(factory.get(), scene.get(), "GMove1");
    auto n2 = makeNode(factory.get(), scene.get(), "GMove2");
    factory->addOutput(*n1, "outG");
    factory->addInput(*n2, "inG");

    factory->addOutputTag<ValueHolder<int>>(*n1, "outG");
    factory->addInputTag<ValueHolder<int>>(*n2, "inG");

    PortLabel* out = factory->getOutputPortByName(*n1, "outG");
    PortLabel* in = factory->getInputPortByName(*n2, "inG");

    ConnectionItem* conn = factory->createConnection(*scene, *in, *out, false);
    ASSERT_NE(conn, nullptr);

    QList<NodeItem*> nodes{n1->item, n2->item};
    GroupItem* group = new GroupItem(registry, nodes, scene.get());

    group->setPos(group->pos() + QPointF(20, 20));
    nodeMoved(registry.get(), group);

    SUCCEED();

    unregisterConnection(registry.get(), conn);
    delete conn;
    delete group;
}
