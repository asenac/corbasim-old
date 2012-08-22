// -*- mode: c++; c-basic-style: "bsd"; c-basic-offset: 4; -*-
/*
 * ObjrefView.cpp
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

#include "ObjrefView.hpp"

using namespace corbasim::app;

ObjrefView::ObjrefView(QMdiArea * area,
        Objref_ptr objref,
        QObject * parent) :
    QObject(parent), m_mdi_area(area), m_objref(objref),
    m_sub_script(NULL), 
    m_script(NULL)
{
    InterfaceDescriptor_ptr factory = objref->interface();
    const QString& name = objref->name();

    const QString menu_entry = QString("%1 (%2)")
        .arg(name).arg(factory->get_fqn());

    m_menu = new QMenu(menu_entry);
    // Takes the ownership
    QMenu * operation = m_menu->addMenu("Operations");
    QMenu * sender = m_menu->addMenu("Senders");

    unsigned int count = factory->operation_count();

    // Inicializa los dialogos a nulo
    m_dialogs.resize(count, NULL);
    m_senders.resize(count, NULL);
    m_subwindows.resize(count, NULL);
    m_subwindows_senders.resize(count, NULL);

    for (unsigned int i = 0; i < count; i++) 
    {
        OperationDescriptor_ptr op = 
            factory->get_reflective_by_index(i);

        const char * name = op->get_name();

        operation->addAction(name)->setData(i);
        sender->addAction(name)->setData(i);
    }

    connect(operation,
            SIGNAL(triggered(QAction*)),
            this,
            SLOT(showRequestDialog(QAction*)));

    connect(sender,
            SIGNAL(triggered(QAction*)),
            this,
            SLOT(showSenderDialog(QAction*)));

    m_menu->addAction("&Script editor", this, SLOT(showScriptEditor()));
    m_menu->addAction("Set &reference", this, SLOT(showSetReference()));
    m_menu->addSeparator();
    m_menu->addAction("&Delete", this, SLOT(deleteObjref()));

#if 0
    // Set reference
    m_sub_reference = new QMdiSubWindow();
    ReferenceValidatedWidget * w = 
        new ReferenceValidatedWidget(
                factory->create_validator());
    m_reference = w;
    m_sub_reference->setWidget(m_reference);
    m_mdi_area->addSubWindow(m_sub_reference);
    m_sub_reference->hide();
    m_sub_reference->setWindowTitle(QString("%1: reference").arg(name));


    connect(w, SIGNAL(updatedReference(CORBA::Object_var)),
            this, SLOT(slotUpdateReference(const CORBA::Object_var&)));
#endif
}

ObjrefView::~ObjrefView()
{
    m_menu->deleteLater();

    for (unsigned int i = 0; i < m_dialogs.size(); i++) 
    {
        delete m_dialogs[i];
        delete m_subwindows[i];
    }

    delete m_script;
    delete m_sub_script;
}

QMenu * ObjrefView::getMenu() const
{
    return m_menu;
}

InterfaceDescriptor_ptr ObjrefView::getFactory() const
{
    return m_objref->interface();
}

void ObjrefView::deleteObjref()
{
    emit deleteObjref(m_objref->id());
}

void ObjrefView::showRequestDialog(int idx)
{
    QMdiSubWindow * w = getRequestWindow(idx);
    w->showNormal();
    getRequestDialog(idx)->show();
    m_mdi_area->setActiveSubWindow(w);
}

void ObjrefView::showRequestDialog(QAction * act)
{
    showRequestDialog(act->data().toInt());
}

void ObjrefView::showSenderDialog(int idx)
{
    QMdiSubWindow * w = getSenderWindow(idx);
    w->showNormal();
    getSenderDialog(idx)->show();
    m_mdi_area->setActiveSubWindow(w);
}

void ObjrefView::showSenderDialog(QAction * act)
{
    showSenderDialog(act->data().toInt());
}

QMdiSubWindow * ObjrefView::getRequestWindow(int idx)
{
    QMdiSubWindow * win = m_subwindows[idx];

    if (!win)
    {
        m_subwindows[idx] = m_mdi_area->addSubWindow(getRequestDialog(idx));
        
        // Window title
        const QString title = QString("%1: %2").arg(m_objref->name()).arg(
            m_objref->interface()->get_reflective_by_index(idx)->get_name());

        m_subwindows[idx]->setWindowTitle(title);
        win = m_subwindows[idx];
    }

    return win;
}

RequestDialog * ObjrefView::getRequestDialog(int idx)
{
    RequestDialog * dlg = m_dialogs[idx];

    if (!dlg)
    {
        OperationDescriptor_ptr op = 
            m_objref->interface()->get_reflective_by_index(idx);

        const char * name = op->get_name();

        dlg = new RequestDialog(op);
        dlg->setWindowTitle(name);

        connect(dlg,
            SIGNAL(sendRequest(Request_ptr)),
            m_objref.get(), 
            SLOT(sendRequest(Request_ptr)));

        m_dialogs[idx] = dlg;
    }

    return dlg;
}

QMdiSubWindow * ObjrefView::getSenderWindow(int idx)
{
    QMdiSubWindow * win = m_subwindows_senders[idx];

    if (!win)
    {
        m_subwindows_senders[idx] = m_mdi_area->addSubWindow(getSenderDialog(idx));
        
        // Window title
        const QString title = QString("%1: %2").arg(m_objref->name()).arg(
            m_objref->interface()->get_reflective_by_index(idx)->get_name());

        m_subwindows_senders[idx]->setWindowTitle(title);
        win = m_subwindows_senders[idx];
    }

    return win;
}

OperationSender * ObjrefView::getSenderDialog(int idx)
{
    OperationSender * dlg = m_senders[idx];

    if (!dlg)
    {
        OperationDescriptor_ptr op = 
            m_objref->interface()->get_reflective_by_index(idx);

        const char * name = op->get_name();

        dlg = new OperationSender(m_objref);
        dlg->initialize(op);
        dlg->setWindowTitle(name);

        m_senders[idx] = dlg;
    }

    return dlg;
}


void ObjrefView::showScriptEditor()
{
    if (!m_sub_script)
    {
        m_script = new SimpleScriptEditor();
        m_script->initialize(m_objref->interface());

        m_sub_script = m_mdi_area->addSubWindow(m_script);
        m_sub_script->setWindowTitle(
                QString("%1: Script editor").arg(m_objref->name()));

        connect(m_script,
            SIGNAL(sendRequest(Request_ptr)),
            m_objref.get(), 
            SLOT(sendRequest(Request_ptr)));
    }
    m_sub_script->showNormal();
    m_script->show();
    m_mdi_area->setActiveSubWindow(m_sub_script);
}

void ObjrefView::showSetReference()
{
#if 0
    m_sub_reference->showNormal();
    m_reference->show();
    m_mdi_area->setActiveSubWindow(m_sub_reference);
#endif
}

// Settings
void ObjrefView::save(QVariant& settings) 
{
    QVariantMap map;
    
    map["fqn"] = m_objref->interface()->get_fqn();

    {
        QVariantList list;

        for (unsigned int i = 0; i < m_dialogs.size(); i++) 
        {
            if (m_dialogs[i])
            {
                QVariantMap op;
                op["operation"] = m_dialogs[i]->getReflective()->get_name();
                m_dialogs[i]->save(op["value"]);
                list << op;
            }
        }

        map["dialogs"] = list;
    }
    {
        QVariantList list;

        for (unsigned int i = 0; i < m_senders.size(); i++) 
        {
            if (m_senders[i])
            {
                QVariantMap op;
                op["operation"] = m_senders[i]->getReflective()->get_name();
                m_senders[i]->save(op["value"]);
                list << op;
            }
        }

        map["senders"] = list;
    }

    settings = map;
}

void ObjrefView::load(const QVariant& settings) 
{
    const QVariantMap map = settings.toMap();

    if (map.contains("dialogs"))
    {
        const QVariantList list = map.value("dialogs").toList();

        for (QVariantList::const_iterator it = list.begin(); 
                it != list.end(); ++it) 
        {
            const QVariantMap map = it->toMap();

            if (map.contains("operation") && map.contains("value"))
            {
                // find the index
                unsigned int i = 0;
                bool found = false;
                for (; !found && i < m_objref->interface()->operation_count(); 
                        i++) 
                {
                    found = (m_objref->interface()->get_reflective_by_index(i)->get_name() 
                            == map.value("operation").toString());
                }

                // load its saved value
                if (found)
                {
                    getRequestDialog(i)->load(map.value("value"));
                }
            }
        }
    }
    if (map.contains("senders"))
    {
        const QVariantList list = map.value("senders").toList();

        for (QVariantList::const_iterator it = list.begin(); 
                it != list.end(); ++it) 
        {
            const QVariantMap map = it->toMap();

            if (map.contains("operation") && map.contains("value"))
            {
                // find the index
                unsigned int i = 0;
                bool found = false;
                for (; !found && i < m_objref->interface()->operation_count(); 
                        i++) 
                {
                    found = (m_objref->interface()->get_reflective_by_index(i)->get_name() 
                            == map.value("operation").toString());
                }

                // load its saved value
                if (found)
                {
                    getRequestDialog(i)->load(map.value("value"));
                }
            }
        }
    }
}

