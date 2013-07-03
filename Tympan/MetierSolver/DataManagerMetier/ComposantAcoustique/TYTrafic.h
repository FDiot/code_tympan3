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

#ifndef __TY_TRAFIC__
#define __TY_TRAFIC__


#include "Tympan/MetierSolver/DataManagerCore/TYElement.h"

#if TY_USE_IHM
#include "Tympan/GraphicIHM/DataManagerIHM/TYTraficWidget.h"
#endif


/**
 * Structure de trafic routier.
 *
 * @author Projet_Tympan
 * @author Projet_Tympan
 *
 */
class TYTrafic: public TYElement
{
    OPROTOSUPERDECL(TYTrafic, TYElement)
    TY_EXTENSION_DECL(TYTrafic)

    // Methodes
public:
    /**
     * Constructeur par defaut.
     */
    TYTrafic();
    /**
     * Constructeur par copie.
     */
    TYTrafic(const TYTrafic& other);
    /**
     * Constructeur.
     */
    TYTrafic(double debitVL, double debitPL);
    /**
     * Destructeur.
     */
    virtual ~TYTrafic();

    ///Operateur =.
    TYTrafic& operator=(const TYTrafic& other);
    ///Operateur ==.
    bool operator==(const TYTrafic& other) const;
    ///Operateur !=.
    bool operator!=(const TYTrafic& other) const;

    virtual bool deepCopy(const TYElement* pOther, bool copyId = true);

    virtual std::string toString() const;

    virtual DOM_Element toXML(DOM_Element& domElement);
    virtual int fromXML(DOM_Element domElement);

    /**
     * Set/Get du debit voiture.
     */
    double getDebitPL() const { return _debitPL; }
    /**
     * Set/Get du debit voiture.
     */
    void setDebitPL(double deb) { _debitPL = deb; }

    /**
     * Set/Get du debit camion.
     */
    double getDebitVL() const { return _debitVL; }
    /**
     * Set/Get du debit camion.
     */
    void setDebitVL(double deb) { _debitVL = deb; }


    // Membres
protected:
    ///Debit voiture.
    double _debitVL;
    ///Debit camion.
    double _debitPL;
};


#endif // __TY_TRAFIC__

