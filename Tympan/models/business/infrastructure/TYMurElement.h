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

#ifndef __TY_MURELEMENT__
#define __TY_MURELEMENT__


#include "Tympan/models/business/geoacoustic/TYAcousticRectangle.h"
#include "TYParoi.h"

/**
 * Comprend les proprietes acoustiques et geometriques d'un element de mur.
 */
class TYMurElement: public TYAcousticRectangle
{
    OPROTODECL(TYMurElement)
    TY_EXTENSION_DECL_ONLY(TYMurElement)
    TY_EXT_GRAPHIC_DECL_ONLY(TYMurElement)

    // Methodes
public:
    /**
     * Constructeur.
     */
    TYMurElement();
    /**
     * Constructeur par copie.
     */
    TYMurElement(const TYMurElement& other);
    /**
     * Destructeur.
     */
    virtual ~TYMurElement();

    ///Operateur =.
    TYMurElement& operator=(const TYMurElement& other);
    ///Operateur ==.
    bool operator==(const TYMurElement& other) const;
    ///Operateur !=.
    bool operator!=(const TYMurElement& other) const;

    virtual bool deepCopy(const TYElement* pOther, bool copyId = true);

    virtual std::string toString() const;

    virtual DOM_Element toXML(DOM_Element& domElement);
    virtual int fromXML(DOM_Element domElement);

    // Redefinie pour faire un setcolor a son parent
    virtual void setColor(const OColor& color);

    virtual TYTabSourcePonctuelleGeoNode getSrcs() const;
    virtual TYSourcePonctuelle srcPonctEquiv() const;
    virtual void distriSrcs();
    virtual bool setSrcsLw();

    virtual double surface() const;
    virtual int intersects(const TYSurfaceInterface* pSurf, OSegment3D& seg) const;
    virtual int intersects(const OSegment3D& seg, OPoint3D& pt) const;

    /**
     * Retourne le materiau de construction associe a cette Surface
     * via son volume parent, s'il existe.
     */
    virtual LPTYMateriauConstruction getMateriau() const { return _pParoi->getMatFace2(); }

    /**
     * Set/Get de la paroi.
     */
    LPTYParoi getParoi() { return _pParoi; }
    /**
     * Set/Get de la paroi.
     */
    const LPTYParoi getParoi() const { return _pParoi; }
    /**
     * Set/Get de la paroi.
     */
    void setParoi(const LPTYParoi pParoi) { _pParoi = pParoi; _pParoi->setParent(this); }


    // Membres
protected:
    ///Paroi
    LPTYParoi _pParoi;
};


///Noeud geometrique de type TYMurElement.
typedef TYGeometryNode TYMurElementGeoNode;
///Smart Pointer sur TYMurElementGeoNode.
typedef SmartPtr<TYMurElementGeoNode> LPTYMurElementGeoNode;
///Collection de noeuds geometriques de type TYMurElement.
typedef std::vector<LPTYMurElementGeoNode> TYTabMurElementGeoNode;


#endif // __TY_MURELEMENT__
