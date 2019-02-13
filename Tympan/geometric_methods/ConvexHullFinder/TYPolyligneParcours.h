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
#include <iostream>

/**
 * \brief Polylines path class used by the TYSetGeometriqueParcours class
 */
class TYPolyligneParcours
{
    int _nNbPoint;					//!< Number of points on the polyline
    int _nNbPointAlloue;			//!< Number of allocated points on the polyline
    TYPointParcours** _PtrPoints;	//!< Array of pointers to points

public:
    TYPolyligneParcours* _PolyligneP1; //!< Pointer to the next polyline (from P1 point)
    TYPolyligneParcours* _PolyligneP0; //!< Pointer to the previous polyline (from P0 point)

    /// Default constructor
    TYPolyligneParcours();
    /// Destructor
    ~TYPolyligneParcours();

    int indexePoint(int i); 	//!< Return point id of point i of the polyline
    int indexePremierPoint();	//!< Return point id of first point of the polyline
    int indexeDernierPoint();	//!< Return point id of last  point of the polyline
    int indexePoint1() { return indexePoint(0);} //!< Return point id of point P0
    int indexePoint2() { return indexePoint(1);} //!< Return point id of point P1

    int nombreDePoint() { return _nNbPoint;} //!< Return the number of points
    void setPoint(int indexe, TYPointParcours* p) { _PtrPoints[indexe] = p;} //!< Change a point
    void ajoutePoint(int indexe, TYPointParcours* p) { setPoint(indexe, p); if (indexe >= _nNbPoint) { _nNbPoint++; }} //!< Add a point
    TYPointParcours     point(int indexe) { return *_PtrPoints[indexe];}	//!< Return a copy the point Pi
    TYPointParcours*    pointPtr(int indexe) { return _PtrPoints[indexe];}  //!< Return a pointer on the point Pi

    bool isInfra(); //!< Return true if P0 and P1 are Infra
    bool isEcran(); //!< Return true if P0 and P1 are Ecran

    /// Add a first polyline with two points p1 and p2
    void ajouteSegment(TYPointParcours* p1, TYPointParcours* p2);

    /// Return true if _PolyligneP0 and _PolyligneP1 exist and are the same
    bool polylignesVoisinesPointentSurLaMemePolyligne();

    /**
     * @brief Return the point id of the next point given by IndexPoint id
     * @param IndexPoint The current point id
     * @param PolyligneSuivante The next polyline
     * @return The next point id
     */
    int indexePointSuivant(int  IndexPoint, TYPolyligneParcours*& PolyligneSuivante);
    /// Return the next polyline pointed by the Point id IndexPoint
    TYPolyligneParcours* polyligneSuivante(int IndexPoint);
    /// Return P0 if IndexPoint is the point id of P1, else P1 if it is the id of P0, else -1
    int autrePointDuSegment(int IndexPoint);

    /// Return true if the polyline belongs to a closed path
    bool estSurUnParcourFermee();
    /// Return true if the polyline is closed
    bool estFermee();
    /// Allocate nNbPoint points to the polyline
    bool allouer(int nNbPoint);
    /// Delete list of points of the polyline
    void desallouer();
    /// Copy operator
    void Copy(TYPolyligneParcours& p);

private:
    bool verifieNaturePolylignes();
};

#endif
//TYPOLYLIGNE_PARCOURS
