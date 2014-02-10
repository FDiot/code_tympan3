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

#ifndef DEFAULT_ENGINE_H
#define DEFAULT_ENGINE_H

#include "Engine.h"
#include <stack>

class DefaultEngine : public Engine
{

public:
    DefaultEngine() : Engine() { nbRayonsTraites = 0;}

    DefaultEngine(Scene* _scene, std::vector<Source> *_sources, Solver* _solver, std::vector<Recepteur> *_recepteurs)
        : Engine(_scene, _sources, _solver, _recepteurs) {  }

    DefaultEngine(const DefaultEngine& other)
    {
        scene = other.scene;
        sources = other.sources;
        solver = other.solver;
        recepteurs = other.recepteurs;
    }

    virtual ~DefaultEngine() { }

    virtual bool process();

    virtual void runStructureBenchmark();

protected:
    Ray* genRay();
    bool traitementRay(Ray* r, std::list<validRay> &result);
    std::stack<Ray*> pile_traitement;

    unsigned long long int nbRayonsTraites;
};

#endif