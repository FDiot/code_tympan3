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

#include "Solver.h"


bool Solver::postTreatmentScene(Scene* scene, std::vector<Source>& sources, std::vector<Recepteur>& recepteurs)
{
    return true;
}

double Solver::leafTreatment(vector<Intersection> &primitives)
{
    return -1.0;
}


bool Solver::valideIntersection(Ray* r, Intersection* inter)
{
    return false;
}

bool Solver::valideRayon(Ray* r)
{
    valid_rays.push_back(r);
    return true;
}

void Solver::clean()
{
    while (!valid_rays.empty())
    {
        Ray* r = valid_rays.back();
        valid_rays.pop_back();
        delete r;
    }
}

void Solver::finish()
{
    return;
}

bool Solver::invalidRayon(Ray* r)
{
    delete r;
    return true;
}

bool Solver::loadParameters()
{
    return true;
}
