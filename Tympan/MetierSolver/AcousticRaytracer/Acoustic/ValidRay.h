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

#include "Tympan/MetierSolver/AcousticRaytracer/Ray/Ray.h"
#include "Tympan/MetierSolver/AcousticRaytracer/Geometry/Shape.h"

/*!
* \namespace ValidRay
* \brief Cet espace de nom regroupe l'ensemble des fonctions nécessaires à la validation d'un rayon.
*/
namespace ValidRay
{

bool validTriangleWithSpecularReflexion(Ray* r, Intersection* inter);
bool validCylindreWithDiffraction(Ray* r, Intersection* inter);

/*!
 * \fn computeRealImpact(Ray *r, Cylindre *cylindre, vec3& impact)
 * \brief Try to compute the intersection of the ray and the ridge
 * \brief Return false if not possible
 */
bool computeRealImpact(Ray *r, Intersection* inter, Cylindre *cylindre, vec3& impact);

/*!
 * \fn isRayClosestFromRidge(Ray *r, const vec3& impact, const vec3& realImpact);
 * \brief Test if ray passes near from the ridge
 */
bool isRayClosestFromRidge(Ray *r, const vec3& impact, const vec3& realImpact);


/*!
 * \fn pathDiffValidationForDiffraction(const Ray *r, const vec3& impact);
 * \brief Test if difference betwen total path length and direct path length is smaller than maxDif
 * \brief in case of diffraction
 */
bool pathDiffValidationForDiffraction(Ray *r, const vec3& impact);

/*!
 * \fn pathDiffValidationForReflection(const Ray *r, const vec3& impact);
 * \brief Test if difference betwen total path length and direct path length is smaller than maxDif
 * \brief in case of reflection
 */
bool pathDiffValidationForReflection(Ray * r, const vec3& impact);

/*!
 * \fn void computeCumulDistanceAndLocalOrigin(Ray *r, vec3& origin, const vec3& impact);
 * \brief search the neares event of type REFLECTION otherwise the source
 */
void computeCumulDistance(Ray *r, const vec3& impact);

#ifdef _ALLOW_TARGETING_
void appendDirectionToEvent(QSharedPointer<Event> e, TargetManager& targets);
#endif //_ALLOW_TARGETING_
};

#endif
