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

bool compareToXAxis(InfoPrim p1, InfoPrim p2)
{
    return p1.box.centroid[0] < p2.box.centroid[0];
}

bool compareToYAxis(InfoPrim p1, InfoPrim p2)
{
    return p1.box.centroid[1] < p2.box.centroid[1];
}

bool compareToZAxis(InfoPrim p1, InfoPrim p2)
{
    return p1.box.centroid[2] < p2.box.centroid[2];
}

KdtreeAccelerator::KdtreeAccelerator(std::vector<Shape*>* _initialMesh,  BBox _globalBox)
{
    initialMesh = _initialMesh;
    globalBox = BBox(_globalBox);

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

    //std::cout << "Lancement de la generation de l'arbre..." << endl;
    //generateMidKdTree(0,globalBox,initialMesh->size(),prims);
    //std::cout << "Fin de la generation de l'arbre." << endl;
    generateSAHKdTree(0, globalBox, edges, initialMesh->size(), prims);

    //for(unsigned int i = 0; i < tableBox.size(); i++)
    //  tableBox.at(i).computePoints();

    //std::cout << "Construction du KdtreeAccelerator terminee." << std::endl;
    //std::cout << "L'arbre comporte " << tableNode.size() << " noeuds." << std::endl;
    //std::cout << "Le KdtreeAccelerator a atteint la profondeur " << realMaxProfondeur << std::endl;

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
        //std::cout<<"Construction d'une feuille avec "<<nbPrims<<" primitives"<<std::endl;
        //std::cout<<"Boite englobante globale : ";
        //globalBox.print();
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

    //std::cout<<"Construction d'un noeud."<<std::endl;
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

    //unsigned int indexFirstFalse = (unsigned int)(firstFalse - tablePrimitive.begin());
    //std::cout<<"La partition a trouve "<<indexFirstFalse<<" primitives inferieur au milieu."<<std::endl;

    KDNode currentNode;
    unsigned int indexCurrentNode = tableNode.size();
    tableNode.push_back(currentNode);
    tableBox.push_back(localBox);

    BBox firstBox = BBox(localBox);
    firstBox.pMax[axis] = split;
    generateMidKdTree(currentProfondeur + 1, firstBox, n0, prims0);
    //free(prims0);

    //std::cout<<"Avant modification, le noeud "<<indexCurrentNode<<" a pour second fils "<<tableNode.at(indexCurrentNode).AboveChild()<<std::endl;
    tableNode.at(indexCurrentNode).createNode(axis, split, tableNode.size());
    //std::cout<<"Apres modification, le noeud "<<indexCurrentNode<<" a pour second fils "<<tableNode.at(indexCurrentNode).AboveChild()<<std::endl;

    BBox secondBox = BBox(localBox);
    secondBox.pMin[axis] = split;
    generateMidKdTree(currentProfondeur + 1, secondBox, n1, prims1);
    //free(prims1);
}

decimal KdtreeAccelerator::traverse(Ray* r, std::list<Intersection> &result)
{

    //std::cout<<"Calcul d'intersection..."<<std::endl;
    // Compute initial parametric range of ray inside kd-tree extent
    decimal tmin, tmax, intermin = -1.;
    if (!globalBox.IntersectP(r->position, r->direction, &tmin, &tmax))
    {
        //std::cout<<"Echecs de l'intersection avec la boite englobante."<<std::endl;
        return -1.;
    }

    stringstream logs;
    unsigned int indexNode = 0;

    // Prepare to traverse kd-tree for ray
    vec3 invDir(1.f / r->direction.x, 1.f / r->direction.y, 1.f / r->direction.z);
#define MAX_TODO 64
    KdToDo todo[MAX_TODO];
    int todoPos = 0;

    // Traverse kd-tree nodes in order for ray
    bool hit = false;
    KDNode* node = &tableNode[0];
    while (node != NULL)
    {
        // Bail out if we found a hit closer than the current node
        if (r->maxt < tmin) { break; }
        if (!node->isLeaf())
        {
            if (trace) { //std::cout << "Intersection avec le noeud d'indice " << indexNode << std::endl; 
			}
            // Process kd-tree interior node

            // Compute parametric distance along ray to split plane
            int axis = node->getAxe();
            float tplane = (node->getAxeValue() - r->position[axis]) * invDir[axis];
            if (trace) { //std::cout << "axe : " << axis << ",tplane : " << tplane << ", split : " << node->getAxeValue() << ", origine : " << r->position[axis] << ", invDir : " << invDir[axis] << endl; 
			}

            // Get node children pointers for ray
            KDNode* firstChild, *secondChild;
            int belowFirst = (r->position[axis] <  node->getAxeValue()) ||
                             (r->position[axis] == node->getAxeValue() && r->direction[axis] >= 0);
            if (belowFirst)
            {
                if (trace) { //std::cout << "Push du noeud (" << indexNode + 1 << ") puis (" << node->AboveChild() << ")" << std::endl; 
				}
                firstChild = node + 1;
                secondChild = &tableNode[node->AboveChild()];
            }
            else
            {
                if (trace) { //std::cout << "Push du noeud (" << node->AboveChild() << ") puis (" << indexNode + 1 << ")" << std::endl; 
				}
                firstChild = &tableNode[node->AboveChild()];
                secondChild = node + 1;
            }

            // Advance to next child node, possibly enqueue other child
            if (tplane > tmax || tplane < 0)
            {
                if (trace) { //std::cout << "On se deplace au premier noeud (tplane = " << tplane << ")" << std::endl; 
				}
                node = firstChild;
            }
            else if (tplane < tmin)
            {
                if (trace) { //std::cout << "On se deplace au second noeud." << std::endl; 
				}
                node = secondChild;
            }
            else
            {
                if (trace) { //std::cout << "On se deplce au premier noeud et on empile le second." << std::endl; 
				}
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
            //std::cout<<"Premiere primitive du noeud : "<<node->getFirstIndex()<<"(max : "<<initialMesh->size()<<")."<<std::endl;
            if (trace) { //std::cout << "La feuille contient " << node->getNbPrimitives() << " primitives." << std::endl; 
			}
            if (nPrimitives == 1)
            {
                Shape* prim = initialMesh->at(node->getFirstIndex());
                // Check one primitive inside leaf node

                Intersection currentIntersection;
                if (prim->getIntersection(*r, currentIntersection) && currentIntersection.t > 0.0001)
                {
                    hit = true;
                    result.push_back(currentIntersection);
                    /*if(inter.t < 0.f || currentIntersection.t < inter.t){
                        inter.t = currentIntersection.t;
                        inter.forme = currentIntersection.forme;
                        inter.p = currentIntersection.p;
                        //inter.material = currentIntersection.material;
                        //inter.normal = glm::vec3(currentIntersection.normal);
                        //inter.pos = glm::vec3(currentIntersection.pos);
                    }*/
                    intermin = leafTreatment::keepFunction(intersectionChoice, result, intermin);
                }
            }
            else if (nPrimitives > 1)
            {
                //logs<<"Arrive sur une feuille avec plusieurs primitives."<<std::endl;
                unsigned int* prims = node->getPrims();
                for (unsigned int i = 0; i < nPrimitives; i++)
                {
                    //const Reference<Primitive> &prim = primitives[prims[i]];
                    Shape* prim = initialMesh->at(prims[i]);

                    // Check one primitive inside leaf node

                    Intersection currentIntersection;
                    if (prim->getIntersection(*r, currentIntersection) && currentIntersection.t > 0.0001)
                    {
                        hit = true;
                        result.push_back(currentIntersection);
                        /*if(inter.t < 0.f || currentIntersection.t < inter.t){
                            inter.t = currentIntersection.t;
                            inter.forme = currentIntersection.forme;
                            inter.p = currentIntersection.p;
                        }*/
                        intermin = leafTreatment::keepFunction(intersectionChoice, result, intermin);
                    }
                }
                if (hit)
                    if (trace) { //std::cout << "Une intersection a ete trouve dans ce noeud." << std::endl; 
					}
                    else if (trace) { //std::cout << "Aucune intersection n'a ete trouve dans ce noeud." << std::endl; 
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
    //std::cout<<"Fin du calcul d'intersection."<<std::endl;
    /*if(!hit && !alreadyFail){
        nbFail++;
        if(nbFail == 50000){
            std::cout<<logs.str()<<std::endl;
            r = Ray(ray);
            alreadyFail = true;
        }
    }*/
    //if(hit)
    //  std::cout<<"Il y a "<<result.size()<<" intersections conservees pour le rayon."<<std::endl;
    return intermin;
}

void KdtreeAccelerator::print()
{
    for (unsigned int i = 0; i < tableNode.size(); i++)
    {
        //std::cout << "Le noeud " << i << " est ";
        if (tableNode.at(i).isLeaf())
        {
            //std::cout << "une feuille." << std::endl;
            //std::cout << "La feuille contient " << tableNode.at(i).getNbPrimitives() << "(1er : ";
            if (tableNode.at(i).getNbPrimitives() == 1)
            {
                //std::cout << tableNode.at(i).getFirstIndex() << ")" << std::endl;
            }
            else
            {
                //std::cout << ((tableNode.at(i).getPrims())[0]) << ")" << std::endl;
            }
        }
        else
        {
            //std::cout << "un noeud interne" << std::endl;
            //std::cout << "Le second fils est a l'indice " << tableNode.at(i).AboveChild() << "(max : " << tablePrimitive.size() << ")" << std::endl;
        }
    }
}

/*void KdtreeAccelerator::display(unsigned int indexNode){
    if(indexNode >= tableNode.size())
        return;
    BBox box = BBox(tableBox.at(indexNode));
    if(box.isNull)
        return;

    glBegin(GL_LINES);
        glVertex3f(box.points[0].x,box.points[0].y,box.points[0].z);
        glVertex3f(box.points[1].x,box.points[1].y,box.points[1].z);

        glVertex3f(box.points[0].x,box.points[0].y,box.points[0].z);
        glVertex3f(box.points[2].x,box.points[2].y,box.points[2].z);

        glVertex3f(box.points[3].x,box.points[3].y,box.points[3].z);
        glVertex3f(box.points[2].x,box.points[2].y,box.points[2].z);

        glVertex3f(box.points[3].x,box.points[3].y,box.points[3].z);
        glVertex3f(box.points[1].x,box.points[1].y,box.points[1].z);

        glVertex3f(box.points[0].x,box.points[0].y,box.points[0].z);
        glVertex3f(box.points[4].x,box.points[4].y,box.points[4].z);

        glVertex3f(box.points[5].x,box.points[5].y,box.points[5].z);
        glVertex3f(box.points[1].x,box.points[1].y,box.points[1].z);

        glVertex3f(box.points[6].x,box.points[6].y,box.points[6].z);
        glVertex3f(box.points[2].x,box.points[2].y,box.points[2].z);

        glVertex3f(box.points[3].x,box.points[3].y,box.points[3].z);
        glVertex3f(box.points[7].x,box.points[7].y,box.points[7].z);

        glVertex3f(box.points[5].x,box.points[5].y,box.points[5].z);
        glVertex3f(box.points[4].x,box.points[4].y,box.points[4].z);

        glVertex3f(box.points[6].x,box.points[6].y,box.points[6].z);
        glVertex3f(box.points[4].x,box.points[4].y,box.points[4].z);

        glVertex3f(box.points[5].x,box.points[5].y,box.points[5].z);
        glVertex3f(box.points[7].x,box.points[7].y,box.points[7].z);

        glVertex3f(box.points[6].x,box.points[6].y,box.points[6].z);
        glVertex3f(box.points[7].x,box.points[7].y,box.points[7].z);
    glEnd();
    if(!tableNode.at(indexNode).isLeaf())
        return;
    unsigned int *prims = tableNode.at(indexNode).getPrims();
    for(unsigned int i = 0; i < tableNode.at(indexNode).getNbPrimitives(); i++){
        Triangle &triangle = initialMesh->at(prims[i]);
        glBegin(GL_TRIANGLES);
            glVertex3f(triangle.data[0],triangle.data[1],triangle.data[2]);
            glVertex3f(triangle.data[3],triangle.data[4],triangle.data[5]);
            glVertex3f(triangle.data[6],triangle.data[7],triangle.data[8]);
        glEnd();
    }

    glColor3ub(255,0,0);
    glBegin(GL_LINES);
        glVertex3f(r.pos.x,r.pos.y,r.pos.z);
        glVertex3f(r.pos.x + 1000.f * r.dir.x,r.pos.y + 1000.f * r.dir.y,r.pos.z + 1000.f * r.dir.z);
    glEnd();
    glColor3ub(255,255,255);
}*/

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

    //Coupe au milieu, on selectionne la plus grande dimension
    //int axis = localBox.MaximumExtend();
    //float split = (localBox.pMin[axis] + localBox.pMax[axis]) / 2.f;

    // Initialize interior node and continue recursion
    // Choose split axis position for interior node
    int bestAxis = -1, bestOffset = -1;
    float bestCost = std::numeric_limits<float>::infinity();//INFINITY;
    float oldCost = isectCost * nbPrims;
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
    //BOOST_ASSERT(nBelow == nP && nAbove == 0); // NOBOOK
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

    //unsigned int indexFirstFalse = (unsigned int)(firstFalse - tablePrimitive.begin());
    //std::cout<<"La partition a trouve "<<indexFirstFalse<<" primitives inferieur au milieu."<<std::endl;

    KDNode currentNode;
    unsigned int indexCurrentNode = tableNode.size();
    tableNode.push_back(currentNode);
    tableBox.push_back(localBox);

    BBox firstBox = BBox(localBox);
    firstBox.pMax[axis] = split;
    generateMidKdTree(currentProfondeur + 1, firstBox, n0, prims0);
    //free(prims0);

    //std::cout<<"Avant modification, le noeud "<<indexCurrentNode<<" a pour second fils "<<tableNode.at(indexCurrentNode).AboveChild()<<std::endl;
    tableNode.at(indexCurrentNode).createNode(axis, split, tableNode.size());
    //std::cout<<"Apres modification, le noeud "<<indexCurrentNode<<" a pour second fils "<<tableNode.at(indexCurrentNode).AboveChild()<<std::endl;

    BBox secondBox = BBox(localBox);
    secondBox.pMin[axis] = split;
    generateMidKdTree(currentProfondeur + 1, secondBox, n1, prims1);
}
