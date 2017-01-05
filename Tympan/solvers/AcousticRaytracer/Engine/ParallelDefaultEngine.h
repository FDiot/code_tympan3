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

#ifndef PARALLEL_DEFAULT_ENGINE_H
#define PARALLEL_DEFAULT_ENGINE_H

#include "DefaultEngine.h"

/**
 * \brief Parallel default engine class
 */
#ifdef TEST_ACCELERATION_RECEPTORS
class ParallelDefaultEngine : public Engine
{

public:

    ParallelDefaultEngine() : Engine() { }

    ParallelDefaultEngine(Scene* _scene, std::vector<Source> *_sources, Solver* _solver, Scene *_recepteurs)
        : Engine(_scene, _sources, _solver, _recepteurs) {  }

    ParallelDefaultEngine(const ParallelDefaultEngine& other)
    {
        scene = other.scene;
        sources = other.sources;
        solver = other.solver;
        recepteurs = other.recepteurs;
    }

    virtual ~ParallelDefaultEngine() { }

    virtual bool process();

};
#else
class ParallelDefaultEngine : public Engine
{

public:

    ParallelDefaultEngine() : Engine() { }

    ParallelDefaultEngine(Scene* _scene, std::vector<Source> *_sources, Solver* _solver, std::vector<Recepteur> *_recepteurs)
        : Engine(_scene, _sources, _solver, _recepteurs) {  }

    ParallelDefaultEngine(const ParallelDefaultEngine& other)
    {
        scene = other.scene;
        sources = other.sources;
        solver = other.solver;
        recepteurs = other.recepteurs;
    }

    virtual ~ParallelDefaultEngine() { }

    virtual bool process();

};

#endif //TEST_ACCELERATION_RECEPTORS
#endif
