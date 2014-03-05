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
	// _tabIntersect
	for (unsigned int i=0; i<_tabIntersect.size(); i++)
	{
		delete _tabIntersect[i];
		_tabIntersect[i] = NULL;
	}
}

void TYTask::main()
{
    // Recupere un pointeur sur le noeud geometrique de la source ponctuelle
    TYSourcePonctuelleGeoNode* pSrcGeoNode = _trajet.getSourcePonctuelle();
    assert(pSrcGeoNode);

    // Recupere les pointeur sur la source ponctuelle et le point de calcul
    TYSourcePonctuelle* pSrc = TYSourcePonctuelle::safeDownCast(pSrcGeoNode->getElement());
    TYPointCalcul* pRcpt = TYPointCalcul::safeDownCast(_trajet.getPointCalcul()->getElement());
    assert(pSrc && pRcpt);

    // Construction du rayon SR
    OSegment3D rayon;
    _trajet.getPtSetPtRfromOSeg3D(rayon);

    // On selectionne les faces de la scene concernes par le calcul acoustique pour la paire concernee
    _solver.getFaceSelector()->selectFaces(_tabIntersect, pSrcGeoNode, rayon);

    // On calcul les trajets acoustiques horizontaux et verticaux reliant la paire source/recepteur
    _solver.getAcousticPathFinder()->computePath(_tabIntersect, _trajet.getTabRays(), rayon, _nNbTrajets);

    // On effectue les calculs acoustiques en utilisant les formules du modele acoustique
	OSpectre spectrum(0.0);

    _solver.getAcousticModel()->compute(_tabIntersect, rayon, pSrc, pRcpt, _trajet.getTabRays(), spectrum);
	_trajet.setSpectre(spectrum);
}
