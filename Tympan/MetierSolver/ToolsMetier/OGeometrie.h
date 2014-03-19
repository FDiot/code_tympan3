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
 */


#ifndef __O_GEOMETRIE_H
#define __O_GEOMETRIE_H

#include "OPoint3D.h"
#include "OVector3D.h"
#include "OBox.h"
#include <cmath>



////////////////////////////////////////////////////////////////////////////////
// Defines
////////////////////////////////////////////////////////////////////////////////

// Defines pour les calculs d'intersection
///Les elements sont confondus.
#define INTERS_CONFONDU 2
///L'intersection existe.
#define INTERS_OUI      1
///Pas d'intersection.
#define INTERS_NULLE    0

///Epsilon
#define EPSILON         0.0000001
///Epsilon precis.
#define EPSILON_PRECIS  0.0000000000001

#ifndef M_PI
///Pi.
#define M_PI        3.1415926535897932384626433832795
#endif

///2Pi.
#define M_2PI       6.283185307179586476925287766559


////////////////////////////////////////////////////////////////////////////////
// Macros
////////////////////////////////////////////////////////////////////////////////

/**
 * \fn inline double ABS(double a)
 * \brief Retourne la valeur absolue.
 *
 * \param a Un nombre.
 *
 * \return Le valeur absolue.
 */
inline double ABS(double a)
{
    return (a >= 0.0 ? a : -a);
}

/**
 * \fn inline double SIGNE(double a)
 * \brief Retourne le signe d'un nombre
 *
 * \param a Un nombre.
 *
 * \return Le signe (1 ou -1).
 */
inline double SIGNE(double a)
{
    return (a >= 0.0 ? 1 : -1);
}

/**
 * \fn inline double MAX(double a, double b)
 * \brief Retourne le plus grand nombre parmi 2
 *
 * \param a Le 1er nombre a comparer.
 * \param b Le 2eme nombre a comparer.
 *
 * \return Le plus grand nombre entre a et b.
 */
inline double MAX(double a, double b)
{
    return (a > b ? a : b);
}

/**
 * \fn inline double MAX3(double a, double b, double c)
 * \brief Retourne le plus grand nombre.
 *
 * \param a Le 1er nombre a comparer.
 * \param b Le 2eme nombre a comparer.
 * \param c Le 3eme nombre a comparer.
 *
 * \return Le plus grand nombre entre a, b et c.
 */
inline double MAX3(double a, double b, double c)
{
    return MAX(MAX(a, b), c);
}

/**
 * \fn inline double MIN(double a, double b)
 * \brief Retourne le plus petit nombre parmi 2.
 *
 * \param a Le 1er nombre a comparer.
 * \param b Le 2eme nombre a comparer.
 *
 * \return Le plus petit nombre entre a et b.
 */
inline double MIN(double a, double b)
{
    return (a < b ? a : b);
}

/**
 * \fn inline double DEGTORAD(double a)
 * \brief Convertit un angle en degres en un angle en radians.
 *
 * \param a Un angle en degres.
 *
 * \return Un angle en radians.
 */
inline double DEGTORAD(double a)
{
    return (a / 180.0 * M_PI);
}

/**
 * \fn inline double RADTODEG(double a)
 * \brief Convertit un angle en radians en un angle en degres.
 *
 * \param a Un angle en radians.
 *
 * \return Un angle en degres.
 */
inline double RADTODEG(double a)
{
    return (a / M_PI * 180.0);
}

/**
 * \fn inline double GRATORAD(double a)
 * \brief Convertit un angle en gradians en un angle en radians.
 *
 * \param a Un angle en gradians.
 *
 * \return Un angle en radians.
 */
inline double GRATORAD(double a)
{
    return (a / 200.0 * M_PI);
}

/**
 * \fn inline double RADTOGRA(double a)
 * \brief Convertit un angle en radians en un angle en gradians.
 *
 * \param a Un angle en radians.
 *
 * \return Un angle en gradians.
 */
inline double RADTOGRA(double a)
{
    return (a / M_PI * 200.0);
}

/**
 * \fn inline double GRATODEG(double a)
 * \brief Convertit un angle en gradians en un angle en degres.
 *
 * \param a Un angle en gradians.
 *
 * \return Un angle en degres.
 */
inline double GRATODEG(double a)
{
    return (a / 200.0 * 180.0);
}

/**
 * \fn inline double DEGTOGRA(double a)
 * \brief Convertit un angle en degres en un angle en gradians.
 *
 * \param a Un angle en degres.
 *
 * \return Un angle en gradians.
 */
inline double DEGTOGRA(double a)
{
    return (a / 180.0 * 200.0);
}

/**
 * \fn inline int ROUND(double a)
 * \brief calcul de la valeur arrondie d'un nombre.
 *
 * \param a Un nombre.
 *
 * \return La valeur arrondie.
 */
inline int ROUND(double a)
{
    return (a >= 0.0 ? (int)(a + 0.49999) : (int)(a - 0.49999));
}

/**
 * \fn inline double BORNE(double a, double b, double c)
 * \brief Borne un nombre.
 *
 * \param a Un nombre.
 * \param b La borne inferieur.
 * \param c La borne superieur.
 *
 * \return Le nombre ou une des bornes.
 */
inline double BORNE(double a, double b, double c)
{
    return (a < b ? b : (a > c ? c : a));
}

/**
 * \fn inline int BORNE(int a)
 * \brief Borne un nombre entre 0 et 255.
 *
 * \param a Un nombre.
 *
 * \return Le nombre ou une des bornes.
 */
inline int BORNE(int a)
{
    return (int) BORNE((double) a, 0, 255);
}


//OCoord3D operator + (const OCoord3D& coord, const OVector3D& vect)
//{
//	return OCoord3D( coord._x + vect._x, coord._y + vect._y, coord._z + vect._z );
//}

//OPoint3D operator + (const OPoint3D& coord, const OVector3D& vect)
//{
//	return OPoint3D( coord._x + vect._x, coord._y + vect._y, coord._z + vect._z );
//}


////////////////////////////////////////////////////////////////////////////////
// Methodes geometriques generalistes
////////////////////////////////////////////////////////////////////////////////

/**
 * \file OGeometrie.h
 * \class OGeometrie
 * \brief Class utilitaire de geometrie.
 *
 * \author Projet_Tympan
 *
 */
class OGeometrie
{
    // Methods
public:
    /**
     * \fn static bool intersDemiSegmentAvecSegment(const OPoint3D& ptS, const OPoint3D& ptA, const OPoint3D& ptB);
     * \brief Retourne true si l'horizontale a partir du point ptS intersecte le segment definit par ptA et ptB.
     *
     * \param ptS Le Le point du demi segment.
     * \param ptA Le 1er point pour le sgment.
     * \param ptB Le 2eme point pour le sgment.
     *
     * \return true  si l'intersection existe,
     *         false sinon.
     */
    static bool intersDemiSegmentAvecSegment(const OPoint3D& ptS, const OPoint3D& ptA, const OPoint3D& ptB);

    /**
     * \fn static int intersDroitesPoints(const OPoint3D& ptA, const OPoint3D& ptB, const OPoint3D& ptC, const OPoint3D& ptD, OPoint3D& ptI);
     * \brief Calcul de l'intersection entre deux droites definies chacune par 2 points.
     *
     * \param ptA Le 1er point pour la 1ere droite.
     * \param ptB Le 2eme point pour la 1ere droite.
     * \param ptC Le 1er point pour la 2eme droite.
     * \param ptD Le 2eme point pour la 2eme droite.
     * \param ptSym Le point resultant de l'intersection.
     *
     * \return INTERS_OUI (1) si l'intersection existe,
     *         INTERS_NULLE (0) s'il n'y a pas d'intersection.
     */
    static int intersDroitesPoints(const OPoint3D& ptA, const OPoint3D& ptB, const OPoint3D& ptC, const OPoint3D& ptD, OPoint3D& ptI);

    /**
     * \fn static int intersDroitesPointVecteur(const OPoint3D& ptA, const OVector3D& vecA, const OPoint3D& ptB, const OVector3D& vecB, OPoint3D& ptI);
     * \brief Calcul de l'intersection entre deux droites definies chacune par un point et un vecteur.
     *
     * \param ptA Point pour la 1ere droite.
     * \param vecA Vecteur pour la 1ere droite.
     * \param ptB Point pour la 2eme droite.
     * \param vecB Vecteur pour la 2eme droite.
     * \param ptSym Le point resultant de l'intersection.
     *
     * \return INTERS_OUI (1) si l'intersection existe,
     *         INTERS_NULLE (0) s'il n'y a pas d'intersection.
     */
    static int intersDroitesPointVecteur(const OPoint3D& ptA, const OVector3D& vecA, const OPoint3D& ptB, const OVector3D& vecB, OPoint3D& ptI);

    /**
     * \fn static double symPointDroite(const OPoint3D& ptA, const OPoint3D& ptB, const OPoint3D& ptP, OPoint3D& ptI);
     * \brief Calul du point symetrique au point passe par rapport a une droite definie par 2 points.
     *
     * \param ptA Le 1er point pour la 1ere droite.
     * \param ptB Le 2eme point pour la 1ere droite.
     * \param ptP Le point dont on veut le symetrique.
     * \param ptI Le point resultant de la symetrie.
     *
     * \return Le coefficient k (compris entre 0 et 1 si le segment
     *         [Point source / Point image] coupe le segment [ptA ptB]).
     */
    static double symPointDroite(const OPoint3D& ptA, const OPoint3D& ptB, const OPoint3D& ptP, OPoint3D& ptI);

    /**
     * \fn static bool pointInPolygonAngleSum(const OPoint3D& ptP, const OPoint3D* pts, int nbPts);
     * \brief Tests if a point is inside a polygon using angle sum algorithm.
     *
     * This solution was motivated by solution wich consists to compute the sum
     * of the angles made between the test point and each pair of points making
     * up the polygon and correspondence with Reinier van Vliet and Remco Lam.
     * If this sum is 2pi then the point is an interior point, if 0 then the point
     * is an exterior point.
     * To determine whether a point is on the interior of a convex polygon in 3D
     * one might be tempted to first determine whether the point is on the plane,
     * then determine it's interior status. Both of these can be accomplished at
     * once by computing the sum of the angles between the test point (ptP) and
     * every pair of edge points pts[i]->pts[i+1]. This sum will only be 2pi if
     * both the point is on the plane of the polygon AND on the interior.
     * The angle sum will tend to 0 the further away from the polygon point ptP becomes.
     *
     * \param ptP Le point a tester.
     * \param pts Un tableau de points constituant le polygone.
     * \param nbPts Le taille du tableau de points (doit etre >= 3).
     *
     * \return <code>true</code> si le point se trouve a l'interieur du polygone,
     *         <code>false</code> sinon.
     */
    static bool pointInPolygonAngleSum(const OPoint3D& ptP, const OPoint3D* pts, int nbPts);

    /**
     * \fn static bool pointInPolygonRayCasting(const OPoint3D& ptP, const OPoint3D* pts, int nbPts, const OVector3D& normal,const OBox & box);
     * \brief Tests if a point is inside a polygon using ray casting algorithm.
     *
     * \param ptP Le point a tester.
     * \param pts Un tableau de points constituant le polygone.
     * \param nbPts Le taille du tableau de points (doit etre >= 3).
     * \param normal La normale au polygone.
     * \param ptMin point min de la bouding box du polygon
     * \param ptMax point max de la bouding box du polygon
     *
     * \return <code>true</code> si le point se trouve a l'interieur du polygone,
     *         <code>false</code> sinon.
     */
    static bool pointInPolygonRayCasting(const OPoint3D& ptP, const OPoint3D* pts, int nbPts);

    /**
     * \fn static bool shortestSegBetween2Lines(const OPoint3D& pt1, const OPoint3D& pt2, const OPoint3D& pt3, const OPoint3D& pt4, OPoint3D& ptA, OPoint3D& ptB, double *mua, double *mub);
     * \brief Calculates the line segment PaPb that is the shortest route between two lines P1P2 and P3P4.
     *
     * Calculates also the values of mua and mub where:
     *    Pa = P1 + mua (P2 - P1)
     *    Pb = P3 + mub (P4 - P3)
     *
     * Remark: The values of mua and mub range from negative to positive infinity.
     * The line segments between P1 P2 and P3 P4 have their corresponding mu
     * between 0 and 1.
     *
     * \param pt1 A 1st point on the 1st line.
     * \param pt2 A 2nd point on the 1st line.
     * \param pt3 A 1st point on the 2nd line.
     * \param pt4 A 2nd point on the 2nd line.
     * \param ptA The 1st point of the resulting segment.
     * \param ptB The 2nd point of the resulting segment.
     * \param mua The mua value result.
     * \param mub The mub value result.
     *
     * \return <code>false</code> if no solution exists,
     *         <code>true</code> otherwise.
     */
    static bool shortestSegBetween2Lines(const OPoint3D& pt1, const OPoint3D& pt2, const OPoint3D& pt3, const OPoint3D& pt4,
                                         OPoint3D& ptA, OPoint3D& ptB, double* mua, double* mub);

    /**
     * \fn  static void boundingBox(OPoint3D* pts, int nbPts, OPoint3D& ptMin, OPoint3D& ptMax);
     * \brief Computes the simple bounding box for a volume using min-max method.
     *
     * \param pts Un tableau de points constituant le volume.
     * \param nbPts Le taille du tableau de points.
     * \param ptMin Le point contenant les valeurs min en X, Y et Z.
     * \param ptMax Le point contenant les valeurs max en X, Y et Z.
     */
    static void boundingBox(OPoint3D* pts, int nbPts, OPoint3D& ptMin, OPoint3D& ptMax);

    /**
     * \fn static void computeNormal(OPoint3D* pts, int nbPts, OVector3D & normal);
     * \brief Computes the normal of the list of points.
     *
     * \param pts Un tableau de points constituant le volume.
     * \param nbPts Le taille du tableau de points.
     * \param resulting normal.
     */
    static void computeNormal(OPoint3D* pts, int nbPts, OVector3D& normal);
};


#endif // __O_GEOMETRIE_H
