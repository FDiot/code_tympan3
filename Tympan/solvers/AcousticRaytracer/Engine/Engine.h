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

#include "Tympan/solvers/AcousticRaytracer/Geometry/Scene.h"
#include "Tympan/solvers/AcousticRaytracer/Acoustic/Source.h"
#include "Tympan/solvers/AcousticRaytracer/Acoustic/Solver.h"
#include "Tympan/solvers/AcousticRaytracer/Acoustic/Recepteur.h"

typedef struct _validRay
{
    Ray* r;     /*!< Pointeur vers le rayon traite. Ne dois pas etre NULL */
    bool valid; /*!< Vrai si le rayon a pu etre valide, c'est a dire un evenement a ete genere. */
} validRay;

class Engine
{

public:
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

};
#endif