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
 */

#ifndef __TY_TERRAIN__
#define __TY_TERRAIN__


#include "Tympan/models/business/TYDefines.h"
#include "Tympan/models/business/geometry/TYGeometryNode.h"
#include "Tympan/models/business/TYColorInterface.h"
#include "Tympan/models/business/material/TYSol.h"

/**
 * Permet d'attribuer des proprietes acoustiques a une zone d'altimetrie.
 */
class TYTerrain: public TYElement, public TYColorInterface
{
    OPROTODECL(TYTerrain)
    TY_EXTENSION_DECL_ONLY(TYTerrain)
    TY_EXT_GRAPHIC_DECL_ONLY(TYTerrain)

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
     * Set/Get du materiau.
     */
    LPTYSol getSol() const { return _pSol;}
    LPTYSol getSol() { return _pSol._pObj; }

    /**
     * Set/Get du materiau.
     */
    void setSol(const LPTYSol pSol) { _pSol = pSol; _pSol->setParent(this); }

    /**
     * Get de l'indicateur de vegetation.
     */
    bool isVegetActive() { return _vegetActive; }

    /**
     * Set/Get de la vegetation.
     */
    LPTYVegetation getVegetation() const { return _pVegetation; }

    /**
     * Set/Get de la vegetation.
     */
    void setVegetation(LPTYVegetation pVeget) { _pVegetation = pVeget; _pVegetation->setParent(this); }

    /**
     * Active ou non l'utilisation de la vegetation et retourne une
     * reference sur celle-ci.
     *
     * @param state Etat d'utilisation de la vegetation.
     *
     * @return Le pointeur sur la vegetation.
     */
    void useVegetation(bool state = true);
    bool isUsingVegetation() { return _vegetActive; }

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
    ///Materiau.
    LPTYSol _pSol;

    ///Vegetation eventuelle.
    LPTYVegetation _pVegetation;

    ///Etat d'utilisation de la vegetation.
    bool _vegetActive;

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
