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

#include "model/NodeModel.hpp"

NodeModel::NodeModel(QObject* parent)
    : QObject(parent)
{}

void
NodeModel::setDisplayedNodeName(const QString& t)
{
    if (m_displayedNodeName == t)
        return;
    m_displayedNodeName = t;
    emit titleChanged(m_displayedNodeName);
}

void
NodeModel::setNodeName(const QString& t)
{
    if (m_nodeName == t)
        return;
    m_nodeName = t;
}

void
NodeModel::setTitleColor(const QColor& c)
{
    if (m_titleColor == c)
        return;
    m_titleColor = c;
    emit titleColorChanged(m_titleColor);
}

void
NodeModel::setActive(bool on)
{
    if (m_active == on)
        return;
    m_active = on;
    emit activeChanged(m_active);
}

void
NodeModel::setVisible(bool on)
{
    if (m_visible == on)
        return;
    m_visible = on;
    emit visibilityChanged(m_visible);
}

void
NodeModel::setPosition(const QPointF& p)
{
    if (m_position == p)
        return;
    m_position = p;
    emit positionChanged(m_position);
}

void
NodeModel::addPort(const QString& name, const QString& displayName, PortSpec::Kind kind)
{
    PortSpec ps{name, displayName, kind};
    m_ports.push_back(ps);
    emit portAdded(ps);
}

void
NodeModel::addParam(const QString& name, const QString& displayName, QWidget* widget, PortSpec::Kind kind)
{
    ParamSpec ps{{name, displayName, kind}, widget};
    m_params.push_back(ps);
    emit paramAdded(ps);
}

void
NodeModel::removePort(const QString& name, PortSpec::Kind kind)
{
    for (int i = 0; i < m_ports.size(); ++i)
    {
        if (m_ports[i].name == name && m_ports[i].kind == kind)
        {
            PortSpec removed = m_ports.takeAt(i);
            emit portRemoved(removed);
            return;
        }
    }
}

void
NodeModel::removeParam(const QString& name, PortSpec::Kind kind)
{
    for (int i = 0; i < m_params.size(); ++i)
    {
        if (m_params[i].name == name && m_params[i].kind == kind)
        {
            ParamSpec removed = m_params.takeAt(i);
            emit paramRemoved(removed);
            return;
        }
    }
}
