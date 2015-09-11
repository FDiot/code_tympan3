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
     * \fn enum EtapeType
     * \brief Indique le type de l'etape.
     * \param   EtapeInfra:Reflexion sur un element d'infrastructure.
                EtapeTopo: Reflexion sur un element de topographie (sol).
                EtapeForet:Changement de milieu (on entre ou on sort d'une foret).
     */
    enum EtapeType
    {
        EtapeInfra,
        EtapeTopo,
        EtapeForet
    };

    /**
     *\fn TYEtape()
     *\brief Constructeur.
     * Constructeur de la classe TYEtape
     */
    TYEtape();
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
     * \fn int getType()
     *         void setType(int etapeType)
     * \brief Set/Get du type de cette etape.
     * \return _type: type de l'etape
     */
    int getType() const { return _type; }
    /**
     * Set/Get du type de cette etape.
     */
    void setType(int etapeType) { _type = etapeType; }

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
    OSpectreComplex getAbsorption() { return _Absorption; }
    /**
     * Set/Get du spectre d'absorption associe au point de depart.
     */
    const OSpectreComplex getAbsorption() const { return _Absorption; }
    /**
     * Set/Get du spectre d'absorption associe au point de depart.
     */
    void setAbsorption(const OSpectreComplex& Abso) { _Absorption = Abso; }

    /**
     * \fn OSpectre getAttenuation()
     *     const OSpectre getAttenuation()
     *     void setAttenuation(const OSpectre & Att)
     * \brief Set/Get du spectre d'attenuation associe a cette etape.
     * \return _Attenuation: spectre d'attenuation
     */
    OSpectre getAttenuation() { return _Attenuation; }
    /**
     * Set/Get du spectre d'attenuation associe a cette etape.
     */
    const OSpectre getAttenuation() const { return _Attenuation; }
    /**
     * Set/Get du spectre d'attenuation associe a cette etape.
     */
    void setAttenuation(const OSpectre& Att) { _Attenuation = Att; }


    // Membres
public:
    ///Le type de l'etape.
    int _type;

    ///Le point de depart associe a cette etape.
    OPoint3D _pt;

    ///Le spectre d'absorption associe au point de depart.
    OSpectreComplex _Absorption;

    ///Le spectre d'attenuation associe a l'etape.
    OSpectre _Attenuation;

};


///Collection de TYEtape.
typedef std::deque<TYEtape> TYTabEtape;


#endif // __TY_ETAPE__
