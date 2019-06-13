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

#include "TYAcousticModel.h"
#include "TYAcousticPathFinder.h"
#include "TYFaceSelector.h"
#include "TYSolver.h"
#include "TYTask.h"
#include "Tympan/solvers/DefaultSolver/TYTrajet.h"

TYTask::TYTask(TYSolver& solver, const tympan::nodes_pool_t& nodes, const tympan::triangle_pool_t& triangles, const tympan::material_pool_t& materials, TYTrajet& trajet, int nNbTrajets)
    : _solver(solver), 
    _trajet(trajet), 
    _nNbTrajets(nNbTrajets), 
    _nodes(nodes),
    _triangles(triangles),
    _materials(materials)
{

}

TYTask::~TYTask()
{
}

void TYTask::main()
{
    TabPoint3D ptsTop;
    TabPoint3D ptsLeft;
    TabPoint3D ptsRight;

    // Construction du rayon SR
    OSegment3D rayon;
    _trajet.getPtSetPtRfromOSeg3D(rayon);

    // On selectionne les faces de la scene concernes par le calcul acoustique pour la paire concernee
    _solver.getFaceSelector()->selectFaces(_tabIntersect, _trajet);

    // On calcul les trajets acoustiques horizontaux et verticaux reliant la paire source/recepteur
    _solver.getAcousticPathFinder()->computePath(_tabIntersect, _trajet, ptsTop, ptsLeft, ptsRight);

    // On effectue les calculs acoustiques en utilisant les formules du modele acoustique
    _solver.getAcousticModel()->compute(_tabIntersect, _trajet, ptsTop, ptsLeft, ptsRight);

    ptsTop.clear();
    ptsLeft.clear();
    ptsRight.clear();
    _tabIntersect.clear();

}
