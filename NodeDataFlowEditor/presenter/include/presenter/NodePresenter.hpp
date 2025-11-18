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

#include "model/NodeModel.hpp"
#include "view/INodeView.hpp"

#include <QObject>
#include <QPointer>
#include <QSet>
#include <QString>

class PortLabel;

/**
 * @brief Acts as the mediator between NodeModel and INodeView.
 *
 * NodePresenter synchronizes state and events between the model (data)
 * and the view (visual representation). It ensures that both remain
 * consistent — for example, when the model changes its ports or title,
 * the view updates automatically, and vice versa.
 *
 * Responsibilities include:
 * - Reflecting property and port changes from the model to the view.
 * - Handling user interactions from the view (clicks, moves, selections).
 * - Maintaining consistent naming and synchronization between model and view ports.
 */
class NodePresenter : public QObject
{
    Q_OBJECT

public:
    /**
     * @brief Construct a NodePresenter that links a NodeModel with an INodeView.
     * @param model Pointer to the node’s model (data source).
     * @param view Pointer to the node’s view (visual representation).
     * @param parent Optional QObject parent.
     *
     * This constructor automatically connects relevant signals and slots
     * to maintain synchronization between the model and the view.
     */
    explicit NodePresenter(NodeModel* model, INodeView* view, QObject* parent = nullptr);

    /**
     * @brief Default destructor.
     */
    ~NodePresenter() override = default;

public slots:
    /**
     * @brief Ensure that the view’s ports match the model’s current definition.
     *
     * Adds any missing ports defined in the model but absent in the view,
     * and removes ports that exist in the view but no longer exist in the model.
     */
    void ensurePortsMatchModel() const;

signals:
    /**
     * @brief Emitted when the node has moved in the scene.
     */
    void nodeMoved();

    /**
     * @brief Emitted when a port is clicked by the user.
     * @param port The clicked port.
     */
    void portClicked(PortLabel* port);

    /**
     * @brief Emitted when a port mouse release occurs.
     * @param port The released port.
     */
    void portReleased(PortLabel* port);

    /**
     * @brief Emitted when the mouse cursor enters a port.
     * @param port The hovered port.
     */
    void portHoverEntered(PortLabel* port);

    /**
     * @brief Emitted when the mouse cursor leaves a port.
     * @param port The port that was previously hovered.
     */
    void portHoverLeaved(PortLabel* port);

    /**
     * @brief Emitted when the node’s selection state changes.
     * @param selected True if the node is selected, false otherwise.
     */
    void selectionChanged(bool selected);

private:
    QPointer<NodeModel> m_model; ///< Pointer to the associated model.
    QPointer<INodeView> m_view;  ///< Pointer to the associated view.

    /**
     * @brief Establish signal-slot connections from the model to the view.
     *
     * Ensures that visual elements (title, color, position, ports) are updated
     * whenever the corresponding model data changes.
     */
    void connectModelToView();

    /**
     * @brief Establish signal-slot connections from the view to the model.
     *
     * Propagates user interactions (movement, clicks, port changes)
     * from the view back into the data model.
     */
    void connectViewToModel();

    /**
     * @brief Extract a set of port names from a vector of ports.
     * @param ports Vector of PortLabel pointers.
     * @return Set of port names.
     */
    static QSet<QString> namesOf(const QVector<PortLabel*>& ports);

    /**
     * @brief Add any ports that exist in the model but not in the view.
     */
    void addPortsMissingInView() const;

    /**
     * @brief Remove any ports that exist in the view but not in the model.
     */
    void removePortsStrayInView() const;
};
