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

#ifndef ENGINE_H
#define ENGINE_H

#include "Tympan/geometric_methods/AcousticRaytracer/Geometry/Scene.h"
#include "Tympan/geometric_methods/AcousticRaytracer/Acoustic/Source.h"
#include "Tympan/geometric_methods/AcousticRaytracer/Acoustic/Solver.h"
#include "Tympan/geometric_methods/AcousticRaytracer/Acoustic/Recepteur.h"

#define TEST_ACCELERATION_RECEPTORS

typedef struct _validRay
{
    Ray* r;     //!< Pointer to a ray. Should not be NULL
    bool valid; //!< Boolean set to True if the ray is validated, which means an event occurs.
} validRay;

/**
 * \brief Base class for engines (DefaultEngine, ParallelDefaultEngine,...)
 */
class Engine
{

public:
#ifdef TEST_ACCELERATION_RECEPTORS
	/// Default constructor
    Engine() : scene(NULL), sources(NULL), solver(NULL), recepteurs(NULL), rayCounter(0) { }
    /// Constructor
    Engine(Scene* _scene, std::vector<Source> *_sources, Solver* _solver, Scene *_recepteurs)
    {
        scene = _scene;
        recepteurs = _recepteurs;
        sources = _sources;
        solver = _solver;
        rayCounter = 0;
    }
    /// Copy constructor
    Engine(const Engine& other)
    {
        scene = other.scene;
        recepteurs = other.recepteurs;
        sources = other.sources;
        solver = other.solver;
        rayCounter = other.rayCounter;
    }
    /// Destructor
    virtual ~Engine() { }

    Scene* getScene() { return scene; } //!< Get the Scene
    void setScene(Scene* _scene) { scene = _scene; } //!< Set the Scene

    std::vector<Source>* getSources() { return sources; } //!< Get the Sources
    void setSources(std::vector<Source> *_sources) { sources = _sources; } //!< Set the Sources

    Solver* getSolver() { return solver; } //!< Get the Solver
    void setSolver(Solver* _solver) { solver = _solver; } //!< Set the Solver

    virtual bool process() { return false;} //!< If implemented, process and return true if success

    virtual void runStructureBenchmark() {} //!< If implemented, run a benchmark for the engine

protected:
    Scene* scene;					//!< Pointer to the scene
    Scene *recepteurs;				//!< Pointer to all the sources
    std::vector<Source> *sources;	//!< Pointer to all the receptors
    Solver* solver;					//!< Pointer to the solver

    unsigned long long int rayCounter; //!< Ray counter
#else
    Engine() : scene(NULL), sources(NULL), solver(NULL), recepteurs(NULL), rayCounter(0) { }

    Engine(Scene* _scene, std::vector<Source> *_sources, Solver* _solver, std::vector<Recepteur> *_recepteurs)
    {
        scene = _scene;
        sources = _sources;
        solver = _solver;
        recepteurs = _recepteurs;
        rayCounter = 0;
    }

    Engine(const Engine& other)
    {
        scene = other.scene;
        sources = other.sources;
        solver = other.solver;
        recepteurs = other.recepteurs;
        rayCounter = other.rayCounter;
    }

    virtual ~Engine() { }

    Scene* getScene() { return scene; }
    void setScene(Scene* _scene) { scene = _scene; }

    std::vector<Source>* getSources() { return sources; }
    void setSources(std::vector<Source> *_sources) { sources = _sources; }

    Solver* getSolver() { return solver; }
    void setSolver(Solver* _solver) { solver = _solver; }

    virtual bool process() { return false;}

    virtual void runStructureBenchmark() {}

protected:
    Scene* scene;
    std::vector<Source> *sources;
    std::vector<Recepteur> *recepteurs;
    Solver* solver;

    unsigned long long int rayCounter;
#endif
};
#endif
