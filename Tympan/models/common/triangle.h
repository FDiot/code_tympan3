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
 * \brief Triangle class
 */
class OTriangle
{
public:
	/// Constructor b
    OTriangle(int p1, int p2, int p3);
    /// Constructor by 3 OPoint3D
    OTriangle(OPoint3D A, OPoint3D B, OPoint3D C);
    /// Destructor
    ~OTriangle();

    /**
     * \fn getSurface()
     * \brief Compute the triangle surface
     */
    double getSurface();

    /**
     * \fn OPoint3D getCentre()
     * \brief Gives back the triangle center
     */
    OPoint3D getCentre();

    int _p1;	//!< Index of the first OPoint3D _A
    int _p2;	//!< Index of the second OPoint3D _B
    int _p3;	//!< Index of the third OPoint3D _C

    OPoint3D _A;	//!< First OPoint3D
    OPoint3D _B;	//!< Second OPoint3D
    OPoint3D _C;	//!< Third OPoint3D

    //!< Get the \c OPoint3D from the specific index.
    OPoint3D& vertex(unsigned i);
    //!< Get the \c OPoint3D from the specific index.
    const OPoint3D& vertex(unsigned i) const;

    //!< Get coordinate (int) from index.
    int& index(unsigned i);
    /// Get coordinate (int) from index.
    int  index(unsigned i) const;

    /// Check the consistency between indexes of points and the points of this OTriangle
    bool checkConsistencyWrtPointsTab(const std::deque<OPoint3D>& points) const;
    /// If inconsistency found between indexes and OPoint3D points, prints it
    std::string reportInconsistencyWrtPointsTab(const std::deque<OPoint3D>& points);

    bool operator==(const OTriangle& other) const;

private:
    //!< A static table of \c OPoint3D.
    static OPoint3D OTriangle::* vertices_m_ptr[3];
    //!< A static table of coordinate.
    static int OTriangle::* indices_m_ptr[3];
};

::std::ostream& operator<<(::std::ostream& os, const OTriangle& );

#endif //TY_TRIANGLE
