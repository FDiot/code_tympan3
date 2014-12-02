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

#ifndef __TY_VEGETATION__
#define __TY_VEGETATION__

#include <qstring.h>
#include "Tympan/models/business/TYElement.h"
#include "Tympan/models/business/acoustic/TYSpectre.h"

/**
 * Classe de definition d'une vegetation.
 */
class TYVegetation: public TYElement
{
    OPROTODECL(TYVegetation)
    TY_EXTENSION_DECL_ONLY(TYVegetation)

    // Methodes
public:
    /**
     * Constructeur.
     */
    TYVegetation();
    /**
     * Constructeur par copie.
     */
    TYVegetation(const TYVegetation& other);
    /**
     * Destructeur.
     */
    virtual ~TYVegetation();

    ///Operateur =.
    TYVegetation& operator=(const TYVegetation& other);
    ///Operateur ==.
    bool operator==(const TYVegetation& other) const;
    ///Operateur !=.
    bool operator!=(const TYVegetation& other) const;

    virtual bool deepCopy(const TYElement* pOther, bool copyId = true);

    virtual std::string toString() const;

    virtual DOM_Element toXML(DOM_Element& domElement);
    virtual int fromXML(DOM_Element domElement);

    /*!
     * \brief Get/Set de l'existence de feuillage
     */
    void setFoliageStatus(bool bStatus) { _bFoliage = bStatus; }
    bool getFoliageStatus() const { return _bFoliage; }

    /**
     * Set/Get de la hauteur.
     */
    double getHauteur() const { return _hauteur; }
    /**
     * Set/Get de la hauteur.
     */
    void setHauteur(double haut) { _hauteur = haut; }

    /**
     * Set/Get du spectre d'attenuation.
     */
    TYSpectre* getSpectreAtt() { return _pSpectreAtt; }
    /**
     * Set/Get du spectre d'attenuation.
     */
    const TYSpectre* getSpectreAtt() const { return _pSpectreAtt; }
    /**
     * Set/Get du spectre d'attenuation.
     */
    void setSpectreAtt(TYSpectre* pAtt) ;

public:
    static QString _vegeName[];
    static unsigned int getIndexVegetation(const QString &vegeName);

    // Membres
protected:
    /// Foliage
    bool _bFoliage;
    ///Hauteur.
    double _hauteur;
    ///Spectre d'attenuation.
    TYSpectre* _pSpectreAtt;
};


#endif // __TY_VEGETATION__
