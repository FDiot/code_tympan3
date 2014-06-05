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

#include "RayCourb.h"
#include "../AcousticRaytracer/Geometry/mathlib.h"

RayCourb::RayCourb() : nbReflex(0)
{
}

RayCourb::RayCourb(const vec3& a) : nbReflex(0)
{
    etapes.insert(etapes.begin(), Step(a));
}

RayCourb::RayCourb(const RayCourb& r) : etapes(r.etapes), nbReflex(r.nbReflex), position(r.position), rencontre(r.rencontre)
{
}


RayCourb& RayCourb::operator= (const RayCourb& P)
{
    etapes = P.etapes;
    nbReflex = P.nbReflex;
    position = P.position;
    rencontre = P.rencontre;

    return *this;
}

void RayCourb::purge()
{
    etapes.clear();
    position.clear();
    rencontre.clear();
}
