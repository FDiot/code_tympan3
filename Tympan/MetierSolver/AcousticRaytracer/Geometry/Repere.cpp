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

#include "Repere.h"

Repere::Repere(vec3 _U, vec3 _V, vec3 _W, vec3 _O) : Base()
{

    name = "unknonw repere";

    U = vec3(_U);
    V = vec3(_V);
    W = vec3(_W);
    O = vec3(_O);

    updateMatrices();
}

Repere::Repere(const Repere& repere) : Base(repere)
{
    U = vec3(repere.U);
    V = vec3(repere.V);
    W = vec3(repere.W);
    O = vec3(repere.O);

    updateMatrices();

}

void Repere::updateMatrices()
{

    //Matrice du repere local vers global
    LtoG[0][0] = U.x; LtoG[0][1] = V.x; LtoG[0][2] = W.x; LtoG[0][3] = O.x;
    LtoG[1][0] = U.y; LtoG[1][1] = V.y; LtoG[1][2] = W.y; LtoG[1][3] = O.y;
    LtoG[2][0] = U.z; LtoG[2][1] = V.z; LtoG[2][2] = W.z; LtoG[2][3] = O.z;
    LtoG[3][0] = 0.0; LtoG[3][1] = 0.0; LtoG[3][2] = 0.0; LtoG[3][3] = 1.0;


    //Matrice du repere global vers local
    //On suppose qu'on travail avec des reperes orthonorme. Donc le passage d'un repere a l'autre est une translation et une rotation.
    //L'inverse de la matrice de rotation est sa transposee
    //L'inverse de la translation est la negation de la matrice de translation
    GtoL[0][0] = U.x; GtoL[0][1] = U.y; GtoL[0][2] = U.z; GtoL[0][3] = -U.x * O.x - U.y * O.y - U.z * O.z;
    GtoL[1][0] = V.x; GtoL[1][1] = V.y; GtoL[1][2] = V.z; GtoL[1][3] = -V.x * O.x - V.y * O.y - V.z * O.z;
    GtoL[2][0] = W.x; GtoL[2][1] = W.y; GtoL[2][2] = W.z; GtoL[2][3] = -W.x * O.x - W.y * O.y - W.z * O.z;
    GtoL[3][0] = 0.0; GtoL[3][1] = 0.0; GtoL[3][2] = 0.0; GtoL[3][3] = 1.0;
}


vec3 Repere::vectorFromLocalToGlobal(vec3& local)
{
    vec3 result;
    result.x = local.x * LtoG[0][0] + local.y * LtoG[0][1] + local.z * LtoG[0][2];
    result.y = local.x * LtoG[1][0] + local.y * LtoG[1][1] + local.z * LtoG[1][2];
    result.z = local.x * LtoG[2][0] + local.y * LtoG[2][1] + local.z * LtoG[2][2];

    return result;
}

vec3 Repere::vectorFromGlobalToLocal(vec3& global)
{
    vec3 result;
    result.x = global.x * GtoL[0][0] + global.y * GtoL[0][1] + global.z * GtoL[0][2];
    result.y = global.x * GtoL[1][0] + global.y * GtoL[1][1] + global.z * GtoL[1][2];
    result.z = global.x * GtoL[2][0] + global.y * GtoL[2][1] + global.z * GtoL[2][2];

    return result;
}

vec3 Repere::positionFromLocalToGlobal(vec3& local)
{
    vec4 local4 = vec4(local);
    vec4 result4;

    result4.x = local4.x * LtoG[0][0] + local4.y * LtoG[0][1] + local4.z * LtoG[0][2] + local4.w * LtoG[0][3];
    result4.y = local4.x * LtoG[1][0] + local4.y * LtoG[1][1] + local4.z * LtoG[1][2] + local4.w * LtoG[1][3];
    result4.z = local4.x * LtoG[2][0] + local4.y * LtoG[2][1] + local4.z * LtoG[2][2] + local4.w * LtoG[2][3];
    result4.w = local4.x * LtoG[3][0] + local4.y * LtoG[3][1] + local4.z * LtoG[3][2] + local4.w * LtoG[3][3];

    vec3 result = vec3(result4.x / result4.w, result4.y / result4.w, result4.z / result4.w);

    return result;
}

vec3 Repere::positionFromGlobalToLocal(vec3& global)
{
    vec4 global4 = vec4(global);
    vec4 result4;

    result4.x = global4.x * GtoL[0][0] + global4.y * GtoL[0][1] + global4.z * GtoL[0][2] + global4.w * GtoL[0][3];
    result4.y = global4.x * GtoL[1][0] + global4.y * GtoL[1][1] + global4.z * GtoL[1][2] + global4.w * GtoL[1][3];
    result4.z = global4.x * GtoL[2][0] + global4.y * GtoL[2][1] + global4.z * GtoL[2][2] + global4.w * GtoL[2][3];
    result4.w = global4.x * GtoL[3][0] + global4.y * GtoL[3][1] + global4.z * GtoL[3][2] + global4.w * GtoL[3][3];

    vec3 result = vec3(result4.x / result4.w, result4.y / result4.w, result4.z / result4.w);

    return result;
}
