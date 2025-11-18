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

#include "view/NodeItemViewAdapter.hpp"
#include "utility/GraphRegistry.hpp"
#include "view/NodeItem.hpp"
#include "view/PortLabel.hpp"

#include <QGraphicsScene>

NodeItemViewAdapter::NodeItemViewAdapter(std::shared_ptr<GraphRegistry> registry, NodeItem* item, QObject* parent)
    : INodeView(parent)
    , m_registry(registry)
    , m_item(item)
{
    wireSignals();
    wireSceneSelection();
}

void
NodeItemViewAdapter::wireSignals()
{
    connect(m_item, &NodeItem::sgnItemMoved, this, &INodeView::sgnItemMoved);

    connect(m_item, &NodeItem::sgnPortMouseClicked, this,
            [this](NodeItem*, PortLabel* p) { emit sgnPortMouseClicked(this, p); });
    connect(m_item, &NodeItem::sgnPortMouseReleased, this,
            [this](NodeItem*, PortLabel* p) { emit sgnPortMouseReleased(this, p); });
    connect(m_item, &NodeItem::sgnPortMouseEntered, this,
            [this](NodeItem*, PortLabel* p) { emit sgnPortMouseEntered(this, p); });
    connect(m_item, &NodeItem::sgnPortMouseLeaved, this,
            [this](NodeItem*, PortLabel* p) { emit sgnPortMouseLeaved(this, p); });
}

void
NodeItemViewAdapter::wireSceneSelection()
{
    if (auto const* sc = m_item->scene())
    {
        connect(sc, &QGraphicsScene::selectionChanged, this, [this] {
            emit sgnSelectedChanged(m_item->isSelected());
        });
    }
}

void
NodeItemViewAdapter::setDisplayedNodeName(const QString& t)
{
    m_item->setDisplayedNodeName(t);
}

void
NodeItemViewAdapter::setTitleColor(const QColor& c)
{
    m_item->setNodeNameColor(c);
}

void
NodeItemViewAdapter::setActive(bool on)
{
    if (on)
        m_registry->activateNode(m_item);
    else
        m_registry->deactivateNode(m_item);
}

bool
NodeItemViewAdapter::active()
{
    return m_registry->isNodeActive(m_item);
}

void
NodeItemViewAdapter::setVisibleNode(bool on)
{
    m_item->changeVisibility(on);
}

QPointF
NodeItemViewAdapter::position() const
{
    return m_item->pos();
}

void
NodeItemViewAdapter::setPosition(const QPointF& p)
{
    if (m_item->pos() != p)
        m_item->setPos(p);
}

PortLabel*
NodeItemViewAdapter::addInput(const QString& n)
{
    return m_item->addInput(n);
}

PortLabel*
NodeItemViewAdapter::addParam(QWidget* widget, const QString& n)
{
    return m_item->addParameter(widget, n);
}

PortLabel*
NodeItemViewAdapter::addOutput(const QString& n)
{
    return m_item->addOutput(n);
}

PortLabel*
NodeItemViewAdapter::addInput(const QString& n, const QString& displayName)
{
    return m_item->addInput(n, displayName);
}

PortLabel*
NodeItemViewAdapter::addParam(QWidget* widget, const QString& n, const QString& displayName)
{
    return m_item->addParameter(widget, n, displayName);
}

PortLabel*
NodeItemViewAdapter::addOutput(const QString& n, const QString& displayName)
{
    return m_item->addOutput(n, displayName);
}

void
NodeItemViewAdapter::removeInput(const QString& n)
{
    m_item->removeInput(n);
}

void
NodeItemViewAdapter::removeParamInput(const QString& n)
{
    m_item->removeParamInput(n);
}

void
NodeItemViewAdapter::removeOutput(const QString& n)
{
    m_item->removeOutput(n);
}

QVector<PortLabel*>
NodeItemViewAdapter::inputs() const
{
    return m_item->inputs();
}

QVector<PortLabel*>
NodeItemViewAdapter::outputs() const
{
    return m_item->outputs();
}

QList<PortLabel*>
NodeItemViewAdapter::paramsInputs() const
{
    return m_item->paramsInputs();
}

NodeItem*
NodeItemViewAdapter::item() const
{
    return m_item;
}

bool
NodeItemViewAdapter::isAGroupNode() const
{
    return m_item->isAGroupNode();
}
