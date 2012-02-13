// -*- mode: c++; c-basic-style: "bsd"; c-basic-offset: 4; -*-
/*
 * qvariant.cpp
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

#include "qvariant.hpp"

#include <corbasim/core/reference_repository.hpp>

namespace corbasim 
{
namespace reflective_gui 
{

    QVariant toQVariant(
            corbasim::core::reflective_base const * reflective,
            corbasim::core::holder& hold)
    {
        using namespace corbasim::core;

        const reflective_type type = reflective->get_type();

        switch(type)
        {
            case TYPE_STRUCT:
                {
                    QVariantMap res;

                    const unsigned int count = reflective->get_children_count();

                    for (unsigned int i = 0; i < count; i++) 
                    {
                        core::holder child = reflective->get_child_value(hold, i);

                        res.insert(reflective->get_child_name(i),
                                toQVariant(reflective->get_child(i),
                                    child));
                    }

                    return res;
                }
            case TYPE_ARRAY:
            case TYPE_SEQUENCE:
                {
                    QVariantList res;

                    const unsigned int length = reflective->get_length(hold);
                    core::reflective_base const * slice = reflective->get_slice();

                    for (unsigned int i = 0; i < length; i++) 
                    {
                        core::holder child = reflective->get_child_value(hold, i);

                        res.push_back(toQVariant(slice, child));
                    }

                    return res;
                }
            case TYPE_BOOL:
                return QVariant(hold.to_value< bool >());
            case TYPE_OCTET:
                return QVariant(hold.to_value< unsigned char >());
            case TYPE_CHAR:
                return QVariant(hold.to_value< char >());
            case TYPE_SHORT:
                return QVariant(hold.to_value< short >());
            case TYPE_USHORT:
                return QVariant(hold.to_value< unsigned short >());
            case TYPE_LONG:
                return QVariant(hold.to_value< int32_t >());
            case TYPE_ULONG:
                return QVariant(hold.to_value< uint32_t >());
            case TYPE_LONGLONG:
                return QVariant((qint64) hold.to_value< int64_t >());
            case TYPE_ULONGLONG:
                return QVariant((quint64) hold.to_value< uint64_t >());

            case TYPE_STRING:
            case TYPE_WSTRING:
                {
                    std::string str(reflective->to_string(hold));
                    return QVariant(str.c_str());
                }
            case TYPE_OBJREF:
                {
                    objrefvar_reflective_base const * objref = 
                        static_cast< objrefvar_reflective_base const * >(
                                reflective);

                    CORBA::Object_var obj = objref->to_object(hold);

                    if (CORBA::is_nil(obj))
                        return QVariant("NIL");
                    
                    try {
                        reference_repository * rr =
                            reference_repository::get_instance();

                        CORBA::String_var str = 
                            rr->object_to_string(obj);

                        return QVariant(str.in());

                    } catch (...) {
                        return QVariant("NIL");
                    }

                }
                break;

            case TYPE_ENUM:
                {
                    // Maybe it works...
                    int32_t value = hold.to_value< int32_t >();

                    const char * str = "Unknown value"; 

                    if (value >= 0 && value < reflective->get_children_count())
                        str = reflective->get_child_name((unsigned int) value);

                    return QVariant(str);
                }

            case TYPE_DOUBLE:
                return QVariant(hold.to_value< double >());
            case TYPE_FLOAT:
                return QVariant(hold.to_value< float >());
            default:
                break;
        }
       
        return QVariant();
    }


    bool fromQVariant(
            corbasim::core::reflective_base const * reflective,
            corbasim::core::holder& hold,
            const QVariant& var)
    {
        using namespace corbasim::core;

        const reflective_type type = reflective->get_type();

        switch(type)
        {

            case TYPE_STRUCT:
                {
                    if (!var.canConvert(QVariant::Map)) return false;

                    QVariantMap map = var.toMap();

                    const unsigned int count = reflective->get_children_count();

                    for (unsigned int i = 0; i < count; i++) 
                    {
                        QVariantMap::const_iterator it = 
                            map.find(reflective->get_child_name(i));

                        if (it != map.end())
                        {
                            core::holder child = reflective->get_child_value(hold, i);
                            
                            if (!fromQVariant(reflective->get_child(i), child, it.value()))
                                return false;
                        }
                    }

                    return true;
                }

            case TYPE_ARRAY:
            case TYPE_SEQUENCE:
                {
                    if (!var.canConvert(QVariant::List)) return false;

                    QVariantList list = var.toList();

                    unsigned int length = (unsigned int) list.size();

                    if (reflective->is_variable_length())
                    {
                        reflective->set_length(hold, length);
                    }
                    else if (reflective->get_length(hold) < length)
                    {
                        // length = reflective->get_length(hold);
                        // Overflow
                        return false;
                    }

                    core::reflective_base const * slice = reflective->get_slice();

                    for (unsigned int i = 0; i < length; i++) 
                    {
                        core::holder child = reflective->get_child_value(hold, i);
                        
                        if (!fromQVariant(slice, child, list.at(i)))
                            return false;
                    }

                    return true;
                }

            case TYPE_BOOL:
                hold.to_value< bool >() = var.toBool();
                return true;

#if 0
            case TYPE_OCTET:
                return QVariant(hold.to_value< unsigned char >());
            case TYPE_CHAR:
                return QVariant(hold.to_value< char >());
            case TYPE_SHORT:
                return QVariant(hold.to_value< short >());
            case TYPE_USHORT:
                return QVariant(hold.to_value< unsigned short >());
            case TYPE_LONG:
                return QVariant(hold.to_value< int32_t >());
            case TYPE_ULONG:
                return QVariant(hold.to_value< uint32_t >());
            case TYPE_LONGLONG:
                return QVariant((qint64) hold.to_value< int64_t >());
            case TYPE_ULONGLONG:
                return QVariant((quint64) hold.to_value< uint64_t >());
#endif

            case TYPE_STRING:
            case TYPE_WSTRING:
                {
                    bool canConvert = var.canConvert(QVariant::String);

                    if (canConvert)
                        reflective->from_string(hold, 
                                var.toString().toStdString());

                    return canConvert;
                }

#if 0
            case TYPE_OBJREF:
                {
                    objrefvar_reflective_base const * objref = 
                        static_cast< objrefvar_reflective_base const * >(
                                reflective);

                    CORBA::Object_var obj = objref->to_object(hold);

                    if (CORBA::is_nil(obj))
                        return QVariant("NIL");
                    
                    try {
                        reference_repository * rr =
                            reference_repository::get_instance();

                        CORBA::String_var str = 
                            rr->object_to_string(obj);

                        return QVariant(str.in());

                    } catch (...) {
                        return QVariant("NIL");
                    }

                }
                break;

            case TYPE_ENUM:
                {
                    // Maybe it works...
                    int32_t value = hold.to_value< int32_t >();

                    const char * str = "Unknown value"; 

                    if (value >= 0 && value < reflective->get_children_count())
                        str = reflective->get_child_name((unsigned int) value);

                    return QVariant(str);
                }
#endif

            case TYPE_DOUBLE:
                {
                    bool canConvert = var.canConvert(QVariant::Double);

                    if (canConvert)
                        hold.to_value< double >() = var.toDouble();

                    return canConvert;
                }
            case TYPE_FLOAT:
                {
                    bool canConvert = var.canConvert(QVariant::Double);

                    if (canConvert)
                        hold.to_value< float >() = var.toFloat();

                    return canConvert;
                }

            default:
                break;
        }

        return false;
    }


} // namespace reflective_gui
} // namespace corbasim
