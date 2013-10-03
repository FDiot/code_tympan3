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

#ifndef __TY_VEGETATION__
#define __TY_VEGETATION__

#include "Tympan/MetierSolver/DataManagerCore/TYElement.h"
#include "Tympan/MetierSolver/DataManagerMetier/ComposantAcoustique/TYSpectre.h"
#include "Tympan/MetierSolver/DataManagerMetier/ComposantGeometrique/TYSegment.h"
#include "TYAtmosphere.h"

#if TY_USE_IHM
#include "Tympan/GraphicIHM/DataManagerIHM/TYVegetationWidget.h"
#endif

/**
 * Classe de definition d'une vegetation.
 *
 * @author Projet_Tympan
 *
 */
class TYVegetation: public TYElement
{
    OPROTOSUPERDECL(TYVegetation, TYElement)
    TY_EXTENSION_DECL(TYVegetation)

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

    /**
     * Calcul du spectre d'attenuation pour le segment seg.
     *
     * @param seg Segment pour lequel est fourni le spectre d'attenuation.
     */
    OSpectre getAtt(const OSegment3D& seg, const TYAtmosphere& Atmo) const;

    /**
     *
     * Calcul du spectre d'attenuation sur une distance donnee
     *
     * @param distance Longueur du trajet parcouru dans la foret
     */
    OSpectre getAtt(const double& distance, const TYAtmosphere& Atmo) const;


    // Membres
protected:
    ///Hauteur.
    double _hauteur;
    ///Spectre d'attenuation.
    TYSpectre* _pSpectreAtt;
};


#endif // __TY_VEGETATION__
