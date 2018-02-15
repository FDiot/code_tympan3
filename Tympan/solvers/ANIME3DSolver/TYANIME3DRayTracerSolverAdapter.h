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

#ifndef TYANIME3D_RAYTRACER_SOLVER_ADAPTER_H
#define TYANIME3D_RAYTRACER_SOLVER_ADAPTER_H

#include "Tympan/geometric_methods/AcousticRaytracer/Tools/SelectorManager.h"
#include "Tympan/geometric_methods/AcousticRaytracer/Acoustic/Solver.h"

/**
* \brief API class to run the ray tracer for the ANIME3D solver
*/
class TYANIME3DRayTracerSolverAdapter : public Solver
{

public:
    virtual bool postTreatmentScene(Scene* scene, std::vector<Source>& sources, std::vector<Recepteur>& recepteurs);

    //virtual double leafTreatment(KdTree *kdtree, BVH* bvh, Ray *r, vector<struct Isect> &primitives);

    /**
    * \brief validate intersections in function of the type of the intersected primitive
    **/
    virtual bool valideIntersection(Ray* r, Intersection* inter);

    /**
    * \brief validate a ray by filtering it with the selectors for acceptance or rejection
    **/
    virtual bool valideRayon(Ray* r);

    /**
    * \brief delete a ray or add it to the list of debug rays
    **/
    virtual bool invalidRayon(Ray* r);

    // * \fn vector<Ray*>* getValidRays()
    // * \brief Return the list of valid rays
    // * \return Pointer to the vector containing the rays validated by the solver
    // */
    // vector<Ray*>* getValidRays(){ return &valid_rays;}

    virtual void finish();

    //virtual void clean();

    bool _useFresnelArea; //!< Flag to use Fresnel weighting


protected:
    SelectorManager<Ray> selectorManagerIntersection;
    SelectorManager<Ray> selectorManagerValidation;

#ifdef _ALLOW_TARGETING_
protected :
    TargetManager targetManager;
#endif //_ALLOW_TARGETING_
};

#endif //TYANIME3D_RAYTRACER_SOLVER_ADAPTER_H
