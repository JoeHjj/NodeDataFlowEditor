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

#include "view/ConnectionItem.hpp"

#include <QApplication>
#include <QGraphicsScene>
#include <QPainter>
#include <QStyleOptionGraphicsItem>
#include <gtest/gtest.h>

// Helper to create a default port
ConnectionPort
createPort(bool isInput, const QString& name = "port")
{
    ConnectionPort port;
    port.isInput = isInput;
    port.portName = name;
    port.scenePos = QPointF(0, 0);
    port.rect = QRectF(0, 0, 10, 10);
    return port;
}

class ConnectionItemTest : public ::testing::Test
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
        scene = new QGraphicsScene();
    }

    void TearDown() override
    {
        delete scene;
    }

    static QApplication* app;
    QGraphicsScene* scene;
};

QApplication* ConnectionItemTest::app = nullptr;

// --- TEST CASES ---

TEST_F(ConnectionItemTest, AddPortsAndUpdatePath)
{
    // Given: two ports (input and output)
    ConnectionPort inputPort = createPort(true, "in1");
    ConnectionPort outputPort = createPort(false, "out1");

    // When: a ConnectionItem is created and ports are added
    ConnectionItem item(inputPort);
    item.addPort(outputPort);

    // Then: the item should have correct input/output ports
    EXPECT_EQ(item.inputPort().portName, "in1");
    EXPECT_EQ(item.outputPort().portName, "out1");

    // When: set the item as active and compatible, and move nodes
    item.setIsActive(true);
    item.setIsCompatible(true);
    item.onNodeMoved(true, QPointF(10, 10), QRectF(0, 0, 10, 10));
    item.onNodeMoved(false, QPointF(100, 50), QRectF(0, 0, 10, 10));
    item.updateEndPoint(QPointF(200, 200));

    // Then: the item should be activated and path should not be empty
    EXPECT_TRUE(item.isActivated());
    EXPECT_FALSE(item.path().isEmpty());
}
