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

#ifndef __TY_RECTANGULAR_MAILLAGE__
#define __TY_RECTANGULAR_MAILLAGE__


#include "Tympan/MetierSolver/DataManagerMetier/ComposantGeometrique/TYRectangle.h"
#include "TYMaillage.h"


/**
 * \file TYRectangularMaillage.h
 * \class TYRectangularMaillage
 * \brief Classe de definition d'un maillage rectangulaire.
 * \version v 1.1
 * \date 2008/01/21
 * \author Projet_Tympan
 *
 */
class TYRectangularMaillage: public TYMaillage
{
    OPROTODECL(TYRectangularMaillage)
    TY_EXT_GRAPHIC_DECL_ONLY(TYRectangularMaillage)
    TY_EXTENSION_DECL_ONLY(TYRectangularMaillage)

    // Methodes
public:
    /**
     * \fn TYRectangularMaillage()
     * \brief Constructeur.
     * Le constructeur de la classe TYRectangularMaillage
     */
    TYRectangularMaillage();

    /**
     * \fn TYRectangularMaillage(const TYRectangularMaillage& other
     * \brief Constructeur par copie.
     *  Le constructeur par copie de la classe TYRectangularMaillage
     */
    TYRectangularMaillage(const TYRectangularMaillage& other);

    /**
     * \fn virtual ~TYRectangularMaillage()
     * \brief Destructeur.
     * Le destructeur de la classe TYRectangularMaillage
     */
    virtual ~TYRectangularMaillage();

    ///Operateur =.
    TYRectangularMaillage& operator=(const TYRectangularMaillage& other);

    ///Operateur ==.
    bool operator==(const TYRectangularMaillage& other) const;

    ///Operateur !=.
    bool operator!=(const TYRectangularMaillage& other) const;

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
     * \fn virtual void make(LPTYRectangle pRect, double densiteX  double densiteY );
     * \brief Rempli la structure de points de calcul a partir d'un rectangle  et de densites de points.
     * \param pRect Le rectangle delimitant le maillage.
     * \param densiteX La densite de points de calcul en X.
     * \param densiteY La densite de points de calcul en Y.
     */
    virtual void make(LPTYRectangle pRect,
                      double densiteX = TY_MAILLAGE_DEFAULT_DENSITE,
                      double densiteY = TY_MAILLAGE_DEFAULT_DENSITE);

    /**
     * \fn LPTYRectangle getRectangle()
     * \brief Retourne le rectangle associe a ce maillage.
     * \return _pRect
     */
    LPTYRectangle getRectangle() const { return _pRect; }

    /**
     * \fn double getDensiteX()
     * \brief Retourne la densite de points de calcul en X.
     * \return _densiteX
     */
    double getDensiteX() const { return _densiteX; }

    /**
     * \fn double getDensiteY()
     * \brief Retourne la densite de points de calcul en Y.
     * \return _densiteY
     */
    double getDensiteY() const { return _densiteY; }

    /**
     * \fn void getDimensionsAndSteps(unsigned long & x,unsigned long & y,OVector3D & stepx,OVector3D & stepy)
     * \brief Renvoie le nombre de lignes et de colonnes du maillage.En fonction de ses dimensions en 3D et de la densite
     */
    void getDimensionsAndSteps(unsigned long& x, unsigned long& y, OVector3D& stepx, OVector3D& stepy) const;

    /**
     * \fn TYPoint * getPosLabel()
     * \brief Renvoie la position sur la maillage permettant l'affichage du label et de la legende.
     * \return &_posLabel
     */
    TYPoint* getPosLabel() { return &_posLabel; }

    /**
     * \brief Return the index
     * \param x Row value
     * \param y Column value
     * \return The index of point calcul
     */
    virtual int getIndexPtCalcul(int x, int y) const;

    /**
     * \brief Return the dimensions in x and y
     * \param x Dimension x
     * \param y Dimension y
     */
    virtual void getDimensions(int& x, int& y) const;

    // Membres
protected:
    TYPoint         _posLabel;

    ///Le rectangle associe a ce maillage.
    LPTYRectangle   _pRect;

    ///La densite de points de calcul en X.
    double          _densiteX;

    ///La densite de points de calcul en Y.
    double          _densiteY;

    ///Numbers of point in row
    double          _nbPointsX;
};


///Smart Pointer sur TYRectangularMaillage.
typedef SmartPtr<TYRectangularMaillage> LPTYRectangularMaillage;

///Noeud geometrique de type TYRectangularMaillage.
typedef TYGeometryNode TYRectangularMaillageGeoNode;

///Smart Pointer sur TYRectangularMaillageGeoNode.
typedef SmartPtr<TYRectangularMaillageGeoNode> LPTYRectangularMaillageGeoNode;

///Collection de noeuds geometriques de type TYRectangularMaillage.
typedef std::vector<LPTYRectangularMaillageGeoNode> TYTabRectangularMaillageGeoNode;


#endif // __TY_RECTANGULAR_MAILLAGE__
