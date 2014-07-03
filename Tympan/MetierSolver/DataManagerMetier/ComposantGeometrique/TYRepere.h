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

#ifndef __TY_REPERE__
#define __TY_REPERE__

#include "Tympan/models/common/3d.h"
#include "Tympan/MetierSolver/DataManagerCore/TYElement.h"
#include "TYPoint.h"

/**
 * Classe de definition d'un repere.
 */
class TYRepere: public TYElement, public ORepere3D
{
    OPROTODECL(TYRepere)
    TY_EXTENSION_DECL_ONLY(TYRepere)

    // Methodes
public:
    /**
     * Constructeur par defaut.
     */
    TYRepere();
    /**
     * Constructeur par copie.
     */
    TYRepere(const TYRepere& other);
    /**
     * Constructeur avec un point et 3 vecteurs.
     *
     * @param origin Le point d'origine.
     * @param vecX Vecteur pourl'axe X.
     * @param vecY Vecteur pourl'axe Y.
     * @param vecZ Vecteur pourl'axe Z.
     */
    TYRepere(const OPoint3D& origin, const OVector3D& vecX, const OVector3D& vecY, const OVector3D& vecZ);

    /**
     * Constructeur avec un repere.
     *
     * @param repere Le repere.
     */
    TYRepere(const ORepere3D& repere);

    /**
     * Destructeur.
     */
    virtual ~TYRepere();

    ///Operateur =.
    TYRepere& operator=(const TYRepere& other);
    ///Operateur ==.
    bool operator==(const TYRepere& other) const;
    ///Operateur !=.
    bool operator!=(const TYRepere& other) const;

    virtual bool deepCopy(const TYElement* pOther, bool copyId = true);

    virtual std::string toString() const;

    virtual DOM_Element toXML(DOM_Element& domElement);
    virtual int fromXML(DOM_Element domElement);

    // Membres
protected:

};


#endif // __TY_REPERE__
