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

#include "presenter/NodePresenter.hpp"
#include "model/NodeModel.hpp"
#include "view/INodeView.hpp"
#include "view/PortLabel.hpp"

#include <QPointF>

NodePresenter::NodePresenter(NodeModel* model, INodeView* view, QObject* parent)
    : QObject(parent)
    , m_model(model)
    , m_view(view)
{
    if (!m_model || !m_view)
        return;

    connectModelToView();
    connectViewToModel();

    if (m_model->position() != QPointF{})
    {
        m_view->setPosition(m_model->position());
    }
    else
    {
        m_model->setPosition(m_view->position());
    }

    ensurePortsMatchModel();
}

void
NodePresenter::connectModelToView()
{
    connect(m_model, &NodeModel::titleChanged, m_view, &INodeView::setDisplayedNodeName);
    connect(m_model, &NodeModel::titleColorChanged, m_view, &INodeView::setTitleColor);
    connect(m_model, &NodeModel::activeChanged, m_view, &INodeView::setActive);
    connect(m_model, &NodeModel::visibilityChanged, m_view, &INodeView::setVisibleNode);
    connect(m_model, &NodeModel::positionChanged, m_view, &INodeView::setPosition);

    connect(m_model, &NodeModel::portAdded, this, [this](const PortSpec& s) {
        if (!m_view)
            return;
        switch (s.kind)
        {
            case PortSpec::Kind::Input:
            {
                m_view->addInput(s.name, s.displayName);
                break;
            }
            case PortSpec::Kind::Output:
            {
                m_view->addOutput(s.name, s.displayName);
                break;
            }
            case PortSpec::Kind::Param:
                break;
        }
    });

    connect(m_model, &NodeModel::paramAdded, this, [this](const ParamSpec& s) {
        if (!m_view)
            return;
        switch (s.kind)
        {
            case PortSpec::Kind::Param:
                m_view->addParam(s.widget, s.name, s.displayName);
                break;
            case PortSpec::Kind::Input:
            case PortSpec::Kind::Output:
                break;
        }
    });

    connect(m_model, &NodeModel::portRemoved, this, [this](const PortSpec& s) {
        if (!m_view)
            return;
        switch (s.kind)
        {
            case PortSpec::Kind::Input:
                m_view->removeInput(s.name);
                break;
            case PortSpec::Kind::Output:
                m_view->removeOutput(s.name);
                break;
            case PortSpec::Kind::Param:
                break;
        }
    });

    connect(m_model, &NodeModel::paramRemoved, this, [this](const ParamSpec& s) {
        if (!m_view)
            return;
        switch (s.kind)
        {
            case PortSpec::Kind::Param:
                m_view->removeParamInput(s.name);
                break;
            case PortSpec::Kind::Input:
            case PortSpec::Kind::Output:
                break;
        }
    });
}

void
NodePresenter::connectViewToModel()
{
    connect(m_view, &INodeView::sgnItemMoved, this, [this] {
        if (!m_model || !m_view)
            return;
        m_model->setPosition(m_view->position()); // Persist position to model
        emit nodeMoved();
    });

    connect(m_view, &INodeView::sgnSelectedChanged, this, [this](bool sel) {
        emit selectionChanged(sel);
    });

    connect(m_view, &INodeView::sgnPortMouseClicked, this,
            [this](INodeView*, PortLabel* p) { emit portClicked(p); /* start connection */ });
    connect(m_view, &INodeView::sgnPortMouseReleased, this,
            [this](INodeView*, PortLabel* p) { emit portReleased(p); /* finalize connection */ });
    connect(m_view, &INodeView::sgnPortMouseEntered, this,
            [this](INodeView*, PortLabel* p) { emit portHoverEntered(p); });
    connect(m_view, &INodeView::sgnPortMouseLeaved, this,
            [this](INodeView*, PortLabel* p) { emit portHoverLeaved(p); });
}

QSet<QString>
NodePresenter::namesOf(const QVector<PortLabel*>& ports)
{
    QSet<QString> out;
    out.reserve(ports.size());
    for (auto* p : ports)
        if (p)
            out.insert(p->name());
    return out;
}

void
NodePresenter::addPortsMissingInView() const
{
    if (!m_model || !m_view)
        return;

    const auto viewInputs = namesOf(m_view->inputs());
    const auto viewOutputs = namesOf(m_view->outputs());
    const auto viewParams = namesOf(QVector<PortLabel*>::fromList(m_view->paramsInputs()));

    for (const auto& s : m_model->ports())
    {
        switch (s.kind)
        {
            case PortSpec::Kind::Input:
                if (!viewInputs.contains(s.name))
                    m_view->addInput(s.name);
                break;
            case PortSpec::Kind::Output:
                if (!viewOutputs.contains(s.name))
                    m_view->addOutput(s.name);
                break;
            case PortSpec::Kind::Param:
                break;
        }
    }
    for (const auto& s : m_model->params())
    {
        switch (s.kind)
        {
            case PortSpec::Kind::Param:
                if (!viewParams.contains(s.name))
                    m_view->addParam(s.widget, s.name);
                break;
            case PortSpec::Kind::Input:
            case PortSpec::Kind::Output:
                break;
        }
    }
}

void
NodePresenter::removePortsStrayInView() const
{
    if (!m_model || !m_view)
        return;

    QSet<QString> modelInputs, modelOutputs, modelParams;
    for (const auto& s : m_model->ports())
    {
        switch (s.kind)
        {
            case PortSpec::Kind::Input:
                modelInputs.insert(s.name);
                break;
            case PortSpec::Kind::Output:
                modelOutputs.insert(s.name);
                break;
            case PortSpec::Kind::Param:
                modelParams.insert(s.name);
                break;
        }
    }

    for (auto* p : m_view->inputs())
        if (p && !modelInputs.contains(p->name()))
            m_view->removeInput(p->name());
    for (auto* p : m_view->outputs())
        if (p && !modelOutputs.contains(p->name()))
            m_view->removeOutput(p->name());
    for (auto* p : m_view->paramsInputs())
        if (p && !modelParams.contains(p->name()))
            m_view->removeParamInput(p->name());
}

void
NodePresenter::ensurePortsMatchModel() const
{
    if (!m_model || !m_view)
        return;
    addPortsMissingInView();
    removePortsStrayInView();
}
