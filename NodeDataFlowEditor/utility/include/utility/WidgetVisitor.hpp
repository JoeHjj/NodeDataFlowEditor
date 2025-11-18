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

#include <QCalendarWidget>
#include <QCheckBox>
#include <QComboBox>
#include <QDateEdit>
#include <QDateTimeEdit>
#include <QDial>
#include <QDoubleSpinBox>
#include <QGroupBox>
#include <QLineEdit>
#include <QList>
#include <QListWidget>
#include <QPlainTextEdit>
#include <QProgressBar>
#include <QPushButton>
#include <QRadioButton>
#include <QScrollArea>
#include <QSlider>
#include <QSpinBox>
#include <QSplitter>
#include <QStackedWidget>
#include <QString>
#include <QTabWidget>
#include <QTableWidget>
#include <QTextEdit>
#include <QTimeEdit>
#include <QToolButton>
#include <QTreeWidget>
#include <QWidget>

class GroupItem;
class PortLabel;
class NodeItem;
class GraphRegistry;

/**
 * @brief Visitor for cloning and synchronizing QWidget-based parameters.
 *
 * WidgetVisitor traverses a QWidget, clones it, and binds the cloned widget
 * to a group parameter so that all linked PortLabel widgets stay synchronized.
 */
struct WidgetVisitor
{
    PortLabel* m_port;      ///< PortLabel to synchronize with
    const QString& m_name;  ///< Parameter/group name
    GroupItem* m_groupItem; ///< GroupItem to add widgets to

    WidgetVisitor(std::shared_ptr<GraphRegistry> registry, PortLabel* p, const QString& n, GroupItem* g);

    void visit(QWidget* w);

private:
    // Input widgets
    void visitLineEdit(QLineEdit* w);
    void visitPlainTextEdit(QPlainTextEdit* w);
    void visitTextEdit(QTextEdit* w);
    void visitSpinBox(QSpinBox* w);
    void visitDoubleSpinBox(QDoubleSpinBox* w);
    void visitComboBox(QComboBox* w);
    void visitCheckBox(QCheckBox* w);
    void visitRadioButton(QRadioButton* w);
    void visitSlider(QSlider* w);
    void visitDial(QDial* w);
    void visitDateEdit(QDateEdit* w);
    void visitTimeEdit(QTimeEdit* w);
    void visitDateTimeEdit(QDateTimeEdit* w);
    void visitCalendarWidget(QCalendarWidget* w);
    void visitListWidget(QListWidget* w);
    void visitTableWidget(QTableWidget* w);
    void visitTreeWidget(QTreeWidget* w);
    void visitProgressBar(QProgressBar* w);
    void visitPushButton(QPushButton* w);
    void visitToolButton(QToolButton* w);

    // Containers
    void visitGroupBox(QGroupBox* w);
    void visitScrollArea(QScrollArea* w);
    void visitTabWidget(QTabWidget* w);
    void visitStackedWidget(QStackedWidget* w);
    void visitSplitter(QSplitter* w);
    void visitGenericContainer(QWidget* w);

    // Fallback
    void visitUnknown(QWidget* w);

    // Utility
    void addGroupWidget(QWidget* w);

private:
    std::shared_ptr<GraphRegistry> m_registry;
};
