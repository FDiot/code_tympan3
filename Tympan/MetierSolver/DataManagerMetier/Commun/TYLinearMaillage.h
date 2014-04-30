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

#ifndef __TY_SEGMENT_MAILLAGE__
#define __TY_SEGMENT_MAILLAGE__


#include "Tympan/MetierSolver/DataManagerMetier/ComposantGeometrique/TYSegment.h"
#include "TYMaillage.h"

/**
 * \file TYLinearMaillage.h
 * \class TYLinearMaillage
 * \brief Classe de definition d'un maillage lineaire.
 * \version v 1.1
 * \author Projet_Tympan
 * \date 2008/01/21
 *
 */
class TYLinearMaillage: public TYMaillage
{
    OPROTOSUPERDECL(TYLinearMaillage, TYMaillage)
    TY_EXT_GRAPHIC_DECL_ONLY(TYLinearMaillage)
    TY_EXTENSION_DECL_ONLY(TYLinearMaillage)

    // Methodes
public:
    /**
     * \fn TYLinearMaillage()
     * \brief Constructeur.
     * Constructeur de la classe TYLinearMaillage
     */
    TYLinearMaillage();

    /**
     * \fn TYLinearMaillage(const TYLinearMaillage& other)
     * \brief Constructeur par copie.
     * Constructeur par copie de la classe TYLinearMaillage
     */
    TYLinearMaillage(const TYLinearMaillage& other);

    /**
     * \fn virtual ~TYLinearMaillage()
     * \brief Destructeur.
     *  Destructeur de la classe TYLinearMaillage
     */
    virtual ~TYLinearMaillage();

    ///Operateur =.
    TYLinearMaillage& operator=(const TYLinearMaillage& other);

    ///Operateur ==.
    bool operator==(const TYLinearMaillage& other) const;

    ///Operateur !=.
    bool operator!=(const TYLinearMaillage& other) const;

    virtual bool deepCopy(const TYElement* pOther, bool copyId = true);

    virtual std::string toString() const;

    virtual DOM_Element toXML(DOM_Element& domElement);
    virtual int fromXML(DOM_Element domElement);

    bool toXML(const std::string& sFilePath);
    bool fromXML(const std::string& sFilePath);

    std::string toXMLString();
    bool fromXMLString(const std::string& sXMLString);

    /**
     * Suppression du resultat de calcul
     */
    virtual void clearResult();

    /**
     * \fn virtual void make(LPTYSegment pSeg, double densite = TY_MAILLAGE_DEFAULT_DENSITE)
     * \brief Rempli la structure de points de calcul a partir d'un segment et d'une densite de points.
     * \param pSeg: Le segment delimitant le maillage.
     * \param densite: La densite de points de calcul.
     */
    virtual void make(LPTYSegment pSeg, double densite = TY_MAILLAGE_DEFAULT_DENSITE);

    /**
     * \fn LPTYSegment getSegment()
     * \brief Retourne le segment associe a ce maillage.
     * \return _pSeg Le segment associe a ce maillage.
     */
    LPTYSegment getSegment() { return _pSeg; }

    /**
     * \fn double getDensite()
     * \brief Retourne la densite de points de calcul.
     * \return _densite: La densite de points de calcu
     */
    double getDensite() { return _densite; }

    // Membres
protected:
    ///Le segment associe a ce maillage.
    LPTYSegment _pSeg;

    ///La densite de points de calcul.
    double _densite;
};

///Smart Pointer sur TYLinearMaillage.
typedef SmartPtr<TYLinearMaillage> LPTYLinearMaillage;

///Noeud geometrique de type TYLinearMaillage.
typedef TYGeometryNode TYLinearMaillageGeoNode;

///Smart Pointer sur TYLinearMaillageGeoNode.
typedef SmartPtr<TYLinearMaillageGeoNode> LPTYLinearMaillageGeoNode;

///Collection de noeuds geometriques de type TYLinearMaillage.
typedef std::vector<LPTYLinearMaillageGeoNode> TYTabLinearMaillageGeoNode;


#endif // __TY_SEGMENT_MAILLAGE__
