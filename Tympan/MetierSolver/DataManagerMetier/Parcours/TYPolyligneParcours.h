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
 *
 *
 *
 */

#ifndef TYPOLYLIGNE_PARCOURS
#define TYPOLYLIGNE_PARCOURS

#include "TYPointParcours.h"

class TYPolyligneParcours
{
    int _nNbPoint;
    int _nNbPointAlloue;
    TYPointParcours** _PtrPoints;

public:
    TYPolyligneParcours* _PolyligneP1;
    TYPolyligneParcours* _PolyligneP0;


    TYPolyligneParcours();
    ~TYPolyligneParcours();

    int indexePoint(int i);
    int indexePremierPoint();
    int indexeDernierPoint();
    int indexePoint1() { return indexePoint(0);}
    int indexePoint2() { return indexePoint(1);}

    int nombreDePoint() { return _nNbPoint;}
    void setPoint(int indexe, TYPointParcours* p) { _PtrPoints[indexe] = p;}
    void ajoutePoint(int indexe, TYPointParcours* p) { setPoint(indexe, p); if (indexe >= _nNbPoint) { _nNbPoint++; }}
    TYPointParcours     point(int indexe) { return *_PtrPoints[indexe];}
    TYPointParcours*    pointPtr(int indexe) { return _PtrPoints[indexe];}

    bool isInfra();
    bool isEcran();


    void ajouteSegment(TYPointParcours* p1, TYPointParcours* p2);

    bool polylignesVoisinesPointentSurLaMemePolyligne();

    int indexePointSuivant(int  IndexPoint, TYPolyligneParcours*& PolyligneSuivante);
    TYPolyligneParcours* polyligneSuivante(int IndexPoint);
    int autrePointDuSegment(int IndexPoint);

    bool estSurUnParcourFermee();
    bool estFermee();
    bool allouer(int nNbPoint);
    void desallouer();
    void Copy(TYPolyligneParcours& p);

private:
    bool verifieNaturePolylignes();
};

#endif//TYPOLYLIGNE_PARCOURS
