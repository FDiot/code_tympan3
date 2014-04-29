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

#include "meteoLin.h"


double meteoLin::cTemp(const vec3& P, vec3& grad) const
{

    // calcul de la celerite
    decimal c = grad_C * P.z + c0;

    // calcul du gradient
    grad.z = grad_C;

    return c;
};

vec3 meteoLin::cWind(const vec3& P) const
{
    // calcul du vent : on a une fonction lineaire fonction de la coordonnee z du point
    vec3 v;

    const double& DVx = jacob_matrix[0][2];
    const double& DVy = jacob_matrix[1][2];

    v.x = DVx * P.z;
    v.y = DVy * P.z;
    v.z = 0;

    return v;
}

void meteoLin::init()
{
    double angle = -M_PIDIV2 - wind_angle;
    double DVx = cos(angle) * grad_V;
    double DVy = sin(angle) * grad_V;

    for (unsigned short i = 0; i < 3; i++)
    {
        for (unsigned short j = 0; j < 3; j++)
        {
            jacob_matrix[i][j] = 0.;
        }
    }

    jacob_matrix[0][2] = DVx;
    jacob_matrix[1][2] = DVy;
}
