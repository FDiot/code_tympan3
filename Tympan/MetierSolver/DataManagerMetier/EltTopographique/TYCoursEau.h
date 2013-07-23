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

#ifndef __TY_COURSEAU__
#define __TY_COURSEAU__


#include "Tympan/MetierSolver/DataManagerMetier/ComposantGeoAcoustique/TYAcousticLine.h"

#if TY_USE_IHM
#include "Tympan/GraphicIHM/DataManagerIHM/TYCoursEauWidget.h"
#include "Tympan/GraphicIHM/DataManagerGraphic/TYCoursEauGraphic.h"
#endif


/**
 * Classe de definition du chemin d'un cours d'eau et de ses proprietes acoustiques.
 *
 * @author Projet_Tympan
 *
 */
class TYCoursEau: public TYAcousticLine
{
    OPROTOSUPERDECL(TYCoursEau, TYAcousticLine)
    TY_EXTENSION_DECL(TYCoursEau)
    TY_EXT_GRAPHIC_DECL(TYCoursEau)

    // Methodes
public:
    /**
     * Constructeur.
     */
    TYCoursEau();
    /**
     * Constructeur par copie.
     */
    TYCoursEau(const TYCoursEau& other);
    /**
     * Destructeur.
     */
    virtual ~TYCoursEau();

    ///Operateur =.
    TYCoursEau& operator=(const TYCoursEau& other);
    ///Operateur ==.
    bool operator==(const TYCoursEau& other) const;
    ///Operateur !=.
    bool operator!=(const TYCoursEau& other) const;

    virtual std::string toString() const;

    virtual DOM_Element toXML(DOM_Element& domElement);
    virtual int fromXML(DOM_Element domElement);

    // Membres
protected:

};


///Noeud geometrique de type TYCoursEau.
typedef TYGeometryNode TYCoursEauGeoNode;
///Smart Pointer sur TYCoursEauGeoNode.
typedef SmartPtr<TYCoursEauGeoNode> LPTYCoursEauGeoNode;
///Collection de noeuds geometriques de type TYCoursEau.
typedef std::vector<LPTYCoursEauGeoNode> TYTabCoursEauGeoNode;


#endif // __TY_COURSEAU__
