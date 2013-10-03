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

#include "Tympan/MetierSolver/AcousticRaytracer/Geometry/BBox.h"
#include <vector>
#include "Tympan/MetierSolver/AcousticRaytracer/Geometry/Shape.h"
#include "Accelerator.h"
#include <stdint.h>

// GridAccel Forward Declarations
struct Voxel;

// Voxel Declarations
struct Voxel
{
    // Voxel Public Methods
    uint32_t size() const { return primitives.size(); }
    Voxel() { allCanIntersect = true; }
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
    std::vector< Shape* > primitives;
    bool allCanIntersect;
};



class GridAccelerator : public Accelerator
{

public:
    GridAccelerator(std::vector<Shape*>* _initialMesh = NULL, BBox _globalBox = BBox());

    virtual ~GridAccelerator()
    {
        for (int i = 0; i < nVoxels[0]*nVoxels[1]*nVoxels[2]; ++i)
            if (voxels[i]) { voxels[i]->~Voxel(); }
        free(voxels);
    }

    virtual bool build();

    //bool getIntersection(Ray &ray,Intersection &inter);
    virtual decimal traverse(Ray* r, std::list<Intersection> &result);

    void setMaxProfondeur(int _maxProfondeur) { maxProfondeur = _maxProfondeur; }
    int getMaxProfondeur() { return maxProfondeur; }

    void setMaxPrimPerLeaf(int _maxPrimPerLeaf) { maxPrimPerLeaf = _maxPrimPerLeaf; }
    int getMaxPrimPerLeaf() { return maxPrimPerLeaf; }

    void print();

    //void display(unsigned int indexNode);

    bool alreadyFail;
    unsigned int nbFail;
    Ray r;
    bool trace;

protected:

    std::vector<Shape*> primitives; //Different de initialMesh, est reordonne par rapport a la structure
    BBox globalBox;

    int maxProfondeur;
    int maxPrimPerLeaf;

    int realMaxProfondeur;

    float isectCost;
    float emptyBonus;
    float traversalCost;

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
    //MemoryArena voxelArena;
    //mutable RWMutex *rwMutex;
};


#endif