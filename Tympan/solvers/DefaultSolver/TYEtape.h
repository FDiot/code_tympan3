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
 * \brief The TYEtape class is used to describe a part (a step) of a path (TYChemin) for the computation of trajectories (TYTrajet) between Source and receptor (Recepteur)
 * \date 2008/01/21
 * \version v 1.1
 * \author Projet_Tympan
 * To a first point of the step is associated an absorption (this
 * one is significant for the first point of the path, which is associated
 * to a source, a directivity and a ground reflection point).
 * A attenuation is also associated, depending of the step length. It is related
 * to the encountered medium (atmosphere, forest, ...).
 */
class TYEtape
{
    // Methods
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
     *\brief Constructor
     */
    TYEtape();
    /**
     * \fn TYEtape(const TYEtape& other)
     * \brief Copy constructor
     */
    TYEtape(const TYEtape& other);
    /**
     *
     * \fn virtual ~TYEtape()
     * \brief Destructor
     */
    virtual ~TYEtape();


    ///Operator =.
    TYEtape& operator=(const TYEtape& other);
    ///Operator ==.
    bool operator==(const TYEtape& other) const;
    ///Operator !=.
    bool operator!=(const TYEtape& other) const;
    /**
     * \fn int getType()
     *         void setType(int etapeType)
     * \brief Set/Get du type de cette etape.
     * \return _type: type de l'etape
     */
    ACOUSTIC_EVENT_TYPES getType() const { return _type; }
    /**
     * Set/Get du type de cette etape.
     */
    void setType(ACOUSTIC_EVENT_TYPES etapeType) { _type = etapeType; }

    /**
     * \fn  OPoint3D getPoint()
     *      void setPoint(const OPoint3D & pt)
     * \brief Get/Set of the first point of the step.
     * \return _pt
     */
    OPoint3D getPoint() const { return _pt; }
    void setPoint(const OPoint3D& pt) { _pt = pt; }

    /**
     * \fn  OSpectreComplex getAbsorption()
     *      const OSpectreComplex getAbsorption()
     *      void setAbsorption(const OSpectreComplex & Abso)
     * \brief Get/Set the absorption spectrum associated to the first point of the step.
     * \return _Absorption
     */
    OSpectreComplex getAbsorption() { return _Absorption; }
    const OSpectreComplex getAbsorption() const { return _Absorption; }
    /**
     * Set/Get du spectre d'absorption associe au point de depart.
     */
    void setAbsorption(const OSpectreComplex& Abso) { _Absorption = Abso; }

    /**
     * \fn OSpectre getAttenuation()
     *     const OSpectre getAttenuation()
     *     void setAttenuation(const OSpectre & Att)
     * \brief Get/Set the attenuation spectrum associated to this step.
     * \return _Attenuation
     */
    OSpectre getAttenuation() { return _Attenuation; }
    const OSpectre getAttenuation() const { return _Attenuation; }
    /**
     * Set/Get du spectre d'attenuation associe a cette etape.
     */
    void setAttenuation(const OSpectre& Att) { _Attenuation = Att; }
	acoustic_event* asEvent() const;

    // Membres
public: 
    ACOUSTIC_EVENT_TYPES _type;                       //!< Acoustic event type
    OPoint3D _pt;                    //!< The starting point of this step
	OSpectreComplex _Absorption;     //!< absorption Spectrum 
    OSpectre _Attenuation;           //!< attenuation Spectrum 
};


///TYEtape collection.
typedef std::deque<TYEtape> TYTabEtape;


#endif // __TY_ETAPE__
