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

#ifndef __TY_TRAJET__
#define __TY_TRAJET__


#include "TYChemin.h"
#include "Tympan/models/common/3d.h"
#include "Tympan/models/common/atmospheric_conditions.h"
#include "Tympan/models/common/acoustic_path.h"
#include "Tympan/models/solver/entities.hpp"

/**
 * \file TYTrajet.h
 * \class TYTrajet
 * \brief  La classe Trajet permet de faire correspondre a un couple Source-Recepteur une collection de Chemins, en plus d'un Chemin direct.
 * \version v 1.1
 * \date 2008/01/21
 * \author Projet_Tympan
 */
class TYTrajet
{

    // Methodes
public:
    /**
     * \fn TYTrajet(TYSourcePonctuelleGeoNode* pSrc = NULL, TYPointCalculGeoNode* pPtCalcul = NULL)
     * \brief Constructeur par defaut.
     * Le constructeur par defaut de la classe TYTrajet
     */
    TYTrajet(tympan::AcousticSource& asrc_, tympan::AcousticReceptor& arcpt_);
    
    /**
     * \fn TYTrajet(const TYTrajet& other)
     * \brief Constructeur par copie.
     * Le constructeur par copie de la classe TYTrajet
     */
    TYTrajet(const TYTrajet& other);
    /**
     * \fn virtual ~TYTrajet()
     * \brief Destructeur.
     * Le Destructeur de la classe TYTrajet
     */
    virtual ~TYTrajet();

    ///Operateur =.
    TYTrajet& operator=(const TYTrajet& other);
    ///Operateur ==.
    bool operator==(const TYTrajet& other) const;
    ///Operateur !=.
    bool operator!=(const TYTrajet& other) const;

    /**
     * \fn void reset()
     * \brief Reset ce trajet.
     */
    void reset();

    /**
     * \fn double getDistance()
     *     const double getDistance()
     * \brief Get/Set de la distance entre la source et le recepteur
     * \return _distance
     */
    double getDistance() { return _distance; }
    const double getDistance() const { return _distance; }

    void setDistance(const double& distance) { _distance = distance; }

    /**
     * \fn size_t getNbChemins()
     * \brief Retourne le nombre de Chemins contenu par ce Trajet (en plus du Chemin direct).
     * \return _chemins.size()
     */
    size_t getNbChemins() { return _chemins.size(); }

    /**
     * \fn TYTabChemin& getChemins()
     * \brief Retourne la collection de Chemins de ce Trajet.
     * \return _chemins
     */
    TYTabChemin& getChemins() { return _chemins; }

    /**
     * \fn TYTabChemin& getCheminsDirect()
     * \brief Retourne le tableau des chemins direct
     * \return _cheminsDirect
     */
    TYTabChemin& getCheminsDirect() {return  _cheminsDirect; }

    /**
     * \fn void addChemin(const TYChemin & chemin)
     * \brief Ajoute un nouveau Chemin.
     */
    void addChemin(const TYChemin& chemin);

    /**
     * \fn void addCheminDirect(const TYChemin & chemin)
     * \brief Ajoute un chemin au tableau des chemins directs
     */
    void addCheminDirect(const TYChemin& chemin);

    /**
     * \fn TYChemin getChemin(int index)
     * \brief Retourn un chemin en fonction de son indice.
     * \return _chemins.at(index)
     */
    TYChemin getChemin(int index) { return _chemins.at(index); }

    /**
     * \fn OSpectre getPNoOp()
     * \brief Renvoie l'attenuation du trajet sans calcul (calcul par une fonction externe)
     */
    OSpectre getPNoOp();

    /**
     * \fn void setPtSetPtR(const OPoint3D& pt1, const OPoint3D& pt2)
     *     void getPtSetPtR(OPoint3D& pt1, OPoint3D& pt2)
     * \brief Get/Set des points sources et recepteur
     */
    void setPtSetPtR(const OPoint3D& pt1, const OPoint3D& pt2) { _ptS = pt1; _ptR = pt2; }
    void getPtSetPtR(OPoint3D& pt1, OPoint3D& pt2) { pt1 = _ptS; pt2 = _ptR; }

    /**
     * \fn void setPtSetPtRfromOSeg3D(const OSegment3D& seg)
     *     void getPtSetPtRfromOSeg3D(OSegment3D& seg)
     * \brief Get/Set des points sources et recepteurs a partir d'un OSegment3D
     */
    void setPtSetPtRfromOSeg3D(const OSegment3D& seg) { _ptS = seg._ptA; _ptR = seg._ptB; }
    void getPtSetPtRfromOSeg3D(OSegment3D& seg) const { seg._ptA = _ptS, seg._ptB = _ptR; }

    /**
     * \fn TYSpectre& getSpectre()
     *     const TYSpectre getSpectre()
     *     void setSpectre(const TYSpectre& spectre)
     * \brief Get/Set du spectre au point de reception
     * \return _sLP
     */
    //TYSpectre& getSpectre() { return _sLP; }
    //const TYSpectre getSpectre() const { return _sLP; }
    //void setSpectre(const TYSpectre& spectre) { _sLP = spectre; }
    OSpectre& getSpectre() { return _sLP; }
    const OSpectre getSpectre() const { return _sLP; }
    void setSpectre(const OSpectre& spectre) { _sLP = spectre; }


    /**
     * \fn OSpectre getPEnergetique(const AtmosphericConditions& atmos)
     * \brief Calcule la pression acoustique en module phase sur le trajet
     */
    OSpectre getPEnergetique(const AtmosphericConditions& atmos);

    /**
     * \fn OSpectre getPInterference(const AtmosphericConditions& atmos)
     * \brief Calcule la pression quadratique sur le trajet
     */
    OSpectre getPInterference(const AtmosphericConditions& atmos);

    // Get the tab of rays
    std::vector<acoustic_path*>& get_tab_rays() { return _tabRays;}

private:
    OSpectre correctTiers(const OSpectreComplex& si, const OSpectreComplex& sj, const AtmosphericConditions& atmos, const double& ri, const double& rj) const;
    void build_tab_rays();

public :
    // Business source
    tympan::AcousticSource& asrc;
    tympan::source_idx asrc_idx;

    // Business receptor
    tympan::AcousticReceptor& arcpt;
    tympan::receptor_idx arcpt_idx;


    // Membres
protected:
    /// Definition du point source dans le repere du site
    OPoint3D _ptS;

    /// Definition du point recepteur dans le repere du site
    OPoint3D _ptR;

    ///La collection de Chemins. 
    TYTabChemin _chemins;

    ///Collection des chemins "direct" sans obstacles
    TYTabChemin _cheminsDirect;

    ///Distance de la source au recepteur
    double _distance;

    /// Spectre au point de reception du trajet integrant la divergence geometrique et la puissance de la source
    //TYSpectre _sLP;
    OSpectre _sLP;

    /// Vector of rays equivalent to chemin
    std::vector<acoustic_path*> _tabRays;
};
#endif // __TY_TRAJET__
