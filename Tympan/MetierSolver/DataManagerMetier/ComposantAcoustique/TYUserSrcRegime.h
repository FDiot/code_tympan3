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

#ifndef __TY_USER_SRC_REGIME__
#define __TY_USER_SRC_REGIME__


#include "TYRegime.h"
#include "TYDirectivite.h"

/**
 * Classe pour definir les differents regimes d'une source ponctuelle.
 *
 * @author Projet_Tympan
 *
 */
class TYUserSrcRegime : public TYRegime
{
    OPROTOSUPERDECL(TYUserSrcRegime, TYRegime)

    // Methodes
public:
    /**
     * Constructeur.
     */
    TYUserSrcRegime();
    /**
     * Constructeur par copie.
     */
    TYUserSrcRegime(const TYUserSrcRegime& other);
    /**
     * Destructeur.
     */
    virtual ~TYUserSrcRegime();

    ///Operateur =.
    TYUserSrcRegime& operator=(const TYUserSrcRegime& other);
    ///Operateur ==.
    bool operator==(const TYUserSrcRegime& other) const;
    ///Operateur !=.
    bool operator!=(const TYUserSrcRegime& other) const;

    virtual bool deepCopy(const TYElement* pOther, bool copyId = true);

    virtual std::string toString() const;

    virtual DOM_Element toXML(DOM_Element& domElement);
    virtual int fromXML(DOM_Element domElement);

    // Membres
public:
    LPTYDirectivite _pDirectivite;

protected:
};

///Tableau de regimes.
typedef std::vector<TYUserSrcRegime> TYTabUserSrcRegimes;

#endif // __TY_USER_SRC_REGIME__
