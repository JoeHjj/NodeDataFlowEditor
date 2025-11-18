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

#include "taggable/Taggable.hpp"
#include "view/ConnectionItem.hpp"

#include <QColor>
#include <QGraphicsItem>
#include <QObject>
#include <QString>

class PortView;

/**
 * @brief Represents a node port in a graph editor.
 *
 * PortLabel manages both the visual representation (label + optional arrow)
 * and user interactions such as hovering, clicking, and editing. Ports
 * can also have multiple connections and carry metadata tags via Taggable.
 */
class PortLabel : public QGraphicsObject, public Taggable
{
    Q_OBJECT

public:
    /**
     * @brief Orientation or type of the port.
     */
    enum class Orientation : uint8_t
    {
        Input,     ///< Receives data from other nodes.
        Parameter, ///< Represents a control parameter input.
        Output     ///< Sends data to other nodes.
    };

    /** @name Constructors & Destructor */
    ///@{
    /**
     * @brief Construct a new PortLabel with separate internal and displayed names.
     * @param name Internal port name.
     * @param displayName Text shown to the user.
     * @param moduleName Name of the parent module.
     * @param orientation Orientation of the port (Input/Parameter/Output).
     * @param parent Optional parent QGraphicsItem.
     */
    explicit PortLabel(QString name,
                       QString displayName,
                       QString moduleName,
                       Orientation orientation = Orientation::Output,
                       QGraphicsItem* parent = nullptr);

    /**
     * @brief Construct a new PortLabel with same internal and displayed name.
     * @param name Port internal and displayed name.
     * @param moduleName Name of the parent module.
     * @param orientation Orientation of the port.
     * @param parent Optional parent QGraphicsItem.
     */
    explicit PortLabel(QString name,
                       QString moduleName,
                       Orientation orientation = Orientation::Output,
                       QGraphicsItem* parent = nullptr);

    /**
     * @brief Destructor.
     */
    ~PortLabel() override;
    ///@}

    /** @name QGraphicsItem Overrides */
    ///@{
    /**
     * @brief Returns the bounding rectangle for painting and interaction.
     */
    QRectF boundingRect() const override;

    /**
     * @brief Paints the port label and arrow.
     * @param painter Painter object for drawing.
     * @param option Style options.
     * @param widget Optional widget context.
     */
    void paint(QPainter* painter,
               const QStyleOptionGraphicsItem* option,
               QWidget* widget = nullptr) override;
    ///@}

    /** @name Orientation & Identity */
    ///@{
    /**
     * @brief Set the orientation of the port.
     * @param o New orientation.
     */
    void setOrientation(Orientation o);

    /**
     * @brief Get the orientation of the port.
     * @return Orientation
     */
    Orientation getOrientation() const;

    /**
     * @brief Check if this is any kind of input port (Input or Parameter).
     * @return true if input or parameter
     */
    bool isAnyInputPort() const;

    /**
     * @brief Check if this is a parameter port.
     * @return true if Parameter
     */
    bool isParameterPort() const;

    /**
     * @brief Check if this is an input port.
     * @return true if Input
     */
    bool isInputPort() const;

    /**
     * @brief Check if this is an output port.
     * @return true if Output
     */
    bool isOutputPort() const;

    /**
     * @brief Set the displayed label of the port.
     * @param text New displayed text.
     */
    void setDisplayName(const QString& text);

    /**
     * @brief Get the displayed label.
     * @return QString Displayed text
     */
    QString displayName() const;

    /**
     * @brief Set the internal name of the port.
     * @param text Internal name
     */
    void setName(const QString& text);

    /**
     * @brief Get the internal name.
     * @return QString Internal name
     */
    QString name() const;

    /**
     * @brief Set the parent module name.
     * @param moduleName Name of module
     */
    void setModuleName(const QString& moduleName);

    /**
     * @brief Get the parent module name.
     * @return QString Module name
     */
    QString moduleName() const;
    ///@}

    /** @name Connections */
    ///@{
    /**
     * @brief Returns a representation of the connection port.
     * @return ConnectionPort
     */
    ConnectionPort getConnectionPortData() const;
    ///@}

    /** @name Visual Feedback */
    ///@{
    /**
     * @brief Set the hovered state for highlighting.
     * @param hovered True if hovered, false otherwise.
     */
    void setHovered(bool hovered);

    /**
     * @brief Set the color of the port.
     * @param color QColor to use.
     */
    void setColor(const QColor& color);
    ///@}

signals:
    /** @brief Emitted when the port is clicked. */
    void sgnPortMouseClicked(PortLabel* port);

    /** @brief Emitted when the mouse is released on the port. */
    void sgnPortMouseReleased(PortLabel* port);

    /** @brief Emitted when mouse enters the port area. */
    void sgnPortMouseEntered(PortLabel* port);

    /** @brief Emitted when mouse leaves the port area. */
    void sgnPortMouseLeaved(PortLabel* port);

    /** @brief Emitted when a connection is added to this port. */
    void sgnConnectionAdded(ConnectionItem* con);

    /** @brief Emitted when a connection is removed from this port. */
    void sgnConnectionRemoved(ConnectionItem* con);

    /** @brief Emitted when the displayed name changes. */
    void sgnDisplayedNameChanged(const QString& newDisplayName);

protected:
    /**
     * @brief Handles scene events such as hover, click, and mouse movement.
     * @param event Pointer to the event.
     * @return true if event was handled.
     */
    bool sceneEvent(QEvent* event) override;

private:
    /**
     * @brief Repositions the label and arrow according to the port orientation.
     */
    void repositionLabel();

private:
    PortView* m_portView = nullptr; ///< Visual component showing label + arrow
    QString m_portName;             ///< Internal port name
    QString m_portDisplayName;      ///< Displayed label
    QString m_moduleName;           ///< Parent module name
    Orientation m_orientation;      ///< Port orientation

    bool m_hovered = false;                          ///< Hovered state
    bool m_clicked = false;                          ///< Clicked state
    QColor m_hoveredColor = QColor(0, 255, 0, 100);  ///< Highlight when hovered
    QColor m_clickedColor = QColor(80, 255, 0, 120); ///< Highlight when clicked
    QColor m_portColor = QColor(110, 110, 110);      ///< Base color
};
