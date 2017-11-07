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

#ifndef SPHERE_H
#define SPHERE_H

#include "Shape.h"

/**
 * \brief Sphere Shape
 */
class Sphere : public Shape
{

public:
	/// Constructor, the Sphere is located at (0,0,0) with a radius 0
    Sphere() : Shape() { position = vec3(0.0, 0.0, 0.0); radius = 0.;}
    /// Constructor with the position pos and the radius r of the sphere
    Sphere(vec3 pos, decimal r) : Shape()
    {
        position = vec3(pos);
        radius = r;

        setBBox();
    }
    /// Copy constructor from a pointed sphere
    Sphere(Sphere* other) : Shape(other)
    {
        position = vec3(other->position);
        radius = other->radius;
        box = other->box;
    }
    /// Return a pointer to this sphere
    virtual Shape* Clone()
    {
        Sphere* newShape = new Sphere(this);
        return newShape;
    }
    /// Copy constructor
    Sphere(const Sphere& other) : Shape(other)
    {
        position = vec3(other.position);
        radius = other.radius;
        box = other.box;
    }
    /**
     * @brief Check if a ray intersect this sphere
     * @param ray The ray
     * @param inter The intersection
     * @return True if succeeds, false if not
     */
    virtual bool getIntersection(Ray& ray, Intersection& inter);
    /// Set the radius of the sphere
    void setRadius(decimal _radius) { radius = _radius; setBBox();  }
    /// Get the radius of the sphere
    decimal getRadius() { return radius; }
    /// Set the center of the sphere
    void setPosition(const vec3& _position) { position = _position; setBBox(); }
    /// Get the position (center) of the sphere
    vec3 getPosition() { return position; }
    virtual int form() { return SPHERE; }

protected:
    vec3 position;	//!< Center of the sphere
    decimal radius;	//!< Radius of the sphere

	void setBBox(){
		box = BBox(vec3(position.x - radius, position.y - radius, position.z - radius), vec3(position.x + radius, position.y + radius, position.z + radius));
	}

};

#endif
