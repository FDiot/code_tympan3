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


//Structure ajencee pour tenir sur 2 octets
typedef struct kdNode
{

    bool isLeaf() { return ((int)(flag & 3)) == 3; }                                            //Renvoi vrai si le noeud est une feuille
    bool isNode() { return ((int)(flag & 3)) < 3; }                                             //Renvoi vrai si le noeud est un noeud
    unsigned int getNbPrimitives() { return nbPrims >> 2; }                                     //Renvoi le nombre de primitives
    unsigned int AboveChild() { return secondChild >> 2; }
    unsigned int* getPrims() { return prims; }
    int getAxe() { return (flag & 3); }                                                         //Renvoi l'indice de l'axe du plan separateur (node)
    float getAxeValue() { return split; }                                                       //Renvoi la valeur de l'axe du plan separateur (node)
    unsigned int getFirstIndex() { return firstIndex; }                                         //Renvoi l'index de la premiere primitive du noeud (all)
    void createLeaf(unsigned int _nbPrims, unsigned int _firstIndex, unsigned int* _prims);     //Initialisation d'une feuille
    void createNode(int axis, float _split, unsigned int nextChild);                            //Initialisation d'un noeud
private:
    union
    {
        int flag;                   //0 : node, axe x, 1 : node, axe y, 2 : node, axe z, 3 : feuille. Utilisation de 2 bits
        unsigned int secondChild;   //Node : 1er fils est consecutif au noeud courant, le second est plus loins
        unsigned int nbPrims;       //Feuille : nombre de primitives. Utilisation de 30 bits
    };

    union
    {
        float split;                //Node : valeur de l'axe separateur
        unsigned int firstIndex;    //Leaf : index de la premiere primitive dans la liste
        unsigned int* prims;        //Leaf : tableau contenant les indexs de primitives
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
* \brief Structure de pile pour la traversee du kd-tree. Stockage des noeuds optimitimes.
*/
struct KdStack
{
    KDNode node;        /*!< Prochain noeud a parcourir en cas d'echecs. */
    float tmin;         /*!< Temps d'entree dans le noeud node. */
    float tmax;         /*!< Temps de sortie du noeud node. */
    unsigned int pos;   /*!< Position dans la pile. */
};

class KdtreeAccelerator : public Accelerator
{

public:
    KdtreeAccelerator(std::vector<Shape*>* _initialMesh = NULL, BBox _globalBox = BBox());

    virtual ~KdtreeAccelerator();

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
    void generateMidKdTree(int currentProfondeur, BBox& localBox, unsigned int nbPrims, unsigned int* prims);
    void generateSAHKdTree(int currentProfondeur, BBox& localBox, TaBRecBoundEdge* edges[3], unsigned int nbPrims, unsigned int* prims);
    std::vector<Shape*>* initialMesh;
//    BBox globalBox;

    std::vector<InfoPrim> tablePrimitive;
    std::vector<BBox> tableBox;
    std::vector<KDNode> tableNode;

    int maxProfondeur;
    int maxPrimPerLeaf;

    int realMaxProfondeur;

    float isectCost;
    float emptyBonus;
    float traversalCost;
};


#endif