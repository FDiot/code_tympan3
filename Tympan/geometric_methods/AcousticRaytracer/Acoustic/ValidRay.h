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
 * \brief Return True if ray passes near enough the ridge for it be in the thickness of the ray
 */
bool isRayPassesNearRidge(Ray *r, const vec3& impact, const vec3& realImpact);


/*!
 * \brief Tests if the addition of a diffraction event at position impact does not cause the pathDifference to exceed MaxPathDifference.
 *  (NB: L = length of the ray from source to impact
 *       l = length of the ray from source to impact taking a direct path between reflexions while ignoring diffractions
 *		 pathDifference = L- l
 *  
 */
bool pathDiffValidationForDiffraction(Ray *r, const vec3& impact);

/*!
 * \brief Tests if the addition of a reflection event at position impact does not cause the pathDifference to exceed MaxPathDifference.
 *  (NB: L = length of the ray from source to impact
 *       l = length of the ray from source to impact taking a direct path between reflexions while ignoring diffractions
 *		 pathDifference = L- l
 */
bool pathDiffValidationForReflection(Ray * r, const vec3& impact);

/*!
 * \brief adds the distance between impact and the last REFLECTION/SOURCE of the ray to the cumulDistance of the ray
 */
void computeCumulDistance(Ray *r, const vec3& impact);

#ifdef _ALLOW_TARGETING_
void appendDirectionToEvent(std::shared_ptr<Event> e, TargetManager& targets);
#endif //_ALLOW_TARGETING_
};

#endif
