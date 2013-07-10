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

#ifndef __TYACOUSTICPATHFINDER__
#define __TYACOUSTICPATHFINDER__

#include "Tympan/MetierSolver/DataManagerCore/TYAcousticPathFinderInterface.h"
#include "TYSolverDefines.h"

class TYSolver;
class TYRayEvent;

class TYAcousticPathFinder : public TYAcousticPathFinderInterface
{
public:
    TYAcousticPathFinder(TYSolver& solver);
    virtual ~TYAcousticPathFinder();

    virtual void computePath(std::vector<TYSIntersection*>& tabIntersect, std::vector<TYRay*>& tabRays, OSegment3D& rayon, unsigned int nNbTrajet);
    virtual void init(const TYSiteNode& site, TYCalcul& calcul);

private :
    // Reference sur le solver
    TYSolver& _solver;

    bool computeParcoursLateraux(std::vector<TYSIntersection*>& tabIntersect, OSegment3D& rayon, const bool& dessus, TYTabPoint3D& ptsLeft, TYTabPoint3D& ptsRight, unsigned int nNbTrajet) const;

	void buildDirectRay(OSegment3D& rayon, std::vector<TYRay*>& tabRays);

	void buildSoilReflectedDirecRay(OSegment3D& rayon, std::vector<TYRay*>& tabRays);

	TYRayEvent* buildPhysicalReflexOnSoil(OSegment3D& rayon);

	// Add new "artificial" rays, sens means from A to B (true) or B to A (false)
	void buildForcedReflexOnSoil(OSegment3D& rayon, std::vector<TYRay*>& tabRays, bool sens = true);

	// Add rays reflected to vertical non natural face
	void buildReflectedRays(std::vector<TYSIntersection*>& tabIntersect, OSegment3D& rayon, std::vector<TYRay*>& tabRays);

	// Add rays around a screen
	void buildDiffractedRays(TYTabPoint3D& pts, OSegment3D& rayon, const bool& vertical, std::vector<TYRay*>& tabRays);

	// Ajoute les points de diffraction à un TYRay
	void addDiffractionPointsToTyRay(TYTabPoint3D& pts, TYRay* ray);

	// Construit un plan pour le calcul
	OPlan getPlanHorizontal(const OSegment3D& seg);

	// Faut il calculer les trajets horizontaux
    bool _bCalcTrajetHorizontaux;

    // Faut-il calculer avec prise en compte des écran artificiels
	bool _useEcran;

	// Faut-il calculer les réflexions sur les parois verticales
	bool _useReflex;

	// Faut-il prendre en compte les conditions favorables à la propagation
    bool _conditionFav;

    // Position des points de réflesion supplémentaires (consitions favorables)
	double _paramH;

	// Pointeur sur la topographie du site
	TYTopographie* _pTopographie;

	// Plan correspondant au sol entre la source et le récepteur
	OPlan _planMoyen;

};

#endif // __TYACOUSTICPATHFINDER__
