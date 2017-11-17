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

#include "Sphere.h"

bool Sphere::getIntersection(Ray& ray, Intersection& inter)
{
    // intersection rayon/sphere
    vec3 dist = position - ray.getPosition();
    decimal B = ray.getDirection() * dist;
    decimal D = B * B - dist * dist + radius * radius;
    if (D < 0.0f)
    {
        return false;
    }
    float t0 = B - sqrtf(D);
    float t1 = B + sqrtf(D);
    if ((t0 > 0.1f) && (t0 < ray.getMaxt()))
    {
        inter.t = t0;
        inter.p = this;
        inter.forme = SPHERE;
        return true;
    }
    if ((t1 > 0.1f) && (t1 < ray.getMaxt()))
    {
        inter.t = t1;
        inter.p = this;
        inter.forme = SPHERE;
        return true;
    }
    return false;
}
