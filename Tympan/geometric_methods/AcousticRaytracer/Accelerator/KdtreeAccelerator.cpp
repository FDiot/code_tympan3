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

#include "KdtreeAccelerator.h"
#include <algorithm>
#include <sstream>
#include <limits>

/**
 * Leaf initialization
 * @param _nbPrims    Number of primitives
 * @param _firstIndex First index of the primitive
 * @param _prims      Array containing the primitives indexes
 */
void KDNode::createLeaf(unsigned int _nbPrims, unsigned int _firstIndex, unsigned int* _prims)
{
    flag = 3;
    nbPrims |= _nbPrims << 2;
    if (_nbPrims < 2)
    {
        firstIndex = _firstIndex;
    }
    else
    {
        prims = _prims;
    }
}

void KDNode::createNode(int axis, float _split, unsigned int nextChild)
{
    flag = axis;
    split = _split;
    secondChild |= nextChild << 2;
}

KdtreeAccelerator::KdtreeAccelerator(std::vector<Shape*>* _initialMesh,  BBox _globalBox) : Accelerator(_initialMesh, _globalBox)
{
    initialMesh = _initialMesh;
    //globalBox = BBox(_globalBox);

    maxProfondeur = 16;
    maxPrimPerLeaf = 2;

    realMaxProfondeur = 0;

    alreadyFail = false;
    nbFail = 0;
    trace = false;

    isectCost = 4.f;
    emptyBonus = 0.5f;
    traversalCost = 1.f;
}

KdtreeAccelerator::~KdtreeAccelerator()
{

}

bool KdtreeAccelerator::build()
{
    if (initialMesh == NULL || initialMesh->empty())
    {
        return false;
    }

    unsigned int* prims = (unsigned int*)malloc(initialMesh->size() * sizeof(unsigned int));
    for (unsigned int i = 0; i < initialMesh->size(); i++)
    {
        InfoPrim newPrim;
        newPrim.box = BBox(initialMesh->at(i)->getBBox());
        newPrim.indexPrim = i;
        tablePrimitive.push_back(newPrim);
        prims[i] = i;
    }

    // Allocate working memory for kd-tree construction
    TaBRecBoundEdge* edges[3];
    for (int i = 0; i < 3; ++i)
    {
        edges[i] = new TaBRecBoundEdge[2 * initialMesh->size()];
    }

    generateSAHKdTree(0, globalBox, edges, initialMesh->size(), prims);

    for (int i = 0; i < 3; ++i)
    {
        delete[] edges[i];
    }

    return true;
}

void KdtreeAccelerator::generateMidKdTree(int currentProfondeur, BBox& localBox, unsigned int nbPrims, unsigned int* prims)
{

    if (currentProfondeur >= maxProfondeur || (int)(nbPrims) <= maxPrimPerLeaf)
    {
        KDNode node;
        unsigned int* newPrims = (unsigned int*)malloc(nbPrims * sizeof(unsigned int));
        for (unsigned int i = 0; i < nbPrims; i++)
        {
            newPrims[i] = prims[i];
        }
        node.createLeaf(nbPrims, newPrims[0], newPrims);
        tableNode.push_back(node);
        tableBox.push_back(localBox);
        if (currentProfondeur > realMaxProfondeur)
        {
            realMaxProfondeur = currentProfondeur;
        }
        free(prims);
        return;
    }

    unsigned int n0(0), n1(0);
    unsigned int* prims0, *prims1;
    prims0 = (unsigned int*)malloc(nbPrims * sizeof(unsigned int));
    prims1 = (unsigned int*)malloc(nbPrims * sizeof(unsigned int));

    //Coupe au milieu, on selectionne la plus grande dimension
    int axis = localBox.MaximumExtend();
    float split = (localBox.pMin[axis] + localBox.pMax[axis]) / 2.f;

    for (unsigned int i = 0; i < nbPrims; i++)
    {
        Shape* shape = initialMesh->at(prims[i]);
        if (shape->getBBox().pMax[axis] < split)
        {
            prims0[n0] = prims[i];
            n0++;
        }
        else if (shape->getBBox().pMin[axis] > split)
        {
            prims1[n1] = prims[i];
            n1++;
        }
        else
        {
            prims0[n0] = prims[i];
            n0++;
            prims1[n1] = prims[i];
            n1++;
        }
    }

    free(prims);

    KDNode currentNode;
    unsigned int indexCurrentNode = tableNode.size();
    tableNode.push_back(currentNode);
    tableBox.push_back(localBox);

    BBox firstBox = BBox(localBox);
    firstBox.pMax[axis] = split;
    generateMidKdTree(currentProfondeur + 1, firstBox, n0, prims0);

    tableNode.at(indexCurrentNode).createNode(axis, split, tableNode.size());

    BBox secondBox = BBox(localBox);
    secondBox.pMin[axis] = split;
    generateMidKdTree(currentProfondeur + 1, secondBox, n1, prims1);
}

decimal KdtreeAccelerator::traverse(Ray* r, std::list<Intersection> &result) const
{

    decimal tmin, tmax, intermin = -1.;
    if (!globalBox.IntersectP(r->position, r->direction, &tmin, &tmax))
    {
        return -1.;
    }

    stringstream logs;

    // Prepare to traverse kd-tree for ray
    vec3 invDir(1.f / r->direction.x, 1.f / r->direction.y, 1.f / r->direction.z);
#define MAX_TODO 64
    KdToDo todo[MAX_TODO];
    int todoPos = 0;

    // Traverse kd-tree nodes in order for ray
    KDNode* node = const_cast<KDNode*>( &tableNode[0] );
    while (node != NULL)
    {
        // Bail out if we found a hit closer than the current node
        if (r->maxt < tmin) { break; }
        if (!node->isLeaf())
        {
            // Process kd-tree interior node
            // Compute parametric distance along ray to split plane
            int axis = node->getAxe();
            float tplane = (node->getAxeValue() - r->position[axis]) * invDir[axis];

            // Get node children pointers for ray
            KDNode* firstChild, *secondChild;
            int belowFirst = (r->position[axis] <  node->getAxeValue()) ||
                             (r->position[axis] == node->getAxeValue() && r->direction[axis] >= 0);
            if (belowFirst)
            {
                firstChild = node + 1;
                secondChild = const_cast<KDNode*>( &tableNode[node->AboveChild()] );
            }
            else
            {
                firstChild = const_cast<KDNode*>( &tableNode[node->AboveChild()] );
                secondChild = node + 1;
            }

            // Advance to next child node, possibly enqueue other child
            if (tplane > tmax || tplane < 0)
            {
                node = firstChild;
            }
            else if (tplane < tmin)
            {
                node = secondChild;
            }
            else
            {
                // Enqueue _secondChild_ in todo list
                todo[todoPos].node = secondChild;
                todo[todoPos].tmin = tplane;
                todo[todoPos].tmax = tmax;
                ++todoPos;
                node = firstChild;
                tmax = tplane;
            }
        }
        else
        {
            // Check for intersections inside leaf node
            unsigned int nPrimitives = node->getNbPrimitives();
            if (nPrimitives == 1)
            {
                Shape* prim = initialMesh->at(node->getFirstIndex());
                // Check one primitive inside leaf node

                Intersection currentIntersection;
                if (prim->getIntersection(*r, currentIntersection) && currentIntersection.t > 0.0001)
                {
                    result.push_back(currentIntersection);
//                    intermin = leafTreatment::keepFunction(intersectionChoice, result, intermin);
                    intermin = (*pLeafTreatmentFunction) (result, intermin);
                }
            }
            else if (nPrimitives > 1)
            {
                unsigned int* prims = node->getPrims();
                for (unsigned int i = 0; i < nPrimitives; i++)
                {
                    Shape* prim = initialMesh->at(prims[i]);

                    // Check one primitive inside leaf node

                    Intersection currentIntersection;
                    if (prim->getIntersection(*r, currentIntersection) && currentIntersection.t > 0.0001)
                    {
                        result.push_back(currentIntersection);
                        //intermin = leafTreatment::keepFunction(intersectionChoice, result, intermin);
                        intermin = (*pLeafTreatmentFunction) (result, intermin);
                    }
                }
            }

            // Grab next node to process from todo list
            if (todoPos > 0)
            {
                --todoPos;
                node = todo[todoPos].node;
                tmin = todo[todoPos].tmin;
                tmax = todo[todoPos].tmax;
            }
            else
            {
                break;
            }
        }
    }

    return intermin;
}

void KdtreeAccelerator::print()
{
    for (unsigned int i = 0; i < tableNode.size(); i++)
    {
        if (tableNode.at(i).isLeaf())
        {
            if (tableNode.at(i).getNbPrimitives() == 1)
            {
            }
            else
            {
            }
        }
        else
        {
        }
    }
}


void KdtreeAccelerator::generateSAHKdTree(int currentProfondeur, BBox& localBox, TaBRecBoundEdge* edges[3], unsigned int nbPrims, unsigned int* prims)
{
    if (currentProfondeur >= maxProfondeur || (int)(nbPrims) <= maxPrimPerLeaf)
    {
        KDNode node;
        unsigned int* newPrims = (unsigned int*)malloc(nbPrims * sizeof(unsigned int));
        for (unsigned int i = 0; i < nbPrims; i++)
        {
            newPrims[i] = prims[i];
        }
        node.createLeaf(nbPrims, newPrims[0], newPrims);
        tableNode.push_back(node);
        tableBox.push_back(localBox);
        if (currentProfondeur > realMaxProfondeur)
        {
            realMaxProfondeur = currentProfondeur;
        }
        free(prims);
        return;
    }

    unsigned int n0(0), n1(0);
    unsigned int* prims0, *prims1;
    prims0 = (unsigned int*)malloc(nbPrims * sizeof(unsigned int));
    prims1 = (unsigned int*)malloc(nbPrims * sizeof(unsigned int));

    // Initialize interior node and continue recursion
    // Choose split axis position for interior node
    int bestAxis = -1, bestOffset = -1;
    float bestCost = std::numeric_limits<float>::infinity();//INFINITY;
    vec3 d = localBox.pMax - localBox.pMin;
    float totalSA = (2.f * (d.x * d.y + d.x * d.z + d.y * d.z));
    float invTotalSA = 1.f / totalSA;
    // Choose which axis to split along
    int axis;
    if (d.x > d.y && d.x > d.z) { axis = 0; }
    else { axis = (d.y > d.z) ? 1 : 2; }
    int retries = 0;

retrySplit:
    // Initialize edges for _axis_
    for (int i = 0; i < static_cast<int>(nbPrims); ++i)
    {
        int pn = prims[i];
        const BBox& bbox = initialMesh->at(pn)->getBBox();
        edges[axis][2 * i] =
            TaBRecBoundEdge(bbox.pMin[axis], pn, true);
        edges[axis][2 * i + 1] =
            TaBRecBoundEdge(bbox.pMax[axis], pn, false);
    }
    std::sort(&edges[axis][0], &edges[axis][2 * nbPrims]);
    // Compute cost of all splits for _axis_ to find best
    int nBelow = 0, nAbove = nbPrims;
    for (int i = 0; i < static_cast<int>(2 * nbPrims); ++i)
    {
        if (edges[axis][i].type == TaBRecBoundEdge::END) { --nAbove; }
        float edget = edges[axis][i].t;
        if (edget > localBox.pMin[axis] &&
            edget < localBox.pMax[axis])
        {
            // Compute cost for split at _i_th edge
            int otherAxis[3][2] = { {1, 2}, {0, 2}, {0, 1} };
            int otherAxis0 = otherAxis[axis][0];
            int otherAxis1 = otherAxis[axis][1];
            float belowSA = 2 * (d[otherAxis0] * d[otherAxis1] +
                                 (edget - localBox.pMin[axis]) *
                                 (d[otherAxis0] + d[otherAxis1]));
            float aboveSA = 2 * (d[otherAxis0] * d[otherAxis1] +
                                 (localBox.pMax[axis] - edget) *
                                 (d[otherAxis0] + d[otherAxis1]));
            float pBelow = belowSA * invTotalSA;
            float pAbove = aboveSA * invTotalSA;
            float eb = (nAbove == 0 || nBelow == 0) ? emptyBonus : 0.f;
            float cost = traversalCost + isectCost * (1.f - eb) *
                         (pBelow * nBelow + pAbove * nAbove);
            // Update best split if this is lowest cost so far
            if (cost < bestCost)
            {
                bestCost = cost;
                bestAxis = axis;
                bestOffset = i;
            }
        }
        if (edges[axis][i].type == TaBRecBoundEdge::START) { ++nBelow; }
    }

	// Create leaf if no good splits were found
    if (bestAxis == -1 && retries < 2)
    {
        ++retries;
        axis = (axis + 1) % 3;
        goto retrySplit;
    }

    float split = edges[bestAxis][bestOffset].t;

    for (unsigned int i = 0; i < nbPrims; i++)
    {
        Shape* triangle = initialMesh->at(prims[i]);
        if (triangle->getBBox().pMax[axis] < split)
        {
            prims0[n0] = prims[i];
            n0++;
        }
        else if (triangle->getBBox().pMin[axis] > split)
        {
            prims1[n1] = prims[i];
            n1++;
        }
        else
        {
            prims0[n0] = prims[i];
            n0++;
            prims1[n1] = prims[i];
            n1++;
        }
    }

    free(prims);

    KDNode currentNode;
    unsigned int indexCurrentNode = tableNode.size();
    tableNode.push_back(currentNode);
    tableBox.push_back(localBox);

    BBox firstBox = BBox(localBox);
    firstBox.pMax[axis] = split;
    generateMidKdTree(currentProfondeur + 1, firstBox, n0, prims0);

    tableNode.at(indexCurrentNode).createNode(axis, split, tableNode.size());

    BBox secondBox = BBox(localBox);
    secondBox.pMin[axis] = split;
    generateMidKdTree(currentProfondeur + 1, secondBox, n1, prims1);
}
