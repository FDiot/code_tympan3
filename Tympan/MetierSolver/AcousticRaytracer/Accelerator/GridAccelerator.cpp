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
 
#include "GridAccelerator.h"

GridAccelerator::GridAccelerator(std::vector<Shape*>* _initialMesh, BBox _globalBox) : Accelerator(_initialMesh, _globalBox)
{
    for (unsigned int i = 0; i < _initialMesh->size(); i++)
    {
        primitives.push_back(_initialMesh->at(i));
    }
}

bool GridAccelerator::build()
{
    // Compute bounds and choose grid resolution
    for (uint32_t i = 0; i < primitives.size(); ++i)
    {
        bounds = bounds.Union(primitives[i]->getBBox());
    }
    vec3 delta = bounds.pMax - bounds.pMin;

    // Find _voxelsPerUnitDist_ for grid
    int maxAxis = bounds.MaximumExtend();
    float invMaxWidth = 1.f / delta[maxAxis];
    //Assert(invMaxWidth > 0.f);
    float cubeRoot = 3.f * powf(float(primitives.size()), 1.f / 3.f);
    float voxelsPerUnitDist = cubeRoot * invMaxWidth;
    for (int axis = 0; axis < 3; ++axis)
    {
        nVoxels[axis] = Round2Int(delta[axis] * voxelsPerUnitDist);
        nVoxels[axis] = Clamp(nVoxels[axis], 1, 64);
    }

    // Compute voxel widths and allocate voxels
    for (int axis = 0; axis < 3; ++axis)
    {
        width[axis] = delta[axis] / nVoxels[axis];
        invWidth[axis] = (width[axis] == 0.f) ? 0.f : 1.f / width[axis];
    }
    int nv = nVoxels[0] * nVoxels[1] * nVoxels[2];
    voxels = (Voxel**)malloc(nv * sizeof(Voxel*));
    memset(voxels, 0, nv * sizeof(Voxel*));

    // Add primitives to grid voxels
    for (uint32_t i = 0; i < primitives.size(); ++i)
    {
        // Find voxel extent of primitive
        BBox pb = primitives[i]->getBBox();
        int vmin[3], vmax[3];
        for (int axis = 0; axis < 3; ++axis)
        {
            vmin[axis] = posToVoxel(pb.pMin, axis);
            vmax[axis] = posToVoxel(pb.pMax, axis);
        }

        // Add primitive to overlapping voxels
        for (int z = vmin[2]; z <= vmax[2]; ++z)
            for (int y = vmin[1]; y <= vmax[1]; ++y)
                for (int x = vmin[0]; x <= vmax[0]; ++x)
                {
                    int o = offset(x, y, z);
                    if (!voxels[o])
                    {
                        // Allocate new voxel and store primitive in it
                        //voxels[o] = (Voxel*)malloc(sizeof(Voxel));
                        //(*voxels[o]) = Voxel(primitives.at(i));
                        voxels[o] = new Voxel(primitives.at(i));
                    }
                    else
                    {
                        // Add primitive to already-allocated voxel
                        voxels[o]->AddPrimitive(primitives.at(i));
                    }
                }
    }

    return true;
}

decimal GridAccelerator::traverse(Ray* r, std::list<Intersection> &result)
{
    decimal rayT;
    decimal intermin = -1.;
    vec3 grid = r->position + r->direction * r->mint;
    if (bounds.Inside(grid))
    {
        rayT = r->mint;
    }
    else if (!bounds.IntersectP(r->position, r->direction, &rayT))
    {
        //PBRT_GRID_RAY_MISSED_BOUNDS();
        return false;
    }
    vec3 gridIntersect = r->position + r->direction * rayT;

    // Set up 3D DDA for ray
    float NextCrossingT[3], DeltaT[3];
    int Step[3], Out[3], Pos[3];
    for (int axis = 0; axis < 3; ++axis)
    {
        // Compute current voxel for axis
        Pos[axis] = posToVoxel(gridIntersect, axis);
        if (r->direction[axis] >= 0)
        {
            // Handle ray with positive direction for voxel stepping
            NextCrossingT[axis] = rayT +
                                  (voxelToPos(Pos[axis] + 1, axis) - gridIntersect[axis]) / r->direction[axis];
            DeltaT[axis] = width[axis] / r->direction[axis];
            Step[axis] = 1;
            Out[axis] = nVoxels[axis];
        }
        else
        {
            // Handle ray with negative direction for voxel stepping
            NextCrossingT[axis] = rayT +
                                  (voxelToPos(Pos[axis], axis) - gridIntersect[axis]) / r->direction[axis];
            DeltaT[axis] = -width[axis] / r->direction[axis];
            Step[axis] = -1;
            Out[axis] = -1;
        }
    }

    // Walk ray through voxel grid
    bool hitSomething = false;
    for (;;)
    {
        // Check for intersection in current voxel and advance to next
        Voxel* voxel = voxels[offset(Pos[0], Pos[1], Pos[2])];
        if (voxel != NULL)
        {
            hitSomething |= voxel->Intersect(r, result, intermin, intersectionChoice);
        }

        // Advance to next voxel

        // Find _stepAxis_ for stepping to next voxel
        int bits = ((NextCrossingT[0] < NextCrossingT[1]) << 2) +
                   ((NextCrossingT[0] < NextCrossingT[2]) << 1) +
                   ((NextCrossingT[1] < NextCrossingT[2]));
        const int cmpToAxis[8] = { 2, 1, 2, 1, 2, 2, 0, 0 };
        int stepAxis = cmpToAxis[bits];
        if (r->maxt < NextCrossingT[stepAxis])
        {
            break;
        }
        Pos[stepAxis] += Step[stepAxis];
        if (Pos[stepAxis] == Out[stepAxis])
        {
            break;
        }
        NextCrossingT[stepAxis] += DeltaT[stepAxis];
    }
    return intermin;
}

bool Voxel::Intersect(Ray* ray, std::list<Intersection> &result, decimal& intermin, leafTreatment::treatment choice)
{

    // Loop over primitives in voxel and find intersections
    bool hitSomething = false;

    for (uint32_t i = 0; i < primitives.size(); ++i)
    {
        Shape* prim = primitives[i];
        Intersection currentIntersection;
        if (prim->getIntersection(*ray, currentIntersection) && currentIntersection.t > 0.0001)
        {
            hitSomething = true;
            result.push_back(currentIntersection);

            intermin = leafTreatment::keepFunction(choice, result, intermin);
        }
    }
    return hitSomething;
}
