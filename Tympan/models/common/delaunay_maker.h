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

#ifndef TY_MC_DELAUNAY_MAKER
#define TY_MC_DELAUNAY_MAKER

#include <qlist.h>

#include "Tympan/models/common/3d.h"
#include "Tympan/models/common/triangle.h"

/**
 * \file delaunay_maker.h
 * \class ODelaunayMaker
 * \brief Delaunay triangulation
 */

class ODelaunayMaker
{
public :
    /// Constructor
    ODelaunayMaker(double triangulatePrecision);
    /// Destructor
    virtual ~ODelaunayMaker();

public :
    /**
     * \fn void reinitParameters(void)
     * \brief Re-initialization all parameters to zero
     */
    void reinitParameters(void);

    /**
     * \fn void setDecimation(double xdecim, double ydecim, double zdecim);
     *
     */
    void setDecimation(double xdecim, double ydecim, double zdecim);

    /**
     * \fn void addVertex(OPoint3D vertex)
     * \brief Add a vertex
     */
    void addVertex(OPoint3D vertex);

    /**
     * \fn bool compute(void);
     * \brief Compute the triangulation
     */
    bool compute(void);

    /**
     * \fn void getBoundaries(double & xmin, double & ymin, double & zmin, double & xmax, double & ymax, double & zmax)
     * \brief Define boundaries
     */
    void getBoundaries(double& xmin, double& ymin, double& zmin, double& xmax, double& ymax, double& zmax);

    /**
     * \fn QList<OTriangle> getFaces(void)
     * \brief Return faces list
     */
    QList<OTriangle> getFaces(void);

    /**
     * \fn QList<OPoint3D> getVertex(void)
     * \brief Return the vertexes list
     */
    QList<OPoint3D> getVertex(void);

private :
    OPoint3D computeCircle(const OPoint3D& p1, const OPoint3D& p2, const OPoint3D& p3);
    void decimate(void);
    void triangulate(void);
    void invertCoordinates(void);

private :
    /// Triangles list
    QList<OTriangle>    _triangleOut;
    /// Vertexes list
    QList<OPoint3D>      _vertexInOut;

    double _dx, _dy, _dz, _xMax, _xMin, _yMax, _yMin, _zMin, _zMax;
    double _xEcartType, _yEcartType, _zEcartType;
    double _xdecim, _ydecim, _zdecim;
    double _triangulatePrecision;
};

#endif //TY_MC_DELAUNAY_MAKER
