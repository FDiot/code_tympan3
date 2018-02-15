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

#ifndef LEAF_TREATMENT_H
#define LEAF_TREATMENT_H

#include "Geometry/Shape.h"

#include <list>

/**
 * \brief Leaf treatment
 */
namespace leafTreatment
{

enum treatment
{
    FIRST = 0,          //!< FIRST
    ALL_BEFORE_TRIANGLE,//!< ALL_BEFORE_TRIANGLE
    ALL_BEFORE_VISIBLE, //!< ALL_BEFORE_VISIBLE
    ALL                 //!< ALL
};

// This function is only used by grid accelerator
decimal keepFunction(treatment choice, std::list<Intersection> &currentIntersections, decimal currentTmin);

/*!
 * \brief Keep only the first intersection encountered before reaching currentTmin and return its corresponding tmin
 */
decimal keepFirst(std::list<Intersection> &currentIntersections, decimal currentTmin);

/*!
 * \brief Keep all intersections encountered before intersecting a visible shape and before reaching currentTmin, and return the tmin of the intersection with the triangle
 * (only the cylinders used for diffraction edge are considered invisible)
 */
decimal keepAllBeforeTriangle(std::list<Intersection> &currentIntersections, decimal currentTmin);

/*!
 * \brief Keep all intersections encountered before intersecting a triangle and before reaching currentTmin, and return the tmin of the intersection with the triangle
 */
decimal keepAllBeforeVisible(std::list<Intersection> &currentIntersections, decimal currentTmin);

/*!
 * \brief Keep all intersections before reaching currentTmin and return the tmin of the first one encountered
 */
decimal keepAll(std::list<Intersection> &currentIntersections, decimal currentTmin);

};

#endif
