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

#ifndef _O_TRIANGLE__
#define _O_TRIANGLE__

#include "OPoint3D.h"

/**
 * \file OTriangle.h
 * \class OTriangle
 * \brief Classe de definition d'un triangle
 */
class OTriangle
{
public:
    /**
    *   Constructor a partir de 3 entiers
    */
    OTriangle(int p1, int p2, int p3);

    /**
    *   Constructor a partir de 3 points sommets
    */
    OTriangle(OPoint3D A, OPoint3D B, OPoint3D C);

    /**
    *   Destructor.
    */
    ~OTriangle();

    /**
     * \fn getSurface()
     * \brief calcul de la surface du triangle
     */
    double getSurface();

    int _p1;
    int _p2;
    int _p3;

    OPoint3D _A;
    OPoint3D _B;
    OPoint3D _C;

    //! Get the \c OPoint3D from the specific index.
    OPoint3D& vertex(unsigned i);
    //! Get the \c OPoint3D from the specific index.
    const OPoint3D& vertex(unsigned i) const;

    //! Get coordinate (int) from index.
    int& index(unsigned i);
    //! Get coordinate (int) from index.
    int  index(unsigned i) const;

private:
    //! A static table of \c OPoint3D.
    static OPoint3D OTriangle::*vertices_m_ptr[3];
    //! A static table of coordinate.
    static int OTriangle::*indices_m_ptr[3];
};
#endif //_O_TRIANGLE__
