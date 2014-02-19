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


#ifndef __O_VECTOR_3D__
#define __O_VECTOR_3D__


#include "OCoord3D.h"


/**
 * \file OVector3D.h
 * \class OVector3D
 * \brief The 3D vector class.
 * \author Projet_Tympan
 */

class OVector3D: public OCoord3D
{
    // Methods
public:
    /**
     * Default constructor.
     */
    OVector3D();

    /**
     * Copy constructor.
     *
     * \param vector The object to be copied.
     */
    OVector3D(const OVector3D& vector);

    /**
     * \fn OVector3D(const OCoord3D& coord);
     * \brief Constructs a new vector from a 3D coordinate. Works like the copy constructor.
     * \param coord A 3D coordinate.
     */
    OVector3D(const OCoord3D& coord);

    /**
     * \fn OVector3D(const OCoord3D& coordFrom, const OCoord3D& coordTo);
     * \brief Constructs a new vector from 2 coordinates.
     *
     * \param coordFrom The 1st coordinate.
     * \param coordTo The 2nd coordinate.
     */
    OVector3D(const OCoord3D& coordFrom, const OCoord3D& coordTo);

    /**
     * \fn OVector3D(double x, double y, double z);
     * \brief Constructs a new vector from 3 doubles.
     *
     * \param x X coordinate.
     * \param y Y coordinate.
     * \param z Z coordinate.
     */
    OVector3D(double x, double y, double z);

    /**
     * Destructor.
     */
    virtual ~OVector3D();

    /**
     * \fn void reset();
     * \brief Reset this vector.
     */
    void reset();

    /**
     * \fn OVector3D & operator=(const OVector3D& vector);
     * \brief Assignment operator.
     *
     * \param vector The source to be assigned.
     */
    OVector3D& operator=(const OVector3D& vector);

    /**
     * \brief The equality operator.
     *
     * \param vector The object reference with which this object is compared.
     */
    bool operator==(const OVector3D& vector) const;

    /**
     * \brief The inequality operator.
     *
     * \param vector The object reference with which this object is compared.
     */
    bool operator!=(const OVector3D& vector) const;

    /**
     * \fn OVector3D operator+(const OVector3D& vector) const;
     * \brief Addition.
     *
     * \param vector The object reference with which this object is added.
     */
    OVector3D operator+(const OVector3D& vector) const;

    /**
     * \fn  OVector3D operator-(const OVector3D& vector) const;
     * \brief Substraction.
     *
     * \param vector The object reference with which this object is substracted.
     */
    OVector3D operator-(const OVector3D& vector) const;

    /**
     * \fn OVector3D operator*(const OVector3D& vector) const;
     * \brief Multiplication.
     *
     * \param vector The object reference with which this object is multiplicated.
     */
    OVector3D operator*(const OVector3D& vector) const;

    /**
     * \fn OVector3D operator*(const double a) const;
     * \brief Multiplication with a scalar (commutative).
     *
     * \param a A scalar value.
     */
    OVector3D operator*(const double a) const;

    /**
     * \fn friend OVector3D operator*(const double a, const OVector3D& vector);
     * \brief Multiplication with a scalar (commutative).
     *
     * \param a A scalar value.
     */
    friend OVector3D operator*(const double a, const OVector3D& vector);

    /**
     * \fn OVector3D cross(const OVector3D& vector) const;
     * \brief Cross product.
     *
     * \param vector The object reference with which this object is multiplicated.
     *
     * \return The result of the cross product.
     */
    OVector3D cross(const OVector3D& vector) const;

    /**
     * \fn void balance(double c1, const OVector3D& vector2, double c2);
     * \brief Balances this vector.
     *
     * \param c1 The balance value for this vector.
     * \param vector2 The other member for the balance.
     * \param c2 The balance value for the second vector.
     */
    void balance(double c1, const OVector3D& vector2, double c2);

    /**
     * \fn double scalar(const OVector3D& vector) const;
     * \brief Performs the scalar product between this object and another vector.
     *
     * \param vector The other member for the scalar product.
     *
     * \return The result of the scalar product.
     */
    double scalar(const OVector3D& vector) const;

    /**
     * \fn double norme() const;
     * \brief Computes the length of this vector.
     *
     * \return The length of this vector.
     */
    double norme() const;

    /**
     * \fn OVector3D normal(const OVector3D& vector2, const OVector3D& vector3) const;
     * \brief Computes the normal with this vector and 2 others.
     *
     * @return The normal.
     */
    OVector3D normal(const OVector3D& vector2, const OVector3D& vector3) const;

    /**
     * \fn void normalize();
     * \brief Normalizes this vector.
     */
    void normalize();

    /**
     * \fn void invert();
     * \brief Inverts this vector.
     */
    void invert();

    /**
     * \fn double angle(const OVector3D& vector) const;
     * \brief Computes the angle between this vector and another vector.
     *
     * \param vector The vector to mesure the angle with this vector.
     */
    double angle(const OVector3D& vector) const;

    /**
    * \fn OVector3D getRotationOz(double alpha, OVector3D V)
    * \brief Returns the vector after a rotation around z axis
    * \ x -> xprime. Both of these vectors will be given in the
    * \ original base.
    */
    OVector3D getRotationOz(double alpha);

    /**
    * \fn OVector3D getRotationOzBase2(double alpha, OVector3D V)
    * \brief Returns the vector after a rotation around z axis
    * \ and gives back the coordinates of xprime or yprime in the
    *\ new basis Bprime.
    */
    OVector3D getRotationOzBase2(double alpha);

    /**
    * \fn OVector3D getRotationOy(double alpha, OVector3D V)
    * \brief Returns the vector after a rotation around z axis
    */
    OVector3D getRotationOy(double alpha);

    /**
    * \fn OVector3D getRotationOyBase2(double alpha, OVector3D V)
    * \brief Returns the vector after a rotation around y axis
    * \ and gives back the coordinates of xprime or zprime in the
    *\ new basis Bprime.
    */
    OVector3D getRotationOyBase2(double alpha);

    /**
    * \fn OVector3D getRotationOzOy(double alpha, double theta, OVector3D V)
    * \brief Returns the vector after 2 rotations around z axis and
    * \y axis. It gives the coordinates of xsecond, ysecond and zsecond in the
    *\ first basis B.
    */
    OVector3D getRotationOzOy(double alpha, double theta);
};

inline OCoord3D operator + (const OCoord3D& coord, const OVector3D& vect)
{
    return OCoord3D(coord._x + vect._x, coord._y + vect._y, coord._z + vect._z);
}

#endif // __O_VECTOR_3D__
