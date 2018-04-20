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
#include "Geometry/Triangle.h"

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
        case ALL:
            return keepAll(currentIntersections, currentTmin);
            break;
        default:
            return keepFirst(currentIntersections, currentTmin);
            break;
    }
}

decimal keepFirst(std::list<Intersection> &currentIntersections, decimal currentTmin)
{
    if (currentIntersections.empty()) { return currentTmin; }

    //Set the max tmin to currentTmin
    decimal tmin = currentTmin;
    std::list<Intersection>::iterator firstInter = currentIntersections.end();

    //Browse through all intersections found
    for (std::list<Intersection>::iterator it = currentIntersections.begin(); it != currentIntersections.end(); it++)
    {
        //Check if the current intersection happens before the closest one we found so far in term of tmin
        if (tmin < 0. || it->t <= tmin)
        {
            tmin = it->t;
            firstInter = it;
        }
    }

    //if some intersection has been found, copy it, clear the list of intersections and put back the copy into the list
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

    return tmin;
}

decimal keepAllBeforeTriangle(std::list<Intersection> &currentIntersections, decimal currentTmin)
{
    if (currentIntersections.empty()) { return currentTmin; }

    //Set the max tmin to currentTmin
    decimal tmin = currentTmin;

    //Find the tmin of the first intersection with a triangle
    for (std::list<Intersection>::iterator it = currentIntersections.begin(); it != currentIntersections.end(); it++)
    {
        Triangle* triangle = dynamic_cast<Triangle*>(it->p);
        if (triangle && (tmin < 0. || it->t < tmin))
        {
            tmin = it->t;
        }
    }
    
    std::list<Intersection>::iterator it = currentIntersections.begin();
    //Erase all intersections that happen after tmin
    while(it != currentIntersections.end())
    {
        if (tmin > 0. && it->t > tmin)
           currentIntersections.erase(it++);
        else
           it++;
    
    }

    return tmin;
}

decimal keepAllBeforeVisible(std::list<Intersection> &currentIntersections, decimal currentTmin)
{
    if (currentIntersections.empty()) { return currentTmin; }

    //Set the max tmin to currentTmin
    decimal tmin = currentTmin;

    //Find the tmin of the first intersection with a visible shape
    for (std::list<Intersection>::iterator it = currentIntersections.begin(); it != currentIntersections.end(); it++)
    {
        if (it->p->isVisible() && (tmin < 0. || it->t < tmin))
        {
            tmin = it->t;
        }
    }


    std::list<Intersection>::iterator it = currentIntersections.begin();
    //Erase all intersections that happen after tmin
    while(it != currentIntersections.end())
    {
        if (tmin > 0. && it->t > tmin)
            currentIntersections.erase(it++);
        else
           it++;
    }

    return tmin;
}

decimal keepAll(std::list<Intersection> &currentIntersections, decimal currentTmin)
{
    if (currentIntersections.empty()) { return currentTmin; }

    //Set the max tmin to currentTmin
    decimal tmin = currentTmin;

    //Find the tmin of the first intersection
    for (std::list<Intersection>::iterator it = currentIntersections.begin(); it != currentIntersections.end(); it++)
    {
        if (tmin < 0. || it->t <= tmin)
        {
            tmin = it->t;
        }
    }

    return tmin;
}

}; //namespace
