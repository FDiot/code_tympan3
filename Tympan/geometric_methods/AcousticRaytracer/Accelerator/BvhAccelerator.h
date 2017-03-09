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

#include "Tympan/geometric_methods/AcousticRaytracer/Geometry/BBox.h"
#include <vector>
#include "Tympan/geometric_methods/AcousticRaytracer/Geometry/Shape.h"
#include "Accelerator.h"

struct BVHBuildNode;

// BVHAccel Forward Declarations
struct BVHPrimitiveInfo;
struct LinearBVHNode;

/**
 * \brief A Bounding Volume Hierarchy (BVH) Accelerator
 */
class BvhAccelerator : public Accelerator
{

public:
	/// Default constructor
    BvhAccelerator(std::vector<Shape*>* _initialMesh = NULL, BBox _globalBox = BBox(), unsigned int maxProf = 8, const string& sm = "sah");
    /// Destructor
    virtual ~BvhAccelerator() { free(nodes); }

    virtual bool build();

    virtual decimal traverse(Ray* r, std::list<Intersection> &result) const;

    /// Set maximal depth
    void setMaxProfondeur(int _maxProfondeur) { maxProfondeur = _maxProfondeur; }
    /// Get maximal depth
    int getMaxProfondeur() { return maxProfondeur; }
    /// Set maximal primitives per leaf
    void setMaxPrimPerLeaf(int _maxPrimPerLeaf) { maxPrimPerLeaf = _maxPrimPerLeaf; }
    /// Get maximal primitives per leaf
    int getMaxPrimPerLeaf() { return maxPrimPerLeaf; }
    /// Get the vector of bounding boxes
    std::vector<BBox>& getBBox() { return tableBox; }

    /// Print not implemented:
    void print();
/* Uncommented cause not used:
    bool alreadyFail;
    unsigned int nbFail;
    Ray r;
    bool trace;
*/
protected:
    // BVHAccel Private Methods
    BVHBuildNode* recursiveBuild(std::vector<BVHPrimitiveInfo> &buildData, unsigned int start, unsigned int end,
                                 unsigned int* totalNodes, std::vector<Shape* > &orderedPrims);
    unsigned int flattenBVHTree(BVHBuildNode* node, unsigned int* offset);

    // BVHAccel Private Data
    unsigned int maxPrimsInNode;	//!< Maximal primitives in node
    enum SplitMethod { SPLIT_MIDDLE, SPLIT_EQUAL_COUNTS, SPLIT_SAH };
    SplitMethod splitMethod;	//!< Split method
    LinearBVHNode* nodes;		//!< Nodes list

    std::vector<Shape*> primitives; //!< Pointer to all the shapes (different from initialMesh) as it is reordered
    BBox globalBox;			//!< Global bounding box

    std::vector<BBox> tableBox; //!< Bounding boxes vector

    int maxProfondeur;		//!< Maximal depth
    int maxPrimPerLeaf;		//!< Maximal primitives per leaf

    int realMaxProfondeur;	//!< Real maximal depth
/* Uncommented cause not used:
    float isectCost;
    float emptyBonus;
    float traversalCost; */
};


#endif
