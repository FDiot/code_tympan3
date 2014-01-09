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
 
#include <cmath>


//-------------------------------------------------------------------------------------------------------------------------------------------//
//
//-------------------------------------------------------------------------------------------------------------------------------------------//

double interpo(const vec3* triangle, vec3 P)
{
    // rend la coordonnee P.z

    const vec3& A = triangle[0];
    const vec3& B = triangle[1];
    const vec3& C = triangle[2];

    double del = (B.x - A.x) * (C.y - A.y) - (C.x - A.x) * (B.y - A.y);

    double a = ((B.x - P.x) * (C.y - P.y) - (C.x - P.x) * (B.y - P.y)) / del;
    double b = ((C.x - P.x) * (A.y - P.y) - (A.x - P.x) * (C.y - P.y)) / del;
    double c = ((A.x - P.x) * (B.y - P.y) - (B.x - P.x) * (A.y - P.y)) / del;

    //  P.z = a*A.z + b*B.z + c*C.z;

    //  return P.z;

    return a * A.z + b * B.z + c * C.z;
}