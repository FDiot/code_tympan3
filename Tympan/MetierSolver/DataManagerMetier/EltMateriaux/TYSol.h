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

#ifndef __TY_SOL__
#define __TY_SOL__


#include "TYVegetation.h"
#include "TYAtmosphere.h"

#if TY_USE_IHM
#include "Tympan/GraphicIHM/DataManagerIHM/TYSolWidget.h"
#endif

/**
 * Classe de definition d'un sol.
 */
class TYSol: public TYElement
{
    OPROTOSUPERDECL(TYSol, TYElement)
    TY_EXTENSION_DECL(TYSol)

    // Methodes
public:
    /**
     * Constructeur.
     */
    TYSol();
    /**
     * Constructeur par copie.
     */
    TYSol(const TYSol& other);
    /**
     * Destructeur.
     */
    virtual ~TYSol();

    ///Operateur =.
    TYSol& operator=(const TYSol& other);
    ///Operateur ==.
    bool operator==(const TYSol& other) const;
    ///Operateur !=.
    bool operator!=(const TYSol& other) const;

    virtual bool deepCopy(const TYElement* pOther, bool copyId = true);

    virtual std::string toString() const;

    virtual DOM_Element toXML(DOM_Element& domElement);
    virtual int fromXML(DOM_Element domElement);

    /**
     * Set/Get de la resistivite.
     */
    double getResistivite() const { return _resistivite; }

    /**
     * Set/Get de la resistivite.
     */
    void setResistivite(double res) { _resistivite = res; calculZc(); }

    /**
     * Set/Get de l'epaisseur.
     */
    double getEpaisseur() const { return _epaisseur; }

    /**
     * Set/Get de l'epaisseur.
     */
    void setEpaisseur(double epais) { _epaisseur = epais; }

    /**
     * Set/Get de l'impedance.
     */
    OSpectreComplex getImpedance() { return _pImpedance; }

    /**
     * Set/Get de l'impedance.
     */
    const OSpectreComplex getImpedance() const { return _pImpedance; }

    /**
     * Set/Get de l'impedance.
     */
    void setImpedance(const OSpectreComplex imp) { _pImpedance = imp; }


    /**
     * Get de l'indicateur de vegetation.
     */
    bool getVegetActive() { return _vegetActive; }

    /**
     * Set/Get de la vegetation.
     */
    TYVegetation* getVegetation() { return _pVegetation; }

    /**
     * Set/Get de la vegetation.
     */
    const TYVegetation* getVegetation() const { return _pVegetation; }

    /**
     * Set/Get de la vegetation.
     */
    void setVegetation(TYVegetation* pVeget) { _pVegetation = pVeget; _pVegetation->setParent(this); }

    /**
     * Active ou non l'utilisation de la vegetation et retourne une
     * reference sur celle-ci.
     *
     * @param state Etat d'utilisation de la vegetation.
     *
     * @return La reference sur la vegetation.
     */
    TYVegetation* useVegetation(bool state = true);

    /**
     * Calule le spectre d'absorption du sol
     * en fonction de l'angle d'incidence de l'onde acoustique.
     *
     * @param angle Angle d'incidence de l'onde acoustique.
     * @param Rr distance du trajet reflechi sur le sole entre le point de reflexion et le point suivant
     * @param pAtmo atmosphere courante du calcul
     */
    OSpectreComplex abso(const double& angle, const double& rR, const TYAtmosphere& Atmo) const;

protected:
    /**
     * Calcul de Zc, impedance caracteristique du sol a partir de
     * la resistance a l'ecoulement du sol exprimee en kRayl.
     * d'apres la formulation de DELANY-BAZLEY
     *
     * @return Le spectre de l'impedence caracteristique du sol
     */
    void calculZc();

    /**
     * Calcul de l'impedance specifique du sol Zc
     */
    OSpectreComplex calculZs(const double phi, const TYAtmosphere& atmos) const;

    /**
     * Calcul de w d'apres : "H. JONASSON : The propagation and screening of wheel/rail noise"
     *
     * @param rR distance du trajet reflechi sur le sol entre la source et le point recepteur
     * @param phi angle d'incidence de l'onde acoustique
     *
     * @return w un OSpectreComplex
     */
    OSpectreComplex calculW(const OSpectreComplex& Zs, const double rR, const double phi, const TYAtmosphere& atmos) const;

public:  //Temporairement
    /**
     * Calcul de la fonction Fw facteur de perte pour l'effet de sol
     *
     * @param w LPTYSpectre
     * @return fw LPTYSpectre representant le facteur de perte
     */
    OSpectreComplex calculFw(const OSpectreComplex& w) const;

    /**
     * Calcul de K, nombre d'onde dans la surface du sol
     * d'apres la formulation de DELANY-BAZLEY
     *
     * @return Le spectre du nombre d'onde dans la surface du sol
     */
    void calculNombreDOnde(const TYAtmosphere& atmos);

    /**
     * Get/Set du statut totalement reflechissant
     */
    void setMiroir(const bool& bMiroir) { _bMiroir = bMiroir; }
    bool getMiroir() { return _bMiroir; }
    const bool getMiroir() const { return _bMiroir; }

    /**
     * \fn calculQ(const double& angle, const double& rR, const TYAtmosphere& Atmo) const
     * \brief Calcul du coefficient de reflexion du sol en ondes spheriques
     * \param angle Angle d'incidence de l'onde acoustique
     * \param rR distance du trajet reflechi sur le sol entre le point de reflexion et le point suivant
     * \param pAtmo atmosphere courante du calcul
     * \return Q OSpectreComplex
     */
    OSpectreComplex calculQ(const double& angle, const double& rR, const TYAtmosphere& Atmo) const;

protected:
    /**
     * Calcul du coefficient de reflexion du sol en onde plane
     *
     * @param phi Angle d'incidence de l'onde acoustique
     *
     * @return rP OSpectreComplex
     */
    OSpectreComplex calculRp(const OSpectreComplex& Zs, const double& phi) const;

    /**
     * Calcul du coefficient de reflexion du sol en ondes spheriques
     *
     * @param phi Angle d'incidence de l'onde acoustique
     * @param rp coefficient de reflexion du sol en ondes planes
     *
     * @return Q OSpectreComplex
     */
    OSpectreComplex calculQ(const double& phi, const OSpectreComplex& rp, const OSpectreComplex& fw) const ;

    /**
     * \fn calculQ(const double& phi, const OSpectreComplex & fw) const
     * \brief Calcul du coefficient de reflexion du sol en ondes spheriques
     * \param phi Angle d'incidence de l'onde acoustique
     * \param fw spectre
     * \return Q OSpectreComplex
     */
    OSpectreComplex calculQ(const double& phi, const OSpectreComplex& fw) const;



private:
    /**
     * Fonctions utilisees dans le calcul de la fonction Fw
     *
     * @param un complexe appartenant au vecteur [w]
     *
     * @return un complexe repondant au cas specifie
     */
    TYComplex erfcCas1(const TYComplex& wValue) const;
    inline TYComplex erfcCas2(const TYComplex& wValue) const;
    inline TYComplex erfcCas3(const TYComplex& wValue) const;

    /**
     * Fonction utilisee dans le traitement du signe de G
     *
     * @param TYComplex W Valeur locale de la fonction W
     * @param TYComplex G Valeur locale de la fonction G
     *
     * @return TYComplex
     */
    inline TYComplex sgnReIm(const TYComplex& W, const TYComplex& G) const ;


    // Membres
protected:
    ///Resistance a l'ecoulement.
    double _resistivite;

    ///Epaisseur du sol.
    double _epaisseur;

    ///Statut totalement reflechissant
    bool _bMiroir;

    ///Spectre d'impedance caracteristique du sol.
    OSpectreComplex _pImpedance;

    ///Spectre de nombre d'onde
    OSpectreComplex _pNombreDOnde;

    ///Vegetation eventuelle.
    TYVegetation* _pVegetation;

    ///Etat d'utilisation de la vegetation.
    bool _vegetActive;
};


#endif // __TY_SOL__
