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

#ifndef __TY_SOURCE__
#define __TY_SOURCE__

#include "Tympan/models/business/TYElement.h"

///Tableau associatif Regime/Spectre.
typedef std::map<int, LPTYSpectre> TYMapRegimeSpectre;


/**
 * Classe de base des sources.
 */
class TYSource: public TYElement
{
    OPROTODECL(TYSource)
    TY_EXTENSION_DECL_ONLY(TYSource)

    // Methodes
public:
    /**
     * Constructeur.
     */
    TYSource();
    /**
     * Constructeur par copie.
     */
    TYSource(const TYSource& other);
    /**
     * Destructeur.
     */
    virtual ~TYSource();

    ///Operateur =.
    TYSource& operator=(const TYSource& other);
    ///Operateur ==.
    bool operator==(const TYSource& other) const;
    ///Operateur !=.
    bool operator!=(const TYSource& other) const;

    virtual bool deepCopy(const TYElement* pOther, bool copyId = true);

    virtual std::string toString() const;

    virtual DOM_Element toXML(DOM_Element& domElement);
    virtual int fromXML(DOM_Element domElement);

    /**
     * Set/Get du spectre.
     */
    virtual TYSpectre* getSpectre(int regime = -1) const { return _pSpectre._pObj; }

    /**
     * Get/Set du Spectre (independemment d'un eventuel attenuateur) - utilise par le widget -
     */
    virtual TYSpectre* getCurrentSpectre() const { return _pSpectre._pObj ; }

    virtual void setSpectre(LPTYSpectre pSpectre) { _pSpectre = pSpectre; }

    // Membres
protected:
    LPTYSpectre _pSpectre;
};


#endif // __TY_SOURCE__

