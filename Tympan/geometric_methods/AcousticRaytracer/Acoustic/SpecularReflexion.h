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

#ifndef SPECULARREFLEXION_H
#define SPECULARREFLEXION_H

#include "Event.h"
/**
 * \brief Specular reflection class Event
 */
class SpecularReflexion : public Event
{

public:
	/// Default constructor
    SpecularReflexion(const vec3& position = vec3(0.0, 0.0, 0.0), const vec3& incomingDirection = vec3(0.0, 0.0, 0.0), Shape* _shape = NULL):
        Event(position, incomingDirection, _shape) { nbResponseLeft = initialNbResponse = 1; type = SPECULARREFLEXION;}
    /// Copy constructor
    SpecularReflexion(const SpecularReflexion& other) : Event(other)
    {
        type = SPECULARREFLEXION;
    }
    /// Destructor
    virtual ~SpecularReflexion()
    {

    };

    virtual bool getResponse(vec3& r, bool force = false);

    virtual bool isAcceptableResponse(vec3& test)
    {
        return false;
    }
    /// Get incident angle
    virtual double getAngle();
};

#endif
