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
     *\fn TYEtape()
     *\brief Constructor
     */
    TYEtape() {}
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
    OSpectreComplex& getAbsorption() { return _spectrum; }
    const OSpectreComplex& getAbsorption() const { return _spectrum; }

    void setAbsorption(const OSpectreComplex& Abso) { _spectrum = Abso; }

    /**
     * \fn OSpectre getAttenuation()
     *     const OSpectre getAttenuation()
     *     void setAttenuation(const OSpectre & Att)
     * \brief Get/Set the attenuation spectrum associated to this step.
     * \return _Attenuation
     */
    OSpectreComplex& getAttenuation() { return _spectrum; }
    const OSpectreComplex& getAttenuation() const { return _spectrum; }

    void setAttenuation(const OSpectre& Att) { _spectrum = Att; }

    /// return as acoustic_event (allowing to watch path from source to receptor)
    acoustic_event* asEvent() const;

    // Membres
public:
    OPoint3D _pt;                    //!< The first point of this step
    ACOUSTIC_EVENT_TYPES _type;      //!< Acoustic event type
    OSpectreComplex _spectrum;       //!< Spectrum
};


///TYEtape collection.
typedef std::deque<TYEtape> TYTabEtape;


#endif // __TY_ETAPE__
