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

#ifndef RECEPTEUR_H
#define RECEPTEUR_H

#include "Tympan/solvers/AcousticRaytracer/Geometry/Shape.h"
#include "Tympan/solvers/AcousticRaytracer/Geometry/Sphere.h"
#include "Tympan/models/common/mathlib.h"
#include "Tympan/solvers/AcousticRaytracer/Base.h"

class Recepteur : public Sphere
{
public:

	Recepteur(const vec3& pos, const decimal& r) : Sphere(pos, r) 
	{
		name = "unknow receptor";
	}

    Recepteur(const Recepteur& other) : Sphere(other)
    {
        id = other.id;
    }

    virtual ~Recepteur() { }

    Shape* getShape() { return dynamic_cast<Shape*>(this); }

    vec3 getPosition() { return this->getBBox().centroid; }
    bool intersectionRecepteur(vec3& origine, vec3& directeur, float tmax, Intersection& result);

    unsigned int getId() { return id;}
    void setId(unsigned int _id) { id = _id; }

protected:
    unsigned int id;
};

#endif
