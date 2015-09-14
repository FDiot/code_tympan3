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

#ifndef __TY_ETAPE__
#define __TY_ETAPE__

#include "Tympan/models/common/3d.h"
#include "Tympan/models/common/spectre.h"
#include "Tympan/models/common/acoustic_path.h"

#include <vector>
#include <deque>


/**
 * \file TYEtape.h
 * \class TYEtape
 * \brief La classe Etape represente une portion d'un Chemin pour le calcul des Trajets entre Source et Recepteur
 * \date 2008/01/21
 * \version v 1.1
 * \author Projet_Tympan
 * A un point de depart est associe une absorption (significative pour
 * le 1er point d'un Chemin, associe a une source ponctuelle et sa
 * directivite et a un point de reflexion sur le sol).
 * Une attenuation est aussi associee a la distance effectuee par
 * l'Etape. Celle-ci est influence par le milieu parcouru (atmosphere,
 * foret...).
 */
class TYEtape
{
    // Methodes
public:
    /**
     *\fn TYEtape()
     *\brief Constructeur.
     * Constructeur de la classe TYEtape
     */
    TYEtape() : _spectrum(OSpectreComplex(TYComplex(1.,0.))) {}
    /**
     * \fn TYEtape(const TYEtape& other)
     * \brief Constructeur par copie
     * Constructeur par copie de la classe TYEtape.
     */
    TYEtape(const TYEtape& other);
    /**
     *
     * \fn virtual ~TYEtape()
     * \brief Destructeur
     * Destructeur de la classe TYEtape.
     */
    virtual ~TYEtape();


    ///Operateur =.
    TYEtape& operator=(const TYEtape& other);
    ///Operateur ==.
    bool operator==(const TYEtape& other) const;
    ///Operateur !=.
    bool operator!=(const TYEtape& other) const;

    /**
     * \fn  OPoint3D getPoint()
     *      void setPoint(const OPoint3D & pt)
     * \brief Set/Get du point de depart associe a cette etape.
     * \return _pt: point de depart associe a l'etape
     */
    OPoint3D getPoint() const { return _pt; }
    /**
     * Set/Get du point de depart associe a cette etape.
     */
    void setPoint(const OPoint3D& pt) { _pt = pt; }

    /**
     * \fn  OSpectreComplex getAbsorption()
     *      const OSpectreComplex getAbsorption()
     *      void setAbsorption(const OSpectreComplex & Abso)
     * \brief Set/Get du spectre d'absorption associe au point de depart.
     * \return _Absorption: spectre d'absorption
     */
    OSpectreComplex& getAbsorption() { return _spectrum; }
    const OSpectreComplex& getAbsorption() const { return _spectrum; }
   
    /**
     * Set/Get du spectre d'absorption associe au point de depart.
     */
    void setAbsorption(const OSpectreComplex& Abso) { _spectrum = Abso; }

    /**
     * \fn OSpectre getAttenuation()
     *     const OSpectre getAttenuation()
     *     void setAttenuation(const OSpectre & Att)
     * \brief Set/Get du spectre d'attenuation associe a cette etape.
     * \return _Attenuation: spectre d'attenuation
     */
    OSpectreComplex& getAttenuation() { return _spectrum; }
    const OSpectreComplex& getAttenuation() const { return _spectrum; }

    /**
     * Set/Get du spectre d'attenuation associe a cette etape.
     */
    void setAttenuation(const OSpectre& Att) { _spectrum = Att; }

    // return as acoustic_event (allowing to watch path from source to receptor
    acoustic_event* asEvent() const;

    // Membres
public:
    ///Le point de depart associe a cette etape.
    OPoint3D _pt;
    ACOUSTIC_EVENT_TYPES _type;      /*!< Type d'evenement */
    OSpectreComplex _spectrum;
};


///Collection de TYEtape.
typedef std::deque<TYEtape> TYTabEtape;


#endif // __TY_ETAPE__
