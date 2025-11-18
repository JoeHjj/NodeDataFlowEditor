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

#include "view/PortLabel.hpp"
#include <QApplication>
#include <QEvent>
#include <QGraphicsScene>
#include <QGraphicsSceneMouseEvent>
#include <gtest/gtest.h>

class PortLabelTest : public ::testing::Test
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
        scene = nullptr;
    }

    QGraphicsScene* scene;
    static QApplication* app;
};

QApplication* PortLabelTest::app = nullptr;

class TestablePortLabel : public PortLabel
{
public:
    using PortLabel::PortLabel;
    bool publicSceneEvent(QEvent* event) { return sceneEvent(event); }
};

TEST_F(PortLabelTest, HoverEnterLeave)
{
    // Given a TestablePortLabel in a scene
    TestablePortLabel label("Port", "Module", PortLabel::Orientation::Input);
    scene->addItem(&label);

    // When a hover enter event is sent
    QEvent hoverEnter(QEvent::GraphicsSceneHoverEnter);
    bool enterResult = label.publicSceneEvent(&hoverEnter);

    // Then the event is processed successfully
    EXPECT_TRUE(enterResult);

    // When a hover leave event is sent
    QEvent hoverLeave(QEvent::GraphicsSceneHoverLeave);
    bool leaveResult = label.publicSceneEvent(&hoverLeave);

    // Then the event is processed successfully
    EXPECT_TRUE(leaveResult);
}

TEST_F(PortLabelTest, MousePressRelease)
{
    // Given a TestablePortLabel in a scene
    TestablePortLabel label("Port", "Module", PortLabel::Orientation::Input);
    scene->addItem(&label);

    // When a mouse press event is sent
    QGraphicsSceneMouseEvent press(QEvent::GraphicsSceneMousePress);
    bool pressResult = label.publicSceneEvent(&press);

    // Then the event is processed successfully
    EXPECT_TRUE(pressResult);

    // When a mouse release event is sent
    QGraphicsSceneMouseEvent release(QEvent::GraphicsSceneMouseRelease);
    bool releaseResult = label.publicSceneEvent(&release);

    // Then the event is processed successfully
    EXPECT_TRUE(releaseResult);
}
