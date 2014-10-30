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

#ifndef ACCELERATOR_H
#define ACCELERATOR_H

#include "Tympan/solvers/AcousticRaytracer/Geometry/Shape.h"
#include "LeafTreatment.h"
#include <list>

class Accelerator
{

public:
    Accelerator() { intersectionChoice = leafTreatment::FIRST; defineLeafFunction(); }
    Accelerator(std::vector<Shape*> *_shapes, BBox& _globalBox) : shapes(_shapes),
																  globalBox(_globalBox)
    {
        intersectionChoice = leafTreatment::FIRST;
		defineLeafFunction();
    }

    Accelerator(const Accelerator& other)
    {
        shapes = other.shapes;
        globalBox = other.globalBox;
        intersectionChoice = other.intersectionChoice;
		pLeafTreatmentFunction = other.pLeafTreatmentFunction;
    }

    virtual ~Accelerator() { }

    leafTreatment::treatment getIntersectionChoice() { return intersectionChoice; }
    void setIntersectionChoice(leafTreatment::treatment _intersectionChoice) { intersectionChoice = _intersectionChoice; }

    virtual bool build() { return false; }

    virtual decimal traverse(Ray* r, std::list<Intersection> &result) const { return -1.; }

protected:
	void defineLeafFunction()
	{
		switch (intersectionChoice)
		{
			case leafTreatment::treatment::FIRST:
				pLeafTreatmentFunction = leafTreatment::keepFirst;
				break;
			case leafTreatment::treatment::ALL_BEFORE_TRIANGLE:
				pLeafTreatmentFunction = leafTreatment::keepAllBeforeTriangle;
				break;
			case leafTreatment::treatment::ALL_BEFORE_VISIBLE:
				pLeafTreatmentFunction = leafTreatment::keepAllBeforeVisible;
				break;
			default:
				pLeafTreatmentFunction = leafTreatment::keepFirst;
				break;
		}
	}

	// Pointeur sur la fonction de traitement de "feuilles"
	decimal (*pLeafTreatmentFunction) (std::list<Intersection> &, decimal);

    leafTreatment::treatment intersectionChoice;

    std::vector<Shape*> *shapes;
    BBox globalBox;
};

#endif