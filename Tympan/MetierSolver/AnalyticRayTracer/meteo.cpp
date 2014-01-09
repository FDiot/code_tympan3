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

#include "meteo.h"

meteo::meteo()
{
    gradC = 0.5;
    gradV = 0.5;
    c0 = 340;
	windDirection = 0.0;
}

meteo::meteo(const double& gradC, const double& gradV, const double& windDir, const double& c0) : gradC(gradC), gradV(gradV), windDirection(windDir), c0(c0) {} ;

meteo::~meteo()
{
}

double meteo::cLin(const vec3& P, vec3& grad) const
{

    // calcul de la celerite
    decimal c = gradC * P.z + c0;

    // calcul du gradient
    grad.z = gradC;

    return c;
};

vec3 meteo::vent(const vec3& P, std::map<std::pair<int, int>, decimal> &jacob) const
{

    // calcul du vent : on a une fonction lineaire fonction de la coordonnee z du point
    vec3 v;

	double angle = -(M_PI/2.0) - (windDirection * M_PI /180.0);
	double DVx = cos(angle) * gradV;
	double DVy = sin(angle) * gradV;

    v.x = DVx * P.z;
    v.y = DVy * P.z;
    v.z = 0;



    //v.x = 0.5 * Meteo.gradV * P.z;
    //v.y = 0.5 * Meteo.gradV * P.z;
    //v.z = 0;

    // calcul de la jacobienne
    jacob[std::make_pair(1, 3)] = DVx;
    jacob[std::make_pair(2, 3)] = DVy;


    //jacob[make_pair(1, 3)] = 0.5 * Meteo.gradV;
    //jacob[make_pair(2, 3)] = 0.5 * Meteo.gradV;

    return v;
}
