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

#ifndef __TY_CHEMIN__
#define __TY_CHEMIN__


#include <deque>
#include "TYEtape.h"


/**
 * \file TYChemin.h
 * \brief representation un des parcours le plus optimal de S--->R
 * \author Projet_Tympan
 * \version v1.1
 */

typedef enum {CHEMIN_DIRECT, CHEMIN_SOL, CHEMIN_ECRAN, CHEMIN_REFLEX} TYTypeChemin;
/**
 * \class TYChemin
 *  \brief representation un des parcours le plus optimal de S--->R
 *  La classe Chemin permet de representer un des parcours entre une Source et un Recepteur. Elle est principalement composee d'une collection d'Etapes.
 */
class TYChemin
{
    // Methodes
public:
    /**
     * \fn TYChemin()
     *\brief Constructeur.
     *\ constucteur de la classe TYChemin
     */
    TYChemin();
    /**
     * \fn TYChemin(const TYChemin& other)
     *\brief Constructeur par copie.
     *\ constucteur de la classe TYChemin
     */
    TYChemin(const TYChemin& other);
    /**
     * \fn virtual ~TYChemin()
     * \brief Destructeur.
     * Deconstructeur de la classe TYChemin
     */
    virtual ~TYChemin();

    ///Operateur =.
    TYChemin& operator=(const TYChemin& other);
    ///Operateur ==.
    bool operator==(const TYChemin& other) const;
    ///Operateur !=.
    bool operator!=(const TYChemin& other) const;

    /**
     * \fn void calcAttenuation(const TYTabEtape& tabEtapes, const TYAtmosphere & atmos)
     * \brief Calcule l'attenuation globale du chemin
     */
    void calcAttenuation(const TYTabEtape& tabEtapes, const TYAtmosphere& atmos);

    /**
     * \fn OSpectreComplex& getAttenuation()
     *     const OSpectreComplex& getAttenuation()
     * \brief Retourne l'attenuation du chemin
     * \return _attenuation
     */
    OSpectreComplex& getAttenuation() { return _attenuation; }
    const OSpectreComplex& getAttenuation() const { return _attenuation; }

    /**
     * \fn void setAttenuation (const OSpectreComplex& att)
     * \brief Set de l'attenuation
     */
    void setAttenuation(const OSpectreComplex& att) { _attenuation = att; }
    /**
    * \fn double getLongueur()
    *     const double getLongueur()
    *     void setLongueur(const double & longueur)
    *\brief Get/Set de la longueur du chemin
    *\return _longueur
    */
    double getLongueur() { return _longueur; }
    const double getLongueur() const { return _longueur; }

    void setLongueur(const double& longueur) { _longueur = longueur; }

    /**
    * \fn   double getDistance()
    *       const double getDistance()
    *       void setDistance(const double & distance)
    * \brief Get/Set de la distance de la source au recepteur
    *\return _distance
    *
    */
    double getDistance() { return _distance; }
    const double getDistance() const { return _distance; }

    void setDistance(const double& distance) { _distance = distance; }

    /**
     *\fn void setType(const int& type)
     *\brief Change le type du chemin
     */
    void setType(const int& type) { _typeChemin = (TYTypeChemin)type; } const

    /**
     * \fn int getType()
     * \brief Retourne le type du chemin
     * \return _typeChemin
     */
    int getType() const { return _typeChemin; }
    // Membres
protected:
    ///Le type de chemin (influe sur le traitement)
    TYTypeChemin _typeChemin;

    /// Longueur totale du chemin
    double _longueur;

    /// Distance (a vol d'oiseau) de la source au recepteur
    double _distance;

    /// Spectre d'attenuation du chemin
    OSpectreComplex _attenuation;
};


///Collection de TYChemin.
typedef std::deque<TYChemin> TYTabChemin;



#endif // __TY_CHEMIN__
