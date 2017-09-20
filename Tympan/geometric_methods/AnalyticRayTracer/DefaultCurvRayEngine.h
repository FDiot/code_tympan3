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

#ifndef DEFAULT_CURV_RAY_ENGINE_H
#define DEFAULT_CURV_RAY_ENGINE_H

#include "Tympan/geometric_methods/AcousticRaytracer/Engine/DefaultEngine.h"
#include <stack>

/**
 * \brief Engine for analytical ray curve tracing
 */
#ifdef TEST_ACCELERATION_RECEPTORS
class DefaultCurvRayEngine : public DefaultEngine
{

public:
	/// Default constructor
    DefaultCurvRayEngine() : DefaultEngine() {}
    /// Constructor
    DefaultCurvRayEngine(Scene* _scene, std::vector<Source> *_sources, Solver* _solver, Scene *_recepteurs)
        : DefaultEngine(_scene, _sources, _solver, _recepteurs) {  }
    /// Copy constructor
    DefaultCurvRayEngine(const DefaultCurvRayEngine& other)
    {
        scene = other.scene;
        sources = other.sources;
        solver = other.solver;
        recepteurs = other.recepteurs;
    }
    /// Destructor
    virtual ~DefaultCurvRayEngine() { }

    /// Main routine
    virtual bool process();

protected:
    /**
     * @brief Ray treatment
     * @param r Ray
     * @param result List of valid rays
     * @return True if succeeds
     */
    virtual bool traitementRay(Ray* r, std::list<validRay> &result);
};
#else
class DefaultCurvRayEngine : public DefaultEngine
{

public:
    DefaultCurvRayEngine() : DefaultEngine() {}

    DefaultCurvRayEngine(Scene* _scene, std::vector<Source> *_sources, Solver* _solver, std::vector<Recepteur> *_recepteurs)
        : DefaultEngine(_scene, _sources, _solver, _recepteurs) {  }

    DefaultCurvRayEngine(const DefaultCurvRayEngine& other)
    {
        scene = other.scene;
        sources = other.sources;
        solver = other.solver;
        recepteurs = other.recepteurs;
    }

    virtual ~DefaultCurvRayEngine() { }

    virtual bool process();

protected:
    virtual bool traitementRay(Ray* r, std::list<validRay> &result);
};
#endif

#endif //_CURV_RAY
