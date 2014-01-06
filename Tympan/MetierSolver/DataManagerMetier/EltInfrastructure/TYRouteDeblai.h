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

#ifndef __TY_ROUTE_DEBLAI__
#define __TY_ROUTE_DEBLAI__


#include "Tympan/MetierSolver/DataManagerMetier/ComposantGeoAcoustique/TYAcousticLine.h"
#include "Tympan/MetierSolver/DataManagerMetier/ComposantAcoustique/TYTrafic.h"
#include "TYRoute.h"
#include "Tympan/MetierSolver/DataManagerMetier/EltTopographique/TYContrainte.h"

#if TY_USE_IHM
#include "Tympan/GraphicIHM/DataManagerIHM/TYRouteDeblaiWidget.h"
#include "Tympan/GraphicIHM/DataManagerGraphic/TYRouteGraphic.h"
#endif


/**
 * Comprend les proprietes acoustiques et geometriques d'une route de deblai.
 *
 * @author Projet_Tympan
 *
 */
class TYRouteDeblai: public TYRoute, public TYContrainte
{
    OPROTOSUPERDECL(TYRouteDeblai, TYRoute)
    TY_EXTENSION_DECL(TYRouteDeblai)
    TY_EXT_GRAPHIC_DECL(TYRoute)

    // Methodes
public:
    /**
     * Constructeur.
     */
    TYRouteDeblai();
    /**
     * Constructeur par copie.
     */
    TYRouteDeblai(const TYRouteDeblai& other);
    /**
     * Destructeur.
     */
    virtual ~TYRouteDeblai();

    ///Operateur =.
    TYRouteDeblai& operator=(const TYRouteDeblai& other);
    ///Operateur ==.
    bool operator==(const TYRouteDeblai& other) const;
    ///Operateur !=.
    bool operator!=(const TYRouteDeblai& other) const;

    virtual bool deepCopy(const TYElement* pOther, bool copyId = true);

    virtual std::string toString() const;

    virtual DOM_Element toXML(DOM_Element& domElement);
    virtual int fromXML(DOM_Element domElement);

    /**
    * Renvoie la largeur de la route
    */
    double getLargeur() { return largeur; }

    /**
    * Set la largeur de la route
    */
    void setLargeur(double _largeur) { largeur = _largeur; }

    /**
     * Mise a jour des caracteristiques acoustiques de la route
     */
    virtual bool updateAcoustic();

    /**
     * Distribution des sources
     */
    virtual void distriSrcs();

    /**
    * Redefinition de la fonction de la classe Contrainte. Calcul une contrainte pour la topographie.
    */
    virtual bool computeConstraint();

protected:

    double largeur; /*!< Largeur de la route complete*/
};


///Noeud geometrique de type TYRoute.
typedef TYGeometryNode TYRouteDeblaiGeoNode;
///Smart Pointer sur TYRouteGeoNode.
typedef SmartPtr<TYRouteDeblaiGeoNode> LPTYRouteDeblaiGeoNode;
///Collection de noeuds geometriques de type TYRoute.
typedef std::vector<LPTYRouteDeblaiGeoNode> TYTabRouteDeblaiGeoNode;


#endif // __TY_ROUTE__
