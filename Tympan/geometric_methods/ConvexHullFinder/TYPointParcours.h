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

#ifndef TYPOINT_PARCOURS_H
#define TYPOINT_PARCOURS_H

#define SEUIL_DISTANCE_POINTS_CONFONDUS 0.02 	//!< Below a distance of 2 centimeters, the points are considered on the same location
#define SEUIL_DETERMNANT_COLINEAIRE     (1E-10)	//!< Below this absolute value, the determinant is not a valid criteria to assess the collinearity of two vectors

/**
 * \struct TYPointParcours
 * \brief Point of a path
 */
struct TYPointParcours
{
    double x;	//!< x coordinate of the point
    double y;	//!< y coordinate of the point
    double z;	//!< z coordinate of the point
    int Identifiant;	//!< Point id
    bool isInfra;		//!< Flag set to indicate if the point is an infrastructure
    bool isEcran;		//!< Flag set to indicate if the point is a screen
    static const double _dSeuilDistanceCarre; //!< Square of the threshold distance between two points to indicate they lie on the location

    bool operator == (TYPointParcours& p); //!< Return true if this point and p are on same location

    double normeCarree(); //!< Return x^2+y^2

    static bool Confondus(TYPointParcours* P1, TYPointParcours* P2); //!< Return true if P1 and P2 are on the same location (separated by less than a threshold distance)
    static double ZCross(TYPointParcours SR, TYPointParcours SP); //!< Return cross product applied to SR and SP points
    static bool IntersectionDroites(TYPointParcours& P1, TYPointParcours& P2, TYPointParcours& P3, TYPointParcours& P4, TYPointParcours& P); //!< Return false if (P1P2) and (P3P4) are parallel, true if not with P the intersection
    static bool IntersectionSegments(TYPointParcours& P1, TYPointParcours& P2, TYPointParcours& P3, TYPointParcours& P4, TYPointParcours& P); //!< Return false if [P1P2] and [P3P4] does not intersect, true if not with P the intersection
    static double Scalaire(TYPointParcours& P1, TYPointParcours& P2, TYPointParcours& P3, TYPointParcours& P4); //!< Return scalar product P2P1.P4P3
    static double Scalaire(TYPointParcours& vecteur1, TYPointParcours& vecteur2); //!< Return scalar product vecteur1.vecteur2
    static TYPointParcours vecteur2D(TYPointParcours& P1, TYPointParcours& P2); //!< Return a TYPointParcours point P=P1P2=P2-P1
    static double AbscisseCurviligneCarreSurSR(TYPointParcours& P, TYPointParcours& S, TYPointParcours& R); //!< Return the square of the curvilinear abscissa of point P on [SR]
};

#endif//TYPOINT_PARCOURS_H
