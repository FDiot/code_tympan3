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

#ifndef __TY_SOURCE_BAFFLEE__
#define __TY_SOURCE_BAFFLEE__

#include "Tympan/models/business/geoacoustic/TYAcousticRectangle.h"
#include "TYSourcePonctuelle.h"

class OSegment3D;

/**
 * Classe de definition d'une source bafflee.
 * Ce type de source correspond aux bouches de ventilation.
 * Un rectangle acoustique est associee a cette source, il
 * correspond a la face sur laquelle se trouve la source.
 */
class TYSourceBafflee: public TYSourcePonctuelle
{
    OPROTODECL(TYSourceBafflee)

    // Methodes
public:
    /**
     * Constructeur.
     */
    TYSourceBafflee();
    /**
     * Constructeur par copie.
     */
    TYSourceBafflee(const TYSourceBafflee& other);
    /**
     * Destructeur.
     */
    virtual ~TYSourceBafflee();

    ///Operateur =.
    TYSourceBafflee& operator=(const TYSourceBafflee& other);
    ///Operateur ==.
    bool operator==(const TYSourceBafflee& other) const;
    ///Operateur !=.
    bool operator!=(const TYSourceBafflee& other) const;

    virtual bool deepCopy(const TYElement* pOther, bool copyId = true);

    virtual std::string toString() const;

    virtual DOM_Element toXML(DOM_Element& domElement);
    virtual int fromXML(DOM_Element domElement);

    /**
     * Set/Get du rectangle acoustique associe a cette source.
     */
    //    void setAcousticRectangle(LPTYAcousticRectangle pAcousticRectangle) ;
    void setAcousticRectangle(LPTYRectangle pAcousticRectangle) ;

    /**
     * Set/Get du rectangle acoustique associe a cette source.
     */
    //    LPTYAcousticRectangle getAcousticRectangle() { return _pAcousticRectangle; }
    LPTYRectangle getAcousticRectangle() { return _pAcousticRectangle; }

    /**
     * Set/Get du rectangle acoustique associe a cette source.
     */
    //    const LPTYAcousticRectangle getAcousticRectangle() const { return _pAcousticRectangle; }
    const LPTYRectangle getAcousticRectangle() const { return _pAcousticRectangle; }

    // Membres
protected:
    ///Le rectangle acoustique associe a cette source.
    LPTYRectangle _pAcousticRectangle;

};


///Smart Pointer sur TYSourceBafflee.
typedef SmartPtr<TYSourceBafflee> LPTYSourceBafflee;

///Noeud geometrique de type TYSourceBafflee.
typedef TYGeometryNode TYSourceBaffleeGeoNode;
///Smart Pointer sur TYSourceBaffleeGeoNode.
typedef SmartPtr<TYSourceBaffleeGeoNode> LPTYSourceBaffleeGeoNode;
///Collection de noeuds geometriques de type TYSourceBafflee.
typedef std::vector<LPTYSourceBaffleeGeoNode> TYTabSourceBaffleeGeoNode;



#endif // __TY_SOURCE_BAFFLEE__
