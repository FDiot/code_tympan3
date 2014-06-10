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




#include "OMatrix.h"
#include "OGeometrie.h"
#include <stdio.h>


OMatrix::OMatrix()
{
    // Init matrice unite
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

OVector3D OMatrix::operator*(const OVector3D& vector) const
{
    double  echelle;
    OVector3D vecRes;

    vecRes._x = _m[0][0] * vector._x + _m[0][1] * vector._y + _m[0][2] * vector._z + _m[0][3];
    vecRes._y = _m[1][0] * vector._x + _m[1][1] * vector._y + _m[1][2] * vector._z + _m[1][3];
    vecRes._z = _m[2][0] * vector._x + _m[2][1] * vector._y + _m[2][2] * vector._z + _m[2][3];

    echelle = _m[3][0] * vector._x + _m[3][1] * vector._y + _m[3][2] * vector._z + _m[3][3];

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

#ifdef _DEBUG_GEO
    printf("Ox V : %f %f %f\n", vector._x, vector._y, vector._z);
#endif

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

#ifdef _DEBUG_GEO
        show();
#endif

    }
    else
    {
        //MsgErr ("MatAligneVecteurSurOx : Vecteur nul");
    }

    return res;
}

int OMatrix::aligneVecteurSurOy(const OVector3D& vector)
{
    int res = 0;

    double  cos1, sin1, cos2, sin2;
    double  n1, n2;

#ifdef _DEBUG_GEO
    printf("Oy V : %f %f %f\n", vector._x, vector._y, vector._z);
#endif

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

#ifdef _DEBUG_GEO
        show();
#endif

    }
    else
    {
    }

    return res;
}

int OMatrix::invert()
{
    int i, j;

    // Calculate the 4x4 determinant
    // if the determinant is zero,
    // then the inverse matrix is not unique.
    double det = determinant();

    // Calculate the adjoint matrix.
    adjoint();

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
    // Copie de cette matrice
    OMatrix mat(*this);

    // Inversion
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
    // Copie de cette matrice
    OMatrix mat(*this);

    // Adjoint
    mat.adjoint();

    return mat;
}

double OMatrix::determinant()
{
    double ans;
    double a1, a2, a3, a4, b1, b2, b3, b4, c1, c2, c3, c4, d1, d2, d3, d4;

    // Assign to individual variable names to aid selecting
    // correct elements.

    a1 = _m[0][0]; b1 = _m[0][1];
    c1 = _m[0][2]; d1 = _m[0][3];

    a2 = _m[1][0]; b2 = _m[1][1];
    c2 = _m[1][2]; d2 = _m[1][3];

    a3 = _m[2][0]; b3 = _m[2][1];
    c3 = _m[2][2]; d3 = _m[2][3];

    a4 = _m[3][0]; b4 = _m[3][1];
    c4 = _m[3][2]; d4 = _m[3][3];

    ans = a1 * OMatrix::mat3x3Det(b2, b3, b4, c2, c3, c4, d2, d3, d4)
          - b1 * OMatrix::mat3x3Det(a2, a3, a4, c2, c3, c4, d2, d3, d4)
          + c1 * OMatrix::mat3x3Det(a2, a3, a4, b2, b3, b4, d2, d3, d4)
          - d1 * OMatrix::mat3x3Det(a2, a3, a4, b2, b3, b4, c2, c3, c4);

    return ans;
}

double OMatrix::mat2x2Det(double a, double b, double c, double d)
{
    double ans;

    ans = a * d - b * c;

    return ans;
}

double OMatrix::mat3x3Det(double a1, double a2, double a3, double b1, double b2, double b3, double c1, double c2, double c3)
{
    double ans;

    ans = a1 * OMatrix::mat2x2Det(b2, b3, c2, c3)
          - b1 * OMatrix::mat2x2Det(a2, a3, c2, c3)
          + c1 * OMatrix::mat2x2Det(a2, a3, b2, b3);

    return ans;
}

