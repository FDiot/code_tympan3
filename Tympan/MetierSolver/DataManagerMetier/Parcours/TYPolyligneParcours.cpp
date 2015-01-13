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


#ifdef TYMPAN_USE_PRECOMPILED_HEADER
#include "Tympan/MetierSolver/DataManagerMetier/TYPHMetier.h"
#endif // TYMPAN_USE_PRECOMPILED_HEADER

#include "TYPolyligneParcours.h"

#include "Tympan/MetierSolver/ToolsMetier/GlobalMacros.h"
#include <stdio.h>
#include <assert.h>

TYPolyligneParcours::TYPolyligneParcours()
{
    _nNbPoint = 0;
    _nNbPointAlloue = 0;
    _PtrPoints = NULL;
    _PolyligneP0 = NULL;
    _PolyligneP1 = NULL;
}

TYPolyligneParcours::~TYPolyligneParcours()
{
    desallouer();
}

bool TYPolyligneParcours::allouer(int nNbPoint)
{
    this->_nNbPointAlloue = nNbPoint;
    this->_PtrPoints = new TYPointParcours*[_nNbPointAlloue];
    return NULL != this->_PtrPoints;
}

void TYPolyligneParcours::desallouer()
{
    SAFE_DELETE_LIST(this->_PtrPoints)
}
void TYPolyligneParcours::ajouteSegment(TYPointParcours* p1, TYPointParcours* p2)
{
    //Pour l'instant, cette fonction est tres simplifiee (on considere qu'on n'a pas encore de points):
    assert(_nNbPoint == 0);
    assert(_PtrPoints == NULL);
    allouer(2);
    ajoutePoint(0, p1);
    ajoutePoint(1, p2);
    bool bverifieNaturePolylignes = verifieNaturePolylignes();
    assert(bverifieNaturePolylignes);
}

bool TYPolyligneParcours::isInfra()
{
    assert(_nNbPoint == 2);
    return (_PtrPoints[0]->isInfra && _PtrPoints[1]->isInfra);
}

bool TYPolyligneParcours::isEcran()
{
    assert(_nNbPoint == 2);
    return (_PtrPoints[0]->isEcran && _PtrPoints[1]->isEcran);
}

bool TYPolyligneParcours::verifieNaturePolylignes()
{
    assert(_nNbPoint == 2);
    bool isInfra0 = _PtrPoints[0]->isInfra;
    bool isInfra1 = _PtrPoints[1]->isInfra;
    bool isEcran0 = _PtrPoints[0]->isEcran;
    bool isEcran1 = _PtrPoints[1]->isEcran;
    //1. verifier que l'infrastructure ne se melange pas a la topo,
    //et que dans l;'infra, les ecrans ne se melangent pas avec le reste:
    if (isInfra0 != isInfra1 || isEcran0 != isEcran1)
    {
        return false;
    }
    //2. verifier la coherence infra-ecran,
    if ((!isInfra0 && isEcran0) || (!isInfra1 && isEcran1))
    {
        return false;
    }

    return true;
}

bool TYPolyligneParcours::estSurUnParcourFermee()
{
    assert(_nNbPoint == 2);
    if (_PolyligneP1 == _PolyligneP0 == NULL)
    {
        return estFermee();
    }
    else//il faut parcourir toutes les voisines jusqu'a bouclage
    {
        int IndexPoint = _PtrPoints[0]->Identifiant;
        TYPolyligneParcours* PolyligneSuivante = NULL;
        do
        {
            IndexPoint = indexePointSuivant(IndexPoint, PolyligneSuivante);
        }
        while (PolyligneSuivante != NULL && this != PolyligneSuivante);
        bool bFermee = (this == PolyligneSuivante);
        return bFermee;
    }
    return false;
}

int TYPolyligneParcours::indexePointSuivant(int IndexPoint, TYPolyligneParcours*& PolyligneSuivante)
{
    //Verifications
    assert(_nNbPoint == 2);
    bool bAssert = !polylignesVoisinesPointentSurLaMemePolyligne();
    if (bAssert)
    {
        bAssert = true;
    }
    //Cherchons l'index du prochain point; il ne doit pas etre egal a l'indexe courant
    int IndexPointSuivant = autrePointDuSegment(IndexPoint);
    assert(IndexPointSuivant >= 0);
    //Trouver la polyligne suivante
    PolyligneSuivante = polyligneSuivante(IndexPointSuivant);//point suivant, partage par la prochaine polyligne

    return IndexPointSuivant;
}

bool TYPolyligneParcours::polylignesVoisinesPointentSurLaMemePolyligne()
{
    assert(_nNbPoint == 2); //_PolyligneP0 & _PolyligneP1 sont relatives aux points P0 & P1
    if (_PolyligneP0 == _PolyligneP1)
    {
        if (_PolyligneP0 != NULL)
        {
            return true;
        }
    }
    return false;
}

int TYPolyligneParcours::autrePointDuSegment(int    IndexPoint)
{
    assert(_nNbPoint == 2); //_PolyligneP0 & _PolyligneP1 sont relatives aux points P0 & P1
    if (IndexPoint == _PtrPoints[0]->Identifiant)
    {
        return _PtrPoints[1]->Identifiant;
    }
    else
    {
        if (IndexPoint == _PtrPoints[1]->Identifiant)
        {
            return _PtrPoints[0]->Identifiant;
        }
    }

    return -1;//pas de point trouve
}

TYPolyligneParcours* TYPolyligneParcours::polyligneSuivante(int IndexPoint)
{
    //IndexPoint = point suivant, partage par la prochaine polyligne
    if (IndexPoint == _PtrPoints[0]->Identifiant)
    {
        return _PolyligneP0;
    }
    if (IndexPoint == _PtrPoints[1]->Identifiant)
    {
        return _PolyligneP1;
    }
    return NULL;
}


bool TYPolyligneParcours::estFermee()
{
    if (indexePremierPoint() != indexeDernierPoint())
    {
        return false;
    }
    return true;
}

int TYPolyligneParcours::indexePoint(int i)
{
    assert(_nNbPoint > 0);
    return _PtrPoints[i]->Identifiant;
}

int TYPolyligneParcours::indexePremierPoint()
{
    return _PtrPoints[0]->Identifiant;
}

int TYPolyligneParcours::indexeDernierPoint()
{
    return _PtrPoints[_nNbPoint - 1]->Identifiant;
}



void TYPolyligneParcours::Copy(TYPolyligneParcours& p)
{
    desallouer();
    allouer(p._nNbPoint);
    //Copie des ptr de points
    for (int i = 0; i < _nNbPoint; i++)
    {
        _PtrPoints[i] = p._PtrPoints[i];
    }
    //Copie des ptr de TYPolyligneParcours
    _PolyligneP0 = p._PolyligneP0;
    _PolyligneP1 = p._PolyligneP1;
}
