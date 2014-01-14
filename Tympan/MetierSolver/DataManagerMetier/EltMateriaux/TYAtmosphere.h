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

#ifndef __TY_ATMOSPHERE__
#define __TY_ATMOSPHERE__

#include "Tympan/MetierSolver/DataManagerCore/TYElement.h"
#include "Tympan/MetierSolver/DataManagerMetier/ComposantAcoustique/TYSpectre.h"
#include "Tympan/MetierSolver/DataManagerMetier/ComposantGeometrique/TYSegment.h"

#if TY_USE_IHM
#include "Tympan/GraphicIHM/DataManagerIHM/TYAtmosphereWidget.h"
#endif

///Pression de reference exprimee en Pascal.
#define TY_EDF_PR 101325.0
///Zero absolu (Kelvin).
#define TY_EDF_0K 273.15
///Temperature de reference (20° C) exprimee en Kelvin.
#define TY_EDF_T0 293.15


/**
 * Classe de definition d'une atmosphere.
 *
 * @author Projet_Tympan
 * @author Projet_Tympan
 */
class TYAtmosphere: public TYElement
{
    OPROTOSUPERDECL(TYAtmosphere, TYElement)
    TY_EXTENSION_DECL(TYAtmosphere)

    // Methodes
public:
    /**
     * Constructeur.
     */
    TYAtmosphere();
    /**
     * Constructeur par copie.
     */
    TYAtmosphere(const TYAtmosphere& other);
    /**
     * Destructeur.
     */
    virtual ~TYAtmosphere();

    ///Operateur =.
    TYAtmosphere& operator=(const TYAtmosphere& other);
    ///Operateur ==.
    bool operator==(const TYAtmosphere& other) const;
    ///Operateur !=.
    bool operator!=(const TYAtmosphere& other) const;

    virtual bool deepCopy(const TYElement* pOther, bool copyId = true);

    virtual std::string toString() const;

    virtual DOM_Element toXML(DOM_Element& domElement);
    virtual int fromXML(DOM_Element domElement);

    /**
     * Set/Get de la pression.
     */
    double getPression() const { return _pression; }
    /**
     * Set/Get de la pression.
     */
    void setPression(double press);

    /**
     * Set/Get de la temperature.
     */
    double getTemperature() const { return _temperature; }
    /**
     * Set/Get de la temperature.
     */
    void setTemperature(double temp);

    /**
     * Set/Get de l'hygrometrie.
     */
    double getHygrometrie() const { return _hygrometrie; }
    /**
     * Set/Get de l'hygrometrie.
     */
    void setHygrometrie(double hygro);
    /**
     * Set/Get de la vitesse du vent.
     */
    double getVitVent() const { return _vitVent; }
    /**
     * Set/Get de la vitesse du vent.
     */
    void setVitVent(double vit) { _vitVent = vit; }
    /**
     * Get/Set de la vitesse du son
     */
    double getVitSon() const { return _vitSon; }

    /**
     * Get/Set de la vitesse du son
     */
    void setVitSon(const double& val) { _vitSon = val; }

    /**
     * Set/Get de la direction du vent.
     */
    double getDirVent() const { return _dirVent; }
    /**
     * Set/Get de la direction du vent.
     */
    void setDirVent(double dir) { _dirVent = dir; }

    /**
     * Get/Set de l'impedance specifique de l'air
     */
    double getImpedanceSpecifique() const { return _impedanceSpecifique; }

    /**
     * Get/Set de l'impedence specifique
     */
    void setImpedanceSpecifique(const double& val) { _impedanceSpecifique = val; }

    /**
     * Lecture de la fraction molaire d'humidite (utilise par les cas tests)
     */
    double getFractionMolaire() { return _h_molaire; }
    const double getFractionMolaire() const { return _h_molaire; }

    /**
     * Fonction qui recalcule l'ensemble des proprietes de l'atmosphere
     * Cette fonction est appelee a chaque changement d'un parametre
     */
    void calculProprietes();

    /**
     * Get du spectre de nombre d'onde
     */
    OSpectre& getKAcoust() { return _spectreKAcoust; }
    const OSpectre& getKAcoust() const { return _spectreKAcoust; }

    /**
     * Set/Get du spectre d'attenuation.
     */
    TYSpectre& getSpectreAtt() { return _spectreAtt; }

    /**
     * Set/Get du spectre d'attenuation.
     */
    const TYSpectre& getSpectreAtt() const { return _spectreAtt; }
    /**
     * Set/Get du spectre d'attenuation.
     */
    void setSpectreAtt(const TYSpectre& Att) { _spectreAtt = Att; _spectreAtt.setParent(this); }

    /**
     * Calcul du spectre d'attenuation pour un segment donne.
     *
     * @param seg Segment pour lequel est calcule le spectre d'attenuation
     */
    OSpectre getAtt(const TYSegment& seg) const;

    /**
     * Calcul du spectre d'attenuation pour une longueur de traversee donnee
     *
     * @param distance double representant la longueur du trajet parcouru dans l'atmosphere
     * @return un LPTYSpectre
     */
    OSpectre getAtt(const double& distance) const;

    /// Methodes specifiques a HARMONOISE

    /**
     * Get du profil turbulent.
     */
    void getProfilMeteo(double& A, double& B, double& C, double& D) { A = _A; B = _B; C = _C; D = _D; }
    void setProfilMeteo(const double& A, const double& B, const double& C, const double& D) { _A = A; _B = B; _C = C; _D = D; }

    /**
     * Get/Set de la classe meteo
     */
    int getClasseMeteo() { return _stabilityClass; }
    const int getClasseMeteo() const { return _stabilityClass; }

    void setClasseMeteo(const int& stability) { _stabilityClass = stability; }

protected:
    /**
     * Calcule la fraction molaire de vapeur.
     */
    void hrToFMol();

    /**
     * Calcul du spectre d'attenuation atmospherique d'apres ISO9613.
     */
    void calculSpectre();
    /**
     * Calcul de la vitesse du son
     */
    void calculC();
    /**
     * Calcul du spectre de nombre d'onde acoustique
     */
    void calculK();
    /**
     * calcul de l'impedance caracteristique de l'air rho0.c
     */
    void calculImpedance();

    // Membres
protected:
    ///Pression en Pascal.
    double _pression;
    ///Temperature en degres Celsius.
    double _temperature;
    ///Hygrometrie en pourcentage d'humidite relative.
    double _hygrometrie;
    /// Hygrometrie exprimee en fraction molaire de vapeur d'eau
    double _h_molaire;
    ///Vitesse du vent en m/s.
    double _vitVent;
    ///Direction du vent.
    double _dirVent;
    ///Vitesse du son
    double _vitSon;
    ///Impedance caracteristique de l'air (rho.C)
    double _impedanceSpecifique;
    ///Spectre d'attenuation.
    TYSpectre _spectreAtt;
    ///Spectre de nombre d'onde
    OSpectre _spectreKAcoust;

    /// SPECIFIQUE HARMONOISE
    /// Description du profil du champ sonore et de la turbulence
    double _A; // linear sound speed gradient
    double _B; // logarithmic sound speed gradient
    double _C; // turbulence strength
    double _D; // displacement height

    /// Classe meteo
    int _stabilityClass;



public:
    ///Impedance de reference de l'air
    static const double _impedanceRef;

    /**
        BIBLIOGRAPHIE :

      [1] Harmonoise Programming the P2P propagation model : Page 26

    */
};


#endif // __TY_ATMOSPHERE__
