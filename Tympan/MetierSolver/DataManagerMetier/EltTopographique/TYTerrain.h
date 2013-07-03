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
 
/*
 *
 *
 *
 *
 */

#ifndef __TY_TERRAIN__
#define __TY_TERRAIN__


#include "Tympan/MetierSolver/DataManagerMetier/EltMateriaux/TYSol.h"
#include "Tympan/MetierSolver/DataManagerMetier/ComposantGeometrique/TYGeometryNode.h"
#include "Tympan/MetierSolver/DataManagerCore/TYColorInterface.h"

#if TY_USE_IHM
#include "Tympan/GraphicIHM/DataManagerIHM/TYTerrainWidget.h"
#include "Tympan/GraphicIHM/DataManagerGraphic/TYTerrainGraphic.h"
#endif


/**
 * Permet d'attribuer des proprietes acoustiques a une zone d'altimetrie.
 *
 * @author Projet_Tympan
 *
 */
class TYTerrain: public TYElement, public TYColorInterface
{
    OPROTOSUPERDECL(TYTerrain, TYElement)
    TY_EXTENSION_DECL(TYTerrain)
    TY_EXT_GRAPHIC_DECL(TYTerrain)

    // Methodes
public:
    /**
     * Constructeur.
     */
    TYTerrain();
    /**
     * Constructeur par copie.
     */
    TYTerrain(const TYTerrain& other);
    /**
     * Destructeur.
     */
    virtual ~TYTerrain();

    ///Operateur =.
    TYTerrain& operator=(const TYTerrain& other);
    ///Operateur ==.
    bool operator==(const TYTerrain& other) const;
    ///Operateur !=.
    bool operator!=(const TYTerrain& other) const;

    virtual bool deepCopy(const TYElement* pOther, bool copyId = true);

    virtual std::string toString() const;

    virtual DOM_Element toXML(DOM_Element& domElement);
    virtual int fromXML(DOM_Element domElement);

    /**
     * Set/Get du type.
     */
    //  int getType() const { return _type; }
    /**
     * Set/Get du type.
     */
    //  void setType(int type) { _type = type; }

    /**
     * Set/Get du materiau.
     */
    LPTYSol getSol() const { return _pSol;}

    /**
     * Set/Get du materiau.
     */
    void setSol(const LPTYSol pSol) { _pSol = pSol; _pSol->setParent(this); }

    /**
     * Set/Get de la liste de points.
     */
    virtual TYTabPoint& getListPoints() { return _listPoints; }
    /**
     * Set/Get de la liste de points.
     */
    virtual const TYTabPoint& getListPoints() const { return _listPoints; }
    /**
     * Set/Get de la liste de points.
     */
    virtual void setListPoints(const TYTabPoint& liste) { _listPoints = liste; setIsGeometryModified(true); }

    /**
     * Evaluation de la surface
     */
    double surface();



    // Membres
protected:
    ///Type.
    //  int _type;
    ///Materiau.
    LPTYSol _pSol;
    ///Liste de points.
    TYTabPoint _listPoints;
};


///Noeud geometrique de type TYTerrain.
typedef TYGeometryNode TYTerrainGeoNode;
///Smart Pointer sur TYTerrainGeoNode.
typedef SmartPtr<TYTerrainGeoNode> LPTYTerrainGeoNode;
///Collection de noeuds geometriques de type TYTerrain.
typedef std::vector<LPTYTerrainGeoNode> TYTabTerrainGeoNode;


#endif // __TY_TERRAIN__
