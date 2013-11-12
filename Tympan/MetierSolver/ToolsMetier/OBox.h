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


#ifndef __O_BOX__
#define __O_BOX__

#include "OCoord3D.h"
#include "OPoint3D.h"

/**
 * \file OBox.h
 * \class OBox
 * \brief The box class.
 * \author Projet_Tympan
 */

class OBox
{
    // Methods
public:
    /**
     * Default constructor.
     */
    OBox();


    /**
     * Copy constructor.
     *
     * \param box The object to be copied.
     */
    OBox(const OBox& box);

    /**
     * \fn OBox(const OCoord3D& min,const OCoord3D & max)
     * \brief Constructor from a 2 3D points.
     *
     * \param min  The minimum point.
     * \param max  The maximum point.
     */
    OBox(const OCoord3D& min, const OCoord3D& max);


    /**
     * \fn OBox(double x1, double y1, double z1,double x2,double y2, double z2)
     * \brief Constructs a new object from 6 doubles.
     *
     * \param x1 X  min coordinate.
     * \param y1 Y  min coordinate.
     * \param z1 Z  min coordinate.
     * \param x2 X  max coordinate.
     * \param y2 Y  max coordinate.
     * \param z2 Z  max coordinate.
     */
    OBox(double x1, double y1, double z1, double x2, double y2, double z2);

    /**
     * Destructor.
     */
    virtual ~OBox() {}


    /**
     * Assignment operator.
     *
     * \param box The source to be assigned.
     */
    virtual OBox& operator=(const OBox& box);


    /**
     * The equality operator.
     *
     * \param box The object reference with which this object is compared.
     */
    virtual bool operator==(const OBox& box) const;


    /**
     * \fn bool operator!=(const OBox& box) const
     * \brief The inequality operator.
     *
     * \param box The object reference with which this object is compared.
     */
    virtual bool operator!=(const OBox& box) const { return !operator==(box);}


    /**
     * \fn bool isInside(const OPoint3D& pt) const
     * \brief Test whether the point is inside the box or not.
     *
     * \param pt The point to test.
     */
    virtual bool isInside(const OPoint3D& pt) const;

    /**
     * \fn bool isInside2D(const OPoint3D& pt) const
     * \brief Test whether the point is inside the box or not (from upper point of view).
     *
     * \param pt The point to test.
     */
    virtual bool isInside2D(const OPoint3D& pt) const;

    /**
     * \fn bool isInContact(const OBox& box) const
     * \brief Test whether the boxes are in contact or not.
     *
     * \param box The box to test.
     */
    virtual bool isInContact(const OBox& box) const;


    /**
     * \fn void Enlarge(const OPoint3D& pt)
     * \brief Enlarge the box with the point if the point is outside the box.
     *
     * \param pt The point to test/make it larger.
     */
    virtual void Enlarge(const OPoint3D& pt);


    /**
     * \fn void Enlarge(float x, float y, float z)
     * \brief Enlarge the box with the point (x,y, z) if the point is outside the box.
     *
     * \param x coordonnee x du point a tester
     * \param y coordonnee y du point a tester
     * \param z coordonnee z du point a tester
     */
    virtual void Enlarge(float x, float y, float z);


    /**
     * \fn void Enlarge(const OBox& box)
     * \brief Enlarge this box with the box passed if this box does not contain the box passed.
     *
     * \param box The box to test.
     */
    virtual void Enlarge(const OBox& box);


    /**
     * \fn void Translate(const OPoint3D& vectorTranslate)
     * \brief Translate this box
     *
     * \param vector translation.
     */
    virtual void Translate(const OPoint3D& vectorTranslate);

public:
    OPoint3D _min;
    OPoint3D _max;
};

#endif // __O_BOX__
