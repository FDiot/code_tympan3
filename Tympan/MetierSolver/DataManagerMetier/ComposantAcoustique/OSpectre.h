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

#ifndef __O_SPECTRE__
#define __O_SPECTRE__

#include "Tympan/MetierSolver/DataManagerCore/TYElement.h"

///Type de spectre.
enum TYSpectreType { SPECTRE_TYPE_ATT, SPECTRE_TYPE_ABSO, SPECTRE_TYPE_LW, SPECTRE_TYPE_LP, SPECTRE_TYPE_AUTRE };
///Representation du spectre.
enum TYSpectreForm { SPECTRE_FORM_TIERS, SPECTRE_FORM_OCT, SPECTRE_FORM_CST_DF, SPECTRE_FORM_UNSHAPED };
///Etat du spectre (dB/grandeur physique).
enum TYSpectreEtat { SPECTRE_ETAT_DB, SPECTRE_ETAT_LIN };


//Nombre de frequences.
static const unsigned int TY_SPECTRE_DEFAULT_NB_ELMT = 31;

///Valeur par defaut pour les spectres.
static const double TY_SPECTRE_DEFAULT_VALUE = -200.0;

///Collection des frequences
typedef std::vector<float> OTabFreq;


/**
 * Structure de spectre allegee. Utilisee uniquement pour le stockage dans la matrice de transfert
 *
 * @author Projet_Tympan
 *
 */

struct TYSpectreLeger
{
    float _module[TY_SPECTRE_DEFAULT_NB_ELMT];

    bool operator == (const TYSpectreLeger& other) const
    {
        bool res = true;

        for (unsigned int i = 0; i < TY_SPECTRE_DEFAULT_NB_ELMT; i++)
        {
            if (_module[i] != other._module[i])
            {
                res = false;
                break;
            }
        }

        return res;
    }
    bool operator != (const TYSpectreLeger& other) const
    {
        return !(*this == other);
    }

};

/**
 * Permet de stocker des valeurs de puissance accoustique pour differentes frequences.
 *
 * @author Projet_Tympan
 *
 */
class OSpectre
{
    // Methodes
public:
    /**
     * Constructeur par defaut.
     */
    OSpectre();

    /**
     * Constructeur par defaut avec une valeur par defaut
     */
    OSpectre(double defaultValue);

    /**
     * Constructeur a partir d'un tableau de valeurs (besoin pour harmonoise)
     * valeur ==> Tableau des valeurs par frequence
     * nbVal  ==> Nombre de valeurs dans le tableau
     * decalage ==> decalage en frequence par rapport a la bande standard TYMPAN (16-16000)
     */
    OSpectre(const double* valeurs, const short& nbVal, const short& decalage);

    /**
     * Constructeur par copie.
     */
    OSpectre(const OSpectre& other);

    /**
     * Destructeur.
     */
    virtual ~OSpectre();

    /// Operateur d'affectation
    virtual OSpectre& operator= (const OSpectre& other);

    /// Operateur d'egalite
    virtual bool operator== (const OSpectre& other) const;

    /// Operateur d'inegalite
    virtual bool operator != (const OSpectre& other) const;

    /// operateur de multiplication par un Spectre
    virtual OSpectre operator * (const OSpectre& spectre) const;

    /// operateur de multiplication par un coeff de type double
    virtual OSpectre operator * (const double& coefficient) const;

    /// Operateur d'addition pour ajouter une valeur constante a l'ensemble du spectre
    virtual OSpectre operator + (const double& valeur) const;

    /// Sommation arithmetique de deux spectres en 1/3 d'octave.
    virtual OSpectre operator + (const OSpectre& spectre) const;

    /// Soustraction arithmetique de deux spectres en 1/3 d'octave.
    virtual OSpectre operator - (const OSpectre& spectre) const;

    /**
     * Verification de la validite d'un spectre.
     * L'invalidite est causee par : donnees corrompues, calcul impossible.
     *
     * @return Etat du spectre (valide = true / invalide = false).
     */
    virtual bool isValid() const { return _valid; }

    /**
     * Force l'etat de validite du spectre
     */
    virtual void setValid(const bool& valid = true) { _valid = valid; }

    /// Set/Get du type de spectre.
    virtual TYSpectreType getType() const { return _type; }

    /// Set/Get du type de spectre.
    virtual void setType(TYSpectreType type) { _type = type; }

    /// Get de l'etat du Spectre.
    virtual TYSpectreEtat getEtat() const {return _etat; } ;
    virtual const TYSpectreEtat getEtat() {return _etat;};

    /// Force l'etat du spectre (a utiliser avec prudence ...)
    void setEtat(TYSpectreEtat etat) { _etat = etat; }

    /// Set/Get du tableau des valeurs reelles
    virtual double* getTabValReel() { return _module; }
    virtual const double* getTabValReel() const {return _module; }

    /// Nombre de valeurs dans le spectre
    virtual unsigned int getNbValues();

    virtual const unsigned int getNbValues() const ;

    /**
     * Initialisation d'un spectre a une valeur.
     * @param valeur Valeur par defaut.
     */
    virtual void setDefaultValue(const double& valeur = TY_SPECTRE_DEFAULT_VALUE);

    /**
     * Attribution d'une valeur complexe au tableau Frequence/Complexe.
     *
     * @param freq Frequence pour laquelle on attribut une nouvelle valeur.
     * @param reel partie reelle.
     * @param imag partie imaginaire
     */
    virtual void setValue(const float& freq, const double& reel = 0.0);

    /**
     * Recuperation d'une valeur reelle au tableau Frequence/Complexe
     * a partir d'une frequence.
     *
     * @param freq Frequence pour laquelle on recherche la valeur.
     * @param pValid Positionner a false si la frequence passee n'est pas valide pour ce spectre.
     *
     * @return La valeur reelle du complexe correspondant.
     */
   virtual  double getValueReal(float freq, bool* pValid = 0);

    /**
    * Recuperation d'un intervalle de valeurs reelles au tableau frequence/complexe
    *
    * @param[out] valeurs Tableau des valeurs. Le tableau doit avoir ete alloue avant l'appel
    * @param[in] nbVal Nombre de valeurs a recuperer dans le spectre
    * @param[in] decalage Indice de la premiere valeur a recuperer
    */
    virtual void getRangeValueReal(double* valeurs, const short& nbVal, const short& decalage);

    /// Calcule la valeur globale dB[Lin] d'un spectre en tiers d'octave.
    virtual double valGlobDBLin() const;

    /// Calcule la valeur globale dB[A] d'un spectre en tiers d'octave.
    virtual double valGlobDBA() const;

    /// Conversion en dB.
    virtual OSpectre toDB() const;

    /// Conversion en grandeur physique.
   virtual  OSpectre toGPhy() const;

    /// Sommation arithmetique de deux spectres en 1/3 d'octave.
    virtual OSpectre sum(const OSpectre& spectre) const;

    /// Ajoute une valeur constante a l'ensemble du spectre
    virtual OSpectre sum(const double& valeur) const;

    /// Sommation energetique de deux spectres en 1/3 d'octave.
    virtual OSpectre sumdB(const OSpectre& spectre) const;

    /// Soustraction arithmetique de deux spectres en 1/3 d'octave.
    virtual OSpectre subst(const OSpectre& spectre) const;

    /// soustrait une valeur constante a l'ensemble du spectre
    virtual OSpectre subst(const double& valeur) const;

    /// Soustraction energetique de deux spectres en 1/3 d'octave.
    virtual OSpectre substdB(const OSpectre& spectre) const;

    /// multiplication de deux spectres terme a terme.
    virtual OSpectre mult(const OSpectre& spectre) const;

    /// Multiplication d'un spectre par un scalaire.
    virtual OSpectre mult(const double& coefficient) const;

    /// division de deux spectres terme a terme.
    virtual OSpectre div(const OSpectre& spectre) const;

    /// Division d'un spectre par une constante.
    virtual OSpectre div(const double& coefficient) const;

    /// Division d'une constante par un spectre.
    virtual OSpectre invMult(const double& coefficient = 1.0) const;

    virtual OSpectre inv() const;

    /// Eleve un spectre a une puissance.
    virtual OSpectre power(const double& puissance) const;

    /// Calcule le log base n d'un spectre (n=10 par defaut).
    virtual OSpectre log(const double& base = 10.0) const;

    /// calcule 10^spectre pour chaque terme du spectre.
    virtual OSpectre tenPow() const;

    /// calcule la racine carree d'un spectre.
    virtual OSpectre racine() const;

    /// Calcule e^(coef * spectre)
    virtual OSpectre exp(const double coef = 1.0);

    /// calcul le sin de la partie reelle du spectre
    virtual OSpectre sin() const;

    /// calcul le cos de la partie reelle du spectre
    virtual OSpectre cos() const;

    /// calcul le tan de la partie reelle du spectre
    virtual OSpectre tan() const;

    /// Retourne le spectre en valeur absolues
    virtual OSpectre abs() const;

    /**
     * \fn OSpectre sqrt() const
     * \brief retourne la racine carree du spectre
     */
    virtual OSpectre sqrt() const;

    /**
     * \fn OSpectre max()
     * \brief retourne la valeur max du spectre
     */
    virtual double valMax();

    /// Borne les valeurs du spectre en min et en max
    virtual OSpectre seuillage(const double& min = -200.0, const double max = 200.0);

    /// Cumule les valeurs du spectre
    virtual double sigma();
    virtual const double sigma() const;

    // Conversion en spectre "leger"
    void toSpectreLeger(TYSpectreLeger& spectre) const;

    // initialisation des valeurs reelles a partir d'un spectre "leger"
    virtual void fromSpectreLeger(const TYSpectreLeger& spectre);

    /// Existence d'une tonalite marquee
    virtual bool isTonalite()const;

    // === FONCTIONS MEMBRES STATIQUES

    /// Cree un spectre en lin
    static OSpectre getEmptyLinSpectre(const double& valInit = 1.0E-20);

    /// retourne l'indice associe a une frequence
    static int getIndice(const float& freq) { return _mapFreqIndice[freq]; }

    /// Definition des frequences min et max de travail
    static void setFMin(const float& fMin) { _fMin = fMin; }
    static void setFMax(const float& fMax) { _fMax = fMax; }

    /**
     * Retourne le tableau des frequences exactes.
     * L'appelant est responsable de liberer la memoire allouee pour
     * creer ce tableau.
     *
     * @return Le tableau des frequences exactes.
     */
    static OTabFreq getTabFreqExact();

	/**
     * \fn OSpectre getOSpectreFreqExact()
	 * \brief Retourne le tableau des frequences exactes.
     * \return Le tableau des frequences exactes.
     */
	static OSpectre getOSpectreFreqExact();

    /// Construction du tableau frequence/indice
    static std::map<float, int>  setMapFreqIndice();

    /// Construit un spectre de ponderation A.
    static OSpectre pondA();

    /// Construit un spectre de ponderation B.
    static OSpectre pondB();

    /// Construit un spectre de ponderation C
    static OSpectre pondC();

    /**
     * Retourne un spectre representant la longueur d'onde associee a chaque frequence
     * @param double Vitesse de propagation de l'onde
     * @return un TYSpectre
     */
    static OSpectre getLambda(const double& c);

    // Membres
public:

protected:

    // ==== MEMBRES STATIQUES

    ///Tableau des frequences en Hz centrales normalisees en tiers d'octave.
    static const float _freqNorm[];

    /// Frequence minimale de travail
    static float _fMin;

    /// Frequence minimale de travail
    static float _fMax;

    /// Valeur par defaut du spectre
    static double _defaultValue;

    /// Carte de correspondance entre frequence et indice du tableau
    static std::map<float, int> _mapFreqIndice;

    // ==== AUTRES MEMBRES

    ///Validite du spectre.
    bool _valid;

    ///Type de spectre.
    TYSpectreType _type;

    ///Etat du spectre (grandeur physique ou dB).
    TYSpectreEtat _etat;

    ///Forme de representation du spectre tiers d'octave, octave, delta f constant, non structure.
    TYSpectreForm _form;

    /// Tableau des valeurs reelles
    double _module[TY_SPECTRE_DEFAULT_NB_ELMT];
};


typedef std::vector<OSpectre> OTabSpectre;
typedef std::vector<std::vector<OSpectre> > OTab2DSpectre;

#endif // __O_SPECTRE__
