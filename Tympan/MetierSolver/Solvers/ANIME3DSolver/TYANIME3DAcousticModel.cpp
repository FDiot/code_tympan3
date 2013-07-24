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

#include <map>
#include <list>
#include <vector>
#include <string>
#include <fstream>
#include <iomanip>

#include "Tympan/MetierSolver/ToolsMetier/OBox2.h"

#include "Tympan/MetierSolver/DataManagerCore/TYAcousticModelInterface.h"
#include "Tympan/MetierSolver/DataManagerCore/TYSolverInterface.h"

#include "Tympan/MetierSolver/DataManagerMetier/ComposantGeometrique/TYPolygon.h"
#include "Tympan/MetierSolver/DataManagerMetier/Commun/TYCalcul.h"

#include "Tympan/MetierSolver/AcousticRaytracer/global.h"
#include "Tympan/MetierSolver/AcousticRaytracer/Tools/Logger.h"
#include "Tympan/MetierSolver/AcousticRaytracer/Geometry/Cylindre.h"
#include "Tympan/MetierSolver/AcousticRaytracer/Geometry/Triangulate.h"
#include "Tympan/MetierSolver/AcousticRaytracer/Engine/Simulation.h"

#include "TYANIME3DSolver.h"
#include "TYANIME3DAcousticModel.h"


//TYANIME3DAcousticModel::TYANIME3DAcousticModel() :	_calcul(TYCalcul()),
//													_site(TYSiteNode()),
//													_tabSurfIntersect(NULL),
//													_tabTYRays(TYTabRay())
//{ 
//}

TYANIME3DAcousticModel::TYANIME3DAcousticModel(TYCalcul& calcul, const TYSiteNode& site, TYTabRay &tabRayons, TYStructSurfIntersect* tabStruct) :
																									_calcul(calcul),
																									_site(site),
																									_tabSurfIntersect(tabStruct),
																									_tabTYRays(tabRayons)
{
    _nbRays = _tabTYRays.size();

    OSpectreComplex s1 = OSpectreComplex(OSpectre(1.0));
    OSpectreComplex s0 = OSpectreComplex(OSpectre(0.0));

    _pressAcoustEff = OTabSpectreComplex(_nbRays, s0);
    _absAtm  = OTabSpectreComplex(_nbRays, s1);
    _absRefl = OTabSpectreComplex(_nbRays, s1);
    _absDiff = OTabSpectreComplex(_nbRays, s1);

	_atmos = *(calcul.getAtmosphere());

	// deepcopy is used instead of classical assignation to avoid problem with pointer
	_topo.deepCopy(site.getTopographie());
	TYSiteNode* pSiteParent = static_cast<TYSiteNode*>(&(const_cast<TYSiteNode&>(site)));
	_topo.setParent(static_cast<TYElement*>(pSiteParent));
	// need to call "sortTerrain()" to have a correct identification of terrain when needed
	_topo.sortTerrains();

	// _alti parameter initialized
	_alti = _topo.getAltimetrie().getRealPointer();

    _listeTerrains = _topo.getListTerrain(); 
    _listeTriangles = (*_topo.getAltimetrie()).getListFaces();  

    TYMapElementTabSources mapElementSources = calcul.getResultat()->getMapEmetteurSrcs();
    calcul.getAllSources(mapElementSources, _tabSources);
    calcul.getAllRecepteurs(_tabRecepteur);

	_c = _atmos.getVitSon();  
	_K = _atmos.getKAcoust();  
	_lambda = OSpectre::getLambda(_c); 

    _useFresnelArea = globalUseFresnelArea;
}

TYANIME3DAcousticModel::~TYANIME3DAcousticModel()
{

}


void TYANIME3DAcousticModel::ComputeAbsAtm()
{
    for (int i = 0; i < _nbRays; i++)
    {
		_absAtm[i] = OSpectreComplex(_atmos.getAtt(_tabTYRays[i]->getLength()));
    }
}

void TYANIME3DAcousticModel::ComputeAbsRefl()
{
    double angle = 0.0; // angle d'incidence de l'onde acoustique
    bool NoReflex = true;
    TYMur mur;
    int idBatiment = 0, idFace = 0;
	double sizeRay = 0.0;
	int indice = 0;
    bool OK = FALSE;
	int rayNbr = 0; // Gives the ray number at step i we have to look for inside pathLengthMatrix
	int reflIndice = 0; // Gives the number at which refl occurs
    LPTYRay tyRay;                  // rayon TYMPAN
    OPoint3D Prefl, Pprec, Psuiv;    //pt de reflexion, pt precedent et suivant
    TYTabRayEvent events;           // liste des evenements
	OTabDouble tabPondFresnel;      // tableau des ponderations de Fresnel
    int nbFacesFresnel;             // nbr de faces contenues dans la zone de Fresnel
	TYTabPoint3D triangleCentre;	// Contains all triangles centres



//    OSpectreComplex coefRefl;       // coefficients de reflexion
	OSpectreComplex spectreAbs;
    OSpectreComplex one;
    OSpectreComplex prod;
    OSpectreComplex sum;
    OSpectreComplex prod1;
    OSpectreComplex sum1;
	OSpectreComplex pond;
//    OTabSpectreComplex tabCoefRefl; // tableau des coeff de reflexion sur les terrains

    for (int i = 0; i < _nbRays; i++) // boucle sur les rayons
    {
		std::vector<int> tabRefl = _tabTYRays[i]->getIndexOfEvents(TYREFLEXION | TYREFLEXIONSOL);

		// Be carefull, "sizeRay" must be length covered by the ray traveling from event before reflexion 
		// to event after reflexion passing by reflexion point
		sizeRay = _tabTYRays[i]->getLength(); 

		rayNbr = i;
		one = OSpectreComplex(OSpectre(1.0));
		prod = one;
		prod1 = one;
		sum = OSpectreComplex(OSpectre(0.0));
		sum1 = OSpectreComplex(OSpectre(0.0));
		pond = OSpectreComplex(OSpectre(0.0));

		TYSol* pSol = NULL;
		
		for(int j = 0; j < tabRefl.size(); j++)
		{
			// Initialisation des spectres
			spectreAbs = OSpectreComplex(OSpectre(0.0));

			Prefl = _tabTYRays[i]->getEvents().at(tabRefl[j])->pos;
			Pprec = _tabTYRays[i]->getEvents().at(tabRefl[j])->previous->pos;
			Psuiv = _tabTYRays[i]->getEvents().at(tabRefl[j])->next->pos;
			idFace = _tabTYRays[i]->getEvents().at(tabRefl[j])->idFace1;

			reflIndice = tabRefl[j];
			NoReflex = false; // There is at least one reflection

			// Cas particulier d'une reflexion sur le sol
			if (_tabTYRays[i]->getEvents().at(tabRefl[j])->type == TYREFLEXIONSOL) 
            {
				angle = _tabTYRays[i]->getEvents().at(tabRefl[j])->angle;

				pSol = _topo.terrainAt(Prefl)->getSol();
				pSol->calculNombreDOnde(_atmos);
				spectreAbs = pSol->abso(angle, sizeRay, _atmos);
				double resistivite = _topo.terrainAt(Prefl)->getSol()->getResistivite();
				spectreAbs.getEtat();
			}
            else
			{
				spectreAbs = _tabSurfIntersect[idFace].spectreAbso;  // Recuperation du spectre d'absorption
			}

			// Avec ponderation de Fresnel :
            if(_useFresnelArea)
            { 		
				if (_tabTYRays[i]->getEvents().at(tabRefl[j])->type == TYREFLEXIONSOL) 
				{
					std::cout<< "We start using Fresnel Area on the ground" << std::endl;
					tabPondFresnel = ComputeFrenelWeighting(angle, Pprec, Prefl, Psuiv, rayNbr, reflIndice, triangleCentre);  // calcul des ponderations de Frenel
					nbFacesFresnel = tabPondFresnel.size();  // nbr de triangles dans le zone de Fresnel
					//nbFacesFresnel = 1;
					std::cout << "il y a N ponderations : " << nbFacesFresnel << std::endl;
					
					sum = OSpectreComplex(OSpectre(0.0));
					
					for(int k = 0; k < nbFacesFresnel; k++)
					{ // boucle sur les faces = intersection plan de l'objet intersecte / ellipsoide de Fresnel
						pSol = _topo.terrainAt(triangleCentre[k])->getSol();
						pSol->calculNombreDOnde(_atmos);
						spectreAbs = pSol->abso(angle, sizeRay, _atmos);
						spectreAbs.getEtat();
						pond = spectreAbs * tabPondFresnel[k];
						sum = sum + pond; // calcul du coeff de reflexion moy en ponderant avec les materiaux
					}	
					prod = prod * sum;
				}
				else
				{
					std::cout<< "We start using Fresnel Area on a building" << std::endl;
					tabPondFresnel = ComputeFrenelWeighting(angle, Pprec, Prefl, Psuiv, rayNbr, reflIndice, triangleCentre);  // calcul des ponderations de Frenel
					nbFacesFresnel = tabPondFresnel.size();  // nbr de triangles dans le zone de Fresnel
					std::cout << "il y a N ponderations : " << nbFacesFresnel << std::endl;
				
					sum = OSpectreComplex(OSpectre(0.0));

					for(int k = 0; k < nbFacesFresnel; k++)
					{ // boucle sur les faces = intersection plan de l'objet intersecte / ellipsoide de Fresnel
						pond = spectreAbs * tabPondFresnel[k];
						sum = sum + pond;	// calcul du coeff de reflexion moy en ponderant avec les materiaux
					}
					prod = prod * sum;
				}

			}
			else
			{ 
				prod = spectreAbs; 
			}
			std::cout<< "End of Fresnel Area" << std::endl;
    	}
		if(NoReflex==false) 
		_absRefl[i] = prod;
	}
}

		/////////////////////////////////////:

		//tyRay = _tabTYRays[i];  
  //      events = tyRay->getEvents();
		//sizeRay2 = getSize2(i);
		//rayNbr = i;

  //      for (k = 1; k < events.size()-1; k++) // boucle sur les evenements du rayon
  //      {
  //          ev = events[k];

		//	std::cout << "Il y " << events.size()-1 << " evenements" << std::endl;

  //          if (ev->type == TYREFLEXION || ev->type == TYREFLEXIONSOL) // cas d'une reflexion
  //          {
  //              NoReflex = false;

    //            Pref  = ev->pos;             // pt de reflexion
    //            Pprec = events[k-1]->pos;  // pt precent la reflexion
				//Psuiv = events[k+1]->pos;    // pt suivant la reflexion

				//std::cout << "Psource xyz = " << Pprec._x << " " << Pprec._y << " " << Pprec._z << std::endl;
				//std::cout << "Preflex xyz = " << Pref._x << " " << Pref._y << " " << Pref._z << std::endl;
				//std::cout << "Precept xyz = " << Psuiv._x << " " << Psuiv._y << " " << Psuiv._z << std::endl;

    //            idFace = ev->idFace1;

				//reflIndice = k; // Reflexion occurs at point k

				//// Test pour verifier le bon fonctionnement de l'appel fonction
				//// En attendant que zone fresnel soit operationnelle
				//// tabPondFresnel = ComputeFrenelWeighting(angle, Pprec, Pref, Psuiv, rayNbr, reflIndice);

				//if (ev->type == TYREFLEXIONSOL) // cas particulier d'une reflexion sur le sol
    //            {
				//	 angle = ev->angle;   
				//	 spectreAbs = _topo.terrainAt(Pref)->getSol()->abso(angle, sizeRay2, _atmos);
				//}
    //            else
 //   				spectreAbs = _tabSurfIntersect[idFace].spectreAbso;  // recuperation du spectre d'absorption

    //            // Avec ponderation de Fresnel :

    //            if(_useFresnelArea)
    //            { 				
				//	std::cout<< "We start using Fresnel Area" << std::endl;
				//	tabPondFresnel = ComputeFrenelWeighting(angle, Pprec, Pref, Psuiv, rayNbr, reflIndice);  // calcul des ponderations de Frenel

				//	nbFacesFresnel = tabPondFresnel.size();  // nbr de triangles dans le zone de Fresnel

				//	std::cout << "il y a N ponderations : " << nbFacesFresnel << std::endl;

    //                for(l=0; l<nbFacesFresnel; l++) // boucle sur les faces = intersection plan de l'objet intersecte / ellipsoide de Fresnel
    //                    sum = sum + spectreAbs * tabPondFresnel[l]; // calcul du coeff de reflexion moy en ponderant avec les materiaux

    //                prod = prod * sum;
    //            }
    //            else { prod = spectreAbs; }
				//std::cout<< "End of Fresnel Area" << std::endl;
//            }
//        }
//
//		if(NoReflex==false) 
//		//	_absRefl[i] = prod * (tyRay->getDistanceSR() / sizeRay2 ); 
//		_absRefl[i] = prod;
//    }
//}

//TYMateriauConstruction TYANIME3DAcousticModel::getMateriauFace(Shape* pSurf, const vec3& seg) const
//{
//    TYMateriauConstruction mat;
//    TYMurElement* pMurElem = TYMurElement::safeDownCast(pSurf);
//
//    if (pMurElem != NULL) // Cas d'une face de bi¨«timent ou d'un ecran
//    {
//        TYMur* pMur = TYMur::safeDownCast(pMurElem->getParent());
//
//        OVector3D normale = pSurf->normal();
//        OVector3D rayon(seg._ptA, seg._ptB);
//
//        if (normale.scalar(rayon) < 0.0) // Rayon incident de direction oppose a la normale = face exterieure
//        {
//            mat = pMur->getParoi()->getMatFace2();
//        }
//        else // Materiau de la face interieure
//        {
//            mat = pMur->getParoi()->getMatFace1();
//        }
//    }
//    else // Cas d'une machine ou d'un acousticVolume quelconque
//    {
//        TYElement* pParentSurface = pSurf->getParent();
//        TYAcousticVolume* pVolParentSurface = NULL;
//        while (pParentSurface && !(pParentSurface->inherits("TYAcousticVolume")))
//        {
//            pParentSurface = pParentSurface->getParent();
//        }
//
//        pVolParentSurface = (TYAcousticVolume*)pParentSurface;
//        mat = *pVolParentSurface->getMateriau();
//    }
//
//    return mat;
//}








void TYANIME3DAcousticModel::ComputeAbsDiff()
{
    double delta;                // difference de marche
    OSpectre nbF;                // nbr de Fresnel de chq diffration
    OSpectreComplex absArrete;   // absorption sur chaque arrete
    OSpectreComplex prod = OSpectre(1.0); // produit des absorptions sur chaque arrete
    OSpectre kDelta;             // intermediaire de calcul
    OSpectre mod;                // module du nombre complexe absArrete
    int signe = 1;
	int idBatiment = 0, idFace = 0;
	double precDiff; 
	double diffEnd; 
	double precEnd;
	OVector3D vDiffPrec;
	OVector3D vDiffSuiv;
	OVector3D n1;	
	OVector3D n2;
	OVector3D normal;

	//	int rayNbr; // Gives the ray number at step i
	//LPTYRay tyRay;               // le rayon TYMPAN associe
    //double dd, d1, d2;           // distances directe, au pt precedent et au pt suivant
    //OPoint3D Pdif, Pprec, Psuiv; // pt de diffraction, pt suivant la diffraction et pt precedant la diffraction
    //std::vector<TYRayEvent*> events;
    //TYRayEvent *ev = NULL;
	//OVector3D vDPprec, vDPsuiv, normal, n1, n2;
// CETTE PARTIE EST EN CHANTIER ET N'EST POINT TERMINEE.
    for (int i = 0; i < _nbRays; i++) // boucle sur les rayons
    {
//		ray = *_tabTYRays[i];

        //tyRay = _tabTYRays[i];
        //events = tyRay->getEvents();
        //nbEvents = events.size();

		//rayNbr = i;
		prod = OSpectre(1.0);

		//std::vector<int> tabDiff = ray.getIndexOfEvents(TYDIFFRACTION); // gets a vector where diff occur
		std::vector<int> tabDiff = _tabTYRays[i]->getIndexOfEvents(TYDIFFRACTION); // gets a vector where diff occur

		for (int j = 0; j < tabDiff.size(); j++)
		{
			precDiff = _tabTYRays[i]->getEvents().at(tabDiff[j])->previous->distNextEvent;
			diffEnd = _tabTYRays[i]->getEvents().at(tabDiff[j])->distEndEvent;
			precEnd = _tabTYRays[i]->getEvents().at(tabDiff[j])->previous->distEndEvent;

			vDiffPrec = OVector3D(_tabTYRays[i]->getEvents().at(tabDiff[j])->pos, _tabTYRays[i]->getEvents().at(tabDiff[j])->previous->pos);
			vDiffSuiv = OVector3D(_tabTYRays[i]->getEvents().at(tabDiff[j])->pos, _tabTYRays[i]->getEvents().at(tabDiff[j])->next->pos);
			n1 = _tabSurfIntersect[ _tabTYRays[i]->getEvents().at(tabDiff[j])->idFace1].normal; // normale de la 1ere face
			n2 = _tabSurfIntersect[ _tabTYRays[i]->getEvents().at(tabDiff[j])->idFace2].normal; // normale de la 2e face
			normal = n1 + n2; // somme des normales

			if (vDiffPrec.scalar(normal)>0.0 && vDiffSuiv.scalar(normal)>0.0)
			{
				signe = -1;
			}
			else
			{
				signe = 1;
			}
			delta = signe * (precDiff + diffEnd - precEnd);
			kDelta = _K * delta;
            nbF = _lambda.invMult(2.0 * delta);
			
			if(delta >  (_lambda.div(20)).valMax())
			{
				mod = (((nbF * 20.0 + 3.0)).sqrt()).inv();
			}
			else{
				mod = 1;
			}

            absArrete = OSpectreComplex(mod, kDelta);
            prod = prod * absArrete;
		}
		_absDiff[i] = prod;

		//for (int j = 1; j < nbEvents - 1; j++) // boucle sur les evenements
  //      {
  //          ev = events[j];

  //          if (events[j]->type == TYDIFFRACTION) // cas d'une diffraction
  //          {
		//		Pdif  = ev->pos;          // pt de diffraction
		//		Pprec = events[j-1]->pos; // pt de diffraction precent 
		//		Psuiv = events[j+1]->pos; // pt de diffraction suivant

  //              //d1 = Pdif.distFrom(Pprec);  
  //              //d2 = Pdif.distFrom(Psuiv);  
  //              dd = Pprec.distFrom(Psuiv); // rayon direct
		//		d1 = getDistance(rayNbr, j-1);  
  //              d2 = getDistance(rayNbr, j);  
  // 
		//		vDPprec = OVector3D(Pdif, Pprec); // vecteur du pt de diff vers le pt prec
  //              vDPsuiv = OVector3D(Pdif, Psuiv); // vecteur du pt de diff vers le pt suiv

		//		n1 = _tabSurfIntersect[events[j]->idFace1].normal; // normale de la 1ere face
		//		n2 = _tabSurfIntersect[events[j]->idFace2].normal; // normale de la 2e face

		//		normal = n1 + n2; // somme des normales

		//		if( vDPprec.scalar(normal)>0.0 && vDPsuiv.scalar(normal)>0.0 ) 
		//		{
		//			signe=-1;
		//		}
		//		else
		//		{
		//			signe=1;
		//		}

  //              delta = signe * (d1 + d2 - dd);

  //              kDelta = _K * delta;
  //              nbF = _lambda.invMult(2.0 * delta);

		//		// ANCIENNE VALEUR
  //              //mod = (((nbF * 20.0 + 3.0).abs()).sqrt()).inv();
		//		// VERIFIER SI .valmax() est le bon argument
		//		if(delta >  (_lambda.div(20)).valMax())
		//		{
		//			mod = (((nbF * 20.0 + 3.0)).sqrt()).inv();
		//		}
		//		else{
		//			mod = 1;
		//		}

  //              absArrete = OSpectreComplex(mod, kDelta);
  //              prod = prod * absArrete;
  //          }
  //      }
  //      _absDiff[i] = prod;
    }
}

// calcul de la zone de Fresnel pour une diffraction donnee - Approximation : zone = boite englobante de l'ellipsoide de Fresnel
OBox2 TYANIME3DAcousticModel::ComputeFrenelArea(double angle, OPoint3D Pprec, OPoint3D Prefl, OPoint3D Psuiv, int rayNbr, int reflIndice)
{
    OBox2 fresnelArea;  // boundingBox de l'ellispsoide de Fresnel

	Prefl = _tabTYRays[rayNbr]->getEvents().at(reflIndice)->pos;
	Pprec = _tabTYRays[rayNbr]->getEvents().at(reflIndice)->previous->pos;
	Psuiv = _tabTYRays[rayNbr]->getEvents().at(reflIndice)->next->pos;

	double distPrefPsuiv = _tabTYRays[rayNbr]->getEvents().at(reflIndice)->distNextEvent;
	double dr = _tabTYRays[rayNbr]->getEvents().at(reflIndice)->previous->distNextEvent + distPrefPsuiv;
	double dd = _tabTYRays[rayNbr]->getEvents().at(reflIndice)->distPrevNext;

	OSpectre f = OSpectre::getOSpectreFreqExact(); //frequence
	std::cout << "f = " << f.valMax() << std::endl;
    OSpectreComplex Q = _topo.terrainAt(Prefl)->getSol()->calculQ(angle, distPrefPsuiv, _atmos) ; // coeff de reflexion du sol au pt de reflexion
    const double c1 = 0.5 * _c /(dr - dd);  // cste de calcul
	OSpectre phaseQdivPI = Q.getPhase().div(M_PI);
	OSpectre fmin = (OSpectre(0.5) - phaseQdivPI)*c1;
    OSpectre fmax = (OSpectre(1.0) - phaseQdivPI)*c1; 
    OSpectre fc = (fmin * fmax).sqrt();      // frequence de transition

	std::cout << "fmin = " << fmin.valMax() << std::endl;
	std::cout << "fmax = " << fmax.valMax() << std::endl;
	std::cout << "fc = " << fc.valMax() << std::endl;

    //const OSpectre F = (OSpectre(1.0)-((f*f).div(fc*fc)).exp(1.0))*32.0; // parametre de l'ellipsoide de Fresnel F_{\lambda} via la formule definie dans la publie de D. van Maercke et J. Defrance (CSTB-2007)
 
	OSpectre A = (fc*fc)*-1;
	OSpectre B = f*f;
	OSpectre C = B.div(A);
	//const OSpectre F = (OSpectre(1.0)-(C.exp()))*32;
	// F fixe a 1 pour eviter une boite trop grande a basse freq
	const OSpectre F = OSpectre(1.0); 
	
	//OSpectre lF = _lambda * F;	// produit lambda*Flbd
	// lF fixe a lambda/2
	OSpectre lF = _lambda.div(2.0)*F;

    // 1/ boite dans le repere (Ox,Oy,Oz)
    // Construction de la bb de l'ellipsoide de Fresnel
	
	// S' depends on the point where the reflection occurs
	int reflFace =  _tabTYRays[rayNbr]->getEvents().at(reflIndice)->idFace1;
	TYAcousticSurface *face = TYAcousticSurface::safeDownCast(_tabSurfIntersect[reflFace].pSurfGeoNode->getElement());	
	OPlan P = face->getPlan();
	OPoint3D SIm = P.symPtPlan(Pprec);
	double dSImR = SIm.distFrom(Psuiv);

    // Dimensions de la boite de Fresnel : on choisit de prendre la + grande boite
	// grd cote de la boite
	// (lF + dSImR).valMax(); 
	// petit cote de la boite == hauteur ici
	//((lF*(lF + 2.*dSImR)).sqrt()).valMax();

	// On fixe une frequence de 100 Hz pour etudier de plus pres la boite
	double L = lF.getValueReal(100.f) + dSImR;                  
	double l = sqrt(lF.getValueReal(100.f)*(lF.getValueReal(100.f)+2.*dSImR));

	// On choisit de fixer la longueur de la boite a S'R
	// et sa largeur a 0.5*S'R en attendant de mieux dimensionner.
	// Cela permet de tester la boite et l'algorithme de ponderation
	//double L = dSImR;
	//double l = 0.5*dSImR;
	
	std::cout << "L = " << L << " l = " << l << std::endl;

    // Boite de Fresnel placee a l'origine du repere
    OBox box = OBox(OCoord3D(0,0,0),OCoord3D(L,l,l));
	ORepere3D repere; 
	fresnelArea = OBox2(box, repere);

    // Translation au pt O milieu du segment [SIm R]
	OPoint3D O = OPoint3D(0.5*(SIm._x + Psuiv._x), 0.5*(SIm._y + Psuiv._y), 0.5*(SIm._z + Psuiv._z)); 
	const OPoint3D vt = OPoint3D(O._x-(0.5*L), O._y-(0.5*l), O._z-(0.5*l)); // Translation depuis le centre
	fresnelArea.Translate(vt); // translation de la zone de Fresnel
	
    //3/ Changement de repere pour faire pivoter la boite
	// The aim here is to make a rotation of the bounding box
	// We want its main axis to be aligned with Vector v2 i.e. [OPsuiv].
	// We have to keep in mind that axes are defined from the bottom left corner
	// of the box.
	// This step is a bit tricky : one has to keep in mind in
	// which plane(s) the rotation(s) occur(s).
	// 3D rotation : 2 rotations around two different axes 
	// (pay attention to get the right ones!);
	// 2D rotation : only one movement => define the right axis.
	// Let x1, y1, z1 and x2, y2, z2 be the vectors after the first rotation
	// and the second one, respectively expressed in the new basis B1 and B2. 
	// Let xf, yf, zf be the final vectors given to the box, i.e. x2,y2 and z2
	// in B0.

	// Let I be the point at coordinate(L, l/2, l/2)
	// This point will be useful to create a vector from the box centre to I
	// This is a way of creating an axis within the box in order to compute 
	// the angle between the box main axis and vector v2
	OPoint3D I = OPoint3D(fresnelArea._H._x, O._y, O._z);

	// Let v1 be the vector between the box centre (O) and I
	// Let v2 be the vector between the box centre (O) and Psuiv
	OVector3D v1 = OVector3D(O,I);
    OVector3D v2 = OVector3D(O,Psuiv);

	v2.normalize();
	v1.normalize();

	OVector3D x0 = v1;

	// When v1 and v2 are in xOy plane only
	if (v1._z == 0 && v2._z == 0)
	{
		// Creates a basis with 3 vectors x0, y0 and z0.
		// x0 -> x2, y0 -> y2 and z0 remains the same.
		OVector3D y0 = OCoord3D(0.0,1.0,0.0);
		OVector3D z0 = OCoord3D(0.0,0.0,1.0);
		double alpha = v1.angle(v2);
		OVector3D x2 = getRotationOzOy(alpha, 0.0, x0);
		OVector3D y2 = getRotationOzOy(alpha, 0.0, y0);
		x2.normalize();
		y2.normalize();

		// Final step : New ORepere3D
		ORepere3D r = ORepere3D(O, x2, y2, z0);
		fresnelArea.BoxRotationOzOy(alpha, 0.0);
	    fresnelArea = OBox2(fresnelArea, r, O);
		return fresnelArea;
	}

	// When v1 and v2 are in xOz plane only
	if (v1._y == 0 && v2._y == 0)
	{
		// Creates a basis with 3 vectors x0, y0 and z0.
		// x0 -> x2, z0 -> z2 and y0 remains the same.
		OVector3D y0 = OCoord3D(0.0,1.0,0.0);
		OVector3D z0 = OCoord3D(0.0,0.0,1.0);
		double theta = v1.angle(v2);
		OVector3D x2 = getRotationOzOy(0.0, theta, x0);
		OVector3D z2 = getRotationOzOy(0.0, theta, z0);
		x2.normalize();
		z2.normalize();

		// Final step : New ORepere3D
		ORepere3D r = ORepere3D(O, x2, y0, z2); 
		fresnelArea._center._x = O._x;
		fresnelArea._center._y = O._y;
		fresnelArea._center._z = O._z;
		fresnelArea.BoxRotationOzOy(0.0, theta);
	    fresnelArea = OBox2(fresnelArea, r, O);
		return fresnelArea;
	}

	// When it's a 3d rotation, things get a little more "funny".
	// 2 steps needed : always a first rotation around Z axis to get Ox
	// just "under" v2 and then a rotation around Y depending on the angle.
	// Second step : rotate vectors x' and z' around Oy axis : x'' is now
	// aligned with vector v2.
	// Let's call them x0, x1, and x2.
	// Between the two rotations, (x1, y1, z1) have to become a new basis B1.
	else
	{
		// First step is to perform a rotation around Oz axis : 
		// Vectors x and y will change and become x' and y', respectively.
		std::cout << "Rotation 3d " << std::endl;
		OVector3D y0 = OCoord3D(0.0,1.0,0.0);
		OVector3D z0 = OCoord3D(0.0,0.0,1.0);

		// Construction du projete en supposant un sol plat
		const OCoord3D proj = OCoord3D(Psuiv._x, Psuiv._y, 0.0);

		// Vector from the bb center to proj
		OVector3D O2H = OVector3D(O,proj);
		double alpha = v1.angle(O2H);

		OVector3D x1b0 = getRotationOz(alpha, x0);
		OVector3D y1b0 = getRotationOz(alpha, y0);

		// This step defines x1, y1 and z1 as a new basis B1.
		OVector3D x1b1 = getRotationOzBase2(alpha, x1b0);
		OVector3D y1b1 = getRotationOzBase2(alpha, y1b0);
		x1b1.normalize();
		y1b1.normalize();
	
		// Second step
		// Returns the angle necessary to perform the rotation around OY axis
		double theta = O2H.angle(v2);
		std::cout << "angle theta" << theta << std::endl;

		OVector3D x2b1 = getRotationOy(theta, x1b1);
		OVector3D z2b1 = getRotationOy(theta, z0);

		// x2 , y2 and z2 in B0
		OVector3D x2b0 = getRotationOzOy(alpha, theta, x0);
		OVector3D z2b0 = getRotationOzOy(0.0, theta, z0);

		// Final step : New ORepere3D
		ORepere3D r = ORepere3D(O,x2b0 ,y1b0, z2b0); 
		fresnelArea.BoxRotationOzOy(alpha, theta);
	    fresnelArea = OBox2(fresnelArea, r, O);

		return fresnelArea;
	}
}

OTabDouble TYANIME3DAcousticModel::ComputeFrenelWeighting(double angle, OPoint3D Pprec, OPoint3D Prefl, OPoint3D Psuiv, int rayNbr, int reflIndice, TYTabPoint3D& triangleCentre)
{
    OTabDouble tabPond;				// tableau des ponderation de Fresnel
    OTabDouble tabSurface;			// tableau des surfaces des triangles de la zone de Fresnel
    double surfaceTot = 0.0;		// somme des surfaces des triangles de la zone de Fresnel
	const double& delaunay = 10e-6;	// Parameter needed to compute triangulation

	std::cout << "Dans ComputeFrenelWeighting :" << std::endl;
	// And bb was born
	OBox2 fresnelArea = ComputeFrenelArea(angle, Pprec, Prefl, Psuiv, rayNbr, reflIndice);

	// fE/fF/fG/fH are the faces under the box corners E/F/G/H
	LPTYPolygon fE = (*_topo.getAltimetrie()).getFaceUnder(fresnelArea._E);
	LPTYPolygon fF = (*_topo.getAltimetrie()).getFaceUnder(fresnelArea._F);
	LPTYPolygon fG = (*_topo.getAltimetrie()).getFaceUnder(fresnelArea._G);
	LPTYPolygon fH = (*_topo.getAltimetrie()).getFaceUnder(fresnelArea._H);

	// Defines 4 planes from the 4 surfaces
	OPlan ePlane = fE->plan();
	OPlan fPlane = fF->plan();
	OPlan gPlane = fG->plan();
	OPlan hPlane = fH->plan();

	// These are the four projections onto the planes defined previously
	// This is a way of taking into account the ground altitude
	OPoint3D eProj = ePlane.projPtPlan(fresnelArea._E);
	OPoint3D fProj = fPlane.projPtPlan(fresnelArea._F);
	OPoint3D gProj = gPlane.projPtPlan(fresnelArea._G);
	OPoint3D hProj = hPlane.projPtPlan(fresnelArea._H);

	// We get back the points inside the zone
	TYTabPoint pointsInside;
	int nbPts = _alti->getPointsInBox(eProj, fProj, gProj,hProj, pointsInside);

	// We create a tabPoint containing all the points : pointsInside the zone + projection
	TYTabPoint ptsTriangulation = pointsInside;
	ptsTriangulation.push_back(eProj);
	ptsTriangulation.push_back(fProj);
	ptsTriangulation.push_back(gProj);
	ptsTriangulation.push_back(hProj);

	// We triangulate the zone defined by the four projections
	// i.e. the one between eProj, fProj, gProj, hProj
	// and with the points which are inside this zone
	// We compute the triangulation inside the zone of interest
	_oTriangles = ComputeTriangulation(ptsTriangulation, delaunay);
	
	// We wanna get each triangle centre in order to know the ground type under the centre
	// Then we get the triangle surface and compute fresnel weighting
	double trSurface = 0.0;
	unsigned long nbTriangles = _oTriangles.size();
	tabPond.reserve(nbTriangles);
	tabSurface.reserve(nbTriangles);

	// For each triangle, we get the centre and the surface
	for(int k = 0; k < nbTriangles; k++)
	{
		OTriangle triangleK = _oTriangles.at(k);
		tabSurface.push_back(triangleK.getSurface());
		surfaceTot += tabSurface.at(k);
		triangleCentre.push_back(triangleK.getCentre());
	}			
	std::cout << "Surface totale = " << surfaceTot << std::endl;		
	
	// This part computes a ponderation depending on the triangle surface
	for(int k = 0; k < nbTriangles; k++)
	{
		tabPond.push_back(tabSurface.at(k)/surfaceTot);
	}

	std::cout << "End of ComputeFresnelWeighting " << std::endl;
	
	return tabPond;
}

std::vector<OTriangle> TYANIME3DAcousticModel::ComputeTriangulation(const TYTabPoint& points, const double& delaunay)
{

	// This function returns a vector of triangles created within the bounding box
    unsigned int i = 0, j = 0;	// Indices needed	
	ODelaunayMaker oDelaunayMaker(delaunay);

    // Set des vertex
    unsigned int nbOfPts = static_cast<uint32>(points.size());
    for (i = 0; i < nbOfPts; i++)
    {
        oDelaunayMaker.addVertex(points[i]);
    }

    // Generate
    oDelaunayMaker.compute();

    // Get mesh : A triangle is a combination of a face with vertices
    QList<OTriangle> triangles = oDelaunayMaker.getFaces();
	QList<OPoint3D> vertexes = oDelaunayMaker.getVertex();

    unsigned long nbTriangles = triangles.count();
	// This is the returned vector of triangles
	std::vector<OTriangle> listeTriangle;

	// This step associates the corners with their coordinates
	// Each face has 3 corners named _p1/_p2/_p3 that need to be 
	// linked  with the OPoint3D _A/_B/_C
	for(int m = 0; m < nbTriangles; m++)
	{
		listeTriangle.push_back(triangles[m]);	// We get the triangle
		listeTriangle.at(m)._A._x = vertexes[listeTriangle[m]._p1]._x;
		listeTriangle.at(m)._A._y = vertexes[listeTriangle[m]._p1]._y;
		listeTriangle.at(m)._A._z = vertexes[listeTriangle[m]._p1]._z;
		listeTriangle.at(m)._B._x = vertexes[listeTriangle[m]._p2]._x;
		listeTriangle.at(m)._B._y = vertexes[listeTriangle[m]._p2]._y;
		listeTriangle.at(m)._B._z = vertexes[listeTriangle[m]._p2]._z;
		listeTriangle.at(m)._C._x = vertexes[listeTriangle[m]._p3]._x;
		listeTriangle.at(m)._C._y = vertexes[listeTriangle[m]._p3]._y;
		listeTriangle.at(m)._C._z = vertexes[listeTriangle[m]._p3]._z;
	}

	return listeTriangle;
}

void TYANIME3DAcousticModel::ComputePressionAcoustEff()
{

    //const double W0 = pow(10.0, -12.0);
    //double dSR;  // distance source recepteur
    //LPTYRay tyRay = NULL;    // rayon

	OSpectre phase; // phase du nombre complexe _pressAcoustEff[i] pour chq i
    OSpectre mod;   // module du nombre complexe _pressAcoustEff[i] pour chq i
	const double rhoc = _atmos.getImpedanceSpecifique(); //400.0;   // kg/m^2/s
    double c1;      // constante du calcul
    OPoint3D S, P0; // pt de la source et pt du 1er evenement
    OSegment3D seg; // premier segment du rayon
    OSpectre directivite, wSource; // fonction de directivite et spectre de puissance de la source
    TYSourcePonctuelle* source; // source
    OSpectreComplex prodAbs; // produit des differentes absorptions
	double totalRayLength; // Computes the total ray length including reflections only (diffractions are not included)
	double distSR1; // Computes the distance between the source and the first reflection
	double distToNext; // Computes the distance
	bool reflection = false;

    for (int i = 0; i < _nbRays; i++) // boucle sur les rayons
    {
		totalRayLength = 0.0; // Computes the total ray length including reflections only (diffractions are not included)
		distSR1 = 0.0; // Computes the distance between the source and the first reflection
		distToNext = 0.0; // Computes the distance 
		reflection = false;
		source = _tabTYRays[i]->getSource();
		std::vector<int> reflTab = _tabTYRays[i]->getIndexOfEvents(TYREFLEXION | TYREFLEXIONSOL);

		if(reflTab.size() != 0)
		{
			reflection = true;
		}

		for(int j = 0; j < reflTab.size(); j++)
		{
			distToNext += _tabTYRays[i]->getEvents().at(reflTab[j])->distEndEvent;
		}

		if(reflection == true)
		{
			distSR1 = _tabTYRays[i]->getEvents().at(0)->distEndEvent;
			totalRayLength = distSR1 + distToNext;
		}
		else
		{
			totalRayLength = _tabTYRays[i]->getEvents().at(0)->distEndEvent;
		
		}
		
		c1 = 4.0 * M_PI * totalRayLength * totalRayLength;
		S  = *source->getPos();
		P0 = _tabTYRays[i]->getEvents().at(1)->pos;
        seg = OSegment3D(S, P0);
		
		directivite = source->lwApparenteSrcDest(seg, _atmos);
		wSource = (*source->getSpectre()).toGPhy(); // ou wSource = (*TYSourcePonctuelle::safeDownCast(_tabSources[i]->getElement())->getSpectre()).toGPhy(); // new !!

        prodAbs = _absAtm[i] * _absRefl[i] * _absDiff[i];

        mod = ((directivite * wSource * rhoc) * (1. / c1)).sqrt() * prodAbs;
		phase = _K.mult(_tabTYRays[i]->getLength());

        _pressAcoustEff[i] = OSpectreComplex(mod, phase);
	}
}

OTab2DSpectreComplex TYANIME3DAcousticModel::ComputePressionAcoustTotalLevel()
{
    OSpectre C;         // facteur de coherence et son carre
    OSpectre un = OSpectre(1.0);
    OSpectreComplex sum1;	//somme partielle
    OSpectreComplex sum3;
    OSpectre sum2;			//somme partielle
    OSpectre mod;		    // module et module au carre
	const OSpectre K2 = _K*_K;				// nombre d'onde au carre
 
	double incerRel = 0.001;  // incertitude relative sur la taille du rayon au carree
    double cst = (pow(2., 1. / 6.) - pow(2., -1. / 6.)) * (pow(2., 1. / 6.) - pow(2., -1. / 6.)) / 3.0 + incerRel * incerRel; // constante pour la definition du facteur de coherence
	//double dSR;				// distance source/recepteur
	double totalRayLength;

    const int nbSources    = _tabSources.size();           // nbr de sources de la scene
    const int nbRecepteurs = _tabRecepteur.size();         // nbr de recepteurs de la scene

	TYSourcePonctuelle* source;
    //TYPointControl* recept;
	TYPointCalcul* recept;

    OTab2DSpectreComplex tabPressionAcoust(nbSources);

	for (int i = 0; i < nbSources; i++){
		tabPressionAcoust[i].resize(nbRecepteurs);
	}
	//LPTYRay tyRay;

    for (int i = 0; i < nbSources; i++) // boucle sur les sources
    {
        for (int j = 0; j < nbRecepteurs; j++) // boucle sur les recepteurs
        {
			sum1 = OSpectreComplex(OSpectre(0.0), OSpectre(0.0));
            sum2 = OSpectreComplex(OSpectre(0.0), OSpectre(0.0));

			// TEST
			sum3 = OSpectreComplex(OSpectre(0.0), OSpectre(0.0));
			//

            for (int k = 0; k < _nbRays; k++) // boucle sur les rayons allant de la source au recepteur
            {
                //tyRay =  _tabTYRays[k];

				source = _tabTYRays[k]->getSource();
				recept = _tabTYRays[k]->getRecepteur();
                //recept = (TYPointControl*) tyRay->getRecepteur();

				if (source == (TYSourcePonctuelle*)(_tabSources[i]->getElement()) && recept == (TYPointCalcul*)(_tabRecepteur[j]->getElement()))   // test si la source est celle en cours idem pour recepteur
                {
                    //dSR = tyRay->getDistanceSR();
					totalRayLength = _tabTYRays[k]->getLength();
                    mod = (_pressAcoustEff[k]).getModule();
                    //C = (K2 * dSR * dSR * (-1) * cst).exp();
					//TRUE ONE
					//C = (K2 * totalRayLength * totalRayLength * (-1) * cst).exp();
					C = 1.0;
					//sum1 = _pressAcoustEff[k] * C;
					sum3 = _pressAcoustEff[k] * C;
					sum1 = sum1 + sum3;
                    sum2 = sum2 + mod * mod * (un - C*C);
                }
            }
			// Be carefull sum of p²!= p² of sum
			sum1 = sum1.getModule() * sum1.getModule();
			tabPressionAcoust[i][j] = sum1 + sum2;
        }
    }
    return tabPressionAcoust;
}

OTab2DSpectreComplex TYANIME3DAcousticModel::ComputeAcousticModel()
{

    ComputeAbsAtm();

    ComputeAbsRefl();

    ComputeAbsDiff();

    ComputePressionAcoustEff();

    return ComputePressionAcoustTotalLevel();
}

double TYANIME3DAcousticModel::getSize2(int rayNbr)
{
    double raySize = 0.0; // initialisation
// TO CHANGE	int nbr_elements = _pathLengthMatrix.at(rayNbr).size();

// TO CHANGE    for (int i = 0; i < nbr_elements; i++)
// TO CHANGE    {
// TO CHANGE		raySize += _pathLengthMatrix[rayNbr][i];
// TO CHANGE    }
    
	return raySize;
}

double TYANIME3DAcousticModel::getDistance(int rayNbr, int indice)
{
	double length = .0;
// TO CHANGE	length = _pathLengthMatrix[rayNbr][indice];
	return length;
}

OVector3D TYANIME3DAcousticModel::getRotationOz(double alpha, OVector3D V)
{
	double cosA = cos(alpha);
	double sinA = sin(alpha);
	OVector3D Vfinal;
	// Computes the calculations
	Vfinal._x = cosA*V._x - sinA*V._y;
	Vfinal._y = sinA*V._x + cosA*V._y;
	Vfinal._z = V._z;
	return Vfinal;
}

OVector3D TYANIME3DAcousticModel::getRotationOy(double alpha, OVector3D V)
{
	double cosA = cos(alpha);
	double sinA = sin(alpha);
	OVector3D Vfinal;
	// Computes the calculations
	Vfinal._x = cosA*V._x - sinA*V._z;
	Vfinal._y = V._y;
	Vfinal._z = sinA*V._x + cosA*V._z;
	return Vfinal;
}

OVector3D TYANIME3DAcousticModel::getRotationOzBase2(double alpha, OVector3D V)
{
	double cosA = cos(alpha);
	double sinA = sin(alpha);
	OVector3D Vfinal;
	// Computes the calculations
	Vfinal._x = cosA*V._x + sinA*V._y;
	Vfinal._y = -sinA*V._x + cosA*V._y;
	Vfinal._z = V._z;
	return Vfinal;
}

OVector3D TYANIME3DAcousticModel::getRotationOyBase2(double alpha, OVector3D V)
{
	double cosA = cos(alpha);
	double sinA = sin(alpha);
	OVector3D Vfinal;
	// Computes the calculations
	Vfinal._x = cosA*V._x + sinA*V._z;
	Vfinal._y = V._y;
	Vfinal._z = - sinA*V._x + cosA*V._z;
	return Vfinal;
}

OVector3D TYANIME3DAcousticModel::getRotationOzOy(double alpha, double theta, OVector3D V)
{
	double cosA = cos(alpha);
	double sinA = sin(alpha);
	double cosB = cos(theta);
	double sinB = sin(theta);
	OVector3D Vfinal;
	// Computes the calculations
	Vfinal._x = cosA*cosB*V._x - sinA*V._y - cosA*sinB*V._z;
	Vfinal._y = sinA*cosB*V._x + cosA*V._y - sinA*sinB*V._z;
	Vfinal._z = sinB*V._x + cosB*V._z;
	return Vfinal;	
}

//OVector3D TYANIME3DAcousticModel::getRotationOx(double alpha, OVector3D V)
//{
//	double cosA = cos(alpha);
//	double sinA = sin(alpha);
//	OVector3D Vfinal;
//	// Computes the calculations
//	Vfinal._x = V._x;
//	Vfinal._y = cosA*V._y - sinA*V._z;
//	Vfinal._z = sinA*V._y + cosA*V._z;
//	return Vfinal;
//}