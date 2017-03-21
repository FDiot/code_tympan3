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

#ifndef SIMULATION_H
#define SIMULATION_H

#include "Tympan/geometric_methods/AcousticRaytracer/Base.h"
#include "Tympan/geometric_methods/AcousticRaytracer/Geometry/Scene.h"
#include "Tympan/geometric_methods/AcousticRaytracer/Acoustic/Solver.h"
#include "Tympan/geometric_methods/AcousticRaytracer/Acoustic/Source.h"
#include "Tympan/geometric_methods/AcousticRaytracer/Acoustic/Recepteur.h"
#include "Tympan/geometric_methods/AcousticRaytracer/Acoustic/Material.h"
#include "Tympan/geometric_methods/AcousticRaytracer/Tools/Logger.h"
#include "AcousticRaytracerConfiguration.h"
#include "DefaultEngine.h"
#include "ParallelDefaultEngine.h"

enum engineChoice
{
    DEFAULT = 0,
    PARALLELDEFAULT,
    GPU
};


/*!
* \file Simulation.h
* \class Simulation
* \brief Class hosting all the informations needed for ray tracing. It contains the function to
* launch the rays treatment and the main loop of the program. To run a simulation, a call to launchSimulation() is enough
*/
class Simulation : public Base
{

public:
	/// Constructor
    Simulation() : solver(NULL), engine(NULL), engineC(DEFAULT), materialManager(NULL) 
    { 
       compteurSource = 0;
       compteurRecepteur = 0;
       configuration = AcousticRaytracerConfiguration::get();
    }
    /// Copy constructor
    Simulation(const Simulation& other)
    {
        solver = new Solver(other.solver);
        engine = NULL;
        engineC = other.engineC;
        compteurSource = other.compteurSource;
        compteurRecepteur = other.compteurRecepteur;
        materialManager = new MaterialManager(*(other.materialManager));
        *configuration = *other.configuration;
    }
    /// Destructor
    virtual ~Simulation() { }

    /*!
    * \fn Scene* getScene()
    * \brief Get a pointer to the scene
    * \return Pointer to the scene
    */
    Scene* getScene() { return &scene; }

#ifdef TEST_ACCELERATION_RECEPTORS
    /*!
     * \fn Scene* get_receptors_landscape();
     * \brief Return the geometric distribution of receptors
     */
    Scene* get_receptors_landscape() { return &receptors_landscape; }
#endif

    /*!
    * \fn void setSolver(Solver *_solver)
    * \brief Tool function to set the acoustic solver for the simulation
    */
    void setSolver(Solver* _solver) { solver = _solver; }

    /*!
    * \fn Solver* getSolver()
    * \brief Get the acoustic solver
    * \return Pointer to the solver
    */
    Solver* getSolver() { return solver; }

    /*!
    * \fn void clean()
    * \brief Clean the simulation: the scene, sources, and receptors and all the rays are deleted.
    */
    void clean();

    /*!
    * \fn bool launchSimulation()
    * \brief Program main loop. Extract all the rays from the sources then treat them. The loop finishes when
    * the treatment stack is empty and there is no more sources creating rays. Once the ray is treated, either it is added to the stack
    * to continue its path or is suppressed. The receptor search is launched once the ray is treated. Single-threaded function.
    * \return True if succeeds
    */
    bool launchSimulation();

    /*!
    * \fn void addSource(const Source& s)
    * \brief Tool function to add a source to the simulation.
    * \param s : Source
    */
    void addSource(const Source& s) { sources.push_back(s); sources.back().setId(compteurSource); compteurSource++;}
    /*!
    * \fn std::vector<Source>& getSources()
    * \brief Return the sources list of the scene
    * \return Vector containing the sources
    */
    std::vector<Source>& getSources() { return sources; }

    /*!
    * \fn void addRecepteur(const Recepteur& r)
    * \brief Tool function to add quickly a receptor for the simulation
    * \param r : Receptor
    */
    void addRecepteur(const Recepteur& r) { 
        recepteurs.push_back(r); 
        recepteurs.back().setId(compteurRecepteur);
#ifdef TEST_ACCELERATION_RECEPTORS
        receptors_landscape.addShape( new Recepteur(r) );
#endif
        compteurRecepteur++;
    }
    /*!
    * \fn std::vector<Recepteur>& getRecepteurs()
    * \brief Return a vector of all receptors of the scene
    * \return Vector
    */
    std::vector<Recepteur>& getRecepteurs() { return recepteurs; }

    /// Set/Get the pointer to the MaterialManager
    void setMaterialManager(MaterialManager* _materialManager) { materialManager = _materialManager; }
    MaterialManager* getMaterialManager() { return materialManager; }

    /// Set the engine (by default, the DefaultEngine)
    void setEngine(engineChoice engine = DEFAULT) { engineC = engine; }
    
    /// Get the configuration
    AcousticRaytracerConfiguration* getConfiguration() { return configuration; }

//    void runBenchmark() { engine = new DefaultEngine(&scene, &sources, solver, &recepteurs); engine->runStructureBenchmark(); }

protected:
    Scene scene; 						//!< Description of the geometry in an accelerated structure
#ifdef TEST_ACCELERATION_RECEPTORS
    Scene receptors_landscape; 			//!< Geometric distribution of receptors
#endif

    Solver* solver;             		//!< Pointer to a solver (acoustic method)
    MaterialManager* materialManager;	//!< Pointer to a MaterialManager object

    std::vector<Source> sources;        //!< All the acoustic sources for the Simulation
    std::vector<Recepteur> recepteurs;  //!< All the acoustic receptors for the Simulation

    engineChoice engineC;				//!< Enum giving the Engine choice
    Engine* engine;						//!< Pointer to the selected Engine

    unsigned int compteurSource;		//!< Source counter
    unsigned int compteurRecepteur;		//!< Receptor counter
    
    AcousticRaytracerConfiguration* configuration;  //!< Pointer to the Acoustic RayTracer configuration
};
#endif
