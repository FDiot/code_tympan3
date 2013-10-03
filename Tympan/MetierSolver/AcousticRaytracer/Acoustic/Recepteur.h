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

#include "Tympan/MetierSolver/AcousticRaytracer/Geometry/Shape.h"
#include "Tympan/MetierSolver/AcousticRaytracer/Geometry/Sphere.h"
#include "Tympan/MetierSolver/AcousticRaytracer/Geometry/mathlib.h"
#include "Tympan/MetierSolver/AcousticRaytracer/Base.h"

//#ifdef USE_QT
//  #include "RecepteurGraphic.h"
//#endif


class Recepteur : public Base
{

    //#ifdef USE_QT
    //  //WIDGET_DECL(Recepteur)
    //  GRAPHIC_DECL(Recepteur)
    //#endif

public:

    Recepteur(Shape* _shape = new Sphere(), std::string _name = "unknow receptor") : Base()
    {
        shape = _shape;
        name = _name;
    }

    Recepteur(const Recepteur& other) : Base(other)
    {
        shape = other.shape->Clone();
        id = other.id;
    }

    virtual ~Recepteur() { delete shape;}

    Shape* getShape() { return shape; }
    void setShape(Shape* _shape) { shape = _shape; }

    vec3 getPosition() { return shape->getBBox().centroid; }
    bool intersectionRecepteur(vec3& origine, vec3& directeur, float tmax, Intersection& result);

    unsigned int getId() { return id;}
    void setId(unsigned int _id) { id = _id; }

protected:
    Shape* shape;
    unsigned int id;
};

#endif
