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

#ifndef __O_COORD_3D__
#define __O_COORD_3D__


/**
 * \file OCoord3D.h
 * \class OCoord3D
 * \brief The 3D coordinate class.
 * \author Projet_Tympan
 */

class OVector3D;

class OCoord3D
{
    // Methods
public:
    /**
     * Default constructor.
     */
    OCoord3D();

    /**
     * Copy constructor.
     *
     * @param coord The object to be copied.
     */
    OCoord3D(const OCoord3D& coord);

    /**
     * Constructs a new object from 3 doubles.
     *
     * @param x X coordinate.
     * @param y Y coordinate.
     * @param z Z coordinate.
     */
    OCoord3D(double x, double y, double z);

    /**
     * Destructor.
     */
    virtual ~OCoord3D();

    // Déclaration d'amitié avec une méthode définie dans OGeometrie.h
    friend OCoord3D operator + (const OCoord3D& coord, const OVector3D& vect);

    /**
     * Assignment operator.
     *
     * @param coord The source to be assigned.
     */
    OCoord3D& operator=(const OCoord3D& coord);

    /**
     * The equality operator.
     *
     * @param coord The object reference with which this object is compared.
     */
    bool operator==(const OCoord3D& coord) const;

    /**
     * The inequality operator.
     *
     * @param coord The object reference with which this object is compared.
     */
    bool operator!=(const OCoord3D& coord) const;

    /**
     * \fn  void setCoords(double, double, double)
     * \brief Sets the coordinates as an array of double.
     */
    void setCoords(double x, double y, double z);

    /**
     * \fn  void setCoords(double coords[3]);
     * \brief Sets the coordinates as an array of double.
     */
    void setCoords(double coords[3]);

    /**
     * \fn void getCoords(double coords[3]);
     * \brief Gets the coordinates as an array of double.
     */
    void getCoords(double coords[3]);

    /**
     * \fn double* getCoords();
     * \brief Gets the coordinates as an array of double.
     */
    double* getCoords();

    /**
     * cast operator to get coordinates via a table.
     */
    operator const double* () const { return &_value[0]; }

    /**
     * cast operator to get coordinates via a table.
     */
    operator double* () { return &_value[0]; }


    // Members
public:
    union
    {
        struct
        {
            ///Coordonnee en X.
            double _x;
            ///Coordonnee en Y.
            double _y;
            ///Coordonnee en Z.
            double _z;
        };
        double _value[3];
    };
};


#endif // __O_COORD_3D__
