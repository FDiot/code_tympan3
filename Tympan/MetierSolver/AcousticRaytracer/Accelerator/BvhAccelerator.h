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

#ifndef BVHACCELERATOR_H
#define BVHACCELERATOR_H

#include "Tympan/MetierSolver/AcousticRaytracer/Geometry/BBox.h"
#include <vector>
#include "Tympan/MetierSolver/AcousticRaytracer/Geometry/Shape.h"
#include "Accelerator.h"

struct BVHBuildNode;

// BVHAccel Forward Declarations
struct BVHPrimitiveInfo;
struct LinearBVHNode;


class BvhAccelerator : public Accelerator
{

public:
    BvhAccelerator(std::vector<Shape*>* _initialMesh = NULL, BBox _globalBox = BBox(), unsigned int maxProf = 8, const string& sm = "sah");

    virtual ~BvhAccelerator() { free(nodes); }

    virtual bool build();

    virtual decimal traverse(Ray* r, std::list<Intersection> &result) const;

    void setMaxProfondeur(int _maxProfondeur) { maxProfondeur = _maxProfondeur; }
    int getMaxProfondeur() { return maxProfondeur; }

    void setMaxPrimPerLeaf(int _maxPrimPerLeaf) { maxPrimPerLeaf = _maxPrimPerLeaf; }
    int getMaxPrimPerLeaf() { return maxPrimPerLeaf; }

    std::vector<BBox>& getBBox() { return tableBox; }

    void print();

    bool alreadyFail;
    unsigned int nbFail;
    Ray r;
    bool trace;

protected:
    // BVHAccel Private Methods
    BVHBuildNode* recursiveBuild(std::vector<BVHPrimitiveInfo> &buildData, unsigned int start, unsigned int end,
                                 unsigned int* totalNodes, std::vector<Shape* > &orderedPrims);
    unsigned int flattenBVHTree(BVHBuildNode* node, unsigned int* offset);

    // BVHAccel Private Data
    unsigned int maxPrimsInNode;
    enum SplitMethod { SPLIT_MIDDLE, SPLIT_EQUAL_COUNTS, SPLIT_SAH };
    SplitMethod splitMethod;
    LinearBVHNode* nodes;

    std::vector<Shape*> primitives; //Different de initialMesh, est reordonne par rapport a la structure
    BBox globalBox;

    std::vector<BBox> tableBox;

    int maxProfondeur;
    int maxPrimPerLeaf;

    int realMaxProfondeur;

    float isectCost;
    float emptyBonus;
    float traversalCost;
};


#endif