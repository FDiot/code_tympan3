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

#ifndef BRUTE_FORCE_ACCELERATOR_H
#define BRUTE_FORCE_ACCELERATOR_H

#include "Accelerator.h"

class BruteForceAccelerator : public Accelerator
{

public:
    BruteForceAccelerator() { }
    BruteForceAccelerator(std::vector<Shape*> *_shapes, BBox& _globalBox) : Accelerator(_shapes, _globalBox) {    }

    BruteForceAccelerator(const BruteForceAccelerator& other)
    {
        shapes = other.shapes;
        globalBox = other.globalBox;
        intersectionChoice = other.intersectionChoice;
    }

    virtual ~BruteForceAccelerator() { }

    virtual bool build() { return true; }

    virtual decimal traverse(Ray* r, std::list<Intersection> &result);

};
#endif