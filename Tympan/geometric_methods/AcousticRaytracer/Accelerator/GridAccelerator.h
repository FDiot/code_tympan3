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

#ifndef GRIDACCELERATOR_H
#define GRIDACCELERATOR_H

#include "Geometry/BBox.h"
#include <vector>
#include "Geometry/Shape.h"
#include "Accelerator.h"
#include <stdint.h>

// GridAccel Forward Declarations
struct Voxel;

// Voxel Declarations
struct Voxel
{
    /// Get size
    uint32_t size() const { return primitives.size(); }
    /// Default constructor
    Voxel() { allCanIntersect = true; }
    /// Constructor
    Voxel(Shape* op)
    {
        allCanIntersect = true;
        primitives.push_back(op);
    }
    void AddPrimitive(Shape* prim)
    {
        primitives.push_back(prim);
    }
    bool Intersect(Ray* ray, std::list<Intersection> &result, decimal& intermin, leafTreatment::treatment choice);
private:
    std::vector< Shape* > primitives; //!< Vector containing the primitives
    bool allCanIntersect;	//!< Flag not used
};


/**
 * \brief Regular grid Accelerator
 */
class GridAccelerator : public Accelerator
{

public:
	/// Constructor
    GridAccelerator(std::vector<Shape*>* _initialMesh = NULL, BBox _globalBox = BBox());
    /// Destructor
    virtual ~GridAccelerator()
    {
        for (int i = 0; i < nVoxels[0]*nVoxels[1]*nVoxels[2]; ++i)
            if (voxels[i]) { voxels[i]->~Voxel(); }
        free(voxels);
    }

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
    /// Print (not implemented)
    void print();
/* Uncommented cause not used:
    bool alreadyFail;
    unsigned int nbFail;
    Ray r;
    bool trace;
*/
protected:

    std::vector<Shape*> primitives; //!< Pointer to all the shapes (different from initialMesh) as it is reordered
    BBox globalBox;			//!< Root bounding box

    int maxProfondeur;		//!< Maximal depth
    int maxPrimPerLeaf;		//!< Maximal primitives per leaf

    int realMaxProfondeur;	//!< Real maximal depth
/* Uncommented cause unused:
    float isectCost;
    float emptyBonus;
    float traversalCost;
*/
private:
    // GridAccel Private Methods
    int posToVoxel(const vec3& P, int axis) const
    {
        int v = (int)((P[axis] - bounds.pMin[axis]) *
                      invWidth[axis]);
        return Clamp(v, 0, nVoxels[axis] - 1);
    }
    float voxelToPos(int p, int axis) const
    {
        return bounds.pMin[axis] + p * width[axis];
    }
    inline int offset(int x, int y, int z) const
    {
        return z * nVoxels[0] * nVoxels[1] + y * nVoxels[0] + x;
    }

    // GridAccel Private Data
    int nVoxels[3];
    BBox bounds;
    vec3 width, invWidth;
    Voxel** voxels;
};


#endif
