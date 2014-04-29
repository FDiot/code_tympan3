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


#ifndef __O_HPLANE_3D__
#define __O_HPLANE_3D__


#include "OCoord3D.h"
#include "OVector3D.h"
#include "OPoint3D.h"
#include "OSegment3D.h"


/**
 * \file OHPlane3D.h
 * \class OHPlane3D
 * \brief The 3D Plane class using Hessian normal form
 *
 * the general equation of a plane  is: a.x + b.y + c.z = d
 *
 * by defining the components of the unit normal vector , N = (Nx, Ny, Nz)
 *  Nx = a / sqrt(a²+b²+c²)
 *  Ny = b / sqrt(a²+b²+c²)
 *  Nz = c / sqrt(a²+b²+c²)
 *
 * and the constant: p = d / sqrt(a²+b²+c²)
 *
 * Then the Hessian normal form of the plane is: N.X = -p
 *
 * \author Projet_Tympan
 *
 */
class OHPlane3D
{
    // Methods
public:
    /**
     * Default constructor.
     */
    OHPlane3D();

    /**
     * \brief Copy constructor.
     *
     * \param Line The object to be copied.
     */
    OHPlane3D(const OHPlane3D& Plane);

    /**
     * \brief Constructs a new Plane.
     *
     * \param normal The normal to the plane.
     * \param origin A point of the plane
     */
    OHPlane3D(const OVector3D& normal, const OPoint3D& origin);

    /**
     * \brief Destructor.
     */
    virtual ~OHPlane3D();

    /**
     * \brief set a new Plane.
     *
     * \param normal The normal to the plane.
     * \param origin A point of the plane
     */
    void set(const OVector3D& normal, const OPoint3D& origin);

    /**
     * \brief Assignment operator.
     *
     * \param Plane The source to be assigned.
     */
    OHPlane3D& operator=(const OHPlane3D& Plane);

    /**
     * \brief The equality operator.
     *
     * \param Plane The object reference with which this object is compared.
     */
    bool operator==(const OHPlane3D& Plane) const;

    /**
     * \brief The inequality operator.
     *
     * \param Plane The object reference with which this object is compared.
     */
    bool operator!=(const OHPlane3D& Plane) const;

    /**
     * \fn int intersects(const OPoint3D& pt1, const OPoint3D& pt2, OPoint3D& ptIntersec, double& t) const;
     * \brief Calcul de l'intersection avec un segment defini par deux points.
     *
     * Remarque :
     *  Dans le cas ou le segment est compris dans la ligne, le resultat
     *  est INTERS_CONFONDU, et les coordonnees de l'intersection ne
     *  sont pas mises a jour.
     *
     * \param pt1 Le 1er point du segment.
     * \param pt2 Le 2eme point du segment.
     * \param ptIntersec Le point correspondant a l'intersection.
     * \param t le coefficient barycentrique du pt d'intersection sur (pt1, pt2).
     *
     * \return INTERS_OUI (1) si l'intersection existe,
     *         INTERS_CONFONDU (2) si les elements sont confondus,
     *         INTERS_NULLE (0) s'il n'y a pas d'intersection.
     */
    int intersects(const OPoint3D& pt1, const OPoint3D& pt2, OPoint3D& ptIntersec, double& t) const;
    /**
     * \fn int intersects(const OSegment3D& seg, OPoint3D& ptIntersec) const;
     * \brief Calcul de l'intersection avec un segment
     *
     * Remarque :
     *  Dans le cas ou le segment est compris dans la ligne, le resultat
     *  est INTERS_CONFONDU, et les coordonnees de l'intersection ne
     *  sont pas mises a jour.
     *
     * \param seg Le segment.
     * \param ptIntersec Le point correspondant a l'intersection.
     *
     * \return INTERS_OUI (1) si l'intersection existe,
     *         INTERS_CONFONDU (2) si les elements sont confondus,
     *         INTERS_NULLE (0) s'il n'y a pas d'intersection.
     */
    int intersects(const OSegment3D& seg, OPoint3D& ptIntersec) const;


private:
    OVector3D   _N;
    OPoint3D    _O;
    double      _p;
};


#endif // __O_Line_3D__
