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

#ifndef BASE_H
#define BASE_H

#include <string>

/**
 * \brief Base class of Event, Material, PostFilter, Ray, Repere, Scene, Shape, Simulation, Source
 */
class Base
{

public:
	/// Default constructor
    Base()
    {
        name = "unkown element";
    }
    /// Copy constructor
    Base(const Base& other)
    {
        name = other.name;
    }
    /// Destructor
    virtual ~Base()
    {
    }
    /// Get the name of the object
    std::string getName() { return name; }
    /// Set the name of the object
    void setName(const std::string& _name) { name = _name; }

protected:
    std::string name; //!< Each instantiated object may be named
};

#endif
