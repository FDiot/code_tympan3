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


#ifndef __O_BOX2__
#define __O_BOX2__

#include "OPoint3D.h"
#include "ORepere3D.h"
#include "OBox.h"

/**
 * \file OBox2.h
 * \class OBox2
 * \brief classe de definition d'une boite (pas forcement parallele aux axes a la difference de la OBox)
 * \author Projet_Tympan
 *
 */
class OBox2 : public OBox
{
    // Methods
public:
    /**
     * \fn OBox2()
     * \brief Default constructor.
     */
    OBox2();

    /**
     * \fn OBox2(const OBox2 & box)
     * \brief Copy constructor.
     * \param box The object to be copied.
     */
    OBox2(const OBox2& box);

	/**
     * \fn OBox2(const OBox & box, ORepere3D & repere)
     * \brief Constructor from a box and its local coordinate system
     * \param box The box define in the local coordinate system
     * \param repere The global coordinate system of the box
     */
    OBox2(const OBox& box, const ORepere3D& repere);

    /**
     * \fn OBox2(const OBox & box, ORepere3D & repere)
     * \brief Constructor from a box and its local coordinate system
     * \param box The box define in the local coordinate system
     * \param repere The global coordinate system of the box
	 * \param centre Is the box centre, this point is the middle of [S'R]
     */
    OBox2(const OBox2& box, const ORepere3D& repere, const OPoint3D& centre);

    /**
     * \fn OBox2(const OCoord3D& min,const OCoord3D & max, ORepere3D & repere)
     * \brief Constructor from a 2 3D points and its local coordinate system
     * \param min  The minimum point.
     * \param max  The maximum point.
     * \param repere The local coordinate system (where the box's sides are parallel to its axes)
     */

    OBox2(const OCoord3D& min, const OCoord3D& max, const ORepere3D& repere);

	/**
	 * \fn OBox2(const ORepere3D, const OCoord3D& min, const OCoord3D& max); 
	 * \brief Constructor : from a local coordinate system centered inside the box
	 * \param repere Local coordinate system from the box center
	 * \param min The minimum point i.e. the one on the left bottom corner.
	 * \param max The maximum point i.e. the one on the right upper corner.
	 * \param A(0,0,0) B(0,1,0) C(1,1,0) D(1,0,0) E(1,0,1) F(0,0,1) G(0,1,1) H(1,1,1)
	*/
	OBox2(const ORepere3D repere, const OCoord3D& A, const OCoord3D& B, const OCoord3D& C, 
			const OCoord3D& D, const OCoord3D& E, const OCoord3D& F, const OCoord3D& G, 
			const OCoord3D& H); 


    /**
     * Destructor.
     */
    virtual ~OBox2() {}

    /**
     * Assignment operator.
     *
     * \param box The box to be assigned.
     */
    virtual OBox2& operator=(const OBox2& box);

    /**
     * The equality operator.
     *
     * \param box The object reference with which this object is compared.
     */
    virtual bool operator==(const OBox2& box) const;

    /**
     * The inequality operator.
     *
     * \param box The object reference with which this object is compared.
     */
    virtual bool operator!=(const OBox2& box) const { return !operator==(box);}

	/** 
	 *\fn OCoord3D BoxCoord(int N);
	 *\brief Returns the coordinates of one of the box corner.
	 *\ We consider that the first corner is the one on the bottom left side
	 *\ and then we go clockwise, upper and the same.
	 *\ N must be between 0 and 8, where 0 represents the box center.
	 *\ 1(0,0,0) 2(0,1,0) 3(1,1,0) 4(1,0,0) 5(1,0,1) 6(0,0,1) 7(0,1,1) 8(1,1,1)
	 *\param N is the corner we want the coordinates of.
	 */
	OPoint3D BoxCoord(int N) const;

	/**
	 *\fn OVector3D getRotationOzOy(double alpha, double theta, OVector3D V);
	 *\brief Returns a vector after 2 rotations around Oz and Oy axis.
	 *\param alpha is the first angle needed to rotate around Oz.
	 *\param theta is the second angle needed to rotate around Oy.
	 *\param V is the initial vector.
	*/
	OVector3D GetRotationOzOy(double alpha, double theta, OVector3D V);

    /**
     * \fn bool isInside(const OPoint3D& pt) const
     * \brief Test whether the point is inside the box or not.
     * \param pt The point to test.
     */
    virtual bool isInside(const OPoint3D& pt) const;

    /**
     * \fn bool isInside2D(const OPoint3D& pt) const
     * \brief Test whether the point is inside the box or not (from upper point of view).
     * \param pt The point to test.
     */
    virtual bool isInside2D(const OPoint3D& pt) const;

    /**
     * \fn bool isInContact(const OBox2& box) const
     * \brief Test whether the boxes are in contact or not.
     * \param box The box to test.
     */
    //virtual bool isInContact(const OBox2& box) const;

    /**
     * \fn void Enlarge(const OPoint3D& pt)
     * \brief Enlarge the box with the point if the point is outside the box.
     * \param pt The point to test/make it larger.
     */
    //virtual void Enlarge(const OPoint3D& pt);

    /**
     * \fn void Enlarge(const OBox2& box)
     * \brief Enlarge this box with the box passed if this box does not contain the box passed.
     * \param box The box to test.
     */
    //virtual void Enlarge(const OBox2& box);

    /**
     * \fn void Translate(const OPoint3D& vectorTranslate)
     * \brief Translate this box
     * \param vector translation.
     */
    virtual void Translate(const OPoint3D& vectorTranslate);

	/**
	 *\fn void BoxRotationOzOy(const OBox2& box);
	 *\brief Computes the box rotation around Oz and Oy (usual coordinates system).
	 *\param box is an OBox2.
	 */
	void BoxRotationOzOy(double alpha, double theta);

	/**
	 * fn OBox2 boxRotation(const OVector3D& v1, const OVector3D& v2)
	 * brief return a box rotated by two vectors
	 */
	OBox2 boxRotation(const OPoint3D& O, const OPoint3D& P2);

private:
	/**
	 * fn OBox2 rotInXOYOnly(const OVector3D& v1, const OVector3D& v2)
	 * brief return a box rotated by two vectors
	 */
	OBox2 rotInXOYOnly(const OVector3D& v1, const OVector3D& v2, const OPoint3D& O, const OPoint3D& P2);

	/**
	 * fn OBox2 rotInXOZOnly(const OVector3D& v1, const OVector3D& v2)
	 * brief return a box rotated by two vectors
	 */
	OBox2 rotInXOZOnly(const OVector3D& v1, const OVector3D& v2, const OPoint3D& O, const OPoint3D& P2);

	/**
	 * fn OBox2 rot3D(const OVector3D& v1, const OVector3D& v2)
	 * brief return a box rotated by two vectors
	 */
	OBox2 rot3D(const OVector3D& v1, const OVector3D& v2, const OPoint3D& O, const OPoint3D& P2);

public:
    ORepere3D _repere;
	OPoint3D _center;
	OPoint3D _A;
	OPoint3D _B;
	OPoint3D _C;
	OPoint3D _D;
	OPoint3D _E;
	OPoint3D _F;
	OPoint3D _G;
	OPoint3D _H;
	double _length;
	double _height;
	double _width;
};

#endif // __O_BOX2__
