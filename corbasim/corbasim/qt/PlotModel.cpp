#include "PlotModel.hpp"
#include <iostream>
#include <boost/next_prior.hpp>

using namespace corbasim::qt;

PlotModel::PlotModel(QObject *parent)
    : QStandardItemModel(parent)
{
    QStringList headers;
    headers << "Plotable parameters";

    setHorizontalHeaderLabels(headers);
}

PlotModel::~PlotModel()
{
}

bool PlotModel::setData(const QModelIndex & index, 
        const QVariant& value, int role)
{
    // TODO get the current value
    
    bool res = QStandardItemModel::setData(index, value, role);

    if (role == Qt::CheckStateRole)
    {
        QList< int > path;

        QModelIndex parent = index;

        // Calculate path
        do {
            path.push_front(parent.row());
            parent = parent.parent();
        } while(parent.isValid());
        
        if (path.size() > 2)
        {
            const FirstLevelItem& item = 
                *boost::next(m_items.begin(), path.front());

            // Remove instance index
            path.pop_front();

            QStandardItem * mitem = itemFromIndex(index);

            if (mitem->checkState() == Qt::Checked)
                emit createdPlot(item.name, item.reflective, path);
            else
                emit deletedPlot(item.name, item.reflective, path);
        }
    }

    return res;
}

void insertRecursive(QStandardItem * parent, 
        corbasim::core::reflective_base const * reflective)
{
    const unsigned int count = reflective->get_children_count();

    for (unsigned int i = 0; i < count; i++) 
    {
        corbasim::core::reflective_base const * child =
            reflective->get_child(i);

        const corbasim::core::reflective_type type = child->get_type();

        bool isCheckable = false;

        QStandardItem * childItem = 
            new QStandardItem(reflective->get_child_name(i));
        childItem->setEditable(false);

        // Recursive
        if (type == corbasim::core::TYPE_STRUCT || 
                child->is_repeated())
        {
            insertRecursive(childItem, child);
        }

        // Just primitive types are plotables
        if (child->is_primitive() || 
                (child->is_repeated() && 
                 child->get_slice()->is_primitive()))
        {
            childItem->setCheckable(true);

            isCheckable = true;
        }

        // Removes innecessary items
        // Still necessary
        /*
        if (!isCheckable && !childItem->rowCount())
            delete childItem;
        else
        */
            parent->appendRow(childItem);
    }
}

void PlotModel::registerInstance(const QString& name,
        core::interface_reflective_base const * reflective)
{
    FirstLevelItem item;
    item.name = name;
    item.reflective = reflective;
    m_items.push_back(item);

    QStandardItem * ifItem = new QStandardItem(name);
    ifItem->setEditable(false);

    const unsigned int count = reflective->operation_count();

    for (unsigned int i = 0; i < count; i++) 
    {
        core::operation_reflective_base const * op =
            reflective->get_reflective_by_index(i);

        QStandardItem * opItem = new QStandardItem(op->get_name());
        opItem->setEditable(false);

        insertRecursive(opItem, op);

        /*
        if (opItem->rowCount())*/
            ifItem->appendRow(opItem);
        /*else
            delete opItem;*/
    }

    appendRow(ifItem);
}

void PlotModel::unregisterInstance(const QString& name)
{
    int i = 0;
    for (FirstLevelItems_t::iterator it = m_items.begin(); 
            it != m_items.end(); ++it, ++i) 
    {
        if (name == it->name)
        {
            removeRows(i, 1);

            m_items.erase(it);
            break;
        }
    }
}
void PlotModel::deletePlot(const QString& id, const QList< int >& path)
{
    // TODO optimize

    unsigned int i = 0;
    for (FirstLevelItems_t::const_iterator it = m_items.begin(); 
            it != m_items.end(); ++it, ++i) 
    {
        if (id == it->name)
        {
            // navigate until last element

            // Instance element
            QModelIndex idx = index(i, 0);
            
            for (int i = 0; i < path.size(); i++) 
            {
                idx = index(path[i], 0, idx);
            }

            QStandardItem * item = itemFromIndex(idx);

            item->setCheckState(Qt::Unchecked);

            break;
        }
    }
}
