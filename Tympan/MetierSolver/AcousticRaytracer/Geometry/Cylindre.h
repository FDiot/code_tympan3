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
 
#ifndef CYLINDRE_H
#define CYLINDRE_H

#include "Shape.h"
#include "Repere.h"

//#ifdef USE_QT
//  #include "CylindreGraphic.h"
//#endif

class Cylindre : public Shape
{

    //#ifdef USE_QT
    //  //WIDGET_DECL(Shape)
    //  GRAPHIC_DECL(Cylindre)
    //#endif

public:

    Cylindre(Shape* _p1 = NULL, Shape* _p2 = NULL, std::vector<vec3>* _vertices = NULL, unsigned int v1 = 0, unsigned int v2 = 1, decimal _epaisseur = 0.2);
    Cylindre(const Cylindre& other);

    virtual ~Cylindre() {};

    virtual bool isVisible() { return false;}

    virtual bool getIntersection(Ray& ray, Intersection& inter);

    void setAngleOuverture(decimal angle) { angleOuverture = angle; }
    decimal getAngleOuverture() { return angleOuverture; }

    Shape* getFirstShape() { return p1; }
    Shape* getSecondShape() { return p2; }

    virtual bool sample(decimal density, std::vector<vec3>& samples);


protected:

    void computeMesh();

    Shape* p1;
    Shape* p2;

    decimal epaisseur;
    decimal hauteur;
    decimal angleOuverture;

    Repere localRepere;

    vector<vec3> points;
    vector<Shape*> mesh;

};

#endif
