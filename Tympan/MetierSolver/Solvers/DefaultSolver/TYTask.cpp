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

/*
 *
 */


#include "TYAcousticModel.h"
#include "TYAcousticPathFinder.h"
#include "TYFaceSelector.h"
#include "TYSolver.h"
#include "TYTask.h"
#include "Tympan/MetierSolver/DataManagerMetier/Commun/TYTrajet.h"

TYTask::TYTask(TYSolver& solver, TYTrajet& trajet, int nNbTrajets)
    : _solver(solver), _trajet(trajet), _nNbTrajets(nNbTrajets), _tabIntersect(0)
{

}

TYTask::~TYTask()
{
    if (_tabIntersect)
    {
        delete [] _tabIntersect;
    }
}

void TYTask::main()
{
    TYTabPoint3D ptsTop;
    TYTabPoint3D ptsLeft;
    TYTabPoint3D ptsRight;

    // Recupere un pointeur sur le noeud geometrique de la source ponctuelle
    TYSourcePonctuelleGeoNode* pSrcGeoNode = _trajet.getSourcePonctuelle();

    // Recupere les pointeur sur la source ponctuelle et le point de calcul
    TYSourcePonctuelle* pSrc = NULL;
    if (pSrcGeoNode) { pSrc = dynamic_cast<TYSourcePonctuelle*>(pSrcGeoNode->getElement()); }

    TYPointCalcul* pPointCalc = dynamic_cast<TYPointCalcul*>(_trajet.getPointCalcul()->getElement());
    if (!(pSrc && pPointCalc)) { return; }

    // Construction du rayon SR
    OSegment3D rayon;
    _trajet.getPtSetPtRfromOSeg3D(rayon);

    // Initialisation du tableau des intersections
    if (!_tabIntersect)
    {
        _tabIntersect = new TYSIntersection[_solver.getTabPolygonSize()];
    }

    // On selectionne les faces de la scene concernes par le calcul acoustique pour la paire concernee
    _solver.getFaceSelector()->selectFaces(_tabIntersect, pSrcGeoNode, rayon);

    // On calcul les trajets acoustiques horizontaux et verticaux reliant la paire source/recepteur
    _solver.getAcousticPathFinder()->computePath(_tabIntersect, rayon, ptsTop, ptsLeft, ptsRight, _nNbTrajets);

    // On effectue les calculs acoustiques en utilisant les formules du modele acoustique
    _solver.getAcousticModel()->compute(_tabIntersect, rayon, _trajet, ptsTop, ptsLeft, ptsRight);

    if (_tabIntersect)
    {
        delete [] _tabIntersect;
    }
    _tabIntersect = NULL;

    ptsTop.clear();
    ptsLeft.clear();
    ptsRight.clear();
}
