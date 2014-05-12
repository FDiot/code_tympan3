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
 */

#include "ODelaunayMaker.h"
#include <math.h>

ODelaunayMaker::ODelaunayMaker(double triangulatePrecision)
{
    _xEcartType = 0; _yEcartType = 0; _zEcartType = 0;
    _xdecim = triangulatePrecision;
    _ydecim = triangulatePrecision;
    _zdecim = 0.0;
    _xMin = 0; _yMin = 0; _zMin = 0;
    _xMax = 0; _yMax = 0; _zMax = 0;
    _dx = 0; _dy = 0; _dz = 0;
    _triangulatePrecision = triangulatePrecision;
}

ODelaunayMaker::~ODelaunayMaker()
{
}

void ODelaunayMaker::reinitParameters(void)
{
    _triangleOut.clear();
    _vertexInOut.clear();

    _xEcartType = 0; _yEcartType = 0; _zEcartType = 0;
    _xdecim = 0.01; _ydecim = 0.01; _zdecim = 0.0;

    _xMin = 0; _yMin = 0; _zMin = 0;
    _xMax = 0; _yMax = 0; _zMax = 0;

    _dx = 0; _dy = 0; _dz = 0;
}

void ODelaunayMaker::setDecimation(double xdecim, double ydecim, double zdecim)
{
    _xdecim = xdecim;
    _ydecim = ydecim;
    _zdecim = zdecim;
}

bool ODelaunayMaker::compute(void)
{
    _triangleOut.clear();

    decimate();
    triangulate();
    return true;
}

void ODelaunayMaker::getBoundaries(double& xmin, double& ymin, double& zmin, double& xmax, double& ymax, double& zmax)
{
    xmin = _xMin ; ymin = _yMin; zmin = _zMin;
    xmax = _xMax ; ymax = _yMax; zmax = _zMax;
}

QList<OTriangle> ODelaunayMaker::getFaces(void)
{
    return _triangleOut;
}

QList<OPoint3D> ODelaunayMaker::getVertex(void)
{
    return _vertexInOut;
}

OPoint3D ODelaunayMaker::computeCircle(const OPoint3D& p1, const OPoint3D& p2, const OPoint3D& p3)
{
    double cx, cy;

    if (fabs(p2._y - p1._y) < _triangulatePrecision)
    {
        double mx2 = 0.5 * (p2._x + p3._x);
        double my2 = 0.5 * (p2._y + p3._y);
        cx = 0.5 * (p2._x + p1._x);
        cy = my2 - (p3._x - p2._x) * (cx - mx2) / (p3._y - p2._y);
    }
    else if (fabs(p3._y - p2._y) < _triangulatePrecision)
    {
        double mx1 = 0.5 * (p1._x + p2._x);
        double my1 = 0.5 * (p1._y + p2._y);
        cx = 0.5 * (p3._x + p2._x);
        cy = my1 - (p2._x - p1._x) * (cx - mx1) / (p2._y - p1._y);
    }
    else
    {
        double m1 = -(p2._x - p1._x) / (p2._y - p1._y);
        double m2 = -(p3._x - p2._x) / (p3._y - p2._y);
        double mx1 = 0.5 * (p1._x + p2._x);
        double mx2 = 0.5 * (p2._x + p3._x);
        double my1 = 0.5 * (p1._y + p2._y);
        double my2 = 0.5 * (p2._y + p3._y);
        cx = (m1 * mx1 - m2 * mx2 + my2 - my1) / (m1 - m2);
        cy = m1 * (cx - mx1) + my1;
    }

    double dx = p2._x - cx;
    double dy = p2._y - cy;

    return(OPoint3D(cx, cy, sqrt(dx * dx + dy * dy)));
}

void ODelaunayMaker::addVertex(OPoint3D vertex)
{
    _vertexInOut.append(vertex);
}

void ODelaunayMaker::decimate(void)
{
    unsigned int i, j;

    _triangleOut.clear();

    unsigned int nb = _vertexInOut.count();

    if (!nb) { return; }

    double xmin, xmax, ymin, ymax, zmin, zmax; // compute bounding box
    xmin = xmax = _vertexInOut.back()._x;
    ymin = ymax = _vertexInOut.back()._y;
    zmin = zmax = _vertexInOut.back()._z;

    // VERTEX DECIMATION
    for (i = 0; i < nb - 1; i++) // remove multiple vertices and compute bounding box
    {
        OPoint3D tmp(0, 0, 0);
        unsigned int nbSame = 0;

        _xEcartType ++; _yEcartType ++; _zEcartType ++;

        for (j = i + 1; j < nb; j++)
        {
            _xEcartType +=  fabs(_vertexInOut[j]._x - _vertexInOut[i]._x);
            _yEcartType +=  fabs(_vertexInOut[j]._y - _vertexInOut[i]._y);
            _zEcartType +=  fabs(_vertexInOut[j]._z - _vertexInOut[i]._z);
            if (((fabs(_vertexInOut[j]._x - _vertexInOut[i]._x) < _xdecim) && (fabs(_vertexInOut[j]._y - _vertexInOut[i]._y) < _ydecim)) || (fabs(_vertexInOut[j]._z - _vertexInOut[i]._z) < _zdecim))
            {
                if (!nbSame)
                {
                    tmp = _vertexInOut[i];
                    nbSame++;
                }
                tmp._x += _vertexInOut[j]._x;
                tmp._y += _vertexInOut[j]._y;
                tmp._z += _vertexInOut[j]._z;
                nbSame++;
                _vertexInOut[j] = _vertexInOut.back();
                _vertexInOut.pop_back();
                j--;
                nb--;
            }
        }

        _xEcartType = _xEcartType / (nb - i + 1);
        _yEcartType = _yEcartType / (nb - i + 1);
        _zEcartType = _zEcartType / (nb - i + 1);

        if (nbSame)
        {
            _vertexInOut[i]._x = tmp._x / nbSame;
            _vertexInOut[i]._y = tmp._y / nbSame;
            _vertexInOut[i]._z = tmp._z / nbSame;
        }
        if (_vertexInOut[i]._x < xmin) { xmin = _vertexInOut[i]._x; }
        else if (_vertexInOut[i]._x > xmax) { xmax = _vertexInOut[i]._x; }
        if (_vertexInOut[i]._y < ymin) { ymin = _vertexInOut[i]._y; }
        else if (_vertexInOut[i]._y > ymax) { ymax = _vertexInOut[i]._y; }
        if (_vertexInOut[i]._z < zmin) { zmin = _vertexInOut[i]._z; }
        else if (_vertexInOut[i]._z > zmax) { zmax = _vertexInOut[i]._z; }
    }
    _xEcartType = _xEcartType / nb;
    _yEcartType = _yEcartType / nb;
    _zEcartType = _zEcartType / nb;


    if (_vertexInOut.back()._x < xmin) { xmin = _vertexInOut.back()._x; }
    else if (_vertexInOut.back()._x > xmax) { xmax = _vertexInOut.back()._x; }
    if (_vertexInOut.back()._y < ymin) { ymin = _vertexInOut.back()._y; }
    else if (_vertexInOut.back()._y > ymax) { ymax = _vertexInOut.back()._y; }
    if (_vertexInOut.back()._z < zmin) { zmin = _vertexInOut.back()._z; }
    else if (_vertexInOut.back()._z > zmax) { zmax = _vertexInOut.back()._z; }
    double dx = xmax - xmin;
    double dy = ymax - ymin;
    double dz = zmax - zmin;

    // Save bounding box
    _dx = dx; _dy = dy; _dz = dz;
    _xMin = xmin; _xMax = xmax;
    _yMin = ymin; _yMax = ymax;
    _zMin = zmin; _zMax = zmax;
}

void ODelaunayMaker::triangulate(void)
{
    double dx, dy, xmin, xmax, ymin, ymax;

    int i, j, k;
    int nb = _vertexInOut.count();
    if (!nb) { return; }

    dx = _dx; dy = _dy;
    xmin = _xMin; xmax = _xMax;
    ymin = _yMin; ymax = _yMax;

    bool xMain = (dx >= dy);
    double dmax = (xMain ? dx : dy);
    if (dmax < _triangulatePrecision) { return; }
    double xmid = 0.5 * (xmax + xmin);
    double ymid = 0.5 * (ymax + ymin);
    if (xMain)
    {
        for (i = 0; i < nb - 1; i++) // sort along x coordinate
        {
            k = i;
            for (j = i + 1; j < nb; j++)
            {
                if (_vertexInOut[j]._x < _vertexInOut[k]._x)
                {
                    k = j;
                }
            }
            if (k != i)
            {
                OPoint3D tmp(_vertexInOut[k]);
                _vertexInOut[k] = _vertexInOut[i];
                _vertexInOut[i] = tmp;
            }
        }
    }

    else
    {
        for (i = 0; i < nb - 1; i++) // sort along y coordinate
        {
            k = i;
            for (j = i + 1; j < nb; j++)
            {
                if (_vertexInOut[j]._y < _vertexInOut[k]._y)
                {
                    k = j;
                }
            }
            if (k != i)
            {
                OPoint3D tmp(_vertexInOut[k]);
                _vertexInOut[k] = _vertexInOut[i];
                _vertexInOut[i] = tmp;
            }
        }
    }
    // add bounding //   triangle vertices
    _vertexInOut.push_back(OPoint3D(xmid - 2.0 * dmax, ymid - dmax, 0.0));
    _vertexInOut.push_back(OPoint3D(xmid + 2.0 * dmax, ymid - dmax, 0.0));
    _vertexInOut.push_back(OPoint3D(xmid, ymid + 2.0 * dmax, 0.0));

    QList<OPoint3D> circle;
    QList<bool> complete;
    QList<OTriangle> edge;

    _triangleOut.push_back(OTriangle(nb, nb + 1, nb + 2)); // add bounding triangle
    complete.push_back(false);
    circle.push_back(computeCircle(_vertexInOut[nb], _vertexInOut[nb + 1], _vertexInOut[nb + 2]));
    for (i = 0; i < nb; i++) // include each vertex
    {
        for (j = 0; j < _triangleOut.size(); j++)
        {
            if (!complete[j])
            {
                if (xMain)
                {
                    if (circle[j]._x + circle[j]._z < _vertexInOut[i]._x) { complete[j] = true; }
                }
                else
                {
                    if (circle[j]._y + circle[j]._z < _vertexInOut[i]._y) { complete[j] = true; }
                }
                double dx = _vertexInOut[i]._x - circle[j]._x;
                double dy = _vertexInOut[i]._y - circle[j]._y;

                if (dx* dx + dy* dy <= circle[j]._z * circle[j]._z)
                {
                    // add edges
                    edge.push_back(OTriangle(_triangleOut[j]._p1, _triangleOut[j]._p2, 1));
                    edge.push_back(OTriangle(_triangleOut[j]._p2, _triangleOut[j]._p3, 1));
                    edge.push_back(OTriangle(_triangleOut[j]._p3, _triangleOut[j]._p1, 1));
                    _triangleOut[j] = _triangleOut.back(); // remove triangle
                    _triangleOut.pop_back();
                    complete[j] = complete.back();
                    complete.pop_back();
                    circle[j] = circle.back();
                    circle.pop_back();
                    j--;
                }
            }
        }
        for (j = 0; j < edge.size() - 1; j++) // remove multiple edge
        {
            for (k = j + 1; k < edge.size(); k++)
            {
                if ((edge[j]._p1 == edge[k]._p2) && (edge[j]._p2 == edge[k]._p1))
                {
                    edge[j]._p3 = edge[k]._p3 = 0;
                }
            }
        }
        while (edge.size()) // build new triangles with edges
        {
            if (edge.back()._p3)
            {
                _triangleOut.push_back(OTriangle(edge.back()._p1, edge.back()._p2, i));
                complete.push_back(false);
                circle.push_back(computeCircle(_vertexInOut[_triangleOut.back()._p1], _vertexInOut[_triangleOut.back()._p2], _vertexInOut[_triangleOut.back()._p3]));
            }
            edge.pop_back();
        }
    }
    for (i = 0; i < _triangleOut.size(); i++) // remove bounding triangle
    {
        if ((_triangleOut[i]._p1 >= nb) || (_triangleOut[i]._p2 >= nb) || (_triangleOut[i]._p3 >= nb))
        {
            _triangleOut[i] = _triangleOut.back();
            _triangleOut.pop_back();
            i--;
        }
    }
    _vertexInOut.pop_back(); // remove bounding triangle vertices
    _vertexInOut.pop_back();
    _vertexInOut.pop_back();
}
