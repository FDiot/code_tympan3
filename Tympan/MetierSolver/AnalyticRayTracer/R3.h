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

// ********** DO NOT REMOVE THIS BANNER **********
// ORIG-DATE: 27-01-2012
// ORG      : LJLL Universite Pierre et Marie Curie, Paris,  FRANCE
// author Projet_Tympan


#ifndef R3_HPP
#define  R3_HPP

#include <cmath>
#include <cassert>
#include <cstdlib>
#include <iostream>
#include "R2.h"
#include <vector>
#include "Tympan/MetierSolver/ToolsMetier/OPoint3D.h"

typedef double R;

#define EPSILON 0.000001
#define PI (4*atan(1.0))

class R3
{

public:
    // declaration de membre
    R x, y, z;

    // constructeurs
    R3() : x(0.), y(0.), z(0.) {}
    R3(const R& a, const R& b, const R& c) : x(a), y(b), z(c) {}
    R3(const R3& a, const R3& b): x(b.x - a.x), y(b.y - a.y), z(b.z - a.z) {}                          // coordonnees du vecteur ab

    // le constucteur par defaut est inutile
    /*
    R3 (const R3 & a) : x(a.x), y(a.y), z(a.z) { cout << "  const par copy " << endl;}
    R3 ( R3 & a) : x(a.x), y(a.y), z(a.z) { cout << "  const pas const  par copy " << endl; }
    */

    // rappel les operateurs definis dans une classe ont un parametre cache qui est la classe elle meme (*this)

    // les operateurs affectation

    //R3 &  operator =(R3 & P)  {x = P.x; y = P.y; z = P.z;return *this;}

    // les autres operateurs d'affectation
    R3&   operator += (const R3& P) {x += P.x; y += P.y; z += P.z; return *this;}
    R3&   operator -= (const R3& P) {x -= P.x; y -= P.y; z -= P.z; return *this;}

    // operateurs binaires + - * , ^ /
    R3   operator + (const R3& P)const {return R3(x + P.x, y + P.y, z + P.z);}
    R3   operator - (const R3& P)const {return R3(x - P.x, y - P.y, z - P.z);}
    R    operator , (const R3& P)const {return x * P.x + y * P.y + z * P.z;}                                   // produit scalaire
    R3   operator ^(const R3& P)const {return R3(y * P.z - z * P.y, z * P.x - x * P.z, x * P.y - y * P.x) ;}   // produit vectoriel
    R3   operator * (const R& c)const {return R3(x * c, y * c, z * c);}
    R3   operator / (const R& c)const {return R3(x / c, y / c, z / c);}

    // operateurs unaires
    R3   operator - ()const {return R3(-x, -y, -z);}
    R3   operator + ()const {return *this;}

    // les operators tableaux
    // version qui peut modifier la classe via l'adresse de x, y ou z
    R& operator[](const int& i) { if (i == 0) { return x; } else if (i == 1) { return y; } else if (i == 2) { return z; } else {assert(0); exit(1);} ;}
    // version qui retourne une reference const qui ne modifie pas la classe
    const R& operator[](const int& i) const { if (i == 0) { return x; } else if (i == 1) { return y; } else if (i == 2) { return z; } else {assert(0); exit(1);} ;}

    R norme() const { return std::sqrt(x * x + y * y + z * z);}
    R norme2() const { return (x * x + y * y + z * z);}

    R normalize(void)
    {
        R inv, l = this->norme();
        if (l < EPSILON) { return 0.0f; }
        inv = 1.0f / l;
        this->x *= inv;
        this->y *= inv;
        this->z *= inv;
        return l;
    }

    // Produit scalaire
    R mult(const R3& Q) {return Q.x * x + Q.y * y + Q.z * z;}

    static const int  d = 3; // utilisation:  R3::d  (n'ajoute pas de memoire)

};

inline std::ostream& operator <<(std::ostream& f, const R3& P)
{ f << "(" << P.x << ", " << P.y << ", " << P.z << ")"  ; return f; }

inline std::istream& operator >>(std::istream& f,  R3& P)
{ f >> P.x >> P.y >> P.z ; return f; }

inline R3 operator *(const R& c, const R3& P) {return P * c;}

inline std::vector<R3> operator * (const R& a, const std::vector<R3>& v)
{
    std::vector<R3> result ;
    for (unsigned int i = 0; i < v.size(); ++i) { result.push_back(a * v[i]); }
    return result;
}

inline std::vector<R3> operator * (const std::vector<R3>& v, const R& a)
{
    std::vector<R3> result ;
    for (unsigned int i = 0; i < v.size(); ++i) { result.push_back(v[i]*a); }
    return result;
}

inline std::vector<R3> operator + (const std::vector<R3>& u, const std::vector<R3>& v)
{
    assert(u.size() == v.size());
    std::vector<R3> result ;
    for (unsigned int i = 0; i < v.size(); ++i) { result.push_back(u[i] + v[i]); }
    return result;
}

inline OPoint3D R3toOPoint3D(const R3& P) { return OPoint3D(P.x, P.y, P.z); }
inline R3 OPoint3DtoR3(const OPoint3D& P) { return R3(P._x, P._y, P._z); }

#endif
