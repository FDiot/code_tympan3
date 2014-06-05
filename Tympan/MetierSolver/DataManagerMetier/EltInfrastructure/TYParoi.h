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

#ifndef __TY_PAROI__
#define __TY_PAROI__


#include "Tympan/MetierSolver/DataManagerCore/TYElement.h"
#include "Tympan/MetierSolver/DataManagerMetier/EltMateriaux/TYMateriauConstruction.h"

/**
 * Comprend les proprietes acoustiques et geometriques d'une paroi.
 */
class TYParoi: public TYElement
{
    OPROTODECL(TYParoi)
    TY_EXTENSION_DECL_ONLY(TYParoi)

    // Methodes
public:
    /**
     * Constructeur.
     */
    TYParoi();
    /**
     * Constructeur par copie.
     */
    TYParoi(const TYParoi& other);
    /**
     * Destructeur.
     */
    virtual ~TYParoi();

    ///Operateur =.
    TYParoi& operator=(const TYParoi& other);
    ///Operateur ==.
    bool operator==(const TYParoi& other) const;
    ///Operateur !=.
    bool operator!=(const TYParoi& other) const;

    virtual bool deepCopy(const TYElement* pOther, bool copyId = true);

    virtual std::string toString() const;

    virtual DOM_Element toXML(DOM_Element& domElement);
    virtual int fromXML(DOM_Element domElement);

    /**
     * Set/Get de l'epaisseur.
     */
    double getEpaisseur() const { return _epaisseur; }

    /**
     * Set/Get de l'epaisseur.
     */
    void setEpaisseur(double epaisseur) { _epaisseur = epaisseur; }

    /**
     * Set/Get du materiau interne.
     */
    TYMateriauConstruction* getMatStruct() { return _matStruct._pObj; }

    /**
     * Set/Get du materiau interne.
     */
    const TYMateriauConstruction* getMatStruct() const { return _matStruct._pObj; }

    /**
     * Set/Get du materiau interne.
     */
    void setMatStruct(const LPTYMateriauConstruction mat) { _matStruct = mat; _matStruct->setParent(this); }

    /**
     * Set/Get du materiau face 1.
     */
    TYMateriauConstruction* getMatFace1() { return _matFace1._pObj; }

    /**
     * Set/Get du materiau face 1.
     */
    const TYMateriauConstruction* getMatFace1() const { return _matFace1._pObj; }

    /**
     * Set/Get du materiau face 1.
     */
    void setMatFace1(const LPTYMateriauConstruction& mat) { _matFace1 = mat; _matFace1->setParent(this); }

    /**
     * Set/Get du materiau face 2.
     */
    TYMateriauConstruction* getMatFace2() { return _matFace2._pObj; }

    /**
     * Set/Get du materiau face 2.
     */
    const TYMateriauConstruction* getMatFace2() const { return _matFace2._pObj; }

    /**
     * Set/Get du materiau face 2.
     */
    void setMatFace2(const LPTYMateriauConstruction& mat) { _matFace2 = mat; _matFace2->setParent(this); }

    // Membres
protected:
    ///Epaisseur
    double _epaisseur;

    ///Materiau interne
    LPTYMateriauConstruction _matStruct;
    ///Materiau face 1 (interieure)
    LPTYMateriauConstruction _matFace1;
    ///Materiau face 2 (exterieure)
    LPTYMateriauConstruction _matFace2;
};
#endif // __TY_PAROI__
