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

#ifndef __TY_ATTENUATEUR__
#define __TY_ATTENUATEUR__

#include "Tympan/MetierSolver/DataManagerCore/TYElement.h"
#include "TYSpectre.h"


/**
 * Classe de definition d'un attenuateur.
 */
class TYAttenuateur: public TYElement
{
    OPROTOSUPERDECL(TYAttenuateur, TYElement)
    TY_EXTENSION_DECL_ONLY(TYAttenuateur)

    // Methodes
public:
    /**
     * Constructeur.
     */
    TYAttenuateur();

    /**
     * Constructeur par copie.
     */
    TYAttenuateur(const TYAttenuateur& other);

    /**
     * Destructeur.
     */
    virtual ~TYAttenuateur();

    ///Operateur =.
    TYAttenuateur& operator=(const TYAttenuateur& other);

    ///Operateur ==.
    bool operator==(const TYAttenuateur& other) const;

    ///Operateur !=.
    bool operator!=(const TYAttenuateur& other) const;

    virtual bool deepCopy(const TYElement* pOther, bool copyId = true);

    virtual std::string toString() const;

    virtual DOM_Element toXML(DOM_Element& domElement);
    virtual int fromXML(DOM_Element domElement);

    /**
     * Set/Get du type d'attenuateur.
     */
    int getType() const { return _type; }

    /**
     * Set/Get du type d'attenuateur.
     */
    void setType(int type) { _type = type; }

    /**
     * Set/Get du spectre d'attenuation.
     */
    TYSpectre& getSpectreAtt() { return _spectreAtt; }

    /**
     * Set/Get du spectre d'attenuation.
     */
    const TYSpectre& getSpectreAtt() const { return _spectreAtt; }

    /**
     * Set/Get du spectre d'attenuation.
     */
    void setSpectreAtt(const TYSpectre& att) { _spectreAtt = att; _spectreAtt.setParent(this); }


    // Membres
protected:
    ///Type d'attenuateur.
    int _type;

    ///Spectre d'attenuation.
    TYSpectre _spectreAtt;
};


#endif // __TY_ATTENUATEUR__

