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

#include "Tympan/geometric_methods/AcousticRaytracer/Geometry/Shape.h"
#include "LeafTreatment.h"
#include <list>

/**
 * @brief Base class for accelerators
 */
class Accelerator
{

public:
	/// Constructors
    Accelerator() { intersectionChoice = leafTreatment::FIRST; defineLeafFunction(); }
    Accelerator(std::vector<Shape*> *_shapes, BBox& _globalBox) : shapes(_shapes),
																  globalBox(_globalBox)
    {
        intersectionChoice = leafTreatment::FIRST;
		defineLeafFunction();
    }
    /// Copy constructor
    Accelerator(const Accelerator& other)
    {
        shapes = other.shapes;
        globalBox = other.globalBox;
        intersectionChoice = other.intersectionChoice;
		pLeafTreatmentFunction = other.pLeafTreatmentFunction;
    }
    /// Destructor
    virtual ~Accelerator() { }

    /// Get/Set the Intersection choice
    leafTreatment::treatment getIntersectionChoice() { return intersectionChoice; }
    void setIntersectionChoice(leafTreatment::treatment _intersectionChoice = leafTreatment::FIRST) { intersectionChoice = _intersectionChoice; }
    /// Build this accelerator
    virtual bool build() { return false; }
    /// Run this accelerator
    virtual decimal traverse(Ray* r, std::list<Intersection> &result) const { return -1.; }

protected:
    /// To define leaf function
	void defineLeafFunction()
	{
		switch (intersectionChoice)
		{
			case leafTreatment::FIRST:
				pLeafTreatmentFunction = leafTreatment::keepFirst;
				break;
			case leafTreatment::ALL_BEFORE_TRIANGLE:
				pLeafTreatmentFunction = leafTreatment::keepAllBeforeTriangle;
				break;
			case leafTreatment::ALL_BEFORE_VISIBLE:
				pLeafTreatmentFunction = leafTreatment::keepAllBeforeVisible;
				break;
            case leafTreatment::ALL:
                pLeafTreatmentFunction = leafTreatment::keepAll;
                break;
			default:
				pLeafTreatmentFunction = leafTreatment::keepFirst;
				break;
		}
	}

	decimal (*pLeafTreatmentFunction) (std::list<Intersection> &, decimal); //!< Pointer to the treatment function of leaf

    leafTreatment::treatment intersectionChoice;	//!< Intersection choice

    std::vector<Shape*> *shapes;					//!< Vector of pointers to shapes
    BBox globalBox;									//!< Global bounding box
};

#endif
