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

#ifndef __TY_MATERIAUCONSTRUCTION__
#define __TY_MATERIAUCONSTRUCTION__

#include "Tympan/MetierSolver/DataManagerCore/TYElement.h"
#include "Tympan/MetierSolver/DataManagerMetier/ComposantAcoustique/TYSpectre.h"

class TYMateriauConstruction: public TYElement
{
    OPROTODECL(TYMateriauConstruction)
    TY_EXTENSION_DECL_ONLY(TYMateriauConstruction)

    // Methodes
public:
    /**
     * Constructeur.
     */
    TYMateriauConstruction();
    /**
     * Constructeur par copie.
     */
    TYMateriauConstruction(const TYMateriauConstruction& other);
    /**
     * Destructeur.
     */
    virtual ~TYMateriauConstruction();

    ///Operateur =.
    TYMateriauConstruction& operator=(const TYMateriauConstruction& other);
    ///Operateur ==.
    bool operator==(const TYMateriauConstruction& other) const;
    ///Operateur !=.
    bool operator!=(const TYMateriauConstruction& other) const;

    virtual bool deepCopy(const TYElement* pOther, bool copyId = true);

    virtual std::string toString() const;

    virtual DOM_Element toXML(DOM_Element& domElement);
    virtual int fromXML(DOM_Element domElement);

    /**
     * Set/Get de la masse volumique.
     */
    double getMasseVol() const { return _masseVol; }
    /**
     * Set/Get de la masse volumique.
     */
    void setMasseVol(double mv) { _masseVol = mv; }

    /**
     * Set/Get du spectre de transmission.
     */
    TYSpectre& getSpectreTransm() { return _spectreTransm; }
    /**
     * Set/Get du spectre de transmission.
     */
    const TYSpectre& getSpectreTransm() const { return _spectreTransm; }
    /**
     * Set/Get du spectre de transmission.
     */
    void setSpectreTransm(const TYSpectre& transm) { _spectreTransm = transm; _spectreTransm.setParent(this); }

    /**
     * Set/Get du spectre d'absorption.
     */
    TYSpectre& getSpectreAbso() { return _spectreAbso; }
    /**
     * Set/Get du spectre d'absorption.
     */
    const TYSpectre& getSpectreAbso() const { return _spectreAbso; }

    /**
     * Calcul du spectre d'absorption de Eyring
     */

    TYSpectre getSpectreAbsoEyring()const;

    /**
     * Set/Get du spectre d'absorption.
     */
    void setSpectreAbso(const TYSpectre& abso) { _spectreAbso = abso; _spectreAbso.setParent(this); }

private:
    static void InitializeSpectralDatas();
    static TYSpectre _getDefSpectreTrans();
    static TYSpectre _getDefSpectreAbso();
    static TYSpectre* _defSpectreTrans;
    static TYSpectre* _defSpectreAbso;

    // Membres
protected:
    ///Masse Volumique.
    double _masseVol;
    ///Spectre de transmission.
    TYSpectre _spectreTransm;
    ///Spectre d'absorption.
    TYSpectre _spectreAbso;

    /// Spectre de transmission par defaut
    static TYSpectre* getDefSpectreTrans();
    /// Spectre de Absortion par defaut
    static TYSpectre* getDefSpectreAbso();
};


#endif // __TY_MATERIAUCONSTRUCTION__
