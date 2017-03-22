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

#ifndef SOLVER_H
#define SOLVER_H

#include <vector>
#include <deque>

#include "Tympan/geometric_methods/AcousticRaytracer/Geometry/Scene.h"
#include "Tympan/geometric_methods/AcousticRaytracer/Ray/Ray.h"
#include "Recepteur.h"

/*!
* \file Solver.h
* \class Solver
* \brief The Solver class gives an interface to the developer to add easily a new acoustic method using ray tracing.
* This class provides a way for the programmer to interact at each point of the ray tracing algorithm. It is possible
* to add several acoustic objects to model phenomena like diffraction. The features of this class are:
* + post-treatment of the scene
* + leaf treatment in accelerators
* + intersection validation (generation of the proper event)
* + ray validation and export
* \warning The developer should check the consistency of the functions used in the library. For instance, if he chooses to post-process the scene
* by adding diffraction edges without defining the validation function for these edges, then each intersection with an edge will be ignored.
*/

class Solver
{
public:
    /// Default constructor
    Solver()
    {
    }

    /// Copy constructor
    Solver(Solver* _solver)
    {
    }

    /*!
    * \brief Virtual function to post-process the Scene. It has two phases: transform the meta-objects and load the Scene.
    * During the first phase, we iterate on the different meta-objects lists and are extracted elements for the ray tracer.
    * Typically, a wall object would be transformed in a set of Primitives, extract polylines from punctual Sources,...
    * The Scene will be loaded with objects related to acoustic calculation which are not present into the physical description of the Scene.
    * For instance, diffraction edges are modeled by a cylinder and should be generated.
    * \param scene Scene to post-process
    * \param sources Sources
    * \param recepteurs Receptors
    * \return Return true if success
    */
    virtual bool postTreatmentScene(Scene* scene, std::vector<Source>& sources, std::vector<Recepteur>& recepteurs);

    /*!
     * @brief Leaf treatment function for accelerators. The default is to keep only the first intersection.
     * It is possible to keep all the intersections which happens before a type specific intersection. For instance, we can
     * keep all the intersections before an intersection with a triangle considered as blocking.
     * @param primitives All the primitives encountered by the current leaf.
     * @return Maximal time authorized by the ray. It is the intersection time before the first blocking primitive.
     */
    virtual double leafTreatment(vector<Intersection> &primitives);

    /*!
    * \brief Validation function for an intersection. If the intersection is validated, an event is created and added on the ray.
    * \param r : Ray
    * \param inter : Intersection to validate
    * \return Return true if the validation is done
    * \warning The validation functions should agree with the primitives types in the Scene. The unsupported primitives types should throw an
    * exception if not handled by the simulation.
    */
    virtual bool valideIntersection(Ray* r, Intersection* inter);

    /*!
    * \brief Ray validation. The developer may, for instance, choose a filtering on the rays and only validate single paths.
    * If the ray is valid, it is added to the valid rays list and will be used for the export.
    * \param r : Ray to validate. The ray should have a source and a receiver.
    * \return Return true if the ray has been validated.
    */
    virtual bool valideRayon(Ray* r);

    /*!
    * \brief Method to arrange the invalid rays.
    * The invalid rays are put away into a debug_ray array in order to visualize all of the rays
    * \param r : Ray to invalid.
    * \return Return true if the ray has been invalidated.
    */
    virtual bool invalidRayon(Ray* r);

    /// Return a pointer to the validated rays list
    virtual deque<Ray*>* getValidRays() { return &valid_rays;}
    //virtual vector<Ray*>* getValidRays() { return &valid_rays;}

    /// Return a pointer to the invalidated rays list
    deque<Ray*>* getDebugRays() { return &debug_rays;}
    //vector<Ray*>* getDebugRays() { return &debug_rays;}

    /// End the operations
    virtual void finish();
    /// Delete the valid rays array
    virtual void clean();

    /*!
    * \brief Load the computation parameters.
    * \return True if operation succeeds.
    */
    virtual bool loadParameters();

protected:
    deque<Ray*> valid_rays;    //!< Rays list which are validated by the solver
    deque<Ray*> debug_rays;    //!< Rays list which are invalidated by the solver
};

#endif // SOLVER_H
