/*
 * Copyright (C) <2012> <EDF-R&D> <FRANCE>
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
*/

/*
 *
 */




#include "OPrototype.h"
#include "Tympan/MetierSolver/CommonTools/exceptions.hpp"
#include <string.h>

// Declaration des membres statiques.
OPrototype* OPrototype::_prototypes[];
int         OPrototype::_nbPrototypes = 0;
std::unordered_map<std::string, OPrototype::IOProtoFactory::ptr_type> OPrototype::_factory_map;

OPrototype::OPrototype()
{
}

OPrototype::~OPrototype()
{
}

/* static */ void OPrototype::add_factory(const char * classname,
        IOProtoFactory::ptr_type factory)
{
    _factory_map[std::string(classname)] = move(factory);
}

/*static*/ OPrototype* OPrototype::findAndClone(const char* className)
{
    std::unordered_map<std::string, IOProtoFactory::ptr_type>::const_iterator it =
        _factory_map.find(className);

    if (it == _factory_map.end())
    {
        std::string err_msg ("Asked to clone class ");
        err_msg.append (className);
        err_msg.append (" which isn't registered in OPrototype.");
        throw tympan::invalid_data(err_msg) << tympan_source_loc
            << tympan::oproto_classname_errinfo(className);
    }
    else
    {
        return _factory_map[className]->make().release();
    }
}

/*static*/ int OPrototype::findPrototype(const char* className)
{
    if (className == 0) { return -1; }
    // Return 1 if the class className exists, -1 otherwise
    return (_factory_map.count(className) > 0 ? 1 : -1);
}

bool OPrototype::isA(const char* className) const
{
    // Test le nom du type
    return (!strcmp(className, this->getClassName()));
}

/*static*/ bool OPrototype::isTypeOf(const char* className)
{
    if (!strcmp(className, "OPrototype"))
    {
        return true;
    }
    return false;
}

/*static*/ OPrototype* OPrototype::safeDownCast(OPrototype* pObject)
{
    return (OPrototype*) pObject;
}
