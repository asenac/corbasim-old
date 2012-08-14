// -*- mode: c++; c-basic-style: "bsd"; c-basic-offset: 4; -*-
/*
 * file_format_helper.cpp
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

#include "file_format_helper.hpp"
#include <corbasim/json/reflective.hpp>

using namespace corbasim::core;

file_format_factory::file_format_factory()
{
}

file_format_factory::~file_format_factory()
{
}

const file_format_factory* file_format_factory::get_instance()
{
    static file_format_factory instance;
    return &instance;
}

const file_format_helper* 
file_format_factory::get_helper(file_format format) const
{
    switch(format)
    {
        case FILE_FORMAT_JSON:
            return json_file_format_helper::get_instance();

        case FILE_FORMAT_TEXT:
            return text_file_format_helper::get_instance();

        case FILE_FORMAT_BINARY:
            return binary_file_format_helper::get_instance();

        default:
            break;
    }

    return NULL;
}

file_format_helper::file_format_helper()
{
}

file_format_helper::~file_format_helper()
{
}

// json

json_file_format_helper::json_file_format_helper()
{
}

json_file_format_helper::~json_file_format_helper()
{
}

bool json_file_format_helper::load(
        std::istream& is, 
        reflective_base const * reflective, 
        holder h) const
{
    // TODO
    return true;
}

bool json_file_format_helper::save(
        std::ostream& os, 
        reflective_base const * reflective, 
        holder h) const
{
    try 
    {
        json::std_writer_t writer(os, true);
        json::write(writer, reflective, h);
        os << std::endl;
    } 
    catch (...) 
    {
        return false;
    }

    return true;
}

const json_file_format_helper* 
json_file_format_helper::get_instance()
{
    static json_file_format_helper instance;
    return &instance;
}

// text

text_file_format_helper::text_file_format_helper()
{
}

text_file_format_helper::~text_file_format_helper()
{
}

bool text_file_format_helper::load(
        std::istream& is, 
        reflective_base const * reflective, 
        holder h) const
{
    return true;
}

bool text_file_format_helper::save(
        std::ostream& os, 
        reflective_base const * reflective, 
        holder h) const
{
    try
    {
        switch(reflective->get_type())
        {
#define DUMP_TYPE(__TYPE__, cpptype)                                        \
            case TYPE_##__TYPE__:                                           \
                os << h.to_value< cpptype >();                              \
                break;                                                      \
            /***/

            DUMP_TYPE(BOOL, bool);
            DUMP_TYPE(OCTET, unsigned char);
            DUMP_TYPE(CHAR, char);
            DUMP_TYPE(SHORT, short);
            DUMP_TYPE(USHORT, unsigned short);
            DUMP_TYPE(LONG, int32_t);
            DUMP_TYPE(ULONG, uint32_t);
            DUMP_TYPE(LONGLONG, int64_t);
            DUMP_TYPE(ULONGLONG, uint64_t);
            DUMP_TYPE(FLOAT, float);
            DUMP_TYPE(DOUBLE, double);

#undef DUMP_TYPE

            case TYPE_ARRAY:
            case TYPE_SEQUENCE:
                {
                    const unsigned int count = 
                        reflective->get_length(h);

                    if (reflective->is_variable_length())
                    {
                        // TODO save length
                    }

                    for (unsigned int i = 0; i < count; i++) 
                    {
                        holder child_value = reflective->get_child_value(h, i);
                        save(os, reflective->get_child(i), child_value);
                        
                        os << ' ';
                    }
                }
                break;

            case TYPE_STRUCT:
                {
                    const unsigned int count = 
                        reflective->get_children_count();

                    for (unsigned int i = 0; i < count; i++) 
                    {
                        holder child_value = reflective->get_child_value(h, i);
                        save(os, reflective->get_child(i), child_value);

                        os << ' ';
                    }
                }
                break;

            default:
                break;
        }
    }
    catch(...)
    {
        return false;
    }

    return true;
}

const text_file_format_helper* 
text_file_format_helper::get_instance()
{
    static text_file_format_helper instance;
    return &instance;
}

// binary

binary_file_format_helper::binary_file_format_helper()
{
}

binary_file_format_helper::~binary_file_format_helper()
{
}

bool binary_file_format_helper::load(
        std::istream& is, 
        reflective_base const * reflective, 
        holder h) const
{
    return true;
}

bool binary_file_format_helper::save(
        std::ostream& os, 
        reflective_base const * reflective, 
        holder h) const
{
    try
    {
        switch(reflective->get_type())
        {
#define DUMP_TYPE(__TYPE__, cpptype)                                        \
            case TYPE_##__TYPE__:                                           \
                os.write((const char *)&h.to_value< cpptype >(),            \
                        sizeof(cpptype));                                   \
                break;                                                      \
            /***/

            DUMP_TYPE(BOOL, bool);
            DUMP_TYPE(OCTET, unsigned char);
            DUMP_TYPE(CHAR, char);
            DUMP_TYPE(SHORT, short);
            DUMP_TYPE(USHORT, unsigned short);
            DUMP_TYPE(LONG, int32_t);
            DUMP_TYPE(ULONG, uint32_t);
            DUMP_TYPE(LONGLONG, int64_t);
            DUMP_TYPE(ULONGLONG, uint64_t);
            DUMP_TYPE(FLOAT, float);
            DUMP_TYPE(DOUBLE, double);

#undef DUMP_TYPE

            case TYPE_ARRAY:
            case TYPE_SEQUENCE:
                {
                    const unsigned int count = 
                        reflective->get_length(h);

                    if (reflective->is_variable_length())
                    {
                        // TODO save length
                    }

                    for (unsigned int i = 0; i < count; i++) 
                    {
                        holder child_value = reflective->get_child_value(h, i);
                        save(os, reflective->get_child(i), child_value);
                    }
                }
                break;

            case TYPE_STRUCT:
                {
                    const unsigned int count = 
                        reflective->get_children_count();

                    for (unsigned int i = 0; i < count; i++) 
                    {
                        holder child_value = reflective->get_child_value(h, i);
                        save(os, reflective->get_child(i), child_value);
                    }
                }
                break;

            default:
                break;
        }
    }
    catch(...)
    {
        return false;
    }

    return true;
}

const binary_file_format_helper* 
binary_file_format_helper::get_instance()
{
    static binary_file_format_helper instance;
    return &instance;
}


