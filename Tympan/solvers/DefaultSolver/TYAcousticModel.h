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

#ifndef __TYACOUSTICMODEL__
#define __TYACOUSTICMODEL__

#include "Tympan/models/business/TYAcousticModelInterface.h"
#include "TYSolverDefines.h"
#include "Tympan/models/business/TYChemin.h"

class TYTrajet;
class TYSolver;

class TYAcousticModel : public TYAcousticModelInterface
{
public:
    TYAcousticModel(TYSolver& solver);
    virtual ~TYAcousticModel();

    virtual void compute(   const std::deque<TYSIntersection>& tabIntersect, 
                            const OSegment3D& rayon, TYTrajet& trajet, 
                            TabPoint3D& ptsTop, TabPoint3D& ptsLeft, 
                            TabPoint3D& ptsRight );

    void init(const TYCalcul& calcul);

    /**
     * Calcule le chemin sur un segment a partir de la liste des points du parcours
     * en tenant compte de la reflexion sur le sol.
     *
     * @param useSol Un indicateur qui precise si on utilise le sol par default ou le sol specifie.
     * @param rayon Un segment representant le rayon acoustique.
     * @param penteMoyenne La pente moyenne sur le rayon.
     * @param Atmo L'atmosphere courante.
     * @param pSrcGeoNode La source ponctuelle emittrice.
     * @param pts La liste des points du parcours.
     * @param vertical booleen qui insique que l'on traite le plan verticl
     * @param tabChemins Liste des chemins du trajet.
     *
     * @return <code>true</code> si le calcul a reussi;
     *         <code>false</code> sinon.
     */
    virtual bool computeCheminsAvecEcran(   const OSegment3D& rayon, const tympan::AcousticSource& source, 
                                            const TabPoint3D& pts, const bool vertical, 
                                            TYTabChemin& TabChemin, double distance ) const;

    /**
     * Calcule la liste des chemins generes par la reflexion sur les parois verticales.
     *
     * @param rayon Un segment representant le rayon acoustique.
     * @param Atmo L'atmosphere courante.
     * @param pSrcGeoNode La source ponctuelle emittrice.
     * @param tabChemin La liste des chemins generes par la reflexion.
     * @param tabFaces Ensemble des faces du site.
     */
    void computeCheminReflexion(    const std::deque<TYSIntersection>& tabIntersect, const OSegment3D& rayon, 
                                    const tympan::AcousticSource& source, TYTabChemin& TabChemins, 
                                    double distance ) const;

    ///**
    // * Calcule les spectres d'attenuation caracteristiques de la vegetation
    // * pour chaque terrain survole par rayon.
    // *
    // * @param rayon Un segment representant le rayon acoustique.
    // * @param tabSegSp Le tableau dans lequel sont stockes les spectres d'attenuation et segments associes.
    // * @param penteMoyenne La pente moyenne sur le rayon.
    // */
    //void getSpectreAttVegetation(const OSegment3D& rayon, const OSegment3D& penteMoyenne, TYTabStructSegLPSp& tabSegSp) const;

    ///**
    // * Calcule le chemin direct en tenant compte de la vegetation.
    // *
    // * @param rayon Un segment representant le rayon acoustique.
    // * @param Atmo L'atmosphere courante.
    // * @param pSrcGeoNode La source ponctuelle.
    // * @param penteMoyenne La pente moyenne sur le rayon.
    // *
    // * @return Le chemin determine.
    // */
    //void computeCheminAvecVeg(const OSegment3D& rayon, const tympan::AcousticSource& source, const OSegment3D& penteMoyenne, TYTabChemin& TabChemin, double distance) const;

    /**
     * Calcule les chemins avec reflexion au sol s'il n'y a pas d'ecran.
     *
     * @param pCalcul Un pointeur sur le calcul.
     * @param rayon Un segment representant le rayon acoustique.
     * @param Atmo L'atmosphere courante.
     * @param penteMoyenne La pente moyenne sur le rayon.
     * @param pSrcGeoNode La source ponctuelle emittrice.
     * @param tabChemin La liste des chemins generes (1 ou 2 sous conditions favorables).
     * @param penteMoyenne La pente moyenne sur le rayon.
     */
    void computeCheminSansEcran(const OSegment3D& rayon, const tympan::AcousticSource& source, TYTabChemin& TabChemins, double distance) const;

    /**
     * Calcule les chemin pour un terrain parfaitement plat et reflechissant
     *
     * @param rayon Un segment representant le rayon acoustique.
     * @param pSrcGeoNode La source ponctuelle emittrice.
     * @param tabChemin La liste des chemins generes (1 ou 2 sous conditions favorables).
     */
    void computeCheminAPlat(const OSegment3D& rayon, const tympan::AcousticSource& source, TYTabChemin& TabChemins, double distance) const;


    /**
     * Calcule le spectre du facteur C utilise dans le calcul de la diffraction.
     *
     * @param epaisseur Double indiquant l'epaisseur totale de l'obstacle.
     *
     * @return Un OSpectre donnant le coefficient de correction pour chacune des frequences.
     */
    OSpectre calculC(const double& epaisseur) const;

    /**
     * Calcule l'attenuation apportee par la diffraction sur l'ecran.
     *
     * @param rayon Le trajet direct source-recepteur.
     * @param penteMoyenne la pente moyenne entre la source et le recepteur
     * @param miroir indique s'il faut utiliser le source (false) ou son image (true) dans le calcul de la difference de marche
     * @param re Longueur du chemin a prendre en compte.
     * @param epaisseur, double indiquant l'epaisseur totale de l'obstacle.
     * @param vertical, booleen qui indique que l'on traite le plan vertical (attenuation maximale limitee)
     * @param bDiffOk est mis a false par la fonction si la difference de marche est <=0
     * @return L'attenuation apportee par la diffraction sur l'ecran.
     */
    OSpectre calculAttDiffraction(const OSegment3D& rayon, const OSegment3D& penteMoyenne, const bool& miroir, const double& re, const double& epaisseur, const bool& vertical, const bool& avantApres, bool& bDiffOk) const;

    /**
     * Limite la valeur de l'attenuation de l'ecran en fonction d'un critere dependant de la frequence
     * @param sNC Spectre non corrige
     * @param C
     **/
    OSpectre limAttDiffraction(const OSpectre& sNC, const OSpectre& C) const;

    /**
     * Calcule les etapes reliant un point a un autre passant par une reflexion et une vue directe.
     *
     * @param ptDebut Point de depart.
     * @param ptFin Point d'arrivee.
     * @param pEtapes Tableau des etapes calculees.
     *
     * @return bool <code>true</code> si le calcul est juste;
     *              <code>false</code> sinon.
     */
    bool addEtapesSol(const OPoint3D& ptDebut, const OPoint3D& ptFin, const OSegment3D& penteMoyenne, const tympan::AcousticSource& source, const bool& fromSource, const bool& toRecepteur, TYTabEtape& Etapes, double& longueur) const;

    /**
     * Ajoute le trajet direct a chacun des chemins.
     */
    void addEtapeDirectToChemins(const TYEtape& EtapeDirecte, TYTabChemin& TabChemin) const;

    /**
     * Calcul de la contribution de la source au point
     */
    bool solve(TYTrajet& trajet);

private :
    OSpectreComplex getReflexionSpectrumAt(const OPoint3D& position, const OVector3D& direction, double length) const;

    void meanSlope(const OSegment3D& director, OSegment3D& slope) const;

private:
    int  _expGeo;
    int  _typeCalculSol;
    bool _useSol;
    bool _useEcran;
    bool _useReflex;
    bool _conditionFav;
    bool _useAtmo;
    bool _bCalculTrajetsHorizontaux;
    bool _useVegetation;
    bool _interference;
    double _seuilConfondus;
    double _paramH;

    std::unique_ptr<tympan::AtmosphericConditions> pSolverAtmos;

    OSpectre _lambda;
    OSpectreComplex _absoNulle;


    // Reference sur le solver
    TYSolver& _solver;
};

#endif // __TYACOUSTICMODEL__
