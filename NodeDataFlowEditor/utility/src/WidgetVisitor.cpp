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

#include "utility/WidgetVisitor.hpp"
#include "utility/GraphRegistry.hpp"
#include "view/GroupItem.hpp"
#include "view/NodeItem.hpp"
#include "view/PortLabel.hpp"

#include <QCalendarWidget>
#include <QFormLayout>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QListWidget>
#include <QObject>
#include <QProgressBar>
#include <QPushButton>
#include <QRadioButton>
#include <QScrollArea>
#include <QSplitter>
#include <QStackedWidget>
#include <QTabWidget>
#include <QTableWidget>
#include <QToolButton>
#include <QTreeWidget>
#include <QVBoxLayout>

WidgetVisitor::WidgetVisitor(std::shared_ptr<GraphRegistry> registry, PortLabel* p, const QString& n, GroupItem* g)
    : m_port(p)
    , m_name(n)
    , m_groupItem(g)
    , m_registry(registry)
{}

// ---------------------- Generic visit ----------------------
void
WidgetVisitor::visit(QWidget* w)
{
    if (!w)
        return;

    if (auto line = qobject_cast<QLineEdit*>(w))
        visitLineEdit(line);
    else if (auto plain = qobject_cast<QPlainTextEdit*>(w))
        visitPlainTextEdit(plain);
    else if (auto text = qobject_cast<QTextEdit*>(w))
        visitTextEdit(text);
    else if (auto spin = qobject_cast<QSpinBox*>(w))
        visitSpinBox(spin);
    else if (auto dspin = qobject_cast<QDoubleSpinBox*>(w))
        visitDoubleSpinBox(dspin);
    else if (auto combo = qobject_cast<QComboBox*>(w))
        visitComboBox(combo);
    else if (auto check = qobject_cast<QCheckBox*>(w))
        visitCheckBox(check);
    else if (auto radio = qobject_cast<QRadioButton*>(w))
        visitRadioButton(radio);
    else if (auto slider = qobject_cast<QSlider*>(w))
        visitSlider(slider);
    else if (auto dial = qobject_cast<QDial*>(w))
        visitDial(dial);
    else if (auto date = qobject_cast<QDateEdit*>(w))
        visitDateEdit(date);
    else if (auto time = qobject_cast<QTimeEdit*>(w))
        visitTimeEdit(time);
    else if (auto datetime = qobject_cast<QDateTimeEdit*>(w))
        visitDateTimeEdit(datetime);
    else if (auto calendar = qobject_cast<QCalendarWidget*>(w))
        visitCalendarWidget(calendar);
    else if (auto list = qobject_cast<QListWidget*>(w))
        visitListWidget(list);
    else if (auto table = qobject_cast<QTableWidget*>(w))
        visitTableWidget(table);
    else if (auto tree = qobject_cast<QTreeWidget*>(w))
        visitTreeWidget(tree);
    else if (auto prog = qobject_cast<QProgressBar*>(w))
        visitProgressBar(prog);
    else if (auto push = qobject_cast<QPushButton*>(w))
        visitPushButton(push);
    else if (auto tool = qobject_cast<QToolButton*>(w))
        visitToolButton(tool);
    else if (auto group = qobject_cast<QGroupBox*>(w))
        visitGroupBox(group);
    else if (auto scroll = qobject_cast<QScrollArea*>(w))
        visitScrollArea(scroll);
    else if (auto tab = qobject_cast<QTabWidget*>(w))
        visitTabWidget(tab);
    else if (auto stacked = qobject_cast<QStackedWidget*>(w))
        visitStackedWidget(stacked);
    else if (auto split = qobject_cast<QSplitter*>(w))
        visitSplitter(split);
    else
        visitGenericContainer(w);
}

// ---------------------- Basic widgets ----------------------
void
WidgetVisitor::visitLineEdit(QLineEdit* w)
{
    auto clone = std::make_unique<QLineEdit>();
    clone->setText(w->text());
    clone->setAlignment(w->alignment());
    clone->setReadOnly(w->isReadOnly());

    QObject::connect(clone.get(), &QLineEdit::textChanged, [this](const QString& val) {
        auto map = dynamic_cast<NodeItem*>(m_port->parentItem())->parameterWidgets();
        for (auto it = map.begin(); it != map.end(); ++it)
            if (auto ww = qobject_cast<QLineEdit*>(it.key()))
                ww->setText(val);
    });

    addGroupWidget(clone.get());
    clone.release();
}

void
WidgetVisitor::visitPlainTextEdit(QPlainTextEdit* w)
{
    auto clone = std::make_unique<QPlainTextEdit>();
    clone->setPlainText(w->toPlainText());

    QObject::connect(clone.get(), &QPlainTextEdit::textChanged, [clone_ptr = clone.get(), p = this->m_port] {
        QString val = clone_ptr->toPlainText();
        auto map = dynamic_cast<NodeItem*>(p->parentItem())->parameterWidgets();
        for (auto it = map.begin(); it != map.end(); ++it)
            if (auto ww = qobject_cast<QPlainTextEdit*>(it.key()))
                ww->setPlainText(val);
    });

    addGroupWidget(clone.get());
    clone.release();
}

void
WidgetVisitor::visitTextEdit(QTextEdit* w)
{
    auto clone = std::make_unique<QTextEdit>();
    clone->setPlainText(w->toPlainText());

    QObject::connect(clone.get(), &QTextEdit::textChanged, [clone_ptr = clone.get(), p = this->m_port] {
        QString val = clone_ptr->toPlainText();
        auto map = dynamic_cast<NodeItem*>(p->parentItem())->parameterWidgets();
        for (auto it = map.begin(); it != map.end(); ++it)
            if (auto ww = qobject_cast<QTextEdit*>(it.key()))
                ww->setPlainText(val);
    });

    addGroupWidget(clone.get());
    clone.release();
}

void
WidgetVisitor::visitSpinBox(QSpinBox* w)
{
    auto clone = std::make_unique<QSpinBox>();
    clone->setRange(w->minimum(), w->maximum());
    clone->setSingleStep(w->singleStep());
    clone->setValue(w->value());

    QObject::connect(clone.get(), qOverload<int>(&QSpinBox::valueChanged), [p = this->m_port](int val) {
        auto map = dynamic_cast<NodeItem*>(p->parentItem())->parameterWidgets();
        for (auto it = map.begin(); it != map.end(); ++it)
            if (auto ww = qobject_cast<QSpinBox*>(it.key()))
                ww->setValue(val);
    });

    addGroupWidget(clone.get());
    clone.release();
}

void
WidgetVisitor::visitDoubleSpinBox(QDoubleSpinBox* w)
{
    auto clone = std::make_unique<QDoubleSpinBox>();
    clone->setRange(w->minimum(), w->maximum());
    clone->setDecimals(w->decimals());
    clone->setSingleStep(w->singleStep());
    clone->setValue(w->value());

    QObject::connect(clone.get(), qOverload<double>(&QDoubleSpinBox::valueChanged), [p = this->m_port](double val) {
        auto map = dynamic_cast<NodeItem*>(p->parentItem())->parameterWidgets();
        for (auto it = map.begin(); it != map.end(); ++it)
            if (auto ww = qobject_cast<QDoubleSpinBox*>(it.key()))
                ww->setValue(val);
    });

    addGroupWidget(clone.get());
    clone.release();
}

void
WidgetVisitor::visitComboBox(QComboBox* w)
{
    auto clone = std::make_unique<QComboBox>();
    for (int i = 0; i < w->count(); ++i)
        clone->addItem(w->itemText(i), w->itemData(i));
    clone->setCurrentIndex(w->currentIndex());

    QObject::connect(clone.get(), qOverload<int>(&QComboBox::currentIndexChanged), [p = this->m_port](int idx) {
        auto map = dynamic_cast<NodeItem*>(p->parentItem())->parameterWidgets();
        for (auto it = map.begin(); it != map.end(); ++it)
            if (auto ww = qobject_cast<QComboBox*>(it.key()))
                ww->setCurrentIndex(idx);
    });

    addGroupWidget(clone.get());
    clone.release();
}

void
WidgetVisitor::visitCheckBox(QCheckBox* w)
{
    auto clone = std::make_unique<QCheckBox>();
    clone->setText(w->text());
    clone->setChecked(w->isChecked());

    QObject::connect(clone.get(), &QCheckBox::toggled, [p = this->m_port](bool val) {
        auto map = dynamic_cast<NodeItem*>(p->parentItem())->parameterWidgets();
        for (auto it = map.begin(); it != map.end(); ++it)
            if (auto ww = qobject_cast<QCheckBox*>(it.key()))
                ww->setChecked(val);
    });

    addGroupWidget(clone.get());
    clone.release();
}

void
WidgetVisitor::visitRadioButton(QRadioButton* w)
{
    auto clone = std::make_unique<QRadioButton>();
    clone->setText(w->text());
    clone->setChecked(w->isChecked());

    QObject::connect(clone.get(), &QRadioButton::toggled, [p = this->m_port](bool val) {
        auto map = dynamic_cast<NodeItem*>(p->parentItem())->parameterWidgets();
        for (auto it = map.begin(); it != map.end(); ++it)
            if (auto ww = qobject_cast<QRadioButton*>(it.key()))
                ww->setChecked(val);
    });

    addGroupWidget(clone.get());
    clone.release();
}

// ---------------------- Sliders / Dials ----------------------
void
WidgetVisitor::visitSlider(QSlider* w)
{
    auto clone = std::make_unique<QSlider>(w->orientation());
    clone->setRange(w->minimum(), w->maximum());
    clone->setSingleStep(w->singleStep());
    clone->setValue(w->value());

    QObject::connect(clone.get(), &QSlider::valueChanged, [p = this->m_port](int val) {
        auto map = dynamic_cast<NodeItem*>(p->parentItem())->parameterWidgets();
        for (auto it = map.begin(); it != map.end(); ++it)
            if (auto ww = qobject_cast<QSlider*>(it.key()))
                ww->setValue(val);
    });

    addGroupWidget(clone.get());
    clone.release();
}

void
WidgetVisitor::visitDial(QDial* w)
{
    auto clone = std::make_unique<QDial>();
    clone->setRange(w->minimum(), w->maximum());
    clone->setValue(w->value());
    clone->setNotchesVisible(w->notchesVisible());
    clone->setWrapping(w->wrapping());

    QObject::connect(clone.get(), &QDial::valueChanged, [p = this->m_port](int val) {
        auto map = dynamic_cast<NodeItem*>(p->parentItem())->parameterWidgets();
        for (auto it = map.begin(); it != map.end(); ++it)
            if (auto ww = qobject_cast<QDial*>(it.key()))
                ww->setValue(val);
    });

    addGroupWidget(clone.get());
    clone.release();
}

// ---------------------- Date/Time ----------------------
void
WidgetVisitor::visitDateEdit(QDateEdit* w)
{
    auto clone = std::make_unique<QDateEdit>();
    clone->setCalendarPopup(w->calendarPopup());
    clone->setDate(w->date());

    QObject::connect(clone.get(), &QDateEdit::dateChanged, [p = this->m_port](const QDate& val) {
        auto map = dynamic_cast<NodeItem*>(p->parentItem())->parameterWidgets();
        for (auto it = map.begin(); it != map.end(); ++it)
            if (auto ww = qobject_cast<QDateEdit*>(it.key()))
                ww->setDate(val);
    });

    addGroupWidget(clone.get());
    clone.release();
}

void
WidgetVisitor::visitTimeEdit(QTimeEdit* w)
{
    auto clone = std::make_unique<QTimeEdit>();
    clone->setTime(w->time());

    QObject::connect(clone.get(), &QTimeEdit::timeChanged, [p = this->m_port](const QTime& val) {
        auto map = dynamic_cast<NodeItem*>(p->parentItem())->parameterWidgets();
        for (auto it = map.begin(); it != map.end(); ++it)
            if (auto ww = qobject_cast<QTimeEdit*>(it.key()))
                ww->setTime(val);
    });

    addGroupWidget(clone.get());
    clone.release();
}

void
WidgetVisitor::visitDateTimeEdit(QDateTimeEdit* w)
{
    auto clone = std::make_unique<QDateTimeEdit>();
    clone->setCalendarPopup(w->calendarPopup());
    clone->setDateTime(w->dateTime());

    QObject::connect(clone.get(), &QDateTimeEdit::dateTimeChanged, [p = this->m_port](const QDateTime& val) {
        auto map = dynamic_cast<NodeItem*>(p->parentItem())->parameterWidgets();
        for (auto it = map.begin(); it != map.end(); ++it)
            if (auto ww = qobject_cast<QDateTimeEdit*>(it.key()))
                ww->setDateTime(val);
    });

    addGroupWidget(clone.get());
    clone.release();
}

// ---------------------- Calendar / List / Table / Tree ----------------------
void
WidgetVisitor::visitCalendarWidget(QCalendarWidget* w)
{
    auto clone = std::make_unique<QCalendarWidget>();
    clone->setSelectedDate(w->selectedDate());

    QObject::connect(clone.get(), &QCalendarWidget::selectionChanged, [clone_ptr = clone.get(), p = this->m_port] {
        QDate val = clone_ptr->selectedDate();
        auto map = dynamic_cast<NodeItem*>(p->parentItem())->parameterWidgets();
        for (auto it = map.begin(); it != map.end(); ++it)
            if (auto ww = qobject_cast<QCalendarWidget*>(it.key()))
                ww->setSelectedDate(val);
    });

    addGroupWidget(clone.get());
    clone.release();
}

void
WidgetVisitor::visitListWidget(QListWidget* w)
{
    auto clone = std::make_unique<QListWidget>(); // NOSONAR
    for (int i = 0; i < w->count(); ++i)
        clone->addItem(w->item(i)->text());

    addGroupWidget(clone.get());
    clone.release();
}

void
WidgetVisitor::visitTableWidget(QTableWidget* w)
{
    auto clone = std::make_unique<QTableWidget>(w->rowCount(), w->columnCount()); // NOSONAR
    for (int r = 0; r < w->rowCount(); ++r)
        for (int c = 0; c < w->columnCount(); ++c)
            if (auto const* item = w->item(r, c))
            {
                auto itm = std::make_unique<QTableWidgetItem>(item->text());
                clone->setItem(r, c, itm.get());
                itm.release();
            }

    addGroupWidget(clone.release());
}

void
WidgetVisitor::visitTreeWidget(QTreeWidget* w)
{
    auto clone = std::make_unique<QTreeWidget>(); // NOSONAR
    if (w->headerItem())
        clone->setHeaderLabels(QStringList() << w->headerItem()->text(0));

    addGroupWidget(clone.get());
    clone.release();
}

// ---------------------- Progress / Buttons ----------------------
void
WidgetVisitor::visitProgressBar(QProgressBar* w)
{
    auto clone = std::make_unique<QProgressBar>();
    clone->setRange(w->minimum(), w->maximum());
    clone->setValue(w->value());

    addGroupWidget(clone.get());
    clone.release();
}

void
WidgetVisitor::visitPushButton(QPushButton* w)
{
    auto clone = std::make_unique<QPushButton>();
    clone->setText(w->text());

    addGroupWidget(clone.get());
    clone.release();
}

void
WidgetVisitor::visitToolButton(QToolButton* w)
{
    auto clone = std::make_unique<QToolButton>();
    clone->setText(w->text());

    addGroupWidget(clone.get());
    clone.release();
}

// ---------------------- Containers ----------------------
void
WidgetVisitor::visitGroupBox(QGroupBox* w)
{
    auto clone = std::make_unique<QGroupBox>(w->title());
    auto layout = std::make_unique<QVBoxLayout>();
    clone->setLayout(layout.release());

    for (auto* child : w->findChildren<QWidget*>(QString(), Qt::FindDirectChildrenOnly))
    {
        visit(child);
        if (clone->layout())
            clone->layout()->addWidget(child);
    }

    addGroupWidget(clone.get());
    clone.release();
}

void
WidgetVisitor::visitScrollArea(QScrollArea* w)
{
    auto clone = std::make_unique<QScrollArea>();
    clone->setWidgetResizable(w->widgetResizable());
    if (w->widget())
        visit(w->widget());

    addGroupWidget(clone.get());
    clone.release();
}

void
WidgetVisitor::visitTabWidget(QTabWidget* w)
{
    auto clone = std::make_unique<QTabWidget>(); // NOSONAR
    for (int i = 0; i < w->count(); ++i)
    {
        auto* child = w->widget(i);
        visit(child);
        clone->addTab(child, w->tabText(i));
    }

    addGroupWidget(clone.get());
    clone.release();
}

void
WidgetVisitor::visitStackedWidget(QStackedWidget* w)
{
    auto clone = std::make_unique<QStackedWidget>(); // NOSONAR
    for (int i = 0; i < w->count(); ++i)
    {
        auto* child = w->widget(i);
        visit(child);
        clone->addWidget(child);
    }

    addGroupWidget(clone.get());
    clone.release();
}

void
WidgetVisitor::visitSplitter(QSplitter* w)
{
    auto clone = std::make_unique<QSplitter>(w->orientation()); // NOSONAR
    for (auto* child : w->findChildren<QWidget*>(QString(), Qt::FindDirectChildrenOnly))
    {
        visit(child);
        clone->addWidget(child);
    }

    addGroupWidget(clone.get());
    clone.release();
}

void
WidgetVisitor::visitGenericContainer(QWidget* w)
{
    for (auto* child : w->findChildren<QWidget*>())
        visit(child);
}

void
WidgetVisitor::visitUnknown(QWidget* w)
{
    visitGenericContainer(w);
}

void
WidgetVisitor::addGroupWidget(QWidget* w)
{
    if (!m_groupItem || !m_port)
        return;

    auto* g = m_groupItem->addParameter(w, m_name);
    g->setDisplayName(m_port->moduleName() + "_" + m_port->displayName());
    QObject::connect(g, &PortLabel::sgnDisplayedNameChanged, m_port, &PortLabel::setDisplayName);
    m_registry->registerForwardParameter(m_groupItem, g, m_port);
}
