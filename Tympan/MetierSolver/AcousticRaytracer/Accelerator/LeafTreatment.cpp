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

#include "LeafTreatment.h"
#include "Tympan/MetierSolver/AcousticRaytracer/Geometry/Triangle.h"

namespace leafTreatment
{

decimal keepFunction(treatment choice, std::list<Intersection> &currentIntersections, decimal currentTmin)
{
    //std::cout<<"Il faut filtrer "<<currentIntersections.size()<<" intersections."<<std::endl;
    switch (choice)
    {
        case FIRST:
            return keepFirst(currentIntersections, currentTmin);
            break;
        case ALL_BEFORE_TRIANGLE:
            return keepAllBeforeTriangle(currentIntersections, currentTmin);
            break;
        case ALL_BEFORE_VISIBLE:
            return keepAllBeforeVisible(currentIntersections, currentTmin);
            break;
        default:
            return keepFirst(currentIntersections, currentTmin);
            break;
    }
}

decimal keepFirst(std::list<Intersection> &currentIntersections, decimal currentTmin)
{
    if (currentIntersections.empty()) { return currentTmin; }

    decimal tmin = currentTmin;
    std::list<Intersection>::iterator firstInter = currentIntersections.end();

    for (std::list<Intersection>::iterator it = currentIntersections.begin(); it != currentIntersections.end(); it++)
    {
        if (tmin < 0. || it->t <= tmin)
        {
            tmin = it->t;
            firstInter = it;
        }
    }

    if (firstInter != currentIntersections.end())
    {
        Intersection firstIntersection;
        firstIntersection.normal = firstInter->normal;
        firstIntersection.p = firstInter->p;
        firstIntersection.t = firstInter->t;
        firstIntersection.forme = firstInter->forme;
        currentIntersections.clear();
        currentIntersections.push_back(firstIntersection);
    }
    if (currentIntersections.size() > 1)
    {
        std::cout << "Erreur : trop d'intersections." << std::endl;
    }
    return tmin;
}

decimal keepAllBeforeTriangle(std::list<Intersection> &currentIntersections, decimal currentTmin)
{
    if (currentIntersections.empty()) { return currentTmin; }

    decimal tmin = currentTmin;
    for (std::list<Intersection>::iterator it = currentIntersections.begin(); it != currentIntersections.end(); it++)
    {
        Triangle* triangle = dynamic_cast<Triangle*>(it->p);
        if (triangle && (tmin < 0. || it->t < tmin))
        {
            tmin = it->t;
        }
    }

    for (std::list<Intersection>::iterator it = currentIntersections.begin(); it != currentIntersections.end(); it++)
    {
        if (it->t > tmin)
        {
            currentIntersections.erase(it);
            --it;
        }
    }

    return tmin;
}

decimal keepAllBeforeVisible(std::list<Intersection> &currentIntersections, decimal currentTmin)
{
    if (currentIntersections.empty()) { return currentTmin; }

    decimal tmin = currentTmin;
    for (std::list<Intersection>::iterator it = currentIntersections.begin(); it != currentIntersections.end(); it++)
    {
        if (it->p->isVisible() && (tmin < 0. || it->t < tmin))
        {
            tmin = it->t;
        }
    }

    for (std::list<Intersection>::iterator it = currentIntersections.begin(); it != currentIntersections.end(); it++)
    {
        if (tmin < 0. || it->t > tmin)
        {
            currentIntersections.erase(it);
            --it;
        }
    }

    return tmin;
}


}; //namespace
