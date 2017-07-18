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

#ifndef VALIDRAY_H
#define VALIDRAY_H

#include "Ray/Ray.h"
#include "Geometry/Shape.h"

/*!
* \namespace ValidRay
* \brief This namespace contains all the functions needed for a ray validation.
*/
namespace ValidRay
{

bool validTriangleWithSpecularReflexion(Ray* r, Intersection* inter);
bool validCylindreWithDiffraction(Ray* r, Intersection* inter);
bool validRayWithDoNothingEvent(Ray *r, Intersection* inter);

/*!
 * \brief Try to compute the intersection of the ray and the ridge
 * \return False if not possible
 */
bool computeRealImpact(Ray *r, Intersection* inter, Cylindre *cylindre, vec3& impact);

/*!
 * \brief Return True if ray passes near from the ridge
 */
bool isRayClosestFromRidge(Ray *r, const vec3& impact, const vec3& realImpact);


/*!
 * \brief Test if difference between total path length and direct path length is smaller than maxDif
 * in case of diffraction.
 */
bool pathDiffValidationForDiffraction(Ray *r, const vec3& impact);

/*!
 * \brief Test if difference between total path length and direct path length is smaller than maxDif
 *  in case of reflection
 */
bool pathDiffValidationForReflection(Ray * r, const vec3& impact);

/*!
 * \brief Search the nearest event of type REFLECTION otherwise the source
 */
void computeCumulDistance(Ray *r, const vec3& impact);

#ifdef _ALLOW_TARGETING_
void appendDirectionToEvent(std::shared_ptr<Event> e, TargetManager& targets);
#endif //_ALLOW_TARGETING_
};

#endif
