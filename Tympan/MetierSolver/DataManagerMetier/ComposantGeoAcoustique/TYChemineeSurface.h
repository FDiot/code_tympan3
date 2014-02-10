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

#ifndef __TY_CHEMINEE_SURFACE__
#define __TY_CHEMINEE_SURFACE__


#include "TYAcousticRectangle.h"
#include "Tympan/MetierSolver/DataManagerMetier/ComposantAcoustique/TYSourceCheminee.h"

#if TY_USE_IHM
#include "Tympan/GraphicIHM/DataManagerIHM/TYChemineeSurfaceWidget.h"
#endif


/**
 * Classe pour la representation d'une cheminee.
 * C'est une face rectangulaire composee d'une seule source ponctuelle
 * de type source cheminee (TYSourceCheminee).
 * Cette source est conservee dans la source surfacique de la
 * super classe TYAcousticSurface, a l'index 0.
 */
class TYChemineeSurface: public TYAcousticRectangle
{
    OPROTOSUPERDECL(TYChemineeSurface, TYAcousticRectangle)
    TY_EXTENSION_DECL(TYChemineeSurface)

    // Methodes
public:
    /**
     * Constructeur.
     */
    TYChemineeSurface();
    /**
     * Constructeur par copie.
     */
    TYChemineeSurface(const TYChemineeSurface& other);
    /**
     * Destructeur.
     */
    virtual ~TYChemineeSurface();

    virtual int fromXML(DOM_Element domElement);

    ///Operateur =.
    TYChemineeSurface& operator=(const TYChemineeSurface& other);
    ///Operateur ==.
    bool operator==(const TYChemineeSurface& other) const;
    ///Operateur !=.
    bool operator!=(const TYChemineeSurface& other) const;

    virtual std::string toString() const;

    virtual void distriSrcs();

    /**
     * Retourne la source cheminee associee a cette face.
     */
    LPTYSourceCheminee getSourceCheminee();
    /**
     * Associe une source cheminee particuliere a cette face.
     */
    void setSourceCheminee(LPTYSourceCheminee pSrc);


    // Membres
protected:

};


///Smart Pointer sur TYChemineeSurface.
typedef SmartPtr<TYChemineeSurface> LPTYChemineeSurface;

///Noeud geometrique de type TYChemineeSurface.
typedef TYGeometryNode TYChemineeSurfaceGeoNode;
///Smart Pointer sur TYChemineeSurfaceGeoNode.
typedef SmartPtr<TYChemineeSurfaceGeoNode> LPTYChemineeSurfaceGeoNode;
///Collection de noeuds geometriques de type TYChemineeSurface.
typedef std::vector<LPTYChemineeSurfaceGeoNode> TYTabChemineeSurfaceGeoNode;


#endif // __TY_CHEMINEE_SURFACE__
