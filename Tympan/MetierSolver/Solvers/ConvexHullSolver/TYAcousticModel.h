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

#include "Tympan/MetierSolver/DataManagerCore/TYAcousticModelInterface.h"
#include "TYSolverDefines.h"
#include "Tympan/MetierSolver/DataManagerMetier/Commun/TYChemin.h"

class TYTrajet;
class TYSolver;

class TYAcousticModel : public TYAcousticModelInterface
{
public:
    TYAcousticModel(TYSolver& solver);
    virtual ~TYAcousticModel();

    virtual void compute(std::vector<TYSIntersection*>& tabIntersect, OSegment3D& rayon, TYSourcePonctuelle* pSrc, TYPointCalcul* pRcpt, std::vector<TYRay*>& tabRays, OSpectre& spectrum);
    void init(const TYSiteNode& site, const TYCalcul& calcul);

    /**
     * Calcule le spectre du facteur C utilise dans le calcul de la diffraction.
     *
     * @param epaisseur Double indiquant l'epaisseur totale de l'obstacle.
     *
     * @return Un LPTYSpectre donnant le coefficient de correction pour chacune des frequences.
     */
    OSpectre calculC(const double& epaisseur) const;

    /**
     * Limite la valeur de l'attenuation de l'ecran en fonction d'un critere dependant de la frequence
     * @param sNC Spectre non corrige
     * @param C
     **/
    OSpectre limAttDiffraction(const OSpectre& sNC, const OSpectre& C) const;

    /**
     * Calcul de la contribution de la source au point
     */
    bool solve(OSegment3D& rayon, TYSourcePonctuelle* pSrc, std::vector<TYRay*>& tabRays, std::vector<OSpectreComplex>& tabSpectres, OSpectre& spectrum);

    /**
     * Renvoi le materiau de la face rencontree par le rayon incident
     */
    TYMateriauConstruction getMateriauFace(TYAcousticSurface* pSurf, TYAcousticVolume* pVolume) const;

	/*!
	 * \fn OSpectreComplex computeRay(TYRay* ray)
	 * \brief Comppute spectrum at reception point taking into account all events
	 */
	OSpectreComplex computeRay(TYRay* ray, std::vector<TYSIntersection*>& tabIntersect);

	/*!
	 * \fn computeLwSource(const OPoint3D& begin, OBoint3D& end)
	 * \brief compute source power with directivity effect
	 */
	OSpectreComplex computeLwSource(TYRay* ray, const OPoint3D& begin, const OPoint3D& end);

	/*!
	 * \fn computeAttTotal(TYRay* ray, unsigned int& i)
	 * \brief compute total attenuation on the ray
	 */
	OSpectreComplex computeAttTotal(TYRay* ray, OSpectreComplex& S, OSpectreComplex& Qs, OSpectreComplex& Qm, OSpectreComplex& D);

	/*!
	 * \fn computeAttDiffraction(TYRay* ray, unsigned int& i)
	 * \brief compute attenuation of a diffraction
	 */
	OSpectreComplex computeAttDiffraction(TYRay* ray, unsigned int& i);

	/*!
	 * \fn computeAttRefelexionSol(TYRay* ray, unsigned int& i)
	 * \brief compute attenuation of a reflection on soil
	 */
	OSpectreComplex computeAttReflexionSol(TYRay* ray, unsigned int& i);

	/*!
	 * \fn computeAttRefelexionSol(TYRay* ray, unsigned int& i)
	 * \brief compute attenuation of a reflection on soil
	 */
	OSpectreComplex computeAttReflexion(TYRay* ray, std::vector<TYSIntersection*>& tabIntersect, unsigned int& i);

	/*!
	 * \fn double computeScreenThick(TYRay* ray)
	 * \brief Compute screen thick based on list of events TYDIFFRACTION
	 */
	double computeScreenThick(TYRay* ray);

	/*!
	 * \fn void deleteOtherDiffractions(TYRay* ray, const unsigned int begin)
	 * \brief Delete diffractions after treatment (only one authorized)
	 */
	void deleteOtherDiffractions(TYRay* ray, const unsigned int i);

    /**
     * \fn OSpectre getPEnergetique(const TYAtmosphere& atmos)
     * \brief Calcule la pression acoustique en module phase sur le trajet
     */
    OSpectre getPEnergetique(std::vector<TYRay*>& tabRays, std::vector<OSpectreComplex>& tabSpectres);

    /**
     * \fn OSpectre getPInterference(const TYAtmosphere& atmos)
     * \brief Calcule la pression quadratique sur le trajet
     */
    OSpectre getPInterference(std::vector<TYRay*>& tabRays, std::vector<OSpectreComplex>& tabSpectres);

private:
    OSpectre correctTiers(const OSpectreComplex& si, const OSpectreComplex& sj, const double& ri, const double& rj) const;

private:
    int  _expGeo;
    bool _useSol;
    bool _useEcran;
    bool _useReflex;
    bool _conditionFav;
    bool _useAtmo;
    bool _bCalculTrajetsHorizontaux;
    bool _interference;
	bool _bSaveRay;
    double _seuilConfondus;
    double _paramH;
    TYAtmosphere* _pAtmo;
    TYTopographie* _pTopographie;
    OSpectre _lambda;
    OSpectre _absoNulle;

    // Reference sur le solver
    TYSolver& _solver;
};

#endif // __TYACOUSTICMODEL__
