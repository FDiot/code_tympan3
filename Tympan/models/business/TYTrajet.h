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

#ifndef __TY_TRAJET__
#define __TY_TRAJET__


#include "TYChemin.h"
#include "Tympan/models/business/acoustique/TYSourcePonctuelle.h"
#include "TYPointCalcul.h"
#include "Tympan/models/common/3d.h"

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
    TYTrajet(TYSourcePonctuelleGeoNode* pSrc = NULL, TYPointCalculGeoNode* pPtCalcul = NULL);
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
     * \fn TYSourcePonctuelleGeoNode* getSourcePonctuelle()
     *     void setSourcePonctuelle(TYSourcePonctuelleGeoNode* pSrc)
     * \brief Set/Get de la source ponctuelle associee a cette etape.
     * \return _pSrc
     */
    TYSourcePonctuelleGeoNode* getSourcePonctuelle() const { return _pSrc; }
    /**
     * Set/Get de la source ponctuelle associee a cette etape.
     */
    void setSourcePonctuelle(TYSourcePonctuelleGeoNode* pSrc) { _pSrc = pSrc; }

    /**
     * \fn TYPointCalculGeoNode* getPointCalcul()
     *     void setPointCalcul(TYPointCalculGeoNode* pPtCalcul)
     * \brief Set/Get du point de calcul associe a cette etape.
     * \return _pPtCalcul
     */
    TYPointCalculGeoNode* getPointCalcul() const { return _pPtCalcul; }

    /**
     * Set/Get du point de calcul associe a cette etape.
     */
    void setPointCalcul(TYPointCalculGeoNode* pPtCalcul) { _pPtCalcul = pPtCalcul; }

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
    void getPtSetPtRfromOSeg3D(OSegment3D& seg) { seg._ptA = _ptS, seg._ptB = _ptR; }

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
     * \fn OSpectre getPEnergetique(const TYAtmosphere& atmos)
     * \brief Calcule la pression acoustique en module phase sur le trajet
     */
    OSpectre getPEnergetique(const TYAtmosphere& atmos);

    /**
     * \fn OSpectre getPInterference(const TYAtmosphere& atmos)
     * \brief Calcule la pression quadratique sur le trajet
     */
    OSpectre getPInterference(const TYAtmosphere& atmos);

    /**
     * \fn void addRay(const TYRay* ray)
     * \brief add a TYRay to the vector of TYRay _tabRays
     */
    void addRay(const TYRay* ray) { _tabRays.push_back(const_cast<TYRay*>(ray)); }

    /**
     * \fn std::vector<TYRay*>& getTabRays()
     * \brief Getter to the vector of rays : tabRays
     */
    std::vector<TYRay*>& getTabRays() { return _tabRays; }

private:
    OSpectre correctTiers(const OSpectreComplex& si, const OSpectreComplex& sj, const TYAtmosphere& atmos, const double& ri, const double& rj) const;


    // Membres
protected:
    ///La source ponctuelle.
    TYSourcePonctuelleGeoNode* _pSrc;

    ///Le point de calcul (recepteur).
    TYPointCalculGeoNode* _pPtCalcul;

    /// Definition du point source dans le repere du site
    OPoint3D _ptS;

    /// Definition du point recepteur dans le repere du site
    OPoint3D _ptR;

    /// Liste des chemins (TYRay) reliants la source et le récepteur
    std::vector<TYRay*> _tabRays;

    ///La collection de Chemins.
    TYTabChemin _chemins;

    ///Collection des chemins "direct" sans obstacles
    TYTabChemin _cheminsDirect;

    ///Distance de la source au recepteur
    double _distance;

    /// Spectre au point de reception du trajet integrant la divergence geometrique et la puissance de la source
    //TYSpectre _sLP;
    OSpectre _sLP;
};
#endif // __TY_TRAJET__
