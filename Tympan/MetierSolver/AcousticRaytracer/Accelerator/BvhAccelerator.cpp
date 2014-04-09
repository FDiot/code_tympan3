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

#include "BvhAccelerator.h"
#include <algorithm>
#include <stdint.h>

// BVHAccel Local Declarations
struct BVHPrimitiveInfo
{
    BVHPrimitiveInfo() { }
    BVHPrimitiveInfo(int pn, const BBox& b)
        : primitiveNumber(pn), bounds(b)
    {
        centroid =  b.pMin * 0.5  + b.pMax * .5f;
    }
    int primitiveNumber;
    vec3 centroid;
    BBox bounds;
};


struct BVHBuildNode
{
    // BVHBuildNode Public Methods
    BVHBuildNode() { children[0] = children[1] = NULL; }
    void InitLeaf(unsigned int  first, unsigned int n, const BBox& b)
    {
        firstPrimOffset = first;
        nPrimitives = n;
        bounds = b;
    }
    void InitInterior(unsigned int axis, BVHBuildNode* c0, BVHBuildNode* c1)
    {
        children[0] = c0;
        children[1] = c1;
        bounds = c0->bounds.Union(c1->bounds);
        splitAxis = axis;
        nPrimitives = 0;
    }
    BBox bounds;
    BVHBuildNode* children[2];
    unsigned int splitAxis, firstPrimOffset, nPrimitives;
};


struct CompareToMid
{
    CompareToMid(int d, decimal m) { dim = d; mid = m; }
    int dim;
    decimal mid;
    bool operator()(const BVHPrimitiveInfo& a) const
    {
        return a.centroid[dim] < mid;
    }
};

struct ComparePoints
{
    ComparePoints(int d) { dim = d; }
    int dim;
    bool operator()(const BVHPrimitiveInfo& a,
                    const BVHPrimitiveInfo& b) const
    {
        return a.centroid[dim] < b.centroid[dim];
    }
};


struct CompareToBucket
{
    CompareToBucket(int split, int num, int d, const BBox& b)
        : centroidBounds(b)
    { splitBucket = split; nBuckets = num; dim = d; }
    bool operator()(const BVHPrimitiveInfo& p) const;

    int splitBucket, nBuckets, dim;
    const BBox& centroidBounds;
};


bool CompareToBucket::operator()(const BVHPrimitiveInfo& p) const
{
    int b = nBuckets * ((p.centroid[dim] - centroidBounds.pMin[dim]) /
                        (centroidBounds.pMax[dim] - centroidBounds.pMin[dim]));
    if (b == nBuckets) { b = nBuckets - 1; }

	return b <= splitBucket;
}

struct LinearBVHNode
{
    BBox bounds;
    union
    {
        uint32_t primitivesOffset;    // leaf
        uint32_t secondChildOffset;   // interior
    };

    uint8_t nPrimitives;  // 0 -> interior node
    uint8_t axis;         // interior node: xyz
    uint8_t pad[2];       // ensure 32 byte total size
};

static inline bool IntersectP(const BBox& bounds, const Ray& ray,
                              const vec3& invDir, const uint32_t dirIsNeg[3])
{
    // Check for ray intersection against $x$ and $y$ slabs
    float tmin = (bounds[  dirIsNeg[0]].x - ray.position.x) * invDir.x;
    float tmax = (bounds[1 - dirIsNeg[0]].x - ray.position.x) * invDir.x;
    float tymin = (bounds[  dirIsNeg[1]].y - ray.position.y) * invDir.y;
    float tymax = (bounds[1 - dirIsNeg[1]].y - ray.position.y) * invDir.y;
    if ((tmin > tymax) || (tymin > tmax))
    {
        return false;
    }
    if (tymin > tmin) { tmin = tymin; }
    if (tymax < tmax) { tmax = tymax; }

    // Check for ray intersection against $z$ slab
    float tzmin = (bounds[  dirIsNeg[2]].z - ray.position.z) * invDir.z;
    float tzmax = (bounds[1 - dirIsNeg[2]].z - ray.position.z) * invDir.z;
    if ((tmin > tzmax) || (tzmin > tmax))
    {
        return false;
    }
    if (tzmin > tmin)
    {
        tmin = tzmin;
    }
    if (tzmax < tmax)
    {
        tmax = tzmax;
    }
    return (tmin < ray.maxt) && (tmax > ray.mint);
}

// BVHAccel Method Definitions
BvhAccelerator::BvhAccelerator(std::vector<Shape*>* _initialMesh,
                               BBox _globalBox, unsigned int maxProf, const string& sm) : Accelerator(_initialMesh, _globalBox)
{
    maxPrimsInNode = min(255u, maxProf);

    if (sm == "sah") { splitMethod = SPLIT_SAH; }
    else if (sm == "middle") { splitMethod = SPLIT_MIDDLE; }
    else if (sm == "equal") { splitMethod = SPLIT_EQUAL_COUNTS; }
    else
    {
        splitMethod = SPLIT_SAH;
    }

    for (unsigned int i = 0; i < _initialMesh->size(); i++)
    {
        primitives.push_back(_initialMesh->at(i));
    }

}

BVHBuildNode* BvhAccelerator::recursiveBuild(std::vector<BVHPrimitiveInfo> &buildData, uint32_t start,
                                             uint32_t end, uint32_t* totalNodes,
                                             std::vector< Shape* > &orderedPrims)
{

    (*totalNodes)++;
    BVHBuildNode* node = new BVHBuildNode();
    // Compute bounds of all primitives in BVH node
    BBox bbox;
    for (uint32_t i = start; i < end; ++i)
    {
        bbox = bbox.Union(buildData[i].bounds);
    }
    uint32_t nPrimitives = end - start;
    if (nPrimitives == 1)
    {
        // Create leaf _BVHBuildNode_
        uint32_t firstPrimOffset = orderedPrims.size();
        for (uint32_t i = start; i < end; ++i)
        {
            uint32_t primNum = buildData[i].primitiveNumber;
            orderedPrims.push_back(primitives.at(primNum));
        }
        node->InitLeaf(firstPrimOffset, nPrimitives, bbox);
    }
    else
    {
        // Compute bound of primitive centroids, choose split dimension _dim_
        BBox centroidBounds;
        for (uint32_t i = start; i < end; ++i)
        {
            centroidBounds = centroidBounds.Union(buildData[i].centroid);
        }
        int dim = centroidBounds.MaximumExtend();

        // Partition primitives into two sets and build children
        uint32_t mid = (start + end) / 2;

		if (centroidBounds.pMax[dim] == centroidBounds.pMin[dim])
        {
            // Create leaf _BVHBuildNode_
            uint32_t firstPrimOffset = orderedPrims.size();
            for (uint32_t i = start; i < end; ++i)
            {
                uint32_t primNum = buildData[i].primitiveNumber;
                orderedPrims.push_back(primitives.at(primNum));
            }
            node->InitLeaf(firstPrimOffset, nPrimitives, bbox);
            return node;
        }

        // Partition primitives based on _splitMethod_
        switch (splitMethod)
        {
            case SPLIT_MIDDLE:
            {
                // Partition primitives through node's midpoint
                float pmid = .5f * (centroidBounds.pMin[dim] + centroidBounds.pMax[dim]);
                BVHPrimitiveInfo* midPtr = std::partition(&buildData[start],
                                                          &buildData[end - 1] + 1,
                                                          CompareToMid(dim, pmid));
                mid = midPtr - &buildData[0];
                if (mid != start && mid != end)
                    // for lots of prims with large overlapping bounding boxes, this
                    // may fail to partition; in that case don't break and fall through
                    // to SPLIT_EQUAL_COUNTS
                {
                    break;
                }
            }
            case SPLIT_EQUAL_COUNTS:
            {
                // Partition primitives into equally-sized subsets
                mid = (start + end) / 2;
                std::nth_element(&buildData[start], &buildData[mid],
                                 &buildData[end - 1] + 1, ComparePoints(dim));
                break;
            }
        case SPLIT_SAH: default:
            {
                // Partition primitives using approximate SAH
                if (nPrimitives <= 4)
                {
                    // Partition primitives into equally-sized subsets
                    mid = (start + end) / 2;
                    std::nth_element(&buildData[start], &buildData[mid],
                                     &buildData[end - 1] + 1, ComparePoints(dim));
                }
                else
                {
                    // Allocate _BucketInfo_ for SAH partition buckets
                    const int nBuckets = 12;
                    struct BucketInfo
                    {
                        BucketInfo() { count = 0; }
                        int count;
                        BBox bounds;
                    };
                    BucketInfo buckets[nBuckets];

                    // Initialize _BucketInfo_ for SAH partition buckets
                    for (uint32_t i = start; i < end; ++i)
                    {
                        int b = nBuckets *
                                ((buildData[i].centroid[dim] - centroidBounds.pMin[dim]) /
                                 (centroidBounds.pMax[dim] - centroidBounds.pMin[dim]));
                        if (b == nBuckets) { b = nBuckets - 1; }
                        buckets[b].count++;
                        buckets[b].bounds = buckets[b].bounds.Union(buildData[i].bounds);
                    }

                    // Compute costs for splitting after each bucket
                    float cost[nBuckets - 1];
                    for (int i = 0; i < nBuckets - 1; ++i)
                    {
                        BBox b0, b1;
                        int count0 = 0, count1 = 0;
                        for (int j = 0; j <= i; ++j)
                        {
                            b0 = b0.Union(buckets[j].bounds);
                            count0 += buckets[j].count;
                        }
                        for (int j = i + 1; j < nBuckets; ++j)
                        {
                            b1 = b1.Union(buckets[j].bounds);
                            count1 += buckets[j].count;
                        }
                        cost[i] = .125f + (count0 * b0.SurfaceArea() + count1 * b1.SurfaceArea()) /
                                  bbox.SurfaceArea();
                    }

                    // Find bucket to split at that minimizes SAH metric
                    float minCost = cost[0];
                    uint32_t minCostSplit = 0;
                    for (int i = 1; i < nBuckets - 1; ++i)
                    {
                        if (cost[i] < minCost)
                        {
                            minCost = cost[i];
                            minCostSplit = i;
                        }
                    }

                    // Either create leaf or split primitives at selected SAH bucket
                    if (nPrimitives > maxPrimsInNode ||
                        minCost < nPrimitives)
                    {
                        BVHPrimitiveInfo* pmid = std::partition(&buildData[start],
                                                                &buildData[end - 1] + 1,
                                                                CompareToBucket(minCostSplit, nBuckets, dim, centroidBounds));
                        mid = pmid - &buildData[0];
                    }

                    else
                    {
                        // Create leaf _BVHBuildNode_
                        uint32_t firstPrimOffset = orderedPrims.size();
                        for (uint32_t i = start; i < end; ++i)
                        {
                            uint32_t primNum = buildData[i].primitiveNumber;
                            orderedPrims.push_back(primitives.at(primNum));
                        }
                        node->InitLeaf(firstPrimOffset, nPrimitives, bbox);
                        return node;
                    }
                }
                break;
            }
        }
        node->InitInterior(dim,
                           recursiveBuild(buildData, start, mid,
                                          totalNodes, orderedPrims),
                           recursiveBuild(buildData, mid, end,
                                          totalNodes, orderedPrims));
    }
    return node;
}

uint32_t BvhAccelerator::flattenBVHTree(BVHBuildNode* node, uint32_t* offset)
{
    LinearBVHNode* linearNode = &nodes[*offset];
    linearNode->bounds = node->bounds;
    uint32_t myOffset = (*offset)++;
    if (node->nPrimitives > 0)
    {
        linearNode->primitivesOffset = node->firstPrimOffset;
        linearNode->nPrimitives = node->nPrimitives;
    }
    else
    {
        // Creater interior flattened BVH node
        linearNode->axis = node->splitAxis;
        linearNode->nPrimitives = 0;
        flattenBVHTree(node->children[0], offset);
        linearNode->secondChildOffset = flattenBVHTree(node->children[1],
                                                       offset);
    }
    return myOffset;
}

decimal BvhAccelerator::traverse(Ray* ray, std::list<Intersection> &result)
{
    if (!nodes) { return -1.; }

    bool hit = false;
    decimal intermin = -1.;
    vec3 origin = ray->position + ray->direction * ray->mint;
    vec3 invDir(1.f / ray->direction.x, 1.f / ray->direction.y, 1.f / ray->direction.z);
    uint32_t dirIsNeg[3] = { invDir.x < 0, invDir.y < 0, invDir.z < 0 };
    // Follow ray through BVH nodes to find primitive intersections
    uint32_t todoOffset = 0, nodeNum = 0;
    uint32_t todo[64];
    while (true)
    {
        const LinearBVHNode* node = &nodes[nodeNum];
        // Check ray against BVH node
        if (::IntersectP(node->bounds, ray, invDir, dirIsNeg))
        {
            if (node->nPrimitives > 0)
            {
                // Intersect ray with primitives in leaf BVH node
                //PBRT_BVH_INTERSECTION_TRAVERSED_LEAF_NODE(const_cast<LinearBVHNode *>(node));

                Intersection currentIntersection;
                for (uint32_t i = 0; i < node->nPrimitives; ++i)
                {
                    //PBRT_BVH_INTERSECTION_PRIMITIVE_TEST(const_cast<Primitive *>(primitives[node->primitivesOffset+i].GetPtr()));
                    if (primitives.at(node->primitivesOffset + i)->getIntersection(*ray, currentIntersection) && currentIntersection.t > 0.0001)
                    {
                        //PBRT_BVH_INTERSECTION_PRIMITIVE_HIT(const_cast<Primitive *>(primitives[node->primitivesOffset+i].GetPtr()));
                        hit = true;
                        result.push_back(currentIntersection);

                        //intermin = leafTreatment::keepFunction(intersectionChoice, result, intermin);
                        intermin = (*pLeafTreatmentFunction) (result, intermin);
                    }
                    else
                    {
                        //PBRT_BVH_INTERSECTION_PRIMITIVE_MISSED(const_cast<Primitive *>(primitives[node->primitivesOffset+i].GetPtr()));
                    }
                }
                if (todoOffset == 0) { break; }
                nodeNum = todo[--todoOffset];
            }
            else
            {
                // Put far BVH node on _todo_ stack, advance to near node
                //PBRT_BVH_INTERSECTION_TRAVERSED_INTERIOR_NODE(const_cast<LinearBVHNode *>(node));
                if (dirIsNeg[node->axis])
                {
                    todo[todoOffset++] = nodeNum + 1;
                    nodeNum = node->secondChildOffset;
                }
                else
                {
                    todo[todoOffset++] = node->secondChildOffset;
                    nodeNum = nodeNum + 1;
                }
            }
        }
        else
        {
            if (todoOffset == 0) { break; }
            nodeNum = todo[--todoOffset];
        }
    }
    //PBRT_BVH_INTERSECTION_FINISHED();
    return intermin;
}

bool BvhAccelerator::build()
{
    if (shapes->size() == 0)
    {
        nodes = NULL;
        return false;
    }

    // Initialize _buildData_ array for primitives
    std::vector<BVHPrimitiveInfo> buildData;
    buildData.reserve(shapes->size());
    for (uint32_t i = 0; i < shapes->size(); ++i)
    {
        BBox bbox = shapes->at(i)->getBBox();
        buildData.push_back(BVHPrimitiveInfo(i, bbox));
    }

    // Recursively build BVH tree for primitives
    //MemoryArena buildArena;
    uint32_t totalNodes = 0;
    std::vector<Shape* > orderedPrims;
    orderedPrims.reserve(shapes->size());
    BVHBuildNode* root = recursiveBuild(buildData, 0,
                                        primitives.size(), &totalNodes,
                                        orderedPrims);
    primitives.swap(orderedPrims);

    // Compute representation of depth-first traversal of BVH tree
    nodes = (LinearBVHNode*)malloc(totalNodes * sizeof(LinearBVHNode));
    for (uint32_t i = 0; i < totalNodes; ++i)
    {
        new(&nodes[i]) LinearBVHNode;
    }
    uint32_t offset = 0;
    flattenBVHTree(root, &offset);

    return true;
}
