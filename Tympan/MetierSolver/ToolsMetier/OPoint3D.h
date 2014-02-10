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


#ifndef __O_POINT_3D__
#define __O_POINT_3D__


#include "OCoord3D.h"


/**
 * \file OPoint3D.h
 * \class OPoint3D
 * \brief The 3D point class.
 * \author Projet_Tympan
 */

class OPoint3D: public OCoord3D
{
    // Methods
public:
    /**
     * \brief Default constructor.
     */
    OPoint3D();

    /**
     * \brief Copy constructor.
     *
     * \param pt The object to be copied.
     */
    OPoint3D(const OPoint3D& pt);

    /**
     * \brief Constructor from a 3D coord.
     *
     * \param coord The object to be copied.
     */
    OPoint3D(const OCoord3D& coord);

    /**
     * \fn OPoint3D(double x, double y, double z);
     * \brief Constructs a new object from 3 doubles.
     *
     * \param x X coordinate.
     * \param y Y coordinate.
     * \param z Z coordinate.
     */
    OPoint3D(double x, double y, double z);

    /**
     * \fn OPoint3D(double v[]);
     * \brief Constructs a new object from 3 doubles.
     *
     * \param v table of coordinates.
     */
    OPoint3D(double v[]);

    /**
     * \brief Destructor.
     */
    virtual ~OPoint3D();

    /**
     * \fn double distFrom(const OPoint3D& pt) const;
     * \brief Computes the distance from this point to another.
     *
     * \param pt The point to mesure the distance from.
     */
    double distFrom(const OPoint3D& pt) const;

    /**
     * \fn double dist2DFrom(const OPoint3D& pt) const;
     * \brief Computes the distance from this point to another in 2D plan
     */
    double dist2DFrom(const OPoint3D& pt) const;

    /**
     * \fn void set(double x, double y, double z);
     * \brief initialize with 3 doubles.
     *
     * \param x X coordinate.
     * \param y Y coordinate.
     * \param z Z coordinate.
     */
    void set(double x, double y, double z);

    /// Compatibility alias for operator==
    bool isEqual(const OPoint3D& oPoint) const
    {
        return *this == oPoint;
    }
};


#endif // __O_POINT_3D__
