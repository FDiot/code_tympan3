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


#ifndef __O_Line_3D__
#define __O_Line_3D__


#include "OCoord3D.h"
#include "OVector3D.h"
#include "OPoint3D.h"
#include "OSegment3D.h"


/**
 * \file OLine3D.h
 * \class OLine3D
 * \brief The 3D Line class.
 * \author Projet_Tympan
 *
 */
class OLine3D
{
    // Methods
public:
    /**
     * \brief Default constructor.
     */
    OLine3D();

    /**
     * \brief opy constructor.
     *
     * \param Line The object to be copied.
     */
    OLine3D(const OLine3D& Line);

    /**
     * \brief Constructs a new Line from 2 coordinates.
     *
     * \param coordFrom The 1st coordinate.
     * \param coordTo The 2nd coordinate.
     */
    OLine3D(const OCoord3D& origin, const OVector3D& dir);

    /**
     * \brief Destructor.
     */
    virtual ~OLine3D();

    /**
     * \brief Assignment operator.
     *
     * \param Line The source to be assigned.
     */
    OLine3D& operator=(const OLine3D& Line);

    /**
     * \brief The equality operator.
     *
     * \param Line The object reference with which this object is compared.
     */
    bool operator==(const OLine3D& Line) const;

    /**
     * \brief The inequality operator.
     *
     * \param Line The object reference with which this object is compared.
     */
    bool operator!=(const OLine3D& Line) const;

    /**
     * \fn  int intersects(const OPoint3D& pt1, const OPoint3D& pt2, OPoint3D& ptIntersec) const;
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
     *
     * \return INTERS_OUI (1) si l'intersection existe,
     *         INTERS_CONFONDU (2) si les elements sont confondus,
     *         INTERS_NULLE (0) s'il n'y a pas d'intersection.
     */
    int intersects(const OPoint3D& pt1, const OPoint3D& pt2, OPoint3D& ptIntersec) const;

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

    // Line is expressed in a parametric form, ie:
    // P = O + t.D
    // where is the origin point of the line
    // and D the vector of direction

    ///Origin point of the line
    OCoord3D    _Origin;

    /// Vector of direction of the line
    OVector3D   _Dir;
};


#endif // __O_Line_3D__
