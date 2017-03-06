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

#ifndef DO_NOTHING_H
#define DO_NOTHING_H

#include "Event.h"

/**
 * \brief Event that is only a point in a ray path with no direction change
 */
class DoNothing : public Event
{
public:
	/// Constructors
    DoNothing(const vec3& position = vec3(0.0, 0.0, 0.0), const vec3& incomingDirection = vec3(0.0, 0.0, 0.0), Shape* _shape = NULL):
        Event(position, incomingDirection, _shape) { nbResponseLeft = initialNbResponse = 1; type = NOTHING;}

    DoNothing(const DoNothing& other) : Event(other)
    {
        type = NOTHING;
    }
    /// Destructor
    virtual ~DoNothing()
    {

    };

    virtual bool getResponse(vec3& r, bool force = false)
	{
		nbResponseLeft--;
	
		r = from;

		return (nbResponseLeft >= 0);
	}

    virtual bool isAcceptableResponse(vec3& test)
    {
        return false;
    }
};

#endif // DO_NOTHING_H
