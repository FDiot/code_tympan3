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

#ifndef __TY_DIRECTIVITE__
#define __TY_DIRECTIVITE__


#include "Tympan/MetierSolver/DataManagerCore/TYElement.h"


///Permet d'associer une valeur a un couple d'angle.
typedef struct TYDirectiviteAnglesValue
{
    ///L'angle d'incidence theta.
    double _theta;
    ///L'angle d'incidence phi.
    double _phi;
    ///Le coefficient de directivite.
    double _val;

    ///Operateur de copie.
    TYDirectiviteAnglesValue& operator=(const TYDirectiviteAnglesValue& other)
    {
        if (this != &other)
        {
            _theta = other._theta;
            _phi = other._phi;
            _val = other._val;
        }
        return *this;
    }
    ///Operateur de comparaison.
    bool operator==(const TYDirectiviteAnglesValue& other) const
    {
        if (this != &other)
        {
            if (_theta != other._theta) { return false; }
            if (_phi != other._phi) { return false; }
            if (_val != other._val) { return false; }
        }
        return true;
    }
} TYDirectiviteAnglesValue;


///Collection d'associations Angles-Valeur.
typedef std::vector<TYDirectiviteAnglesValue> TYDirectiviteAnglesValueTab;


/**
 * Classe pour definir la directivite d'une source acoustique ponctuelle.
 */
class TYDirectivite: public TYElement
{
    OPROTOSUPERDECL(TYDirectivite, TYElement)
    TY_EXTENSION_DECL_ONLY(TYDirectivite)

    // Methodes
public:
    /**
     * Constructeur.
     */
    TYDirectivite();
    /**
     * Constructeur par copie.
     */
    TYDirectivite(const TYDirectivite& other);
    /**
     * Destructeur.
     */
    virtual ~TYDirectivite();

    ///Operateur =.
    TYDirectivite& operator=(const TYDirectivite& other);
    ///Operateur ==.
    bool operator==(const TYDirectivite& other) const;
    ///Operateur !=.
    bool operator!=(const TYDirectivite& other) const;

    virtual bool deepCopy(const TYElement* pOther, bool copyId = true);

    virtual std::string toString() const;

    virtual DOM_Element toXML(DOM_Element& domElement);
    virtual int fromXML(DOM_Element domElement);

    /**
     * Set/Get de la collection d'associations Angles-Valeur.
     */
    TYDirectiviteAnglesValueTab& getTabAnglesVal() { return _tabAnglesVal; }
    /**
     * Set/Get de la collection d'associations Angles-Valeur.
     */
    const TYDirectiviteAnglesValueTab& getTabAnglesVal() const { return _tabAnglesVal; }
    /**
     * Set/Get de la collection d'associations Angles-Valeur.
     */
    void setTabAnglesVal(const TYDirectiviteAnglesValueTab& tab) { _tabAnglesVal = tab; }


    // Membres
protected:
    ///Collection d'associations Angles-Valeur.
    TYDirectiviteAnglesValueTab _tabAnglesVal;
};


#endif // __TY_DIRECTIVITE__
