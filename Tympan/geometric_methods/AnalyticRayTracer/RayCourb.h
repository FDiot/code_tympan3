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

#ifndef __RAYCOURB_H
#define __RAYCOURB_H

#include <vector>
#include <map>
#include "Tympan/models/common/mathlib.h"
#include "Step.h"

using namespace std;
/**
 * \brief Class to describe a ray curve
 */
class RayCourb
{
public:
	/// Constructor
    RayCourb();
    /// Copy constructor
    RayCourb(const vec3& a);
    RayCourb(const RayCourb& r);
    /// Destructor
    ~RayCourb() {}

    /// Clear all the arrays
    void purge();

    /// Set the steps vector size
    void setSize(const unsigned int taille) { etapes.reserve(taille); }

    /// Operator=
    RayCourb& operator= (const RayCourb& P);

public:
    vector<Step> etapes;                         //!< Time steps vector
    int nbReflex;                                //!< Reflections number
    vector<int> position;                        //!< List of the indices of points where reflection happens (time step number)
    map<int, int> rencontre;                     //!< Tuple (time step, encountered face)
};
#endif //__RAYCOURB_H

