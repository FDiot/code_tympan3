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

#ifndef __O_SPECTRE_COMPLEX__
#define __O_SPECTRE_COMPLEX__

//#include "Tympan/MetierSolver/DataManagerCore/TYElement.h"
#include "OSpectre.h"

/**
 * Permet de stocker des valeurs de puissance accoustique pour differentes frequences.
 *
 * @author Projet_Tympan
 *
 */
class OSpectreComplex : public OSpectre
{
    // Methodes
public:
    /**
     * Constructeur par defaut.
     */
    OSpectreComplex(const double &defModule = TY_SPECTRE_DEFAULT_VALUE, const double &defPhase = 0.0);

     /**
     * Constructeur par defaut avec une valeur par defaut
     */
    OSpectreComplex(const TYComplex& defaultValue);

	/**
     * Constructeur par copie.
     */
    OSpectreComplex(const OSpectreComplex& other);

    /**
     * Constructeur a partir d'un OSpectre qui represente le module du spectre complexe
     */
    OSpectreComplex(const OSpectre& other);

    /**
     * Constructeur a partir de 2 OSpectre : le module et la phase
     */
    OSpectreComplex(const OSpectre& spectre1, const OSpectre& spectre2);

    /**
     * Destructeur.
     */
    virtual ~OSpectreComplex();

    /// Operateur d'affectation
    virtual OSpectreComplex& operator= (const OSpectreComplex& other);

    /// Operateur d'egalite
    virtual bool operator== (const OSpectreComplex& other) const;

    /// Operateur d'inegalite
    virtual bool operator != (const OSpectreComplex& other) const;

    /// Somme de deux spectres complexes
    virtual OSpectreComplex operator + (const OSpectreComplex& spectre) const;

	// Produit de deux spectres complexesen module/phase
    OSpectreComplex operator * (const OSpectreComplex& spectre) const;

	// Computes the operation between a complex spectrum times a spectrum
    OSpectreComplex operator * (const OSpectre& spectre) const;

	/// Operateur de multiplication d'un spectre complexe par un coeff de type double
    OSpectreComplex operator * (const double& coefficient) const;

    /// Multiplication d'un spectre complexe par un scalaire.
    OSpectreComplex multi(const double& coefficient) const;

	// Rapport de deux spectres complexes en module/phase
    virtual OSpectreComplex operator / (const OSpectreComplex& spectre) const;

    /// Set/Get du tableau des valeurs reelles
    virtual double* getTabValReel() { return _module; }
    virtual const double* getTabValReel() const {return _module; }

    /// Set/Get du tableau des valeurs imaginaires
    virtual double* getTabValImag() { return _phase; }
    virtual const double* getTabValImag() const { return _phase; }

    /**
     * Attribution d'une valeur complexe au tableau Frequence/Complexe.
     *
     * @param freq Frequence pour laquelle on attribut une nouvelle valeur.
     * @param reel partie reelle.
     * @param imag partie imaginaire
     */
    virtual void setValue(const float& freq, const double& reel, const double& imag = 0.0);

    /**
     * Attribution d'une valeur complexe au tableau Frequence/Complexe.
     *
     * @param freq Frequence pour laquelle on attribut une nouvelle valeur.
     * @param cplx un TYComplex
     */
    virtual void setValue(const float& freq, const TYComplex& cplx);

    /**
     * Recuperation d'une valeur imaginaire au tableau Frequence/Complexe
     * a partir d'une frequence.
     *
     * @param freq Frequence pour laquelle on recherche la valeur.
     * @param pValid Positionner a false si la frequence passee n'est pas valide pour ce spectre.
     *
     * @return La valeur imaginaire du complexe correspondant.
     */
    virtual double getValueImag(float freq, bool* pValid = 0);

    /**
     * Attribution de la phase a un spectre.
     * @param Un OSpectre.
     */
    virtual void setPhase(const OSpectre& spectre);

    /**
     * Attribution de la phase a un spectre.
     * @param Un double.
     */
    virtual void setPhase(const double& valeur = 0.0);

    /**
     * Lecture de la phase d'un spectre.
     * @return Un TYSpectre.
     */
    virtual OSpectre getPhase() const;

    /**
    * \fn OSpectre getModule() const
    * \brief get du module du spectre
    */
    virtual OSpectre getModule() const;

    /// Conversion en module/phase
    virtual OSpectreComplex toModulePhase() const;

    /// Conversion en module/phase
    virtual OSpectreComplex toReelImaginaire() const;

    /// Somme le spectre en complexe avec un autre spectre complexe
    virtual OSpectreComplex sumComplex(const OSpectreComplex& spectre) const;

    /// Somme d'un spectre complexe avec un spectre "normal"
    virtual OSpectreComplex sumComplex(const OSpectre& spectre) const;

    // === FONCTIONS MEMBRES STATIQUES

    /// Cree un spectre complexe
    static OSpectreComplex getCplxSpectre(const double& valInit = 1.0E-20);

    // Membres
public:

protected:
    /// tableau des valeurs imaginaires
    double _phase[TY_SPECTRE_DEFAULT_NB_ELMT];
};

typedef std::vector<OSpectreComplex> OTabSpectreComplex;
typedef std::vector<std::vector<OSpectreComplex> > OTab2DSpectreComplex;

#endif // __O_SPECTRE_COMPLEX__
