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


#include<math.h>
#include<stdio.h>
#include "Tympan/core/defines.h"
#include "3d.h"

#define MAX(A,B) (((A)>(B))?(A):(B))
#define MIN(A,B) (((A)>(B))?(B):(A))
#define minP(A,B,C) ( (((A).C) > ((B).C)) ? (B) : (A) )
#define coeff_ang(PA,PB) ( ((PB)._y - (PA)._y) / ((PB)._x - (PA)._x) )
#define DOT(u,v)   ((u)._x * (v)._x + (u)._y * (v)._y + (u)._z * (v)._z)


/* OCoord3D *******************************************************************/

OCoord3D::OCoord3D():
    _x(0),
    _y(0),
    _z(0)
{
}

OCoord3D::OCoord3D(double x, double y, double z):
    _x(x),
    _y(y),
    _z(z)
{
}

OCoord3D::OCoord3D(const OCoord3D& coord)
{
    *this = coord;
}

OCoord3D::~OCoord3D()
{
}

OCoord3D& OCoord3D::operator=(const OCoord3D& coord)
{
    if (this != &coord)
    {
        _x = coord._x;
        _y = coord._y;
        _z = coord._z;
    }
    return *this;
}

bool OCoord3D::operator==(const OCoord3D& coord) const
{
    if (this != &coord)
    {
        if (fabs(_x - coord._x) > EPSILON_6) { return false; }
        if (fabs(_y - coord._y) > EPSILON_6) { return false; }
        if (fabs(_z - coord._z) > EPSILON_6) { return false; }
    }
    return true;
}

bool OCoord3D::operator!=(const OCoord3D& coord) const
{
    return !operator==(coord);
}

void OCoord3D::setCoords(double x, double y, double z)
{
    _x = x;
    _y = y;
    _z = z;
}

void OCoord3D::setCoords(double coords[3])
{
    _x = coords[0];
    _y = coords[1];
    _z = coords[2];
}

void OCoord3D::getCoords(double coords[3])
{
    coords[0] = _x;
    coords[1] = _y;
    coords[2] = _z;
}

double* OCoord3D::getCoords()
{
    double* res = new double[3];
    res[0] = _x;
    res[1] = _y;
    res[2] = _z;
    return res;
}


/* OVector3D ******************************************************************/

OVector3D::OVector3D():
    OCoord3D()
{
}

OVector3D::OVector3D(const OVector3D& vector):
    OCoord3D(vector)
{
}

OVector3D::OVector3D(const OCoord3D& coord):
    OCoord3D(coord)
{
}

OVector3D::OVector3D(const OCoord3D& coordFrom, const OCoord3D& coordTo)
{
    OVector3D vecFrom(coordFrom);
    OVector3D vecTo(coordTo);
    *this = vecTo - vecFrom;
}

OVector3D::OVector3D(double x, double y, double z):
    OCoord3D(x, y, z)
{
}

OVector3D::~OVector3D()
{
}

void OVector3D::reset()
{
    _x = 0.0;
    _y = 0.0;
    _z = 0.0;
}

OVector3D& OVector3D::operator=(const OVector3D& vector)
{
    OCoord3D::operator=(vector);
    return *this;
}

bool OVector3D::operator==(const OVector3D& vector) const
{
    return OCoord3D::operator==(vector);
}

bool OVector3D::operator!=(const OVector3D& vector) const
{
    return !operator==(vector);
}

OVector3D OVector3D::operator+(const OVector3D& vector) const
{
    OVector3D vectorRet = *this;
    vectorRet._x = this->_x + vector._x;
    vectorRet._y = this->_y + vector._y;
    vectorRet._z = this->_z + vector._z;
    return vectorRet;
}

OVector3D OVector3D::operator-(const OVector3D& vector) const
{
    OVector3D vectorRet;
    vectorRet._x = this->_x - vector._x;
    vectorRet._y = this->_y - vector._y;
    vectorRet._z = this->_z - vector._z;
    return vectorRet;
}

OVector3D OVector3D::operator*(const OVector3D& vector) const
{
    OVector3D vectorRet;
    // XXX This is meaningless
    vectorRet._x = this->_x * vector._x;
    vectorRet._y = this->_y * vector._y;
    vectorRet._z = this->_z * vector._z;
    return vectorRet;
}

OVector3D OVector3D::operator*(const double a) const
{
    // Appel l'operateur (double) * (OVector3D)
    return (a * (*this));
}

OVector3D operator*(const double a, const OVector3D& vector)
{
    OVector3D vectorRet;
    vectorRet._x = vector._x * a;
    vectorRet._y = vector._y * a;
    vectorRet._z = vector._z * a;
    return vectorRet;
}

OVector3D OVector3D::cross(const OVector3D& vector) const
{
    OVector3D vectorRet;
    vectorRet._x = (this->_y * vector._z) - (this->_z * vector._y);
    vectorRet._y = (this->_z * vector._x) - (this->_x * vector._z);
    vectorRet._z = (this->_x * vector._y) - (this->_y * vector._x);
    return vectorRet;
}

void OVector3D::balance(double c1, const OVector3D& vector2, double c2)
{
    *this = *this * c1 + vector2 * c2;
}

double OVector3D::scalar(const OVector3D& vector) const
{
    return (this->_x * vector._x + this->_y * vector._y + this->_z * vector._z);
}

double OVector3D::norme() const
{
    return sqrt(_x * _x + _y * _y + _z * _z);
}

OVector3D OVector3D::normal(const OVector3D& vector2, const OVector3D& vector3) const
{
    return (vector2 - *this).cross(vector3 - *this);
}

void OVector3D::normalize()
{
    double n = norme();
    if (n > 0.0)
    {
        _x /= n;
        _y /= n;
        _z /= n;
    }
}

void OVector3D::invert()
{
    _x = - _x;
    _y = - _y;
    _z = - _z;
}

double OVector3D::angle(const OVector3D& vector) const
{
    double norme = (this->norme() * vector.norme());
    if (norme == 0.0) { return norme; }
    double cosVal = this->scalar(vector) / norme;
    return (acos(BORNE(cosVal, -1.0, 1.0)));
}

OVector3D OVector3D::getRotationOz(double alpha)
{
    OVector3D& V = *this;
    double cosA = cos(alpha);
    double sinA = sin(alpha);
    OVector3D Vfinal;
    // Computes the calculations
    Vfinal._x = cosA * V._x - sinA * V._y;
    Vfinal._y = sinA * V._x + cosA * V._y;
    Vfinal._z = V._z;
    return Vfinal;
}

OVector3D OVector3D::getRotationOy(double alpha)
{
    OVector3D& V = *this;
    double cosA = cos(alpha);
    double sinA = sin(alpha);
    OVector3D Vfinal;
    // Computes the calculations
    Vfinal._x = cosA * V._x - sinA * V._z;
    Vfinal._y = V._y;
    Vfinal._z = sinA * V._x + cosA * V._z;
    return Vfinal;
}

OVector3D OVector3D::getRotationOzBase2(double alpha)
{
    OVector3D& V = *this;
    double cosA = cos(alpha);
    double sinA = sin(alpha);
    OVector3D Vfinal;
    // Computes the calculations
    Vfinal._x = cosA * V._x + sinA * V._y;
    Vfinal._y = -sinA * V._x + cosA * V._y;
    Vfinal._z = V._z;
    return Vfinal;
}

OVector3D OVector3D::getRotationOyBase2(double alpha)
{
    OVector3D& V = *this;
    double cosA = cos(alpha);
    double sinA = sin(alpha);
    OVector3D Vfinal;
    // Computes the calculations
    Vfinal._x = cosA * V._x + sinA * V._z;
    Vfinal._y = V._y;
    Vfinal._z = - sinA * V._x + cosA * V._z;
    return Vfinal;
}

OVector3D OVector3D::getRotationOzOy(double alpha, double theta)
{
    OVector3D& V = *this;
    double cosA = cos(alpha);
    double sinA = sin(alpha);
    double cosB = cos(theta);
    double sinB = sin(theta);
    OVector3D Vfinal;
    // Computes the calculations
    Vfinal._x = cosA * cosB * V._x - sinA * V._y - cosA * sinB * V._z;
    Vfinal._y = sinA * cosB * V._x + cosA * V._y - sinA * sinB * V._z;
    Vfinal._z = sinB * V._x + cosB * V._z;
    return Vfinal;
}


/* OPoint3D *******************************************************************/

OPoint3D::OPoint3D():
    OCoord3D()
{
}

OPoint3D::OPoint3D(const OPoint3D& pt):
    OCoord3D(pt)
{
}

OPoint3D::OPoint3D(const OCoord3D& coord):
    OCoord3D(coord)
{
}

OPoint3D::OPoint3D(double x, double y, double z):
    OCoord3D(x, y, z)
{
}

OPoint3D::OPoint3D(double v[]):
    OCoord3D(v[0], v[1], v[2])
{
}

OPoint3D::~OPoint3D()
{
}

double OPoint3D::distFrom(const OPoint3D& pt) const
{
    return OVector3D(*this, pt).norme();
}

double OPoint3D::dist2DFrom(const OPoint3D& pt) const
{
    return sqrt((_x - pt._x) * (_x - pt._x) + (_y - pt._y) * (_y - pt._y));
}

void OPoint3D::set(double x, double y, double z)
{
    OCoord3D::setCoords(x, y, z);
}

TabPoint3D OPoint3D::checkPointsMaxDistance(const TabPoint3D& points, const double& distanceMax)
{
    TabPoint3D retTab;
    if (!points.size() || (distanceMax <= 0.1))
    {
        return points;
    }
    bool pointAdded = false;
    // On verifie que les points ne sont pas trop eloignes les uns des autres
    OSegment3D seg;
    seg._ptA = points[0];
    retTab.push_back(seg._ptA);
    double dist(0.0);
    for (unsigned int i = 1; i < points.size(); i++)
    {
        seg._ptB = points[i];
        dist = seg.longueur();
        if (dist < TYSEUILCONFONDUS) { continue; } // elimination du point en doublon
        // Si les points sont trop eloignes...
        if (dist > distanceMax)
        {
            // On ajoute un point entre les deux
            OVector3D vec(seg._ptA, seg._ptB);
            vec.normalize();
            OPoint3D newPt = OVector3D(seg._ptA) + (vec * (dist / 2));
            retTab.push_back(newPt);
            pointAdded = true;
        }
        retTab.push_back(seg._ptB);
        seg._ptA = seg._ptB;
    }
    if (pointAdded) { retTab = checkPointsMaxDistance(retTab, distanceMax); }
    return retTab;
}

TabPoint3D OPoint3D::checkPointsMaxDistance(const OPoint3D& point1,
        const OPoint3D& point2, const double& distanceMax)
{
    TabPoint3D tabPoints;
    tabPoints.push_back(point1);
    tabPoints.push_back(point2);
    return checkPointsMaxDistance(tabPoints, distanceMax);
}


/* Matrix *********************************************************************/

OMatrix::OMatrix()
{
    unite();
}

OMatrix::OMatrix(const OMatrix& matrix)
{
    *this = matrix;
}

OMatrix::OMatrix(double matrix[4][4])
{
    int i, j;
    for (i = 0; i < 4; i++)
    {
        for (j = 0; j < 4; j++)
        {
            _m[i][j] = matrix[i][j];
        }
    }
}

OMatrix::~OMatrix()
{
}

OMatrix& OMatrix::operator=(const OMatrix& matrix)
{
    int i, j;
    if (this != &matrix)
    {
        for (i = 0; i < 4; i++)
        {
            for (j = 0; j < 4; j++)
            {
                _m[i][j] = matrix._m[i][j];
            }
        }
    }
    return *this;
}

bool OMatrix::operator==(const OMatrix& matrix) const
{
    int i, j;
    if (this != &matrix)
    {
        for (i = 0; i < 4; i++)
        {
            for (j = 0; j < 4; j++)
            {
                if (_m[i][j] != matrix._m[i][j]) { return false; }
            }
        }
    }
    return true;
}

bool OMatrix::operator!=(const OMatrix& matrix) const
{
    return !operator==(matrix);
}

OMatrix OMatrix::operator+(const OMatrix& matrix) const
{
    int i, j;
    OMatrix matrixRes;
    for (i = 0; i < 3; i++)
    {
        for (j = 0; j < 3; j++)
        {
            matrixRes._m[i][j] = _m[i][j] + matrix._m[i][j];
        }
    }
    return matrixRes;
}

OMatrix OMatrix::operator-(const OMatrix& matrix) const
{
    int i, j;
    OMatrix matrixRes;
    for (i = 0; i < 3; i++)
    {
        for (j = 0; j < 3; j++)
        {
            matrixRes._m[i][j] = _m[i][j] - matrix._m[i][j];
        }
    }
    return matrixRes;
}

OMatrix OMatrix::operator*(const OMatrix& matrix) const
{
    int i, j, k;
    double  t;
    OMatrix matrixRes;
    for (i = 0; i < 4; i++)
    {
        for (j = 0; j < 4; j++)
        {
            for (k = 0, t = 0; k < 4; k++)
            {
                t += _m[i][k] * matrix._m[k][j];
            }
            matrixRes._m[i][j] = t;
        }
    }
    return matrixRes;
}

OCoord3D OMatrix::dot(const OCoord3D& coord) const
{
    OCoord3D coordRes;
    coordRes._x = _m[0][0] * coord._x + _m[0][1] * coord._y + _m[0][2] * coord._z;
    coordRes._y = _m[1][0] * coord._x + _m[1][1] * coord._y + _m[1][2] * coord._z;
    coordRes._z = _m[2][0] * coord._x + _m[2][1] * coord._y + _m[2][2] * coord._z;
    return coordRes;
}

OCoord3D OMatrix::scale(const OCoord3D& coord) const
{
    OCoord3D coordRes (coord);
    double  scale;
    scale = _m[3][0] * coord._x + _m[3][1] * coord._y + _m[3][2] * coord._z + _m[3][3];
    if ( (scale != 1) && (ABS(scale) >= EPSILON_13))
    {
        coordRes._x /= scale;
        coordRes._y /= scale;
        coordRes._z /= scale;
    }
    return coordRes;
}

OVector3D operator*(const OMatrix& mat, const OVector3D& vec)
{
    OVector3D resVec = mat.dot(vec);
    resVec = mat.scale(resVec);
    return resVec;
}

OPoint3D operator*(const OMatrix& mat, const OPoint3D& pt)
{
    OPoint3D resPt = mat.dot(pt);
    // Translation
    resPt._x = resPt._x + mat._m[0][3];
    resPt._y = resPt._y + mat._m[1][3];
    resPt._z = resPt._z + mat._m[2][3];
    return resPt;
}

OVector3D OMatrix::multNormal(const OVector3D& normal) const
{
    double  echelle;
    OVector3D vecRes;
    // Same as operator* but without the translation (column _m[0->2][3])
    vecRes._x = _m[0][0] * normal._x + _m[0][1] * normal._y + _m[0][2] * normal._z;
    vecRes._y = _m[1][0] * normal._x + _m[1][1] * normal._y + _m[1][2] * normal._z;
    vecRes._z = _m[2][0] * normal._x + _m[2][1] * normal._y + _m[2][2] * normal._z;
    echelle = _m[3][0] * normal._x + _m[3][1] * normal._y + _m[3][2] * normal._z + _m[3][3];
    if (ABS(echelle) < EPSILON_13)
    {
    }
    else
    {
        if (echelle != 1)
        {
            vecRes._x /= echelle;
            vecRes._y /= echelle;
            vecRes._z /= echelle;
        }
    }
    return vecRes;
}

void OMatrix::show()
{
    int i, j;
    for (i = 0; i < 4; i++)
    {
        for (j = 0; j < 4; j++)
        {
            printf("%12.7f ", _m[i][j]);
        }
        printf("\n");
    }
}

void OMatrix::reset()
{
    int i, j;
    for (i = 0; i < 4; i++)
    {
        for (j = 0; j < 4; j++)
        {
            _m[i][j] = 0;
        }
    }
}

void OMatrix::unite()
{
    int i, j;
    for (i = 0; i < 4; i++)
    {
        for (j = 0; j < 4; j++)
        {
            _m[i][j] = (i == j ? 1 : 0);
        }
    }
}

int OMatrix::setTranslation(double x, double y, double z)
{
    unite();
    _m[0][3] = x;
    _m[1][3] = y;
    _m[2][3] = z;
    return 1;
}

int OMatrix::setScale(double x, double y, double z)
{
    unite();
    _m[0][0] = x;
    _m[1][1] = y;
    _m[2][2] = z;
    return 1;
}

int OMatrix::setRotationOx(double a)
{
    double  cosa = cos(a);
    double  sina = sin(a);
    unite();
    _m[1][1] = cosa; _m[1][2] = -sina;
    _m[2][1] = sina; _m[2][2] = cosa;
    return 1;
}

int OMatrix::setRotationOy(double a)
{
    double  cosa = cos(a);
    double  sina = sin(a);
    unite();
    _m[0][0] = cosa;  _m[0][2] = sina;
    _m[2][0] = -sina; _m[2][2] = cosa;
    return 1;
}

int OMatrix::setRotationOz(double a)
{
    double  cosa = cos(a);
    double  sina = sin(a);
    unite();
    _m[0][0] = cosa; _m[0][1] = -sina;
    _m[1][0] = sina; _m[1][1] = cosa;
    return 1;
}

int OMatrix::aligneVecteurSurOx(const OVector3D& vector)
{
    int res = 0;
    double  cos1, sin1, cos2, sin2;
    double  n1, n2;
    n2 = vector.norme();
    if (n2 > EPSILON_13)
    {
        res = 1;
        n1 = sqrt(vector._x * vector._x + vector._y * vector._y);
        if (n1 < EPSILON_13)
        {
            cos1 = 1;
            sin1 = 0;
            cos2 = 0;
            sin2 = (vector._z >= 0 ? 1 : -1);
        }
        else
        {
            cos1 = vector._x / n1;
            sin1 = vector._y / n1;
            cos2 = n1 / n2;
            sin2 = vector._z / n2;
        }
        _m[0][0] = cos2 * cos1;  _m[0][1] = cos2 * sin1;  _m[0][2] = sin2; _m[0][3] = 0;
        _m[1][0] = -sin1;      _m[1][1] = cos1;       _m[1][2] = 0;    _m[1][3] = 0;
        _m[2][0] = -sin2 * cos1; _m[2][1] = -sin2 * sin1; _m[2][2] = cos2; _m[2][3] = 0;
        _m[3][0] = 0;          _m[3][1] = 0;          _m[3][2] = 0;    _m[3][3] = 1;
    }
    return res;
}

int OMatrix::aligneVecteurSurOy(const OVector3D& vector)
{
    int res = 0;
    double  cos1, sin1, cos2, sin2;
    double  n1, n2;
    n2 = vector.norme();
    if (n2 > EPSILON_13)
    {
        res = 1;
        n1 = sqrt(vector._x * vector._x + vector._y * vector._y);
        if (n1 < EPSILON_13)
        {
            cos1 = 1;
            sin1 = 0;
            cos2 = 0;
            sin2 = (vector._z >= 0 ? 1 : -1);
        }
        else
        {
            /* Si le vecteur est dans le plan yOz,  */
            /* on evite la rotation autour de Oz    */
            if (ABS(vector._x) < EPSILON_13)
            {
                n1 = (vector._y < 0.0 ? -n1 : n1);
            }
            cos1 = vector._y / n1;
            sin1 = vector._x / n1;
            cos2 = n1 / n2;
            sin2 = vector._z / n2;
        }
        _m[0][0] = cos1;       _m[0][1] = -sin1;      _m[0][2] = 0;    _m[0][3] = 0;
        _m[1][0] = sin1 * cos2;  _m[1][1] = cos1 * cos2;  _m[1][2] = sin2; _m[1][3] = 0;
        _m[2][0] = -sin1 * sin2; _m[2][1] = -cos1 * sin2; _m[2][2] = cos2; _m[2][3] = 0;
        _m[3][0] = 0;          _m[3][1] = 0;          _m[3][2] = 0;    _m[3][3] = 1;
    }
    return res;
}

int OMatrix::invert()
{
    int i, j;
    double det = determinant();
    adjoint();
    // if the determinant is zero,
    // then the inverse matrix is not unique.
    if (fabs(det) < EPSILON_50)
    {
        return 0;
    }
    // Scale the adjoint matrix to get the inverse.
    for (i = 0; i < 4; i++)
    {
        for (j = 0; j < 4; j++)
        {
            _m[i][j] = _m[i][j] / det;
        }
    }
    return 1;
}

OMatrix OMatrix::getInvert(int* ok /*=0*/) const
{
    OMatrix mat(*this);
    int ret = mat.invert();
    if (ok) { *ok = ret; }
    return mat;
}

void OMatrix::adjoint()
{
    double a1, a2, a3, a4, b1, b2, b3, b4;
    double c1, c2, c3, c4, d1, d2, d3, d4;
    // Assign to individual variable names to aid
    // selecting correct values.
    a1 = _m[0][0]; b1 = _m[0][1];
    c1 = _m[0][2]; d1 = _m[0][3];
    a2 = _m[1][0]; b2 = _m[1][1];
    c2 = _m[1][2]; d2 = _m[1][3];
    a3 = _m[2][0]; b3 = _m[2][1];
    c3 = _m[2][2]; d3 = _m[2][3];
    a4 = _m[3][0]; b4 = _m[3][1];
    c4 = _m[3][2]; d4 = _m[3][3];
    // Row column labeling reversed since we transpose rows & columns.
    _m[0][0]  =   OMatrix::mat3x3Det(b2, b3, b4, c2, c3, c4, d2, d3, d4);
    _m[1][0]  = - OMatrix::mat3x3Det(a2, a3, a4, c2, c3, c4, d2, d3, d4);
    _m[2][0]  =   OMatrix::mat3x3Det(a2, a3, a4, b2, b3, b4, d2, d3, d4);
    _m[3][0]  = - OMatrix::mat3x3Det(a2, a3, a4, b2, b3, b4, c2, c3, c4);
    _m[0][1]  = - OMatrix::mat3x3Det(b1, b3, b4, c1, c3, c4, d1, d3, d4);
    _m[1][1]  =   OMatrix::mat3x3Det(a1, a3, a4, c1, c3, c4, d1, d3, d4);
    _m[2][1]  = - OMatrix::mat3x3Det(a1, a3, a4, b1, b3, b4, d1, d3, d4);
    _m[3][1]  =   OMatrix::mat3x3Det(a1, a3, a4, b1, b3, b4, c1, c3, c4);
    _m[0][2]  =   OMatrix::mat3x3Det(b1, b2, b4, c1, c2, c4, d1, d2, d4);
    _m[1][2]  = - OMatrix::mat3x3Det(a1, a2, a4, c1, c2, c4, d1, d2, d4);
    _m[2][2]  =   OMatrix::mat3x3Det(a1, a2, a4, b1, b2, b4, d1, d2, d4);
    _m[3][2]  = - OMatrix::mat3x3Det(a1, a2, a4, b1, b2, b4, c1, c2, c4);
    _m[0][3]  = - OMatrix::mat3x3Det(b1, b2, b3, c1, c2, c3, d1, d2, d3);
    _m[1][3]  =   OMatrix::mat3x3Det(a1, a2, a3, c1, c2, c3, d1, d2, d3);
    _m[2][3]  = - OMatrix::mat3x3Det(a1, a2, a3, b1, b2, b3, d1, d2, d3);
    _m[3][3]  =   OMatrix::mat3x3Det(a1, a2, a3, b1, b2, b3, c1, c2, c3);
}

OMatrix OMatrix::getAdjoint()
{
    OMatrix mat(*this);
    // Adjoint
    mat.adjoint();
    return mat;
}

double OMatrix::determinant()
{
    // Assign to individual variable names to aid selecting
    // correct elements.
    double a1, a2, a3, a4, b1, b2, b3, b4, c1, c2, c3, c4, d1, d2, d3, d4;
    a1 = _m[0][0]; b1 = _m[0][1];
    c1 = _m[0][2]; d1 = _m[0][3];
    a2 = _m[1][0]; b2 = _m[1][1];
    c2 = _m[1][2]; d2 = _m[1][3];
    a3 = _m[2][0]; b3 = _m[2][1];
    c3 = _m[2][2]; d3 = _m[2][3];
    a4 = _m[3][0]; b4 = _m[3][1];
    c4 = _m[3][2]; d4 = _m[3][3];
    return a1 * OMatrix::mat3x3Det(b2, b3, b4, c2, c3, c4, d2, d3, d4)
           - b1 * OMatrix::mat3x3Det(a2, a3, a4, c2, c3, c4, d2, d3, d4)
           + c1 * OMatrix::mat3x3Det(a2, a3, a4, b2, b3, b4, d2, d3, d4)
           - d1 * OMatrix::mat3x3Det(a2, a3, a4, b2, b3, b4, c2, c3, c4);
}

double OMatrix::mat2x2Det(double a, double b, double c, double d)
{
    return a * d - b * c;
}

double OMatrix::mat3x3Det(double a1, double a2, double a3, double b1, double b2, double b3, double c1, double c2, double c3)
{
    return a1 * OMatrix::mat2x2Det(b2, b3, c2, c3)
          - b1 * OMatrix::mat2x2Det(a2, a3, c2, c3)
          + c1 * OMatrix::mat2x2Det(a2, a3, b2, b3);
}


/* OBox ***********************************************************************/


OBox::OBox()
{
    _min._x = _min._y = _min._z = _max._x = _max._y = _max._z = 0;
}

OBox::OBox(const OBox& box)
{
    _min = box._min;
    _max = box._max;
}

OBox::OBox(const OCoord3D& min, const OCoord3D& max)
{
    _min = min;
    _max = max;
}

OBox::OBox(double x1, double y1, double z1, double x2, double y2, double z2)
{
    _min = OPoint3D(x1, y1, z1);
    _max = OPoint3D(x2, y2, z2);
}

OBox& OBox::operator=(const OBox& box)
{
    if (this != &box)
    {
        _min = box._min;
        _max = box._max;
    }
    return *this;
}

bool OBox::operator==(const OBox& box) const
{
    if (this != &box)
    {
        if (_min != box._min) { return false; }
        if (_max != box._max) { return false; }
    }
    return true;
}

bool OBox::isInside(const OPoint3D& pt) const
{
    if (pt._x < _min._x) { return false; }
    if (pt._x > _max._x) { return false; }
    if (pt._y < _min._y) { return false; }
    if (pt._y > _max._y) { return false; }
    if (pt._z < _min._z) { return false; }
    if (pt._z > _max._z) { return false; }
    return true;
}

bool OBox::isInside2D(const OPoint3D& pt) const
{
    if (pt._x < _min._x) { return false; }
    if (pt._x > _max._x) { return false; }
    if (pt._y < _min._y) { return false; }
    if (pt._y > _max._y) { return false; }
    return true;
}

bool OBox::isInContact(const OBox& box) const
{
    if (_max._x < box._min._x) { return false; }
    if (_min._x > box._max._x) { return false; }
    if (_max._y < box._min._y) { return false; }
    if (_min._y > box._max._y) { return false; }
    if (_max._z < box._min._z) { return false; }
    if (_min._z > box._max._z) { return false; }
    return true;
}

void OBox::Enlarge(const OPoint3D& pt)
{
    if (_min._x == 0 && _min._y == 0 &&  _min._z == 0 &&
        _max._x == 0 && _max._y == 0 &&  _max._z == 0)
    {
        _min._x = pt._x;
        _max._x = pt._x;
        _min._y = pt._y;
        _max._y = pt._y;
        _min._z = pt._z;
        _max._z = pt._z;
    }
    else
    {
        if (pt._x < _min._x) { _min._x = pt._x; }
        if (pt._x > _max._x) { _max._x = pt._x; }
        if (pt._y < _min._y) { _min._y = pt._y; }
        if (pt._y > _max._y) { _max._y = pt._y; }
        if (pt._z < _min._z) { _min._z = pt._z; }
        if (pt._z > _max._z) { _max._z = pt._z; }
    }
}

void OBox::Enlarge(float x, float y, float z)
{
    OPoint3D pt(x, y, z);
    Enlarge(pt);
}

void OBox::Enlarge(const OBox& box)
{
    if (box._min._x < _min._x) { _min._x = box._min._x; }
    if (box._min._y < _min._y) { _min._y = box._min._y; }
    if (box._min._z < _min._z) { _min._z = box._min._z; }
    if (box._max._x > _max._x) { _max._x = box._max._x; }
    if (box._max._y > _max._y) { _max._y = box._max._y; }
    if (box._max._z > _max._z) { _max._z = box._max._z; }
}

void OBox::Translate(const OPoint3D& vectorTranslate)
{
    _min._x += vectorTranslate._x;
    _min._y += vectorTranslate._y;
    _min._z += vectorTranslate._z;
    _max._x += vectorTranslate._x;
    _max._y += vectorTranslate._y;
    _max._z += vectorTranslate._z;
}


/* OGeometry ******************************************************************/

/*static*/ bool OGeometrie::intersDemiSegmentAvecSegment(const OPoint3D& ptS, const OPoint3D& ptA, const OPoint3D& ptB)
{
    double eps = 0.0;
    if (ptS._y == MAX(ptA._y, ptB._y) || ptS._y == MIN(ptA._y, ptB._y))
    {
        eps = EPSILON_5;
    }
    if ((ptS._y + eps) > MAX(ptA._y, ptB._y) || (ptS._y + eps) < MIN(ptA._y, ptB._y) || ptS._x > MAX(ptA._x, ptB._x))
    {
        return false;
    }
    if (ptS._x <= MIN(ptA._x, ptB._x))
    {
        return true;
    }
    double ca = (ptA._x != ptB._x) ? coeff_ang(ptA, ptB) : HUGE_VAL;
    OPoint3D my = minP(ptA, ptB, _y);
    double cp = (ptS._x - my._x) ? coeff_ang(my, ptS) : HUGE_VAL;
    return cp >= ca;
}

/*static*/ int OGeometrie::intersDroitesPoints(const OPoint3D& ptA, const OPoint3D& ptB, const OPoint3D& ptC, const OPoint3D& ptD, OPoint3D& ptI)
{
    int res = INTERS_NULLE;
    double k;
    double d = (ptC._x - ptD._x) * (ptA._y - ptB._y) - (ptC._y - ptD._y) * (ptA._x - ptB._x);
    k = 0;
    if (ABS(d) > 0.0)
    {
        k = ((ptA._x - ptD._x) * (ptA._y - ptB._y) - (ptA._y - ptD._y) * (ptA._x - ptB._x)) / d;
        res = INTERS_OUI;
    }
    else
    {
        d = (ptC._x - ptD._x) * (ptA._z - ptB._z) - (ptC._z - ptD._z) * (ptA._x - ptB._x);
        if (ABS(d) > 0.0)
        {
            k = ((ptA._x - ptD._x) * (ptA._z - ptB._z) - (ptA._z - ptD._z) * (ptA._x - ptB._x)) / d;
            res = INTERS_OUI;
        }
        else
        {
            d = (ptC._y - ptD._y) * (ptA._z - ptB._z) - (ptC._z - ptD._z) * (ptA._y - ptB._y);
            if (ABS(d) > 0.0)
            {
                k = ((ptA._y - ptD._y) * (ptA._z - ptB._z) - (ptA._z - ptD._z) * (ptA._y - ptB._y)) / d;
                res = INTERS_OUI;
            }
        }
    }
    if (res)
    {
        ptI._x = ptD._x + k * (ptC._x - ptD._x);
        ptI._y = ptD._y + k * (ptC._y - ptD._y);
        ptI._z = ptD._z + k * (ptC._z - ptD._z);
    }
    return res;
}

/*static*/ int OGeometrie::intersDroitesPointVecteur(const OPoint3D& ptA, const OVector3D& vecA, const OPoint3D& ptB, const OVector3D& vecB, OPoint3D& ptI)
{
    OVector3D vecA1, vecB1;
    vecA1 = OVector3D(ptA) + vecA;
    vecB1 = OVector3D(ptB) + vecB;
    return intersDroitesPoints(ptA, OPoint3D(vecA1), ptB, OPoint3D(vecB1), ptI);
}

/*static*/ double OGeometrie::symPointDroite(const OPoint3D& ptA, const OPoint3D& ptB, const OPoint3D& ptP, OPoint3D& ptI)
{
    // Determination du point M tel que :
    //  Vecteur PM perpendiculaire a Vecteur AB
    //  Vecteur AM colineaire a Vecteur AB
    OVector3D vecAB(ptA, ptB);
    OVector3D vecPtA(ptA);
    OVector3D vecPtP(ptP);
    double k;
    k = (ptP._x - ptA._x) * vecAB._x + (ptP._y - ptA._y) * vecAB._y + (ptP._z - ptA._z) * vecAB._z;
    k /= vecAB._x * vecAB._x + vecAB._y * vecAB._y + vecAB._z * vecAB._z;
    // Determination de P', image de P par rapport a M
    // P' = OA + AM + MP' = OA + AM + PM
    // Avec OA = ptA
    //      AM = k.AB
    //      PM = PA + kAB = (ptA - ptP) + kAB
    // Soit P' = ptA + kAB + ptA - ptP + kAB
    //         = 2(ptA + kAB) - ptP

    // This is evil ! What the hell has it been put here for ?
    //  *(OCoord3D*)&ptI = *(OCoord3D*) & (2 * (vecPtA + (k * vecAB)) - vecPtP);
    // Why be so complicated
    //    (OCoord3D)ptI = (OCoord3D)(2 * (vecPtA + (k * vecAB)) - vecPtP);
    // A very simple solution !
    ptI = 2 * (vecPtA + (k * vecAB)) - vecPtP;
    return k;
}

/*static*/ bool OGeometrie::pointInPolygonAngleSum(const OPoint3D& ptP, const OPoint3D* pts, int nbPts)
{
    int i;
    double m1m2;
    double angle;
    double anglesum = 0;
    double sign = 1;
    OVector3D vec1(ptP, pts[0]);
    OVector3D vec2;
    for (i = 0; i < nbPts; i++)
    {
        vec2 = OVector3D(ptP, pts[(i + 1) % nbPts]);
        m1m2 = vec1.norme() * vec2.norme();
        if (m1m2 <= EPSILON_6)
        {
            // We are on a node, consider this inside
            return true;
        }
        // Angle forme par les 2 vecteurs
        angle = acos((vec1.scalar(vec2)) / (m1m2));
        // Signe de l'angle
        sign = (vec1.cross(vec2)._z > 0) ? -1 : 1;
        anglesum += (angle * sign);
        vec1 = vec2;
    }
    // Le total des angles doit etre egal a 2Pi (a Epsilon pres)
    return ((ABS(anglesum) > (M_2PI - EPSILON_6)) && (ABS(anglesum) < (M_2PI + EPSILON_6)));
}

#if TY_USE_IHM
//in 2D
/*static*/ bool OGeometrie::pointInPolygonRayCasting(const OPoint3D& ptP, const OPoint3D* pts, int nbPts)
{
    int cross = 0, i;
    for (i = 0; i < nbPts - 1; i++)
    {
        if (intersDemiSegmentAvecSegment(ptP, pts[i], pts[i + 1]))
        {
            cross++;
        }
    }
    if (intersDemiSegmentAvecSegment(ptP, pts[i], pts[0]))
    {
        cross++;
    }
    return cross % 2 != 0;
}
#else // TY_USE_IHM
/*static*/ bool OGeometrie::pointInPolygonRayCasting(const OPoint3D& ptP, const OPoint3D* pts, int nbPts, const OVector3D& normal, const OBox& box)
{
    return false;
}
#endif // TY_USE_IHM

/*static*/ bool OGeometrie::shortestSegBetween2Lines(const OPoint3D& pt1, const OPoint3D& pt2, const OPoint3D& pt3, const OPoint3D& pt4,
                                                     OPoint3D& ptA, OPoint3D& ptB, double* mua, double* mub)
{
    OPoint3D p13, p43, p21;
    double d1343, d4321, d1321, d4343, d2121;
    double numer, denom;
    p13._x = pt1._x - pt3._x;
    p13._y = pt1._y - pt3._y;
    p13._z = pt1._z - pt3._z;
    p43._x = pt4._x - pt3._x;
    p43._y = pt4._y - pt3._y;
    p43._z = pt4._z - pt3._z;
    if ((ABS(p43._x) < EPSILON_6) && (ABS(p43._y) < EPSILON_6) && (ABS(p43._z) < EPSILON_6))
    {
        return false;
    }
    p21._x = pt2._x - pt1._x;
    p21._y = pt2._y - pt1._y;
    p21._z = pt2._z - pt1._z;
    if ((ABS(p21._x) < EPSILON_6) && (ABS(p21._y) < EPSILON_6) && (ABS(p21._z) < EPSILON_6))
    {
        return false;
    }
    d1343 = p13._x * p43._x + p13._y * p43._y + p13._z * p43._z;
    d4321 = p43._x * p21._x + p43._y * p21._y + p43._z * p21._z;
    d1321 = p13._x * p21._x + p13._y * p21._y + p13._z * p21._z;
    d4343 = p43._x * p43._x + p43._y * p43._y + p43._z * p43._z;
    d2121 = p21._x * p21._x + p21._y * p21._y + p21._z * p21._z;
    denom = d2121 * d4343 - d4321 * d4321;
    if (ABS(denom) < EPSILON_6)
    {
        return false;
    }
    numer = d1343 * d4321 - d1321 * d4343;
    *mua = numer / denom;
    *mub = (d1343 + d4321 * (*mua)) / d4343;
    ptA._x = pt1._x + *mua * p21._x;
    ptA._y = pt1._y + *mua * p21._y;
    ptA._z = pt1._z + *mua * p21._z;
    ptB._x = pt3._x + *mub * p43._x;
    ptB._y = pt3._y + *mub * p43._y;
    ptB._z = pt3._z + *mub * p43._z;
    return true;
}

/*static*/ void OGeometrie::boundingBox(OPoint3D* pts, int nbPts, OPoint3D& ptMin, OPoint3D& ptMax)
{
    if (nbPts <= 0)
    {
        return;
    }
    ptMax = ptMin = pts[0];
    for (int i = 0; i < nbPts; i++)
    {
        // Minimuns
        if (pts[i]._x <= ptMin._x)
        {
            ptMin._x = pts[i]._x;
        }
        if (pts[i]._y <= ptMin._y)
        {
            ptMin._y = pts[i]._y;
        }
        if (pts[i]._z <= ptMin._z)
        {
            ptMin._z = pts[i]._z;
        }
        // Maximums
        if (pts[i]._x >= ptMax._x)
        {
            ptMax._x = pts[i]._x;
        }
        if (pts[i]._y >= ptMax._y)
        {
            ptMax._y = pts[i]._y;
        }
        if (pts[i]._z >= ptMax._z)
        {
            ptMax._z = pts[i]._z;
        }
    }
}

/* static */ void OGeometrie::computeNormal(OPoint3D* pts, int nbPts, OVector3D& normal)
{
    float ax, ay, az, bx, by, bz;
    //  Because polygon may be concave, need to accumulate cross products to
    //  determine true normal.
    //assert(nbPts>=3);
    OVector3D vec(0, 0, 0);
    OVector3D vecPt0; //set things up for loop
    OVector3D vecPt1(pts[0]);
    OVector3D vecPt2(pts[1]);
    for (int i = 2; i < nbPts; i++)
    {
        vecPt0 = vecPt1;
        vecPt1 = vecPt2;
        vecPt2 = OVector3D(pts[i]);
        // order is important!!! to maintain consistency with polygon vertex order
        ax = vecPt2._x - vecPt1._x; ay = vecPt2._y - vecPt1._y; az = vecPt2._z - vecPt1._z;
        bx = vecPt0._x - vecPt1._x; by = vecPt0._y - vecPt1._y; bz = vecPt0._z - vecPt1._z;
        vec._x += (ay * bz - az * by);
        vec._y += (az * bx - ax * bz);
        vec._z += (ax * by - ay * bx);
    }
    vec.normalize();
    normal = vec;
}


/* OSegment3D *****************************************************************/

OSegment3D::OSegment3D()
{
    _ptA._x = 0; _ptA._y = 0; _ptA._z = 0;
    _ptB._x = 0; _ptB._y = 1; _ptB._z = 0;
}

OSegment3D::OSegment3D(const OSegment3D& other)
{
    *this = other;
}

OSegment3D::OSegment3D(const OPoint3D& ptA, const OPoint3D& ptB)
{
    _ptA = ptA;
    _ptB = ptB;
}

OSegment3D::~OSegment3D()
{
}

OSegment3D& OSegment3D::operator=(const OSegment3D& other)
{
    if (this != &other)
    {
        _ptA = other._ptA;
        _ptB = other._ptB;
    }
    return *this;
}

bool OSegment3D::operator==(const OSegment3D& other) const
{
    if (this != &other)
    {
        if (_ptA != other._ptA) { return false; }
        if (_ptB != other._ptB) { return false; }
    }
    return true;
}

bool OSegment3D::operator!=(const OSegment3D& other) const
{
    return !operator==(other);
}

OSegment3D OSegment3D::operator*(const OMatrix& matrix) const
{
    return OSegment3D(matrix * _ptA, matrix * _ptB);
}

double OSegment3D::longueur() const
{
    return OVector3D(_ptA, _ptB).norme();
}

int OSegment3D::symetrieOf(const OPoint3D& pt, OPoint3D& ptSym) const
{
    int res = 0;
    // Calcul le pt image a ptSym (pt source) par rapport a la droite definie passant
    // par ce segment
    double k = OGeometrie::symPointDroite(_ptA, _ptB, pt, ptSym);
    // Le coefficient k indique si le segment [Point source / Point image] coupe
    // ce segment, pour cela il faut : 0 < k < 1.
    if ((0.0 <= k) && (k <= 1.0))
    {
        res = 1;
    }
    return res;
}

int OSegment3D::projection(const OPoint3D& pt, OPoint3D& ptProj, double seuilConfondus) const
{
    int res = 0;
    OPoint3D ptSym;
    res = symetrieOf(pt, ptSym);
    res = res && intersects(OSegment3D(pt, ptSym), ptProj, seuilConfondus);
    return res;
}

int OSegment3D::intersects(const OSegment3D& seg, OPoint3D& pt, double seuilConfondus) const
{
    int res = INTERS_NULLE;
    OPoint3D ptA, ptB;
    double mua, mub;
    // Calcul le segment le plus court entre les 2 segments dont on recherche l'eventuelle intersection
    if (OGeometrie::shortestSegBetween2Lines(this->_ptA, this->_ptB, seg._ptA, seg._ptB, ptA, ptB, &mua, &mub))
    {
        // Les coefs mua et mub doivent etre compris entre 0 et 1 pour que les points constituant
        // le segment trouve se trouve sur les segments respectifs a tester
        if ((0.0 <= mua) && (mua <= 1.0) && (0.0 <= mub) && (mub <= 1.0))
        {
            // La norme du segment trouve doit etre inferieur a un seuil pour que l'on puisse admettre
            // que les 2 points constituant ce segment sont quasi confondus
            OVector3D vecSeg(ptA, ptB);
            if (vecSeg.norme() <= seuilConfondus)
            {
                res = INTERS_OUI;
                // Arbitrairement on prend le 1er point du segment trouve comme point d'intersection.
                pt = ptA;
            }
        }
    }
    return res;
}

OPoint3D OSegment3D::centreOf() const
{
    double x, y, z;
    x = ((_ptB._x - _ptA._x) / 2.0) + _ptA._x;
    y = ((_ptB._y - _ptA._y) / 2.0) + _ptA._y;
    z = ((_ptB._z - _ptA._z) / 2.0) + _ptA._z;
    return OPoint3D(x, y, z);
}

OPoint3D OSegment3D::centerOfCurvedPath(const double& R) const
{
    OPoint3D point;
    OPoint3D milieu = centreOf() ;
    double demiLongueur = longueur() / 2;
    double d, x, y, z, q;
    x = milieu._x; y = milieu._y; z = milieu._z;
    if (R > demiLongueur)
    {
        d = sqrt(R * R - demiLongueur * demiLongueur);
        if (_ptA._z != _ptB._z)
        {
            d = d * SIGNE(_ptA._z - _ptB._z);
            q = (_ptB._y - _ptA._y) / (_ptB._z - _ptA._z);
            y = milieu._y + d / sqrt(1 + q * q);
        }
        z = milieu._z + sqrt(ABS(d * d - (y - milieu._y) * (y - milieu._y)));
    }
    point.setCoords(x, y, z);
    return point;
}

double OSegment3D::lengthOfCurvedPath(const double& R)
{
    // Longueur du cote du triangle rectangle
    double cote = longueur() / 2.0;
    // Angle d'ouverture du segment
    double angle = 2 * asin(cote / R);
    return angle * R;
}

OSegment3D OSegment3D::swap() const
{
    OSegment3D seg(*this) ;
    OPoint3D PT = seg._ptA;
    seg._ptA = seg._ptB;
    seg._ptB = PT;
    return seg;
}


/* ORepere3D ******************************************************************/

ORepere3D::ORepere3D():
    _origin(0, 0, 0),
    _vecI(1, 0, 0),
    _vecJ(0, 1, 0),
    _vecK(0, 0, 1)
{
}

ORepere3D::ORepere3D(const ORepere3D& repere)
{
    *this = repere;
}

ORepere3D::ORepere3D(const OPoint3D& origin, const OVector3D& vecI,
		     const OVector3D& vecJ, const OVector3D& vecK):
    _origin(origin),
    _vecI(vecI),
    _vecJ(vecJ),
    _vecK(vecK)
{
}

ORepere3D::ORepere3D(const OMatrix& matrix)
{
    set(matrix);
}

ORepere3D::ORepere3D(const OPoint3D& origin, const OVector3D& vec) :
    _origin(origin),
    _vecI(vec)
{
    /*
    1.  On pose x1 = x2 et z1 = z2 (sinon infinité de solution)
                x1.x1 + y1.y2 + z1.z1 = 0 (produit scalaire)
                y1.y2 = - (x1.x2 + z1.z2)
        donc    y2 = - (x1.x2 + z1.z2) / y1

        Si y1 = 0 (plan xOz) -> n'importe quelle valeur de y valide
    */
    double  x1 = _vecI._x,
            y1 = _vecI._y,
            z1 = _vecI._z;
    double y2 = (y1 != 0.) ? -(x1 * x1 + z1 * z1) / y1 : -(x1 * x1 + z1 * z1);
    _vecJ = OVector3D(x1, -y2, z1);   // -y pour orienter correctement le repère
    _vecK = _vecI.cross(_vecJ);
    _vecI.normalize();
    _vecJ.normalize();
    _vecK.normalize();
}


ORepere3D::~ORepere3D()
{
}

ORepere3D& ORepere3D::operator=(const ORepere3D& repere)
{
    if (this != &repere)
    {
        _origin = repere._origin;
        _vecI = repere._vecI;
        _vecJ = repere._vecJ;
        _vecK = repere._vecK;
    }
    return *this;
}

bool ORepere3D::operator==(const ORepere3D& repere) const
{
    if (this != &repere)
    {
        if (_origin != repere._origin) { return false; }
        if (_vecI != repere._vecI) { return false; }
        if (_vecJ != repere._vecJ) { return false; }
        if (_vecK != repere._vecK) { return false; }
    }
    return true;
}

bool ORepere3D::operator!=(const ORepere3D& repere) const
{
    return !operator==(repere);
}

void ORepere3D::set(const OPoint3D& origin, const OVector3D& vecI, const OVector3D& vecJ, const OVector3D& vecK)
{
    _origin = origin;
    _vecI = vecI;
    _vecJ = vecJ;
    _vecK = vecK;
}

void ORepere3D::set(const OMatrix& matrix)
{
    _vecI._x = matrix._m[0][0];
    _vecI._y = matrix._m[1][0];
    _vecI._z = matrix._m[2][0];
    _vecI.normalize();
    _vecJ._x = matrix._m[0][1];
    _vecJ._y = matrix._m[1][1];
    _vecJ._z = matrix._m[2][1];
    _vecJ.normalize();
    _vecK._x = matrix._m[0][2];
    _vecK._y = matrix._m[1][2];
    _vecK._z = matrix._m[2][2];
    _vecK.normalize();
    _origin._x = matrix._m[0][3];
    _origin._y = matrix._m[1][3];
    _origin._z = matrix._m[2][3];
}

void ORepere3D::normalize()
{
    _vecI.normalize();
    _vecJ.normalize();
    _vecK.normalize();
}

OMatrix ORepere3D::asMatrix() const
{
    ORepere3D rep;
    rep.set(_origin, _vecI, _vecJ, _vecK);
    rep.normalize();
    OMatrix matrix;
    matrix.unite();
    matrix._m[0][0] = rep._vecI._x;
    matrix._m[0][1] = rep._vecJ._x;
    matrix._m[0][2] = rep._vecK._x;
    matrix._m[0][3] = rep._origin._x;
    matrix._m[1][0] = rep._vecI._y;
    matrix._m[1][1] = rep._vecJ._y;
    matrix._m[1][2] = rep._vecK._y;
    matrix._m[1][3] = rep._origin._y;
    matrix._m[2][0] = rep._vecI._z;
    matrix._m[2][1] = rep._vecJ._z;
    matrix._m[2][2] = rep._vecK._z;
    matrix._m[2][3] = rep._origin._z;
    return matrix;
}

/* OHPlane3D ******************************************************************/

OHPlane3D::OHPlane3D()
{
}

OHPlane3D::OHPlane3D(const OHPlane3D& Plane)
{
    _N = Plane._N;
    _p = Plane._p;
    _O = Plane._O;
}

OHPlane3D::OHPlane3D(const OVector3D& normal, const OPoint3D& origin)
{
    _N = normal;
    _O = origin;
    _p = -DOT(_N, _O);
}

OHPlane3D::~OHPlane3D()
{
}

void OHPlane3D::set(const OVector3D& normal, const OPoint3D& origin)
{
    _N = normal;
    _O = origin;
    _p = -DOT(_N, _O);
}

OHPlane3D& OHPlane3D::operator=(const OHPlane3D& Plane)
{
    _N = Plane._N;
    _p = Plane._p;
    _O = Plane._O;
    return *this;
}

bool OHPlane3D::operator==(const OHPlane3D& Plane) const
{
    bool res = true;
    res = res && (_N == Plane._N);
    res = res && (_p == Plane._p);
    return res;
}

bool OHPlane3D::operator!=(const OHPlane3D& Plane) const
{
    return !operator==(Plane);
}

int OHPlane3D::intersects(const OSegment3D& seg, OPoint3D& ptIntersec) const
{
    double t;
    return intersects(seg._ptA, seg._ptB, ptIntersec, t);
}

/**
 * For a detailed description of the algorithm used by this function refer to
 * the following link:
 *
 * Intersections of Lines, Segments and Planes (2D and 3D)
 * <http://geometryalgorithms.com/Archive/algorithm_0104/algorithm_0104B.htm>
 *
 */
int OHPlane3D::intersects(const OPoint3D& pt1, const OPoint3D& pt2, OPoint3D& ptIntersec, double& t) const
{
    OVector3D    u(pt1, pt2);
    OVector3D    w(_O, pt1);
    double     D = DOT(_N, u);
    double     N = -DOT(_N, w);
    if (ABS(D) < EPSILON_13)        // segment is parallel to plane
    {
        if (ABS(N) < EPSILON_13)   // segment lies in plane
        {
            return 2;
        }
        else
        {
            return 0;    // no intersection
        }
    }
    // they are not parallel
    // compute intersect param
    double sI = N / D;
    if ((sI < 0.0f) || (sI > 1.0f))
    {
        return 0;    // no intersection
    }
    ptIntersec = OVector3D(pt1) + sI * u;             // compute segment intersect point
    t = sI;
    return 1;
}
