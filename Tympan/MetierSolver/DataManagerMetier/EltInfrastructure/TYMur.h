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

#ifndef __TY_MUR__
#define __TY_MUR__

class OPoint3D;
class OSegment3D;


#include "Tympan/MetierSolver/DataManagerMetier/ComposantGeoAcoustique/TYAcousticRectangleNode.h"
#include "TYParoi.h"

#if TY_USE_IHM
#include "Tympan/GraphicIHM/DataManagerIHM/TYMurWidget.h"
#include "Tympan/GraphicIHM/DataManagerGraphic/TYMurGraphic.h"
#endif


/**
 * Comprend les proprietes acoustiques et geometriques d'un mur.
 *
 * @author Projet_Tympan
 *
 */
class TYMur: public TYAcousticRectangleNode
{
    OPROTOSUPERDECL(TYMur, TYAcousticRectangleNode)
    TY_EXTENSION_DECL(TYMur)
    TY_EXT_GRAPHIC_DECL(TYMur)

    // Methodes
public:
    /**
     * Constructeur.
     */
    TYMur();
    /**
     * Constructeur par copie.
     */
    TYMur(const TYMur& other);
    /**
     * Destructeur.
     */
    virtual ~TYMur();

    ///Operateur =.
    TYMur& operator=(const TYMur& other);
    ///Operateur ==.
    bool operator==(const TYMur& other) const;
    ///Operateur !=.
    bool operator!=(const TYMur& other) const;

    virtual bool deepCopy(const TYElement* pOther, bool copyId = true);

    virtual std::string toString() const;

    virtual DOM_Element toXML(DOM_Element& domElement);
    virtual int fromXML(DOM_Element domElement);

    virtual TYTabSourcePonctuelleGeoNode getSrcs() const;
    virtual TYSourcePonctuelle srcPonctEquiv() const;
    virtual void distriSrcs();
    virtual bool setSrcsLw();
    virtual void remAllSrcs();

    virtual double surface() const;
    virtual int intersects(const TYSurfaceInterface* pSurf, OSegment3D& seg) const;
    virtual int intersects(const OSegment3D& seg, OPoint3D& pt) const;

    virtual bool makeGrid(int nbCol, int nbRow);
    virtual void updateGrid();

    /**
     * Calule le spectre d'absorption du mur au point donne.
     *
     * @param pt Point pour lequel on calcule le spectre d'absorption du mur.
     * @param spectre Le spectre resultant.
     *
     * @return <code>false</code> si le point passe n'appartient pas au mur;
     *         <code>true</code> sinon.
     */
    bool abso(const TYPoint& pt, TYSpectre& spectre) const;

    /**
     * Calule le spectre de transmission du mur au point donne.
     *
     * @param pt Point pour lequel on calcule le spectre de transmission du mur.
     * @param spectre Le spectre resultant.
     *
     * @return <code>false</code> si le point passe n'appartient pas au mur;
     *         <code>true</code> sinon.
     */
    bool transm(const TYPoint& pt, TYSpectre& spectre) const;

    /**
     * Set/Get de la paroi.
     */
    TYParoi* getParoi() const { return _pParoi; }

    /**
     * Set/Get de la paroi.
     */
    void setParoi(const LPTYParoi pParoi);

    /**
     * Get/Set de l'etat bloque du type de paroi
     */
    void setParoiLocked(const bool& bVal);
    bool isParoiLocked() { return _bParoiLocked; }
    const bool isParoiLocked() const { return _bParoiLocked; }

	/**
     * \fn  LPTYMateriauConstruction getMateriau()
	 * \brief Surcharge de la methode getMateriau pour la dalle
     * \return le materiau de construction de la dalle
	 */
	TYMateriauConstruction* getMateriau();
    
	// Membres
protected:
    /**
     * La paroi pour l'ensemble des elements de mur (TYMurElement) de contruction
     * composant ce mur et pas de type "isSub".
     */
    LPTYParoi _pParoi;

    /**
     * Indicateur de "blocage" du type de paroi
     * (evite une mise a jour depuis l'etage)
     */
    bool _bParoiLocked;
};


///Noeud geometrique de type TYMur.
typedef TYGeometryNode TYMurGeoNode;
///Smart Pointer sur TYMurGeoNode.
typedef SmartPtr<TYMurGeoNode> LPTYMurGeoNode;
///Collection de noeuds geometriques de type TYMur.
typedef std::vector<LPTYMurGeoNode> TYTabMurGeoNode;


#endif // __TY_MUR__
