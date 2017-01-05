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

#ifndef __TY_CHEMIN__
#define __TY_CHEMIN__


#include <deque>
#include "TYEtape.h"
#include "Tympan/models/common/atmospheric_conditions.h"
#include "Tympan/models/common/acoustic_path.h"

/**
 * \file TYChemin.h
 * \brief Representation of one of the most optimal path between source and receptor: S--->R
 * \author Projet_Tympan
 * \version v1.1
 */

typedef enum {CHEMIN_DIRECT, CHEMIN_SOL, CHEMIN_ECRAN, CHEMIN_REFLEX} TYTypeChemin;
/**
 * \class TYChemin
 *  \brief Representation of one of the most optimal path between source and receptor: S--->R.
 *  The class TYChemin represents a path between a Source and a receptor (Recepteur class). It's constituted of a collection of steps (TYEtape class).
 */
class TYChemin
{
    // Methods
public:
    /**
     * \fn TYChemin()
     *\brief Constructor
     */
    TYChemin();
    /**
     * \fn TYChemin(const TYChemin& other)
     *\brief Copy contructor
     */
    TYChemin(const TYChemin& other);
    /**
     * \fn virtual ~TYChemin()
     * \brief Destructor
     */
    virtual ~TYChemin();

    ///Operator =.
    TYChemin& operator=(const TYChemin& other);
    ///Operator ==.
    bool operator==(const TYChemin& other) const;
    ///Operator !=.
    bool operator!=(const TYChemin& other) const;

    /**
     * \fn void calcAttenuation(const TYTabEtape& tabEtapes, const AtmosphericConditions & atmos)
     * \brief Compute the global attenuation on the path
     */
    void calcAttenuation(const TYTabEtape& tabEtapes, const AtmosphericConditions& atmos);

    /**
     * \fn OSpectreComplex& getAttenuation()
     *     const OSpectreComplex& getAttenuation()
     * \brief Return the path attenuation
     * \return _attenuation
     */
    OSpectreComplex& getAttenuation() { return _attenuation; }
    const OSpectreComplex& getAttenuation() const { return _attenuation; }

    /**
     * \fn void setAttenuation (const OSpectreComplex& att)
     * \brief Set the attenuation
     */
    void setAttenuation(const OSpectreComplex& att) { _attenuation = att; }
    /**
    * \fn double getLongueur()
    *     const double getLongueur()
    *     void setLongueur(const double & longueur)
    *\brief Get/Set the path length
    *\return _longueur
    */
    double getLongueur() { return _longueur; }
    const double getLongueur() const { return _longueur; }

    void setLongueur(const double& longueur) { _longueur = longueur; }

    /**
    * \fn   double getDistance()
    *       const double getDistance()
    *       void setDistance(const double & distance)
    * \brief Get/Set the distance between source and receptor
    *\return _distance
    *
    */
    double getDistance() { return _distance; }
    const double getDistance() const { return _distance; }

    void setDistance(const double& distance) { _distance = distance; }

    /**
     *\fn void setType(const int& type)
     *\brief Change the path type
     */
    void setType(const int& type) { _typeChemin = (TYTypeChemin)type; } const

    /**
     * \fn int getType()
     * \brief Return the path type
     * \return _typeChemin
     */
    int getType() const { return _typeChemin; }

    /*!
     * \fn void build_eq_path();
     * \brief Build an acoustic_path from the an array tab of steps
     */
    void build_eq_path(const TYTabEtape& tabEtapes);

    acoustic_path* get_ray() { return _eq_path; }

    // Members
protected:
    /// Path type (has an influence on the algorithm)
    TYTypeChemin _typeChemin;

    /// Total path length
    double _longueur;

    /// Direct distance between source and receptor
    double _distance;

    /// Attenuation spectrum of the path
    OSpectreComplex _attenuation;

    /// Equivalent acoustic_path
    acoustic_path *_eq_path;
};


///TYChemin collection
typedef std::deque<TYChemin> TYTabChemin;



#endif // __TY_CHEMIN__
