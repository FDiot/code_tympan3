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

#ifndef __TY_BOUCHE_SURFACE__
#define __TY_BOUCHE_SURFACE__


#include "TYAcousticRectangle.h"
#include "Tympan/MetierSolver/DataManagerMetier/ComposantAcoustique/TYSourceBafflee.h"


/**
 * Classe pour la representation d'une bouche de ventilation.
 * C'est une face rectangulaire composee d'une seule source ponctuelle
 * de type source bafflee (TYSourceBafflee).
 * Cette source est conservee dans la source surfacique de la
 * super classe TYAcousticSurface, a l'index 0.
 */
class TYBoucheSurface: public TYAcousticRectangle
{
    OPROTODECL(TYBoucheSurface)
    TY_EXTENSION_DECL_ONLY(TYBoucheSurface)

    // Methodes
public:
    /**
     * Constructeur.
     */
    TYBoucheSurface();
    /**
     * Constructeur par copie.
     */
    TYBoucheSurface(const TYBoucheSurface& other);
    /**
     * Destructeur.
     */
    virtual ~TYBoucheSurface();

    virtual int fromXML(DOM_Element domElement);
    ///Operateur =.
    TYBoucheSurface& operator=(const TYBoucheSurface& other);
    ///Operateur ==.
    bool operator==(const TYBoucheSurface& other) const;
    ///Operateur !=.
    bool operator!=(const TYBoucheSurface& other) const;

    virtual std::string toString() const;

    virtual void distriSrcs();

    /**
     * Retourne la source bafflee associee a cette face.
     */
    LPTYSourceBafflee getSourceBafflee();
    /**
     * Associe une source bafflee particuliere a cette face.
     */
    void setSourceBafflee(LPTYSourceBafflee pSrc);


    // Membres
protected:

};


///Smart Pointer sur TYBoucheSurface.
typedef SmartPtr<TYBoucheSurface> LPTYBoucheSurface;

///Noeud geometrique de type TYBoucheSurface.
typedef TYGeometryNode TYBoucheSurfaceGeoNode;
///Smart Pointer sur TYBoucheSurfaceGeoNode.
typedef SmartPtr<TYBoucheSurfaceGeoNode> LPTYBoucheSurfaceGeoNode;
///Collection de noeuds geometriques de type TYBoucheSurface.
typedef std::vector<LPTYBoucheSurfaceGeoNode> TYTabBoucheSurfaceGeoNode;


#endif // __TY_BOUCHE_SURFACE__
