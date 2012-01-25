// -*- mode: c++; c-basic-style: "bsd"; c-basic-offset: 4; -*-
/*
 * ReflectiveGUI.cpp
 * Copyright (C) Cátedra SAES-UMU 2011 <catedra-saes-umu@listas.um.es>
 *
 * CORBASIM is free software: you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License as published
 * by the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * CORBASIM is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "ReflectiveGUI.hpp"
#include <cassert>
#include <limits>

using namespace corbasim::reflective_gui;

// should be a singleton instance of a factory
QWidget * createWidget(corbasim::core::reflective_base const * reflective,
        QWidget * parent)
{
    using namespace corbasim::core;

    if (!reflective)
        return new QLabel("Null reflective type!", parent);

    const reflective_type type = reflective->get_type();

    switch(type)
    {
        case TYPE_BOOL:
            return new BoolWidget(reflective, parent);

            // Integer types
        case TYPE_OCTET:
        case TYPE_CHAR:
        case TYPE_SHORT:
        case TYPE_USHORT:
        case TYPE_LONG:
        case TYPE_ULONG:
        case TYPE_LONGLONG:
        case TYPE_ULONGLONG:
            return new IntegerWidget(reflective, parent);
        
        case TYPE_STRING:
        case TYPE_WSTRING:
            return new StringWidget(reflective, parent);

        case TYPE_OBJREF:
        case TYPE_UNION:
            break;

        case TYPE_ARRAY:
            return new ArrayWidget(reflective, parent);
        case TYPE_SEQUENCE:
            return new SequenceWidget(reflective, parent);

        case TYPE_DOUBLE:
        case TYPE_FLOAT:
            return new FloatWidget(reflective, parent);
        case TYPE_STRUCT:
            return new StructWidget(reflective, parent);
        case TYPE_ENUM:
            return new EnumWidget(reflective, parent);
        default:
            break;
    }

    return new QLabel("Unsupported item!", parent);
}

ReflectiveWidgetBase::ReflectiveWidgetBase(
        core::reflective_base const * reflective) :
    m_reflective(reflective)
{
}

ReflectiveWidgetBase::~ReflectiveWidgetBase()
{
}

corbasim::core::reflective_base const * 
ReflectiveWidgetBase::getReflective() const
{
    return m_reflective;
}

FloatWidget::FloatWidget(core::reflective_base const * reflective,
        QWidget * parent) :
    QDoubleSpinBox(parent), ReflectiveWidgetBase(reflective)
{
    using namespace corbasim::core;

    const reflective_type type = reflective->get_type();

    switch(type)
    {
        case TYPE_DOUBLE:
            setRange(-std::numeric_limits< double >::max(),
                    std::numeric_limits< double >::max());
            break;
        case TYPE_FLOAT:
            setRange(-std::numeric_limits< float >::max(),
                    std::numeric_limits< float >::max());
            break;
        default:
            break;
    }
}

FloatWidget::~FloatWidget()
{
}

void FloatWidget::toHolder(core::holder& holder) {}
void FloatWidget::fromHolder(core::holder& holder){}

IntegerWidget::IntegerWidget(core::reflective_base const * reflective,
        QWidget * parent) :
    QSpinBox(parent), ReflectiveWidgetBase(reflective)
{
    using namespace corbasim::core;

    const reflective_type type = reflective->get_type();

    switch(type)
    {
        case TYPE_OCTET:
            setRange(std::numeric_limits< unsigned char >::min(),
                    std::numeric_limits< unsigned char >::max());
            break;
        case TYPE_CHAR:
            setRange(std::numeric_limits< char >::min(),
                    std::numeric_limits< char >::max());
            break;
        case TYPE_SHORT:
            setRange(std::numeric_limits< short >::min(),
                    std::numeric_limits< short >::max());
            break;
        case TYPE_USHORT:
            setRange(std::numeric_limits< unsigned short >::min(),
                    std::numeric_limits< unsigned short >::max());
            break;
        case TYPE_LONG:
            setRange(std::numeric_limits< int32_t >::min(),
                    std::numeric_limits< int32_t >::max());
            break;
        case TYPE_ULONG:
            setRange(std::numeric_limits< uint32_t >::min(),
                    std::numeric_limits< uint32_t >::max());
            break;
        case TYPE_LONGLONG:
            setRange(std::numeric_limits< int64_t >::min(),
                    std::numeric_limits< int64_t >::max());
            break;
        case TYPE_ULONGLONG:
            setRange(std::numeric_limits< uint64_t >::min(),
                    std::numeric_limits< uint64_t >::max());
            break;
        default:
            break;
    }
}

IntegerWidget::~IntegerWidget()
{
}

void IntegerWidget::toHolder(core::holder& holder) {}
void IntegerWidget::fromHolder(core::holder& holder){}

StringWidget::StringWidget(core::reflective_base const * reflective,
        QWidget * parent) :
    QLineEdit(parent), ReflectiveWidgetBase(reflective)
{
}

StringWidget::~StringWidget()
{
}

void StringWidget::toHolder(core::holder& holder) {}
void StringWidget::fromHolder(core::holder& holder){}

EnumWidget::EnumWidget(core::reflective_base const * reflective,
        QWidget * parent) :
    QComboBox(parent), ReflectiveWidgetBase(reflective)
{
    assert(reflective->is_enum());

    unsigned int count = reflective->get_children_count();

    for (unsigned int i = 0; i < count; i++) 
    {
        addItem(reflective->get_child_name(i));
    }
}

EnumWidget::~EnumWidget()
{
}

void EnumWidget::toHolder(core::holder& holder) {}
void EnumWidget::fromHolder(core::holder& holder){}

BoolWidget::BoolWidget(core::reflective_base const * reflective,
        QWidget * parent) :
    QCheckBox(parent), ReflectiveWidgetBase(reflective)
{
}

BoolWidget::~BoolWidget()
{
}

void BoolWidget::toHolder(core::holder& holder) {}
void BoolWidget::fromHolder(core::holder& holder){}

StructWidget::StructWidget(core::reflective_base const * reflective,
        QWidget * parent) :
    QWidget(parent), ReflectiveWidgetBase(reflective)
{
    assert(reflective->get_type() == core::TYPE_STRUCT);

    QGridLayout * layout = new QGridLayout(this);

    unsigned int count = reflective->get_children_count();

    m_widgets.resize(count, NULL);

    for (unsigned int i = 0; i < count; i++) 
    {
        core::reflective_base const * child = 
            reflective->get_child(i);

        const char * child_name = reflective->get_child_name(i);

        QWidget * child_widget = createWidget(child, this);

        m_widgets[i] = dynamic_cast< ReflectiveWidgetBase* >(
                child_widget);

        child_widget->setObjectName(child_name);

        if (child->is_primitive())
        {
            QLabel * label = new QLabel(child_name, this);

            label->setObjectName(QString(child_name) + "_label");

            layout->addWidget(label, i, 0);
            layout->addWidget(child_widget, i, 1);
        }
        else
        {
            QGroupBox * gb = new QGroupBox(child_name, this);
            gb->setObjectName(QString(child_name) + "_group");

            QHBoxLayout * cLayout = new QHBoxLayout(gb);

            cLayout->addWidget(child_widget);

            gb->setLayout(cLayout);
            layout->addWidget(gb, i, 0, 1, 2);
        }
    }

    setLayout(layout);
}

StructWidget::~StructWidget()
{
}

void StructWidget::toHolder(core::holder& holder) 
{
    const unsigned int count = m_reflective->get_children_count();

    for (unsigned int i = 0; i < count; i++) 
    {
        if (m_widgets[i])
        {
            core::holder child_holder(
                    m_reflective->get_child_value(holder, i));
            m_widgets[i]->toHolder(child_holder);
        }
    }
}

void StructWidget::fromHolder(core::holder& holder)
{
    const unsigned int count = m_reflective->get_children_count();

    for (unsigned int i = 0; i < count; i++) 
    {
        if (m_widgets[i])
        {
            core::holder child_holder(
                    m_reflective->get_child_value(holder, i));
            m_widgets[i]->fromHolder(child_holder);
        }
    }
}

SequenceWidget::SequenceWidget(core::reflective_base const * reflective,
        QWidget * parent) :
    QWidget(parent), ReflectiveWidgetBase(reflective)
{
    QVBoxLayout * layout = new QVBoxLayout;
 
    QHBoxLayout * headerLayout = new QHBoxLayout;
    m_sbLength = new QSpinBox;
    m_sbCurrentIndex = new QSpinBox;

    headerLayout->addWidget(new QLabel("Length"));
    headerLayout->addWidget(m_sbLength);

    headerLayout->addWidget(new QLabel("Index"));
    headerLayout->addWidget(m_sbCurrentIndex);

    layout->addLayout(headerLayout);

    m_stack = new QStackedWidget;
    layout->addWidget(m_stack);

    setLayout(layout);

    QObject::connect(m_sbLength, SIGNAL(valueChanged(int)),
            this, SLOT(lengthChanged(int)));
    QObject::connect(m_sbCurrentIndex, SIGNAL(valueChanged(int)),
            this, SLOT(indexChanged(int)));

    // TODO maximo razonable
    m_sbLength->setRange(0, 9999999);
    m_sbLength->setValue(0);
}

SequenceWidget::~SequenceWidget()
{
}

void SequenceWidget::toHolder(core::holder& holder) {}
void SequenceWidget::fromHolder(core::holder& holder){}

void SequenceWidget::lengthChanged(int len)
{
    int old_length = m_widgets.size();

    if (len > old_length)
    {
        for (int i = 0; i < len - old_length; i++) 
        {
            QWidget * w = createWidget(m_reflective->get_slice(), this);
            m_widgets.push_back(w);
            m_stack->addWidget(w);
        }
    }
    else
    {
        for (int i = len; i < old_length; i++) 
        {
            QWidget * w = m_widgets[i];
            m_stack->removeWidget(w);
        }

        m_widgets.resize(len);
    }

    m_sbCurrentIndex->setRange(0, len-1);
    m_sbLength->setValue(len);
}

void SequenceWidget::indexChanged(int idx)
{
    if (m_widgets.empty())
        return;

    m_stack->setCurrentIndex(idx);
}

ArrayWidget::ArrayWidget(core::reflective_base const * reflective,
        QWidget * parent) :
    QWidget(parent), ReflectiveWidgetBase(reflective)
{
    QVBoxLayout * layout = new QVBoxLayout;
 
    QHBoxLayout * headerLayout = new QHBoxLayout;
    m_sbCurrentIndex = new QSpinBox;

    headerLayout->addWidget(new QLabel("Index"));
    headerLayout->addWidget(m_sbCurrentIndex);

    layout->addLayout(headerLayout);

    m_stack = new QStackedWidget;
    layout->addWidget(m_stack);

    // TODO initialize stack
    
    setLayout(layout);

    QObject::connect(m_sbCurrentIndex, SIGNAL(valueChanged(int)),
            this, SLOT(indexChanged(int)));
}

ArrayWidget::~ArrayWidget()
{
}

void ArrayWidget::toHolder(core::holder& holder) {}
void ArrayWidget::fromHolder(core::holder& holder){}

void ArrayWidget::indexChanged(int idx)
{
    if (m_widgets.empty())
        return;

    m_stack->setCurrentIndex(idx);
}


OperationInputForm::OperationInputForm(
        core::operation_reflective_base const * reflective,
        QWidget * parent) :
    QWidget(parent), m_reflective(reflective)
{
    QGridLayout * layout = new QGridLayout(this);

    const unsigned int count = reflective->get_children_count();

    m_widgets.resize(count, NULL);

    for (unsigned int i = 0; i < count; i++) 
    {
        const core::direction_type type = 
            reflective->get_parameter_direction(i);

        if (type == core::DIRECTION_IN || type == core::DIRECTION_INOUT)
        {
            core::reflective_base const * child = 
                reflective->get_child(i);

            const char * child_name = reflective->get_child_name(i);

            QWidget * child_widget = createWidget(child, this);

            m_widgets[i] = dynamic_cast< ReflectiveWidgetBase* >(
                    child_widget);

            child_widget->setObjectName(child_name);

            if (child->is_primitive())
            {
                QLabel * label = new QLabel(child_name, this);

                label->setObjectName(QString(child_name) + "_label");

                layout->addWidget(label, i, 0);
                layout->addWidget(child_widget, i, 1);
            }
            else
            {
                QGroupBox * gb = new QGroupBox(child_name, this);
                gb->setObjectName(QString(child_name) + "_group");

                QHBoxLayout * cLayout = new QHBoxLayout(gb);

                cLayout->addWidget(child_widget);

                gb->setLayout(cLayout);
                layout->addWidget(gb, i, 0, 1, 2);
            }
        }
    }

    setLayout(layout);
}

OperationInputForm::~OperationInputForm()
{
}

corbasim::core::operation_reflective_base const * 
OperationInputForm::getReflective() const
{
    return m_reflective;
}

corbasim::event::request_ptr OperationInputForm::createRequest()
{
    event::request_ptr req (m_reflective->create_request());
    core::holder holder(m_reflective->get_holder(req));

    const unsigned int count = m_reflective->get_children_count();

    for (unsigned int i = 0; i < count; i++) 
    {
        if (m_widgets[i])
        {
            core::holder child_holder(
                    m_reflective->get_child_value(holder, i));
            m_widgets[i]->toHolder(child_holder);
        }
    }

    return req;
}

