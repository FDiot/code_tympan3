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

#ifndef KDTREEACCELERATOR_H
#define KDTREEACCELERATOR_H

#include "Tympan/solvers/AcousticRaytracer/Geometry/BBox.h"
#include <vector>
#include "Tympan/solvers/AcousticRaytracer/Geometry/Shape.h"
#include "Accelerator.h"

/*!
* \struct kdNode
* \brief Node structure (optimized to be stored on 2 bytes)
*/
typedef struct kdNode
{
    bool isLeaf() { return ((int)(flag & 3)) == 3; }                                            //!< Return true if the node is a leaf
    bool isNode() { return ((int)(flag & 3)) < 3; }                                             //!< Return true if the node is a node
    unsigned int getNbPrimitives() { return nbPrims >> 2; }                                     //!< Return primitives number
    unsigned int AboveChild() { return secondChild >> 2; }
    unsigned int* getPrims() { return prims; }
    int getAxe() { return (flag & 3); }                                                         //!< Return the axis number of the separator plane (node)
    float getAxeValue() { return split; }                                                       //!< Return the axis value of the separator plane (node)
    unsigned int getFirstIndex() { return firstIndex; }                                         //!< Return the index of the first node primitive (all)
    void createLeaf(unsigned int _nbPrims, unsigned int _firstIndex, unsigned int* _prims);     //!< Leaf initialization
    void createNode(int axis, float _split, unsigned int nextChild);                            //!< Node initialization
private:
    union
    {
        int flag;                   //!< 0 : node, x axis, 1 : node, y axis, 2 : node, z axis, 3 : leaf. 2 bits used
        unsigned int secondChild;   //!< Node : the first child is just after current node, second one is further
        unsigned int nbPrims;       //!< Leaf : number of primitives. Use 30 bits integer
    };

    union
    {
        float split;                //!< Node : separator axis value
        unsigned int firstIndex;    //!< Leaf : index of the first primitive in the list
        unsigned int* prims;        //!< Leaf : array containing primitives indexes
    };
} KDNode;

typedef struct infoPrim
{
    unsigned int indexPrim;
    BBox box;
} InfoPrim;

struct CompareToMid
{
    CompareToMid(int d, float m) { dim = d; mid = m; }
    int dim;
    float mid;
    bool operator()(InfoPrim a) const
    {
        return a.box.centroid[dim] < mid;
    }
};

struct TaBRecBoundEdge
{
    // TaBRecBoundEdge Public Methods
    TaBRecBoundEdge() { }
    TaBRecBoundEdge(float tt, int pn, bool starting)
    {
        t = tt;
        primNum = pn;
        type = starting ? START : END;
    }
    bool operator<(const TaBRecBoundEdge& e) const
    {
        if (t == e.t)
        {
            return (int)type < (int)e.type;
        }
        else { return t < e.t; }
    }
    float t;
    int primNum;
    enum { START, END } type;
};

struct KdToDo
{
    KDNode* node;
    float tmin, tmax;
};

/*!
* \struct KdStack
* \brief Stack structure for the k-d tree. Optimized storage for the nodes
*/
struct KdStack
{
    KDNode node;        //!< Next node to browser if fail happens
    float tmin;         //!< Entry time inside the node
    float tmax;         //!< Left time from the node
    unsigned int pos;   //!< Position in the stack
};

/*!
* \class KdtreeAccelerator
* \brief K-d tree Accelerator (based on space splitting)
*/
class KdtreeAccelerator : public Accelerator
{

public:
	/// Constructor
    KdtreeAccelerator(std::vector<Shape*>* _initialMesh = NULL, BBox _globalBox = BBox());
    /// Destructor
    virtual ~KdtreeAccelerator();

    virtual bool build();

    virtual decimal traverse(Ray* r, std::list<Intersection> &result) const;

    /// Set/Get maximal depth
    void setMaxProfondeur(int _maxProfondeur) { maxProfondeur = _maxProfondeur; }
    int getMaxProfondeur() { return maxProfondeur; }

    /// Set/Get maximal primitives per leaf
    void setMaxPrimPerLeaf(int _maxPrimPerLeaf) { maxPrimPerLeaf = _maxPrimPerLeaf; }
    int getMaxPrimPerLeaf() { return maxPrimPerLeaf; }
    /// Get the vector of bounding boxes
    std::vector<BBox>& getBBox() { return tableBox; }
    /// Print the tree (not implemented yet)
    void print();

    bool alreadyFail;	//!< Unused attribute
    unsigned int nbFail;//!< Unused attribute
    Ray r;				//!< Unused attribute
    bool trace;			//!< Unused attribute

protected:
    /// Generate the tree by middle subdivision
    void generateMidKdTree(int currentProfondeur, BBox& localBox, unsigned int nbPrims, unsigned int* prims);
    /// Generate the tree with SAH (Surface Area Heuristic) method
    void generateSAHKdTree(int currentProfondeur, BBox& localBox, TaBRecBoundEdge* edges[3], unsigned int nbPrims, unsigned int* prims);
    std::vector<Shape*>* initialMesh;		//!< Pointer to the mesh
//    BBox globalBox;

    std::vector<InfoPrim> tablePrimitive;	//!< List of primitives and their bounding box
    std::vector<BBox> tableBox;				//!< Bounding boxes list of the tree
    std::vector<KDNode> tableNode;

    int maxProfondeur;		//!< Maximal depth
    int maxPrimPerLeaf;		//!< Maximal primitives per leaf

    int realMaxProfondeur;	//!< Real max depth

    float isectCost;		//!< Parameter for best splitting
    float emptyBonus;		//!< Parameter for best splitting
    float traversalCost;	//!< Parameter for best splitting
};


#endif
