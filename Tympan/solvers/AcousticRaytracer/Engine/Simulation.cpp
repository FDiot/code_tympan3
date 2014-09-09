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

#include "Simulation.h"

void Simulation::clean()
{
    scene.clean();

    // Nettoyage des sources et des recepteurs
    sources.clear();
    recepteurs.clear();

    compteurSource = 0;
    compteurRecepteur = 0;
}

bool Simulation::launchSimulation()
{

    ss << "Lancement de la simulation." << std::endl;
    if (solver) { solver->clean(); }
    if (engine) { delete engine; }
    switch (engineC)
    {
        case DEFAULT:
            engine = new DefaultEngine(&scene, &sources, solver, &recepteurs);
            break;
        case PARALLELDEFAULT:
            engine = new ParallelDefaultEngine(&scene, &sources, solver, &recepteurs);
            break;
        default:
            engine = new DefaultEngine(&scene, &sources, solver, &recepteurs);
            break;
    }
    return engine->process();
}
