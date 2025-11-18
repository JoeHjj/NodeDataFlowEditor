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

#pragma once

#include "view/INodeView.hpp"

class GraphRegistry;
class NodeItem;

/**
 * @brief Adapter class that connects a NodeItem instance to the INodeView interface.
 *
 * NodeItemViewAdapter provides a bridge between the existing NodeItem implementation
 * and the abstract INodeView interface. It allows NodeItem to be used where a generic
 * INodeView is expected without modifying NodeItem itself.
 *
 * This adapter forwards method calls and signals between the interface and the
 * underlying NodeItem instance, ensuring compatibility with presenters and models.
 */
class NodeItemViewAdapter : public INodeView
{
    Q_OBJECT

public:
    /**
     * @brief Construct a NodeItemViewAdapter wrapping a given NodeItem.
     * @param item Pointer to the NodeItem being adapted.
     * @param parent Optional QObject parent.
     */
    explicit NodeItemViewAdapter(std::shared_ptr<GraphRegistry> registry, NodeItem* item, QObject* parent = nullptr);

    /** @copydoc INodeView::setDisplayedNodeName */
    void setDisplayedNodeName(const QString& t) override;

    /** @copydoc INodeView::setTitleColor */
    void setTitleColor(const QColor& c) override;

    /** @copydoc INodeView::setActive */
    void setActive(bool on) override;

    /** @copydoc INodeView::active */
    bool active() override;

    /** @copydoc INodeView::setVisibleNode */
    void setVisibleNode(bool on) override;

    /** @copydoc INodeView::position */
    QPointF position() const override;

    /** @copydoc INodeView::setPosition */
    void setPosition(const QPointF& p) override;

    /** @copydoc INodeView::addInput */
    PortLabel* addInput(const QString& name, const QString& displayName) override;

    /** @copydoc INodeView::addParam */
    PortLabel* addParam(QWidget* widget, const QString& name, const QString& displayName) override;

    /** @copydoc INodeView::addOutput */
    PortLabel* addOutput(const QString& name, const QString& displayName) override;

    /** @copydoc INodeView::addInput */
    PortLabel* addInput(const QString& name) override;

    /** @copydoc INodeView::addParam */
    PortLabel* addParam(QWidget* widget, const QString& name) override;

    /** @copydoc INodeView::addOutput */
    PortLabel* addOutput(const QString& name) override;

    /** @copydoc INodeView::removeInput */
    void removeInput(const QString& name) override;

    /** @copydoc INodeView::removeParamInput */
    void removeParamInput(const QString& name) override;

    /** @copydoc INodeView::removeOutput */
    void removeOutput(const QString& name) override;

    /** @copydoc INodeView::inputs */
    QVector<PortLabel*> inputs() const override;

    /** @copydoc INodeView::outputs */
    QVector<PortLabel*> outputs() const override;

    /** @copydoc INodeView::paramsInputs */
    QList<PortLabel*> paramsInputs() const override;

    /**
     * @brief Access the underlying NodeItem.
     * @return Pointer to the adapted NodeItem instance.
     */
    NodeItem* item() const;

    /** @copydoc INodeView::isAGroupNode */
    bool isAGroupNode() const override;

private:
    std::shared_ptr<GraphRegistry> m_registry;
    NodeItem* m_item{nullptr}; ///< Wrapped NodeItem instance.

    /**
     * @brief Connect relevant NodeItem signals to the adapter’s INodeView signals.
     */
    void wireSignals();

    /**
     * @brief Connect scene selection changes to adapter-level selection updates.
     *
     * Keeps the INodeView selection state synchronized with the NodeItem’s
     * selection in the scene.
     */
    void wireSceneSelection();
};
