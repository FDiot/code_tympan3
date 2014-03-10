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

#ifndef __TY_DALLE__
#define __TY_DALLE__


#include "Tympan/MetierSolver/DataManagerMetier/ComposantGeoAcoustique/TYAcousticPolygon.h"
#include "TYParoi.h"


/**
 * Classe de definition d'une dalle.
 * Pour la representation des sols et plafonds.
 */
class TYDalle: public TYAcousticPolygon
{
    OPROTOSUPERDECL(TYDalle, TYAcousticPolygon)
    TY_EXTENSION_DECL_ONLY(TYDalle)
    TY_EXT_GRAPHIC_DECL_ONLY(TYDalle)

    // Methodes
public:
    /**
     * Constructeur par defaut.
     */
    TYDalle();
    /**
     * Constructeur par copie.
     */
    TYDalle(const TYDalle& other);
    /**
     * Destructeur.
     */
    virtual ~TYDalle();

    ///Operateur =.
    TYDalle& operator=(const TYDalle& other);
    ///Operateur ==.
    bool operator==(const TYDalle& other) const;
    ///Operateur !=.
    bool operator!=(const TYDalle& other) const;

    virtual bool deepCopy(const TYElement* pOther, bool copyId = true);

    virtual std::string toString() const;

    virtual DOM_Element toXML(DOM_Element& domElement);
    virtual int fromXML(DOM_Element domElement);

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
    void setParoi(const LPTYParoi pParoi);

    /**
     * Get/Set de l'etat bloque du type de paroi
     */
    void setParoiLocked(const bool& bVal);
    bool isParoiLocked() { return _bParoiLocked; }
    const bool isParoiLocked() const { return _bParoiLocked; }

    /**
     * \fn  LPTYMateriauConstruction getMateriau()
     * \brief Surcharge de la methode getMateriau pour la dalle
     * \return le materiau de construction de la dalle
     */
    TYMateriauConstruction* getMateriau();

    // Membres
protected:
    ///Une paroi associee a cette dalle.
    LPTYParoi _pParoi;

    /**
     * Indicateur de "blocage" du type de paroi
     * (evite une mise a jour depuis l'etage)
     */
    bool _bParoiLocked;
};


#endif // __TY_DALLE__
