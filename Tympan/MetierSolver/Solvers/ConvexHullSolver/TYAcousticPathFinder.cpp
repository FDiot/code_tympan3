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


#include "TYAcousticPathFinder.h"
#include "Tympan/MetierSolver/DataManagerMetier/Commun/TYCalcul.h"
#include "Tympan/MetierSolver/DataManagerMetier/Parcours/TYCalculParcours.h"
#include "TYSolver.h"

TYAcousticPathFinder::TYAcousticPathFinder(TYSolver& solver)
    : _solver(solver),
      _bCalcTrajetHorizontaux(false),
	  _pTopographie(NULL)
{

}

TYAcousticPathFinder::~TYAcousticPathFinder()
{

}

void TYAcousticPathFinder::init(const TYSiteNode& site, TYCalcul& calcul)
{
    _bCalcTrajetHorizontaux = calcul.getCalculTrajetsHorizontaux();
    _useReflex = calcul.getUseReflexion();
    _conditionFav = calcul.getCondFav();
    _paramH = calcul.getParamH();
    _useEcran = calcul.getUseEcran();

	_pTopographie = site.getTopographie();
}

void TYAcousticPathFinder::computePath(std::vector<TYSIntersection*>& tabIntersect, std::vector<TYRay*>& tabRays, OSegment3D& rayon, unsigned int nNbTrajet)
{
    TYTabPoint3D ptsTop;
    TYTabPoint3D ptsLeft;
    TYTabPoint3D ptsRight;

	// Construction du plan du sol
	OSegment3D penteMoyenne;
	_pTopographie->penteMoy(rayon, penteMoyenne);
	_planMoyen = getPlanHorizontal(penteMoyenne);


	// 1. Construction des chemins dans le plan vertical

    //Recherche des obstacles
    computeParcoursLateraux(tabIntersect, rayon, true, ptsTop, ptsTop, nNbTrajet);

	bool noObstacle = (ptsTop.size() <= 1);

	if (!noObstacle) { buildDiffractedRays(ptsTop, rayon, true, tabRays); }

	// Construction des chemins dans le plan horizontal si demande et presence d'obstacle
    if ( _bCalcTrajetHorizontaux && !noObstacle) 
	{ 
		computeParcoursLateraux(tabIntersect, rayon, false, ptsLeft, ptsRight, nNbTrajet);
		buildDiffractedRays(ptsLeft, rayon, false, tabRays);
		buildDiffractedRays(ptsRight, rayon, false, tabRays);
	}

	// Construction des chemins réflechis (si demandés)
	if (_useReflex)
	{
		buildReflectedRays(tabIntersect, rayon, tabRays);
	}

	// Si il n'y a pas d'obstacle détecté, on construit simplement les chemins directs et réfléchis sur le sol
	if (noObstacle && _conditionFav)
	{
		// Construction des rayons supplémentaire (conditions favorables)
		buildForcedReflexOnSoil(rayon, tabRays, true);
		buildForcedReflexOnSoil(rayon, tabRays, false);
	}

	// Construction du rayon direct source/récepteur
	buildDirectRay(rayon, tabRays);

	// Construction du rayon réfléchi du chemin direct
	buildSoilReflectedDirecRay(rayon, tabRays);

}

bool TYAcousticPathFinder::computeParcoursLateraux(std::vector<TYSIntersection*>& tabIntersect, OSegment3D& rayon, const bool& dessus, TYTabPoint3D& ptsLeft, TYTabPoint3D& ptsRight, unsigned int nNbTrajet) const
{
    unsigned int nbFaces = static_cast<uint32>(_solver.getTabPolygonSize());

    short indiceIntersect = dessus ? 0 : 1;
    TYCalculParcours Parcours(nbFaces, dessus);
    Parcours.NumPlanCoupe = nNbTrajet;

    //Quoiqu'il arrive, on ajoute la source:
    ptsLeft.push_back(rayon._ptA);
    //Si on a une vue de haut, on doit aussi calculer les trajets a droite:
    if (!dessus)
    {
        ptsRight.push_back(rayon._ptA);
    }

    TYTabPoint3D ptsTemp;

    // Creation du segment SR
    Parcours.AjouterSegmentSR(rayon._ptA._value, rayon._ptB._value);

    for (unsigned int i = 0 ; i < tabIntersect.size(); i++)
    {
        if (tabIntersect[i]->bIntersect[indiceIntersect])
        {
            Parcours.AjouterSegmentCoupe((double*)tabIntersect[i]->segInter[indiceIntersect]._ptA._value,
                                         (double*)tabIntersect[i]->segInter[indiceIntersect]._ptB._value,
                                         tabIntersect[i]->isInfra, tabIntersect[i]->isEcran);
        }
    }

    // Traitement du parcours
    Parcours.Traitement();

    //La liste a retourner ne contient pas le dernier point (Recepteur)
    //On retourne 2 listes si on est en vue de haut (parcours lateraux),
    //sinon une seule (vue de face => trajet par dessus les obstacles)
    double dPointTemp[3];
    //Que l'on soit en vue de haut ou non, il faut le trajet gauche
    int nNbrePointsTrajetGauche = Parcours.NombrePointsTrajetGauche();
    OPoint3D pt;
    for (int j = 1; j < (nNbrePointsTrajetGauche - 1); j++) //-1: pas de recepteur en sortie; 1 car la source a deja ete ajoutee
    {
        Parcours.PointTrajetGauche(j, dPointTemp);
        pt._x = dPointTemp[0];
        pt._y = dPointTemp[1];
        pt._z = dPointTemp[2];//x y z
        ptsLeft.push_back(pt);
    }

    if (!dessus)
    {
        int nNbrePointsTrajetDroite = Parcours.NombrePointsTrajetDroite();
        OPoint3D pt;
        for (int j = 1; j < (nNbrePointsTrajetDroite - 1); j++) //-1: pas de recepteur en sortie; 1 car la source a deja ete ajoutee
        {
            Parcours.PointTrajetDroite(j, dPointTemp);
            pt._x = dPointTemp[0];
            pt._y = dPointTemp[1];
            pt._z = dPointTemp[2];//x y z
            ptsRight.push_back(pt);
        }
    }

    return true;
}

OPlan TYAcousticPathFinder::getPlanHorizontal(const OSegment3D& seg)
{
    OPoint3D pt1(seg._ptA);
    OPoint3D pt2(seg._ptB);
    OPoint3D pt3(pt1);

    if (pt2._x != pt1._x)
    {
        pt3._y = pt1._y + 1;
        pt3._x = (pt1._y - pt2._y) * (pt3._y - pt1._y) / (pt2._x - pt1._x) + (pt1._x);
    }
    else
    {
        if (pt1._y != pt2._y)
        {
            pt3._x = pt1._x + 1;
            pt3._y = (pt2._x - pt1._x) * (pt3._x - pt1._x) / (pt1._y - pt2._y) + (pt1._y);
        }
        else // pt1 et pt2 sont confondus on decale pt2 (on construit un plan horizontal)
        {
            pt2._x = pt1._x + 1;
            pt2._y = pt1._y - 1;
            pt3._y = pt1._y + 1;
            pt3._x = (pt1._y - pt2._y) * (pt3._y - pt1._y) / (pt2._x - pt1._x) + (pt1._x);
        }
    }

    return OPlan(pt1, pt2, pt3);
}

void TYAcousticPathFinder::buildDirectRay(OSegment3D& rayon, std::vector<TYRay*>& tabRays)
{
		// Construction du chemin direct
		TYRay *ray = new TYRay();
		ray->setPosSourceGlobal(rayon._ptA);
		ray->setPosReceptGlobal(rayon._ptB);

		TYRayEvent *rayEvent = new TYRayEvent(rayon._ptA);
		rayEvent->type = TYSOURCE;
		ray->addEvent(rayEvent);

		TYRayEvent *rayEvent2 = new TYRayEvent(rayon._ptB);
		rayEvent2->type = TYRECEPTEUR;
		ray->addEvent(rayEvent2);

		tabRays.push_back(ray);
}

void TYAcousticPathFinder::buildSoilReflectedDirecRay(OSegment3D& rayon, std::vector<TYRay*>& tabRays)
{
	TYRay *ray = new TYRay();
	ray->setPosSourceGlobal(rayon._ptA);
	ray->setPosReceptGlobal(rayon._ptB);

	TYRayEvent *rayEvent1 = new TYRayEvent(rayon._ptA);
	rayEvent1->type = TYSOURCE;
	ray->addEvent(rayEvent1);

	ray->addEvent( buildPhysicalReflexOnSoil(rayon) );

	TYRayEvent *rayEvent3 = new TYRayEvent(rayon._ptB);
	rayEvent3->type = TYRECEPTEUR;
	ray->addEvent(rayEvent3);

	tabRays.push_back(ray);
}

TYRayEvent* TYAcousticPathFinder::buildPhysicalReflexOnSoil(OSegment3D& rayon)
{
		// On construit le point symétrique du point A (rayon._ptA) par rapport au plan
		OPoint3D ptSym = _planMoyen.symPtPlan(rayon._ptA);
		OPoint3D ptProj = _planMoyen.projPtPlan(rayon._ptA);

		OPoint3D ptReflex;

		int result = _planMoyen.intersectsSegment(ptSym, rayon._ptB, ptReflex);

		double angleA = OVector3D(ptReflex, ptProj).angle(OVector3D(ptReflex, rayon._ptA));// Angle d'incidence

		// Crée l'évènement et le retourne
		TYRayEvent* rayEvent = new TYRayEvent(ptReflex);
		rayEvent->angle = angleA;
		rayEvent->angletheta = ptSym.distFrom(rayon._ptB);
		rayEvent->type = TYREFLEXIONSOL;
		return rayEvent;
}

void TYAcousticPathFinder::buildForcedReflexOnSoil(OSegment3D& rayon, std::vector<TYRay*>& tabRays, bool sens)
{
		// Liaison avec reflexion
		//              Calcul du point de reflexion
		OSegment3D workSeg = rayon;
		if (!sens) { workSeg = OSegment3D(rayon._ptB, rayon._ptA); }

		OPoint3D ptReflex, ptRayAxH = workSeg._ptA;
		ptRayAxH._z *= _paramH;

		OPoint3D ptSym = _planMoyen.symPtPlan(ptRayAxH);
		OPoint3D ptProj = _planMoyen.projPtPlan(workSeg._ptA);

		int result = _planMoyen.intersectsSegment(ptSym, workSeg._ptB, ptReflex);

		double angleA = OVector3D(ptReflex, ptProj).angle(OVector3D(ptReflex, rayon._ptA));// Angle d'incidence

		// Création du rayon
		TYRay *ray = new TYRay();
		ray->setPosSourceGlobal(rayon._ptA);
		ray->setPosReceptGlobal(rayon._ptB);

		TYRayEvent *rayEvent1 = new TYRayEvent(rayon._ptA);
		rayEvent1->type = TYSOURCE;
		ray->addEvent(rayEvent1);

		TYRayEvent *rayEvent2 = new TYRayEvent(ptReflex);
		rayEvent2->angle = angleA;
		rayEvent2->distNextEvent = ptSym.distFrom(workSeg._ptB);
		rayEvent2->type = TYREFLEXIONSOL;
		ray->addEvent(rayEvent2);

		TYRayEvent *rayEvent3 = new TYRayEvent(rayon._ptB);
		rayEvent3->type = TYRECEPTEUR;
		ray->addEvent(rayEvent3);

		tabRays.push_back(ray);
}

void TYAcousticPathFinder::buildReflectedRays(std::vector<TYSIntersection*>& tabIntersect, OSegment3D& rayon, std::vector<TYRay*>& tabRays)
{
    OSegment3D segInter;
    OSegment3D rayonTmp;

    OSegment3D seg; // Segment source image->recepteur
    OSegment3D segMontant; // Segment source-> point de reflexion
    OSegment3D segDescendant; // Segment point de reflexion->recepteur

    OPoint3D ptSym;
	OPoint3D pt; // Point d'intersection

    size_t nbFaces = tabIntersect.size();

    // Pour chaque face test de la reflexion
    for (unsigned int i = 0; i < nbFaces; i++)
    {
		TYSIntersection* SI = tabIntersect[i];

        // Si la face ne peut interagir on passe a la suivante
        if ( (!SI->isInfra) || !(SI->bIntersect[1]) ) { continue; }

        segInter = SI->segInter[1];

        // Calcul du symetrique de A par rapport au segment
        segInter.symetrieOf(rayon._ptA, ptSym); // On ne s'occupe pas de la valeur de retour de cette fonction
        seg._ptA = ptSym;
        seg._ptB = rayon._ptB; // Segment source image->recepteur

        if (segInter.intersects(seg, pt, TYSEUILCONFONDUS))
        {
            // Construction du segment source->pt de reflexion
            segMontant._ptA = rayon._ptA;
            segMontant._ptB = pt;
            // Construction du segment pt de reflexion-> recepteur
            segDescendant._ptA = segMontant._ptB;
            segDescendant._ptB = rayon._ptB;

			double longueur = seg.longueur(); // Longueur du segment réflechi

            bool intersect = false;
            int j = 0;

            // Si on traverse un autre ecran, qui peut etre de la topo, le chemin de reflexion n'est pas pris en compte
            while ((j < nbFaces) && (!intersect))
            {
                if ((j == i) || !(tabIntersect[j]->bIntersect[1]))
                {
                    j++;
                    continue; // Si la face ne peut interagir on passe a la suivante
                }

                segInter = tabIntersect[j]->segInter[1];

                // On teste si segInter intersecte le segment montant ou
                // le segment descendant dans le plan global).
                // point bidon seul vaut l'intersection ou non
                if ((segInter.intersects(segMontant, pt, TYSEUILCONFONDUS)) ||
                    (segInter.intersects(segDescendant, pt, TYSEUILCONFONDUS)))
                {
                    //intersection trouvee, la boucle peut etre interrompue;
                    intersect = true;
                    break;
                }

                j++;
            }

            if (!intersect) // Si le rayon réflechi n'est pas coupé, il est ajouté
            {
				TYRay *ray = new TYRay();
				ray->setPosSourceGlobal(rayon._ptA);
				ray->setPosReceptGlobal(rayon._ptB);

				TYRayEvent *rayEvent1 = new TYRayEvent(rayon._ptA);
				rayEvent1->type = TYSOURCE;
				ray->addEvent(rayEvent1);

				TYRayEvent *rayEvent2 = new TYRayEvent(pt); // i = indice de la face dans le tableau
				rayEvent2->angletheta = longueur;
				rayEvent2->type = TYREFLEXION;
				rayEvent2->idFace1 = i;
				ray->addEvent(rayEvent2);

				TYRayEvent *rayEvent3 = new TYRayEvent(rayon._ptB);
				rayEvent3->type = TYRECEPTEUR;
				ray->addEvent(rayEvent3);

				tabRays.push_back(ray);
			}
		}
	}
}

void TYAcousticPathFinder::buildDiffractedRays(TYTabPoint3D& pts, OSegment3D& rayon, const bool& vertical, std::vector<TYRay*>& tabRays)
{
	OPoint3D ptSym = _planMoyen.symPtPlan(rayon._ptA);
	
	// On crée les quatre rayons
	TYRay *ray1 = new TYRay(); // Rayon sans réflexion
	ray1->setPosSourceGlobal(rayon._ptA);
	ray1->setPosReceptGlobal(rayon._ptB);

	// On rajoute la source
	TYRayEvent *rayEvent1 = new TYRayEvent(rayon._ptA);
	rayEvent1->type = TYSOURCE;
	ray1->addEvent(rayEvent1);

	// On ajoute l'info "vertical" 
	ray1->setIdentifiant(static_cast<int>(vertical));

	// On duplique sur tous les autres rayons
	TYRay *ray2 = new TYRay(); // rayon avec une réflexion avant
	ray2->addEvent( new TYRayEvent(*rayEvent1) );
	TYRay *ray3 = new TYRay(); // rayon avec une réflexion après
	ray3->addEvent( new TYRayEvent(*rayEvent1) );
	TYRay *ray4 = new TYRay(); // rayon avec deux réflexions (avant et après)
	ray4->addEvent( new TYRayEvent(*rayEvent1) );

	// Pour les rayons 2 et 3 (une seule réflexion), on utilise la position de la source image
	ray2->setPosSourceGlobal(ptSym);
	ray3->setPosSourceGlobal(ptSym);

	// On construit l'évènement réflexion avant
	OSegment3D segAmont(rayon._ptA, pts[1]);
	TYRayEvent *rayEvent2 = buildPhysicalReflexOnSoil(segAmont);

	//Rayons avec une réflexion avant
	ray2->addEvent(rayEvent2);
	ray4->addEvent( new TYRayEvent(*rayEvent2) );

	// Ajout des évènements diffraction
	addDiffractionPointsToTyRay(pts, ray1);
	addDiffractionPointsToTyRay(pts, ray2);
	addDiffractionPointsToTyRay(pts, ray3);
	addDiffractionPointsToTyRay(pts, ray4);

	// On construit l'évènement réflexion après
	OSegment3D segAval(pts[pts.size()-1], rayon._ptB);
	TYRayEvent *rayEvent3 = buildPhysicalReflexOnSoil(segAval);

	//Rayons avec une réflexion après
	ray3->addEvent(rayEvent3);
	ray4->addEvent( new TYRayEvent(*rayEvent3) );

	// On clot tous les rayons
	TYRayEvent *rayEvent4 = new TYRayEvent(rayon._ptB);
	rayEvent4->type = TYRECEPTEUR;
	ray1->addEvent(rayEvent4);
	ray2->addEvent( new TYRayEvent(*rayEvent4) );
	ray3->addEvent( new TYRayEvent(*rayEvent4) );
	ray4->addEvent( new TYRayEvent(*rayEvent4) );

	// On ajoute les rayons au tableau des rayons
	tabRays.push_back(ray1);
	tabRays.push_back(ray2);
	tabRays.push_back(ray3);
	tabRays.push_back(ray4);
}

void TYAcousticPathFinder::addDiffractionPointsToTyRay(TYTabPoint3D& pts, TYRay* ray)
{
	for (size_t i = 1; i < pts.size(); i++) // Le premier point correspond à la source
    {
		TYRayEvent *rayEvent = new TYRayEvent(pts[i]); // i = indice de la face dans le tableau
		rayEvent->type = TYDIFFRACTION;
		ray->addEvent(rayEvent);
	}
}