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
    // Recherche le type correspondant
    int index = findPrototype(className);

    // Si le type existe
    if (index >= 0)
    {
        // Clone le type correspondant trouve
        return _prototypes[index]->clone();
    }

    // Le type n'existe pas, retourne NULL
    return 0;
}

/*static*/ int OPrototype::findPrototype(const char* className)
{
    if (className == 0) { return -1; }

    // Parcours le tableau de prototypes
    for (int i = 0; i < _nbPrototypes; i++)
    {
        // Compare le nom du type
        if (!strcmp(className, _prototypes[i]->getClassName()))
        {
            // Le type existe
            return i;
        }
    }

    // Le type n'existe pas
    return -1;
}

/*static*/ int OPrototype::registerPrototype(OPrototype* pProto)
{
    if (pProto == 0) { return -1; }

    // Si on n'a pas attend le max de prototypes
    if (_nbPrototypes < PROTOTYPE_MAX_NB)
    {
        // Ajoute le prototype
        _prototypes[_nbPrototypes++] = pProto;
    }

    return _nbPrototypes;
}

bool OPrototype::isA(const char* className) const
{
    // Test le nom du type
    return (!strcmp(className, this->getClassName()));
}

bool OPrototype::inherits(const char* className) const
{
    return OPrototype::isTypeOf(className);
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
