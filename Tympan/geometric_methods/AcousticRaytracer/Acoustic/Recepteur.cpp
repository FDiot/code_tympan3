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

#include "Recepteur.h"
#include "Ray/Ray.h"

bool Recepteur::intersectionRecepteur(vec3& origine, vec3& directeur, float tmax, Intersection& result)
{
    Ray r = Ray(origine, directeur);
    r.setMint ( 0.);
    if (tmax < 0.)
    {
        r.setMaxt ( 100000.);
    }
    else
    {
        r.setMaxt(tmax);
    }

    return getIntersection(r, result);
}
