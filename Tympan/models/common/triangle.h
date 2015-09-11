/*
 * Copyright (C) <2012-2014> <EDF-R&D> <FRANCE>
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

#ifndef TY_TRIANGLE
#define TY_TRIANGLE

#include <deque>
#include <string>

#include "Tympan/models/common/3d.h"

/**
 * \file triangle.h
 * \class OTriangle
 * \brief Classe de definition d'un triangle
 */
class OTriangle
{
public:
    OTriangle(int p1, int p2, int p3);
    OTriangle(OPoint3D A, OPoint3D B, OPoint3D C);
    ~OTriangle();

    /**
     * \fn getSurface()
     * \brief calcul de la surface du triangle
     */
    double getSurface();

    /**
     * \fn OPoint3D getCenter()
     * \brief Gives back the triangle centre
     */
    OPoint3D getCentre();

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

    bool checkConsistencyWrtPointsTab(const std::deque<OPoint3D>& points) const;
    std::string reportInconsistencyWrtPointsTab(const std::deque<OPoint3D>& points);

    bool operator==(const OTriangle& other) const;

private:
    //! A static table of \c OPoint3D.
    static OPoint3D OTriangle::* vertices_m_ptr[3];
    //! A static table of coordinate.
    static int OTriangle::* indices_m_ptr[3];
};

::std::ostream& operator<<(::std::ostream& os, const OTriangle& );

#endif //TY_TRIANGLE
