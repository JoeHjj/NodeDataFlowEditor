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
#include "utility/NodeHelper.hpp"
#include "view/ConnectionItem.hpp"
#include "view/EditableLabelItem.hpp"
#include "view/PortLabel.hpp"

#include <QDebug>
#include <QGraphicsProxyWidget>
#include <QLinearGradient>
#include <QPainter>
#include <algorithm>
#include <cmath>

namespace
{
    QSize totalSizeWithSpacing(const QList<QGraphicsProxyWidget*>& widgets, int spacing = 5)
    {
        int totalWidth = 0;
        int totalHeight = 0;

        for (auto const* widget : widgets)
        {
            if (widget)
            {
                QSize size = widget->size().toSize();
                totalWidth = std::max(totalWidth, size.width());
                totalHeight += size.height();
            }
        }

        if (!widgets.isEmpty())
            totalHeight += spacing * (widgets.size() - 1);

        return {totalWidth, totalHeight};
    }
} // namespace

NodeItem::NodeItem(std::shared_ptr<GraphRegistry> registry,
                   QString nodeName,
                   QString nodeDisplayedName,
                   QColor titleColor,
                   QGraphicsItem* parent)
    : QGraphicsObject(parent)
    , m_nodeName(std::move(nodeName))
    , m_displayedNodeName(nodeDisplayedName.isEmpty() ? m_nodeName : nodeDisplayedName)
    , m_nodeNameColor(titleColor)
    , m_registry(std::move(registry))
{
    m_registry->registerNode(this);

    m_nodeNameLabel = new EditableLabelItem(m_displayedNodeName, this);
    m_titleHeight = m_nodeNameLabel->boundingRect().height();

    connect(m_nodeNameLabel, &EditableLabelItem::textChanged, this, [this](const QString& t) {
        setDisplayedNodeName(t);
        emit sgnDisplayedNameChanged(this, t);
    });

    setFlags(ItemIsMovable | ItemIsSelectable);
    setFlag(ItemSendsGeometryChanges, true);
    setAcceptHoverEvents(true);

    updateLayout();
}

NodeItem::NodeItem(std::shared_ptr<GraphRegistry> registry,
                   QString nodeName,
                   QString nodeDisplayedName,
                   QGraphicsItem* parent)
    : NodeItem(registry, nodeName, nodeDisplayedName, generate_glowing_color(), parent)
{
}

NodeItem::~NodeItem()
{
    try
    {
        disconnectAllPorts();
    }
    catch (...)
    {
        qWarning() << "Exception in NodeItem destructor";
    }
    m_registry->unregisterNode(this);
}

PortLabel*
NodeItem::addInput(const QString& name)
{
    auto* input = new PortLabel(name, m_nodeName, PortLabel::Orientation::Input, this);
    connectPorts(input);
    m_inputs.append(input);
    updateLayout();
    m_registry->registerInput(this, input);
    return input;
}

PortLabel*
NodeItem::addOutput(const QString& name)
{
    auto* output = new PortLabel(name, m_nodeName, PortLabel::Orientation::Output, this);
    connectPorts(output);
    m_outputs.append(output);
    updateLayout();
    m_registry->registerOutput(this, output);
    return output;
}

PortLabel*
NodeItem::addParamInput(const QString& name)
{
    auto* input = new PortLabel(name, m_nodeName, PortLabel::Orientation::Parameter, this);
    connectPorts(input);
    updateLayout();
    m_registry->registerParameter(this, input);
    return input;
}

PortLabel*
NodeItem::addInput(const QString& name, const QString& displayName)
{
    auto port = addInput(name);
    if (!port)
        return nullptr;
    port->setDisplayName(displayName);
    return port;
}

PortLabel*
NodeItem::addOutput(const QString& name, const QString& displayName)
{
    auto port = addOutput(name);
    if (!port)
        return nullptr;
    port->setDisplayName(displayName);
    return port;
}

PortLabel*
NodeItem::addParameter(QWidget* widget, const QString& name)
{
    if (!widget)
        return nullptr;

    auto* proxy = new QGraphicsProxyWidget(this);
    proxy->setWidget(widget);
    m_parameterWidgets.insert(widget, proxy);

    auto* port = addParamInput(name);
    m_parameterPorts.insert(port, proxy);

    updateLayout();
    return port;
}

PortLabel*
NodeItem::addParameter(QWidget* widget, const QString& name, const QString& displayName)
{
    auto port = addParameter(widget, name);
    if (!port)
        return nullptr;
    port->setDisplayName(displayName);
    return port;
}

void
NodeItem::removeInput(PortLabel* input)
{
    if (!m_inputs.contains(input))
        return;

    disconnectPorts(input);
    m_inputs.removeOne(input);
    input->deleteLater();
    m_registry->unregisterInput(this, input);
    updateLayout();
}

void
NodeItem::removeOutput(PortLabel* output)
{
    if (!m_outputs.contains(output))
        return;

    disconnectPorts(output);
    m_outputs.removeOne(output);
    output->deleteLater();
    m_registry->unregisterOutput(this, output);
    updateLayout();
}

void
NodeItem::removeParamInput(PortLabel* input)
{
    if (!m_parameterPorts.contains(input))
        return;

    auto* proxy = m_parameterPorts.value(input);
    auto* widget = m_parameterWidgets.key(proxy);

    disconnectPorts(input);
    m_parameterPorts.remove(input);
    m_parameterWidgets.remove(widget);

    input->deleteLater();
    proxy->deleteLater();
    m_registry->unregisterParameter(this, input);

    updateLayout();
}

void
NodeItem::disconnectAllPorts()
{
    for (auto* input : m_inputs)
        if (input)
        {
            disconnectPorts(input);
            input->deleteLater();
        }
    m_inputs.clear();

    for (auto* output : m_outputs)
        if (output)
        {
            disconnectPorts(output);
            output->deleteLater();
        }
    m_outputs.clear();

    for (auto* port : m_parameterPorts.keys())
    {
        auto* proxy = m_parameterPorts.value(port);
        disconnectPorts(port);
        port->deleteLater();
        proxy->deleteLater();
    }
    m_parameterPorts.clear();
    m_parameterWidgets.clear();
}

QVector<PortLabel*>
NodeItem::getAllPorts() const
{
    QVector<PortLabel*> allPorts;
    allPorts.reserve(m_inputs.size() + m_outputs.size() + m_parameterPorts.size());
    allPorts += m_inputs;
    allPorts += m_outputs;
    allPorts += m_parameterPorts.keys().toVector();
    return allPorts;
}

void
NodeItem::updateLayout()
{
    updateRect();

    if (m_nodeNameLabel)
    {
        QRectF lbl = m_nodeNameLabel->boundingRect();
        m_nodeNameLabel->setPos(
            (m_rect.width() - lbl.width()) / 2.0,
            (m_titleHeight - lbl.height()) / 2.0);
    }

    // input port column
    m_maxInputWidth = 0;
    for (auto* port : m_inputs)
        m_maxInputWidth = std::max(m_maxInputWidth, port->boundingRect().width());

    // output port column
    m_maxOutputWidth = 0;
    for (auto* port : m_outputs)
        m_maxOutputWidth = std::max(m_maxOutputWidth, port->boundingRect().width());

    // parameter column (label+widget)
    m_maxParamWidth = 0;

    // labels
    for (auto* port : m_parameterPorts.keys())
        m_maxParamWidth = std::max(m_maxParamWidth, port->boundingRect().width());

    // widgets
    for (auto* proxy : m_parameterPorts)
        m_maxParamWidth = std::max(m_maxParamWidth, proxy->boundingRect().width());

    qreal width =
        m_margin +
        m_maxInputWidth +
        m_spacing +
        m_maxParamWidth +
        m_spacing +
        m_maxOutputWidth +
        m_margin;

    width = std::max(width, static_cast<qreal>(minWidth));

    qreal yInput = m_titleHeight + m_margin;

    for (auto* port : m_inputs)
    {
        port->setPos(m_margin, yInput);
        yInput += port->boundingRect().height() + m_spacing;
    }

    qreal yOutput = m_titleHeight + m_margin;

    for (auto* port : m_outputs)
    {
        port->setPos(width - m_margin - port->boundingRect().width(), yOutput);
        yOutput += port->boundingRect().height() + m_spacing;
    }

    qreal paramX = m_margin + m_maxInputWidth + m_spacing;

    qreal yParam = m_titleHeight + m_margin;

    for (auto it = m_parameterPorts.constBegin(); it != m_parameterPorts.constEnd(); ++it)
    {
        PortLabel* label = it.key();
        QGraphicsProxyWidget* widget = it.value();

        label->setPos(paramX, yParam);

        widget->setPos(paramX, yParam + label->boundingRect().height());

        yParam += label->boundingRect().height() + widget->boundingRect().height() + m_spacing;
    }

    qreal height = std::max({yInput, yOutput, yParam}) + m_margin;
    m_rect = QRectF(0, 0, width, height);

    if (m_nodeNameLabel)
    {
        QRectF lbl = m_nodeNameLabel->boundingRect();
        m_nodeNameLabel->setPos(
            (width - lbl.width()) / 2.0,
            (m_titleHeight - lbl.height()) / 2.0);
    }

    prepareGeometryChange();
    update();
}

void
NodeItem::updateRect()
{
    qreal maxInputWidth = 0;
    int maxInputHeight = 0;
    int maxOutputHeight = 0;
    int maxParamHeight = 0;

    for (auto* in : m_inputs)
        if (in)
        {
            maxInputWidth = std::fmax(maxInputWidth, in->boundingRect().width());
            maxInputHeight += in->boundingRect().height() + m_spacing;
        }

    qreal maxOutputWidth = 0;
    for (auto* out : m_outputs)
        if (out)
        {
            maxOutputWidth = std::fmax(maxOutputWidth, out->boundingRect().width());
            maxOutputHeight += out->boundingRect().height() + m_spacing;
        }

    qreal maxParamWidth = 0;
    for (auto* proxy : m_parameterWidgets.values())
        if (proxy)
        {
            maxParamWidth = std::fmax(maxParamWidth, proxy->boundingRect().width());
            maxParamHeight += proxy->boundingRect().height() + m_spacing;
        }

    qreal width = maxInputWidth + maxOutputWidth + 40 + maxParamWidth + 2 * m_margin + 2 * m_margin;

    qreal contentHeight = m_titleHeight + m_margin;

    contentHeight += std::max({(qreal)maxInputHeight, (qreal)maxOutputHeight, (qreal)maxParamHeight});

    m_rect = QRectF(0, 0, width, contentHeight + m_margin);
}

void
NodeItem::drawBackground(QPainter& painter) const
{
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.setPen(QPen(m_borderColor, 1));
    painter.setBrush(m_bgColor);
    painter.drawRoundedRect(m_rect, 10, 10);
}

void
NodeItem::drawTitle(QPainter& painter) const
{
    QLinearGradient gradient(0, 0, m_rect.width(), 10);
    gradient.setColorAt(0, m_nodeNameColor.lighter(150));
    gradient.setColorAt(1, m_nodeNameColor.darker(120));
    painter.setPen(Qt::NoPen);
    painter.setBrush(gradient);
    painter.drawRoundedRect(QRectF(0, 0, m_rect.width(), m_titleHeight), 10, 10);

    QRectF overlap(0, m_titleHeight - 5, m_rect.width(), 10);
    painter.setBrush(m_bgColor);
    painter.drawRect(overlap);
}

void
NodeItem::drawGlowingBounding(QPainter& painter)
{
    if (!m_hovered && !m_selected)
        return;

    QRectF glowRect = m_rect.adjusted(-2, -2, 2, 2);
    QColor glowColor = m_selected ? QColor(0, 255, 100, 100) : QColor(0, 255, 255, 100);

    QPen glowPen(glowColor);
    glowPen.setWidth(12);
    glowPen.setJoinStyle(Qt::RoundJoin);

    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.setPen(glowPen);
    painter.setBrush(Qt::NoBrush);
    painter.drawRoundedRect(glowRect, 10, 10);
}

void
NodeItem::paint(QPainter* painter, const QStyleOptionGraphicsItem*, QWidget*)
{
    drawBackground(*painter);
    drawTitle(*painter);
    drawGlowingBounding(*painter);
}

bool
NodeItem::sceneEvent(QEvent* event)
{
    switch (event->type())
    {
        case QEvent::GraphicsSceneHoverEnter:
            m_hovered = true;
            update();
            break;
        case QEvent::GraphicsSceneHoverLeave:
            m_hovered = false;
            update();
            break;
        case QEvent::GraphicsSceneMousePress:
        case QEvent::GraphicsSceneMouseRelease:
            update();
            event->accept();
            break;
        default:
            break;
    }
    return QGraphicsItem::sceneEvent(event);
}

QVariant
NodeItem::itemChange(GraphicsItemChange change, const QVariant& value)
{
    if (change == ItemPositionHasChanged)
    {
        m_registry->nodeMoved(this);
        emit sgnItemMoved();
    }
    else if (change == ItemSelectedHasChanged)
    {
        m_selected = value.toBool();
        update();
    }
    return QGraphicsItem::itemChange(change, value);
}

QVector<PortLabel*>
NodeItem::inputs() const
{
    return m_inputs;
}
QVector<PortLabel*>
NodeItem::outputs() const
{
    return m_outputs;
}
bool
NodeItem::isActivated() const
{
    return m_isActive;
}
void
NodeItem::setActive(bool newIsActive)
{
    m_isActive = newIsActive;
}

QMap<QWidget*, QGraphicsProxyWidget*>
NodeItem::parameterWidgets() const
{
    return m_parameterWidgets;
}
QWidget*
NodeItem::getParameterWidget(PortLabel* port) const
{
    return parameterWidgets().key(parameterPorts().value(port), nullptr);
}

QMap<PortLabel*, QGraphicsProxyWidget*>
NodeItem::parameterPorts() const
{
    return m_parameterPorts;
}
QList<PortLabel*>
NodeItem::paramsInputs() const
{
    return m_parameterPorts.keys();
}

void
NodeItem::setDisplayedNodeName(const QString& t)
{
    m_displayedNodeName = t;
    if (m_nodeNameLabel)
    {
        m_nodeNameLabel->blockSignals(true);
        m_nodeNameLabel->setLabelText(t);
        m_nodeNameLabel->blockSignals(false);
    }
    m_titleHeight = m_nodeNameLabel->boundingRect().height();
    updateLayout();
}

void
NodeItem::setNodeName(const QString& t)
{
    m_nodeName = t;
}

QString
NodeItem::displayedNodeName() const
{
    return m_displayedNodeName;
}
QString
NodeItem::nodeName() const
{
    return m_nodeName;
}
void
NodeItem::setNodeNameColor(const QColor& c)
{
    m_nodeNameColor = c;
    update();
}

PortLabel*
NodeItem::getPort(const QGraphicsProxyWidget& proxy) const
{
    for (auto it = m_parameterPorts.constBegin(); it != m_parameterPorts.constEnd(); ++it)
    {
        if (it.value() == &proxy)
            return it.key();
    }
    return nullptr;
}
void
NodeItem::changeVisibility(bool val)
{
    setVisible(val);

    for (auto* input : m_inputs)
    {
        if (input)
            input->setVisible(val);

        auto connections = m_registry->getConnections(input);
        for (auto* conn : connections)
            if (conn)
                conn->setVisible(val);
    }

    for (auto* output : m_outputs)
    {
        if (output)
            output->setVisible(val);

        auto connections = m_registry->getConnections(output);
        for (auto* conn : connections)
            if (conn)
                conn->setVisible(val);
    }

    for (auto* proxy : m_parameterWidgets)
    {
        proxy->setVisible(val);

        for (auto* port : m_parameterPorts.keys())
        {
            if (m_parameterPorts.value(port) == proxy)
            {
                auto connections = m_registry->getConnections(port);
                for (auto* conn : connections)
                    if (conn)
                        conn->setVisible(val);
            }
        }
    }
}

void
NodeItem::connectPorts(const PortLabel* port)
{
    connect(port, &PortLabel::sgnPortMouseClicked, this, [this](PortLabel* p) { emit sgnPortMouseClicked(this, p); });
    connect(port, &PortLabel::sgnPortMouseReleased, this, [this](PortLabel* p) {
        emit sgnPortMouseReleased(this, p);
    });

    connect(port, &PortLabel::sgnPortMouseEntered, this, [this](PortLabel* p) { emit sgnPortMouseEntered(this, p); });
    connect(port, &PortLabel::sgnPortMouseLeaved, this, [this](PortLabel* p) { emit sgnPortMouseLeaved(this, p); });
    connect(port, &PortLabel::sgnDisplayedNameChanged, this, [this](const QString&) {
        updateLayout();
    });
}

void
NodeItem::disconnectPorts(PortLabel* port)
{
    if (!port)
        return;

    disconnect(port, nullptr, this, nullptr); // all signals from port to node
    disconnect(this, nullptr, port, nullptr); // all signals from node to port
}

QRectF
NodeItem::boundingRect() const
{
    return m_rect;
}

void
NodeItem::changeNodeVisibility(bool val)
{
    setVisible(val);

    for (auto* input : m_inputs)
    {
        if (input)
            input->setVisible(val);
    }

    for (auto* output : m_outputs)
    {
        if (output)
            output->setVisible(val);
    }
}

void
NodeItem::removeInput(const QString& name)
{
    for (PortLabel* input : m_inputs)
    {
        if (input && input->name() == name)
        {
            removeInput(input);
            break;
        }
    }

    updateLayout();
}

void
NodeItem::removeOutput(const QString& name)
{
    for (PortLabel* output : m_outputs)
    {
        if (output && output->name() == name)
        {
            removeOutput(output);
            break;
        }
    }

    updateLayout();
}

void
NodeItem::removeParamInput(const QString& name)
{
    for (PortLabel* param : m_parameterPorts.keys())
    {
        if (param && param->name() == name)
        {
            removeParamInput(param);
            break;
        }
    }

    updateLayout();
}
