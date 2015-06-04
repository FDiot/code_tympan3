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

#include "Tympan/solvers/AcousticRaytracer/Base.h"
#include "Tympan/solvers/AcousticRaytracer/Geometry/Scene.h"
#include "Tympan/solvers/AcousticRaytracer/Acoustic/Solver.h"
#include "Tympan/solvers/AcousticRaytracer/Acoustic/Source.h"
#include "Tympan/solvers/AcousticRaytracer/Acoustic/Recepteur.h"
#include "Tympan/solvers/AcousticRaytracer/Acoustic/Material.h"
#include "Tympan/solvers/AcousticRaytracer/Tools/Logger.h"
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
* \brief Classe support hebergeant l'ensemble des informations necessaires au lancer de rayons. Cette classe permet entre autre de lancer le traitement
* des rayons et la boucle principale du programme. Pour lancer le traitement de la Simulation, un appel a launchSimulation() suffit
*/
class Simulation : public Base
{

public:
    Simulation() : solver(NULL), engine(NULL), engineC(DEFAULT), materialManager(NULL) { compteurSource = 0; compteurRecepteur = 0;}
    Simulation(const Simulation& other)
    {
        solver = new Solver(other.solver);
        engine = NULL;
        engineC = other.engineC;
        compteurSource = other.compteurSource;
        compteurRecepteur = other.compteurRecepteur;
        materialManager = new MaterialManager(*(other.materialManager));
    }

    virtual ~Simulation() { }

    /*!
    * \fn Scene* getScene()
    * \brief Renvoie un pointeur sur la scene
    * \return Pointeur sur la scene
    */
    Scene* getScene() { return &scene; }

#ifdef TEST_ACCELERATION_RECEPTORS
    /*!
     * \fn Scene* get_receptors_landscape();
     * \brief return the geoemtric distribution of receptors
     */
    Scene* get_receptors_landscape() { return &receptors_landscape; }
#endif

    /*!
    * \fn void setSolver(Solver *_solver)
    * \brief Fonction outil pour integrer un solveur acoustique a la simulation
    */
    void setSolver(Solver* _solver) { solver = _solver; }

    /*!
    * \fn Solver* getSolver()
    * \brief Renvoie un pointeur sur le solveur acoustique
    * \return Pointeur sur le solveur acoustique
    */
    Solver* getSolver() { return solver; }

    /*!
    * \fn void clean()
    * \brief Nettoie entierement la simulation. La scene, les sources, les recepteurs ainsi que l'ensemble des rayons sont supprimes.
    */
    void clean();

    /*!
    * \fn bool launchSimulation()
    * \brief Boucle principale du programme. Extrait l'ensemble des rayons a partir des sources et traite les rayons. La boucle d'arrete lorsque
    * la pile de traitement est vide et que plus aucune source ne peut generer de rayons. Une fois le rayon traiter, il est soit place en pile
    * de traitement pour poursuivre son chemin ou supprimer. La recherche de recepteur se fait apres le traitement du rayon. Cette fonction est monothread.
    * \return vrai si tout s'est bien passe
    */
    bool launchSimulation();

    /*!
    * \fn void addSource(const Source& s)
    * \brief Fonction outil pour integrer une source a la simulation.
    * \param s : source a integrer
    */
    void addSource(const Source& s) { sources.push_back(s); sources.back().setId(compteurSource); compteurSource++;}
    /*!
    * \fn std::vector<Source>& getSources()
    * \brief Renvoie la liste des sources de la scene
    * \return Vecteur contenant les sources de la simulation
    */
    std::vector<Source>& getSources() { return sources; }

    /*!
    * \fn void addRecepteur(const Recepteur& r)
    * \brief Fonction outil pour placer rapidement un recepteur dans la simulation
    * \param r : recepteur a placer
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
    * \brief Renvoie la liste des recepteurs de la scene
    * \return Vecteur contenant les recepteurs de la simulation
    */
    std::vector<Recepteur>& getRecepteurs() { return recepteurs; }

    void setMaterialManager(MaterialManager* _materialManager) { materialManager = _materialManager; }
    MaterialManager* getMaterialManager() { return materialManager; }

    void setEngine(engineChoice engine = DEFAULT) { engineC = engine; }

//    void runBenchmark() { engine = new DefaultEngine(&scene, &sources, solver, &recepteurs); engine->runStructureBenchmark(); }

protected:
    Scene scene; // Description of the geometry in an accelerated structure
#ifdef TEST_ACCELERATION_RECEPTORS
    Scene receptors_landscape; // geometric distribution of receptors
#endif

    Solver* solver;             /*!< Pointeur vers la "methode acoustique" */
    MaterialManager* materialManager;

    std::vector<Source> sources;        /*!< Ensemble des sources acoustiques gerees par la simulation. */
    std::vector<Recepteur> recepteurs;  /*!< Ensemble des recepteurs acoustiques geres par la simulation. */

    engineChoice engineC;
    Engine* engine;

    unsigned int compteurSource;
    unsigned int compteurRecepteur;

};
#endif
