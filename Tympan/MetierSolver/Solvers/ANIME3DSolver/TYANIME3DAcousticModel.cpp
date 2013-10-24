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

/**
 * \file TYINProAcousticModel.cpp
 * \brief Le modele acoustique de la methode ANIME3D
 * \author Projet_Tympan - EDF-R&D/AMA
 * \date 04 avril 2011
 */


#include "TYANIME3DAcousticModel.h"
#include "Tympan/MetierSolver/DataManagerMetier/ComposantGeometrique/TYPolygon.h"


TYANIME3DAcousticModel::TYANIME3DAcousticModel()
{ 
}

TYANIME3DAcousticModel::TYANIME3DAcousticModel(TYCalcul& calcul, const TYSiteNode& site, TabRays* tabRayons, TYStructSurfIntersect* tabStruct)
{
    _tabTYRays = calcul.getAllRays();

    _nbRays = _tabTYRays.size();

	_tabRays = tabRayons;
	//_tabRays = simulation.getSolver()->getValidRays();
	//    _tabRays = acoustPathFinder.getTabRays();

	_tabSurfIntersect = tabStruct;

    OSpectreComplex s1 = OSpectreComplex(OSpectre(1.0));
    OSpectreComplex s0 = OSpectreComplex(OSpectre(0.0));

    _pressAcoustEff = OTabSpectreComplex(_nbRays, s0);
    _absAtm  = OTabSpectreComplex(_nbRays, s1);
    _absRefl = OTabSpectreComplex(_nbRays, s1);
    _absDiff = OTabSpectreComplex(_nbRays, s1);

	_atmos = *(calcul.getAtmosphere());
	_topo = *site.getTopographie();        

    _listeTerrains = _topo.getListTerrain(); 
    _listeTriangles = (*_topo.getAltimetrie()).getListFaces();  

    TYMapElementTabSources mapElementSources = calcul.getResultat()->getMapEmetteurSrcs();
    calcul.getAllSources(mapElementSources, _tabSources);
    calcul.getAllRecepteurs(_tabRecepteur);

	_c = _atmos.getVitSon();  
	_K = _atmos.getKAcoust();  
	_lambda = OSpectre::getLambda(_c); 
}

TYANIME3DAcousticModel::~TYANIME3DAcousticModel()
{

}


void TYANIME3DAcousticModel::ComputeAbsAtm()
{
    for (int i = 0; i < _nbRays; i++)
    {
        _absAtm[i] = OSpectreComplex(_atmos.getAtt((_tabTYRays[i])->getSize()));
    }
}

void TYANIME3DAcousticModel::ComputeAbsRefl()
{
    OTabSpectreComplex tabCoefRefl;  // tableau des coeff de reflexion sur les terrains
    OSpectreComplex coefRefl;   // coefficients de reflexion

    Ray* ray;                   // rayon
    LPTYRay tyRay;              // rayon TYMPAN
    
	//TYPolygon face;             // face ou a lieu la reflexion
    OPoint3D Pref, Pprec, Psuiv;       //pt de reflexion, pt précédent et suivant
    TYTabRayEvent events;         // liste des evenements
    
	//OVector3D vInc, vNorm;             // vecteurs : normale à la face et vecteur de l'onde incidente reflechie
    
	OTabDouble tabPondFresnel;  // tableau des ponderations de Fresnel
    int nbFacesFresnel;         // nbr de faces contenues dans la zone de Fresnel

    OSpectreComplex one = OSpectreComplex(OSpectre(1.0));
    OSpectreComplex prod = one;
    OSpectreComplex sum = OSpectreComplex(OSpectre(0.0));

    double d1 = 0.0;	// distance du pt de reflexion au point de l'évènement précédent
    double angle = 0.0; // angle d'incidence de l'onde acoustique
	double sizeRay;		// taille du rayon

    int i, j, k, l, indice;
    i = 0; j = 0; k = 0; l = 0; indice = 0;
    bool OK = FALSE;

    //vector< int > rayFacesHistory;      // Liste des identifiants des faces rencontres par le rayon
    //vector< int > rayParoisHistory;     // Liste des identifiants des batiments rencontres par le rayon
    //vector< int > rayPrimitiveHistory;  // Liste des primitives des batiments rencontres par le rayon

    std::vector<Event*> listeEvents;
    Shape* surfaceRefl;
    //TYPE_SURFACE type;

    TYMur mur;
    int idBatiment = 0, idFace = 0;
    bool zoneFresnel = FALSE;
	double* spectreAbs;
	TYRayEvent ev;

    for (i = 0; i < _nbRays; i++) // boucle sur les rayons
    {
        ray = _tabRays->at(i);  
		tyRay = _tabTYRays[i];  
        events = tyRay->getTabEvent();
		sizeRay = tyRay->getSize();

        //rayFacesHistory     = ray->getFaceHistory();
        //rayParoisHistory    = ray->getBatimentHistory();
        //rayPrimitiveHistory = ray->getPrimitiveHistory();

        for (k = 0; k < ray->events.size(); k++) // boucle sur les evenements du rayon
        {
			ev = events[k];

            if (ev.type == SPECULARREFLEXION) // cas d'une reflexion
            {
                Pref  = ev.pos;   // pt de reflexion
                Pprec = events[k - 1].pos; // pt precent la reflexion
				Psuiv = events[k+1].pos;

				surfaceRefl = (ray->events.at(k))->getShape(); //surfaceRefl = (events.at(k))->getShape(); 

            	idFace = surfaceRefl->getFaceId();

				 if (surfaceRefl->isSol())  // cas particulier d'une reflexion sur le sol (il faut remplir la struct _tabSurfIntersect)
				 {

					//vec3 vecIncident = ray->events.at(k)->getIncomingDirection();
					//vInc = OVector3D(Pref, Pprec); 
					//vNorm = _tabSurfIntersect[idFace].normal;

					 angle = ev.angle;   // v.angle(vNorm);     // arcos(v*n/|v|)

					 (_topo.terrainAt(Pref)->getSol()->abso(angle, sizeRay, _atmos)).getRangeValueReal(_tabSurfIntersect[idFace].spectreAbso, 18, 8);
				 }

				spectreAbs = _tabSurfIntersect[idFace].spectreAbso;  // recuperation du spectre d'absorption 
				
				coefRefl = (one - OSpectre(spectreAbs, 18, 8)).sqrt();  // = sqrt(1-alpha_E) + conversion du spectre de type double* en OSpectre

                // Avec ponderation de Fresnel :
                if(zoneFresnel)
                {
                    tabPondFresnel = ComputeFrenelWeighting(tyRay, angle, Pref.distFrom(Psuiv), Pref);  // calcul des ponderations de Frenel
                    nbFacesFresnel = tabPondFresnel.size();  // nbr de triangles dans le zone de Fresnel

                    for(l=0; l<nbFacesFresnel; l++) // boucle sur les faces = intersection plan de l'objet intersecte / ellipsoide de Fresnel
                        sum = sum + coefRefl * tabPondFresnel[l]; // calcul du coeff de reflexion moy en ponderant avec les matériaux

                    prod = prod * sum;
                }
            }
        }

        if (!zoneFresnel) { prod = coefRefl; }

        _absRefl[i] = prod * (tyRay->getDistanceSR() / sizeRay ); //(ray->distanceSourceRecepteur() / ray->getLongueur());
    }
}

//TYMateriauConstruction TYANIME3DAcousticModel::getMateriauFace(Shape* pSurf, const vec3& seg) const
//{
//    TYMateriauConstruction mat;
//    TYMurElement* pMurElem = TYMurElement::safeDownCast(pSurf);
//
//    if (pMurElem != NULL) // Cas d'une face de bâtiment ou d'un ecran
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


void TYANIME3DAcousticModel::ComputeAbsDiff(TYCalcul& calcul, const TYSiteNode& site)
{
    double delta;                // difference de marche
    OSpectre nbF;                //nbr de Fresnel de chq diffration
    OSpectreComplex absArrete;   // absorption sur chaque arrete
    OSpectreComplex prod = OSpectre(1.0); // produit des absorptions sur chaque arrete

    OSpectre kDelta;             // intermediaire de calcul
    OSpectre mod;                // module du nombre complexe absArrete
    Ray* ray;                    // le rayon etudie
    LPTYRay tyRay;               // le rayon TYMPAN associe

    double dd, d1, d2;           // distances directe, au pt précédent et au pt suivant
    OPoint3D Pdif, Pprec, Psuiv; //pt de diffraction, pt précédent et suivant le pt de diffraction
    std::vector<TYRayEvent> events;

    int signe = 1;
    //float zD, zR, zS;
    double alpha, alphaRef;
    OVector3D vSO, vSD, vSR;
    OPoint3D S, R, O; // pts source, recepteur et pt au sol à la verticale du pt du diffraction
    int nbEvents;

    for (int i = 0; i < _nbRays; i++) // boucle sur les rayons
    {
        ray = _tabRays->at(i);
        tyRay = _tabTYRays[i];
        events = tyRay->getTabEvent();
        nbEvents = ray->events.size();
        
        S = events.at(0).pos;
        R = events.at(nbEvents).pos;

        for (int j = 1; j < nbEvents - 1; j++) // boucle sur les evenements
        {
            if (events[j].type == TYDIFFRACTION) // cas d'une diffraction
            {
                Pdif  = events.at(j).pos;
                Pprec = events.at(j - 1).pos;
                Psuiv = events.at(j + 1).pos;

                d1 = Pdif.distFrom(Pprec);  // lg du segment P_prec P_k = P_{j-1} P_j
                d2 = Pdif.distFrom(Psuiv);  // lg du segment P_k P_suiv1 = P_j P_{j+1}
                dd = Pprec.distFrom(Psuiv); // lg du segment P_prec P_suiv = P_{j-1} P_{j+1}


                //O = ?; // TODO : récupérer le pt au sol à la verticale du pt de diffraction

                O = Pdif; // juste pour compilation

                // calcul du signe si Pdif appartient à une arrête de diffraction horizontale :
                if (Pdif._z > S._z && Pdif._z > R._z)
                {
                    signe = 1;
                }
                else if (Pdif._z < S._z && Pdif._z < R._z)
                {
                    signe = -1;
                }
                else
                {
                    vSO = OVector3D(S, O);
                    vSD = OVector3D(S, Pdif);
                    vSR = OVector3D(S, R);
                    alpha    = vSO.angle(vSD);
                    alphaRef = vSO.angle(vSR);

                    if (alpha > alphaRef)
                    {
                        signe = -1;
                    }
                    else
                    {
                        signe = 1;
                    }
                }
                delta = signe * (d1 + d2 - dd);

                kDelta = _K * delta;
                nbF = _lambda.invMult(2.0 * delta);
                mod = ((nbF * 20.0 + 3.0).sqrt()).inv();
                absArrete = OSpectreComplex(mod, kDelta);
                prod = prod * absArrete;
            }
        }
        _absDiff[i] = prod;
    }
}

// calcul de la zone de Fresnel pour une diffraction donnee - Approximation : zone = boite englobante de l'ellipsoide de Fresnel
OBox2 TYANIME3DAcousticModel::ComputeFrenelArea(TYRay* tyRay, double angle, double distPrefPsuiv, OPoint3D Pref)
{
    // TODO : rotation de la bb de la zone de Fresnel

    OBox2 fresnelArea;  // boundingBox de l'ellispsoide de Fresnel

	double dSR = tyRay->getDistanceSR(); //rayon direct
    double dr = tyRay->getSize();      // rayon reflechi

	OSpectre f = OSpectre::getOSpectreFreqExact(); //frequence
    OSpectreComplex Q = _topo.terrainAt(Pref)->getSol()->calculQ(angle, distPrefPsuiv, _atmos) ; // coeff de reflexion du sol au pt de reflexion
    
    const double c1 = 0.5 * _c /(dSR - dr);  // cste de calcul

	OSpectre phaseQdivPI = Q.getPhase().div(M_PI);
	OSpectre fmin = (OSpectre(0.5) - phaseQdivPI)*c1;
    OSpectre fmax = (OSpectre(1.0) - phaseQdivPI)*c1; 
    OSpectre fc = (fmin * fmax).sqrt();      // frequence de transition

    const OSpectre F = (OSpectre(1.0)-((f*f).div(fc*fc)).exp())*32.0; // parametre de l'ellipsoide de Fresnel F_{\lambda} via la formule définie dans la publi de D. van Maercke et J. Defrance (CSTB-2007)
    
    //const double F = 0.5;

    // Construction de la bb de l'ellipsoide de Fresnel
    // 1/ boite dans le repere (Ox,Oy,Oz)
    TYPoint S = tyRay->getPosSourceGlobal();                  // source
    TYPoint R = tyRay->getPosReceptGlobal(); // recepteur
    TYPoint SIm = OPoint3D(S._x,S._y,-S._z);                   // source image
    double dSImR = SIm.distFrom(R);                             //distance source image/recepteur

    OSpectre lF = _lambda * F;                   // produit lambda*Flbd

    // Dimensions de la boite de Fresnel : on choisit de prendre la + grande boite

    double L = (lF + dSImR).valMax();                   // grd cote de la boite
    double l = ((lF*(lF + 2.*dSImR)).sqrt()).valMax();  // petit cote de la boite == hauteur ici

    // boite de Fresnel placee a l'origine du repere
    OBox box = OBox(OCoord3D(0,0,0),OCoord3D(L,l,l));
	fresnelArea._length = L;
	fresnelArea._width = l;
	fresnelArea._height = l;

    // Translation au pt milieu du segment [SIm R]
    OPoint3D O = OPoint3D(0.5*(SIm._x+R._x), 0.5*(SIm._y+R._y), 0.5*(SIm._z+R._z)); // O milieu de [SIm R]
    const OPoint3D vt = OPoint3D(0.5*L-O._x, 0.5*l-O._y, 0.5*l-O._z);   // vecteur de translation
    box.Translate(vt);  // translation de la zone de Fresnel

    // 3/ changement de repere pour faire pivoter la boite
    OVector3D v1 = OCoord3D(dSImR*0.5,0.0,0.0);
    OVector3D v2 = OVector3D(O,R);
    double alpha = v1.angle(v2);    // angle de la rotation

    //fresnelArea.GetGeoNode();

    OVector3D vecI, vecJ, vecK;
    v2.normalize();
    vecI = v2;
    vecJ = OVector3D(1.0,0.0,0.0);  // A changer !
    vecK = OVector3D(0.0,1.0,0.0);  // A changer !
    ORepere3D r = ORepere3D(O, vecI, vecJ, vecK); // le nouveau repere

    fresnelArea = OBox2(box, r);

		
    return fresnelArea;
}

OTabDouble TYANIME3DAcousticModel::ComputeFrenelWeighting(TYRay* ray, double angle, double distPrefPsuiv, OPoint3D Pref)
{
    int nbFacesFresnel;     // nbr de triangles situes dans la zone de Fresnel
    OTabDouble tabPond;     // tableau des ponderation de Fresnel
    OTabDouble tabSurface;  // tableau des surfaces des triangles de la zone de Fresnel
    double surfaceTot;      // somme des surfaces des triangles de la zone de Fresnel
    int i, k;       // indices de boucle

    // calcul de la zone de Fresnel
    OBox fresnelArea = ComputeFrenelArea(ray, angle, distPrefPsuiv, Pref);

    // recuperation des triangles et des sommets de la topo
    //LPTYTopographie topo = site.getTopographie();
    //const TYTabLPPolygon listeTriangles = (*topo->getAltimetrie()).getListFaces();

    // XXX Altimetry refactoring impacts here.

    TYTabPoint listeSommets = _topo.collectPointsForAltimetrie();

	// We check if the box is entirely in one triangle
	// In that case, no need to compute anything.
	LPTYPolygon sE = (*_topo.getAltimetrie()).getFaceUnder(fresnelArea._E);
	LPTYPolygon sF = (*_topo.getAltimetrie()).getFaceUnder(fresnelArea._F);
	LPTYPolygon sG = (*_topo.getAltimetrie()).getFaceUnder(fresnelArea._G);
	LPTYPolygon sH = (*_topo.getAltimetrie()).getFaceUnder(fresnelArea._H);

	TYTabPoint ptsSE = sE->getPoints();
	TYTabPoint ptsSF = sF->getPoints();
	TYTabPoint ptsSG = sG->getPoints();
	TYTabPoint ptsSH = sH->getPoints();
	
	OPoint3D centersE = sE->getCenter();
	OPoint3D centersF = sF->getCenter();
	OPoint3D centersG = sG->getCenter();
	OPoint3D centersH = sH->getCenter();

	//if (centersE == centersF && centersE == centersG && centersE == centersH)
	//{
		double boxSurf = fresnelArea._length * fresnelArea._width;
	//	return tabPond;
	//}
	LPTYPolygon triangle;
	TYTabLPPolygon triangleIn;
	TYTabLPPolygon triangleOut;
	//else
	//{
		// Gets all the triangles in/around the box
		int count = (*_topo.getAltimetrie()).getFacesInBox(fresnelArea, _listeTrianglesBox);

		for (int i=0; i<_listeTrianglesBox.size(); i++)
		{
			triangle = _listeTrianglesBox[i];
			TYTabPoint sommets =  triangle->getPoints(); //Contient tous les sommets du triangle
			for (int i=0; i< sommets.size(); i++)
			{
				int k = 0;
				if(fresnelArea.isInside(sommets[i]));
				{
					k++;
				}
				if(k == 3)
				{
					triangleIn[i] = triangle;
				}
				else 
				{	
					triangleOut[i] = triangle;
				}
			}
		}
		// Computes total triangle surface inside the box
		surfaceTot = 0.0;
		int nbFaces = triangleIn.size();

		for(int j=0; j<nbFaces; j++)
		{
			tabSurface[j]= triangleIn[j]->surface();
			surfaceTot += tabSurface[j];
		}
		for(int j=0; j<nbFaces; j++)
        tabPond[j] = tabSurface[j]/surfaceTot;

		return tabPond;

		////TYTabPoint listeTrianglesFresnel; // liste des triangles de la zone de Fresnel



		//k=0;
		////for(i=0; i< listeSommets.size(); i++) // boucle sur les sommets des triangles de la topo
		////{
		////	if(fresnelArea.isInside(listeSommets[i]))
		////	{
		////		listeTrianglesFresnel[k] = listeSommets[i];
		////		k++;
		////	}
		////}
		////nbFacesFresnel = k;

		////surfaceTot = 0.0;
		////for(i=0; i<nbFacesFresnel; i++) // boucles sur les triangles situes dans la zone de Fresnel
		////{
		//	tabSurface[i] = _listeTriangles[i]->surface();   // calcul de la surface de la face
		////	surfaceTot += tabSurface[i];
		////}

		//for(i=0; i<nbFacesFresnel; i++)
  //      tabPond[i] = tabSurface[i]/surfaceTot;

		//return tabPond;
	//}
}


void TYANIME3DAcousticModel::ComputePressionAcoustEff(TYCalcul& calcul, const TYSiteNode& site)
{
    const double rhoc = 400.0;   // kg/m^2/s
    double c1;      // constante du calcul
    //const double W0 = pow(10.0, -12.0);

	OSpectre phase; // phase du nombre complexe _pressAcoustEff[i] pour chq i
    OSpectre mod;   // module du nombre complexe _pressAcoustEff[i] pour chq i
    double dSR;  // distance source recepteur
    OPoint3D S, P0; // pt de la source et pt du 1er evenement
    LPTYRay tyRay;    // rayon
    OSegment3D seg; // premier segment du rayon
    OSpectre directivite, wSource; // fonction de directivite et spectre de puissance de la source
    TYSourcePonctuelle* source; // source
    OSpectreComplex prodAbs; // produit des differentes absorptions

    for (int i = 0; i < _nbRays; i++) // boucle sur les rayons
    {
        tyRay = _tabTYRays[i];
		source = tyRay->getSource();
		TYTabRayEvent tabRayevent = tyRay->getTabEvent();
		std::vector<int> reflexion_indices;	// Has indices where a reflexion occurs
		int size = reflexion_indices.size();
		double length = .0;
		double begin = .0;
		double end = .0;
		double middle = .0;

		for(int j = 0; j < tabRayevent.size(); j++)
		{
			if(tabRayevent[j].type == TYREFLEXION || tabRayevent[j].type == TYREFLEXIONSOL)
			{
				reflexion_indices.push_back(j);
			}
		}
		// First ray length (Source->1st refl)
		if(reflexion_indices[0] == 1)
		{
			begin = _pathLengthMatrix[i][0]; 
		}
		else{
			begin = tabRayevent[0].pos.distFrom(tabRayevent[reflexion_indices[0]].pos);
		}
		// Last ray length (last refl -> R)
		if(reflexion_indices[size-1] == (tabRayevent.size())-2)
		{
			end = _pathLengthMatrix[i][reflexion_indices[size-1]]; 
		}
		else{
			end = tabRayevent[reflexion_indices[size-1]].pos.distFrom(tabRayevent[tabRayevent.size()-1].pos);
		}

		// Look up in pathlenghtmatrix to get the right distance
		// Or compute the distance using the points coordinates
		for(int k=0; k < size-1; k++)
		{
			double length = .0;
			if(reflexion_indices[k+1] == reflexion_indices[k]+1 )
			{
				middle = _pathLengthMatrix[i][reflexion_indices[k]];
			}
			else
			{
				middle = tabRayevent[reflexion_indices[k]].pos.distFrom(tabRayevent[reflexion_indices[k+1]].pos);
			}
			length += middle;
		}
		// dSR is now the path distance with reflexions ONLY.
		dSR = begin + end + length;

		c1 = 4.0 * M_PI * dSR * dSR;

		S  = *source->getPos();
		P0 = (tyRay->getTabEvent())[1].pos;
        seg = OSegment3D(S, P0);
		
		directivite = source->lwApparenteSrcDest(seg, _atmos);
		wSource = (*source->getSpectre()).toGPhy(); // ou wSource = (*TYSourcePonctuelle::safeDownCast(_tabSources[i]->getElement())->getSpectre()).toGPhy(); // new !!

        prodAbs = _absAtm[i] * _absRefl[i] * _absDiff[i];

        mod = ((directivite * wSource * rhoc) * (1. / c1)).sqrt() * prodAbs;
        phase = _K.mult(tyRay->getSize());

        _pressAcoustEff[i] = OSpectreComplex(mod, phase);
	}
}

OTab2DSpectreComplex TYANIME3DAcousticModel::ComputePressionAcoustTotalLevel(TYCalcul& calcul, const TYSiteNode& site)
{
    OSpectre C;         // facteur de coherence et son carre
    OSpectre un = OSpectre(1.0);
    OSpectreComplex sum1;	//somme partielle
    OSpectre sum2;			//somme partielle
    OSpectre mod;		    // module et module au carre
	const OSpectre K2 = _K*_K;				// nombre d'onde au carré
 
	double incerRel = 0.1;  // incertitude relative sur la taille du rayon au carree
    double cst = (pow(2., 1. / 6.) - pow(2., -1. / 6.)) * (pow(2., 1. / 6.) - pow(2., -1. / 6.)) / 3.0 + incerRel * incerRel; // constante pour la definition du facteur de coherence
	double dSR;				// distance source/recepteur

    const int nbSources    = _tabSources.size();           // nbr de sources de la scene
    const int nbRecepteurs = _tabRecepteur.size();         // nbr de recepteurs de la scene

	TYSourcePonctuelle* source;
    TYPointControl* recept;

    OTab2DSpectreComplex tabPressionAcoust(nbSources);
	
	for (int i = 0; i < nbSources; i++)
		tabPressionAcoust[i].resize(nbRecepteurs);

    LPTYRay tyRay;

    for (int i = 0; i < nbSources; i++) // boucle sur les sources
    {
        for (int j = 0; j < nbRecepteurs; j++) // boucle sur les recepteurs
        {
			sum1 = OSpectreComplex(OSpectre(0.0), OSpectre(0.0));
            sum2 = OSpectreComplex(OSpectre(0.0), OSpectre(0.0));      

            for (int k = 0; k < _nbRays; k++) // boucle sur les rayons allant de la source au recepteur
            {
                tyRay =  _tabTYRays[k];

                source = tyRay->getSource();
                recept = (TYPointControl*) tyRay->getRecepteur();

                if (source == (TYSourcePonctuelle*)(_tabSources[i]->getElement()) && recept == (TYPointControl*)(_tabRecepteur[j]->getElement()))   // test si la source est celle en cours idem pour recepteur
                {
                    dSR = tyRay->getDistanceSR();
                    mod = (_pressAcoustEff[k]).getModule();
                    C = (K2 * dSR * dSR * (-1) * cst).exp();

                    sum1 = sum1 + _pressAcoustEff[k] * C;
                    sum2 = sum2 + mod * mod * (un - C*C);
                }
            }
			OSpectreComplex res = sum1*sum1 + sum2;
			tabPressionAcoust[i][j] = res;
        }
    }
    return tabPressionAcoust;
}

void TYANIME3DAcousticModel::init(TYCalcul& calcul, const TYSiteNode& site, TabRays* tabRayons, TYStructSurfIntersect* tabStruct)
{
    _tabTYRays = calcul.getAllRays();

    _nbRays = _tabTYRays.size();

	_tabRays = tabRayons;
	//_tabRays = simulation.getSolver()->getValidRays();
	//    _tabRays = acoustPathFinder.getTabRays();

	_tabSurfIntersect = tabStruct;

    OSpectreComplex s1 = OSpectreComplex(OSpectre(1.0));
    OSpectreComplex s0 = OSpectreComplex(OSpectre(0.0));

    _pressAcoustEff = OTabSpectreComplex(_nbRays, s0);
    _absAtm  = OTabSpectreComplex(_nbRays, s1);
    _absRefl = OTabSpectreComplex(_nbRays, s1);
    _absDiff = OTabSpectreComplex(_nbRays, s1);

	_atmos = *(calcul.getAtmosphere());
	_topo = *site.getTopographie();        

    _listeTerrains = _topo.getListTerrain(); 
    _listeTriangles = (*_topo.getAltimetrie()).getListFaces();  

    TYMapElementTabSources mapElementSources = calcul.getResultat()->getMapEmetteurSrcs();
    calcul.getAllSources(mapElementSources, _tabSources);
    calcul.getAllRecepteurs(_tabRecepteur);

	_c = _atmos.getVitSon();  
	_K = _atmos.getKAcoust();  
	_lambda = OSpectre::getLambda(_c); 

}


OTab2DSpectreComplex TYANIME3DAcousticModel::ComputeAcousticModel(TYCalcul& calcul, const TYSiteNode& site)
{

    ComputeAbsAtm();

    ComputeAbsRefl();  // méthode à debugger

    //ComputeAbsDiff(calcul, site);

    ComputePressionAcoustEff(calcul, site);

    return ComputePressionAcoustTotalLevel(calcul, site);
}

/*LPTYRay TYANIME3DAcousticModel::convertRaytoTYRay(Ray *r, TYCalcul &calcul, const TYSite &site)
{
    LPTYRay ray;

    //Recuperation de la liste des recepteurs
    TYTabLPPointControl tabRecepteurs =  calcul.getProjet()->getPointsControl();

    //Récupération de la liste des sources poncutelles
    TYMapElementTabSources& mapElementSources=calcul.getResultat()->getMapEmetteurSrcs();
    site.getInfrastructure()->getAllSrcs(&calcul, mapElementSources);
    TYTabSourcePonctuelleGeoNode sources; // Creation d'une collection des sources
    calcul.getAllSources(mapElementSources, sources);

    std::vector<TYRayEvent> events;

    int idRecep = r->getRecepteur()->getId();
    int idSource = r->getSource()->getId();

    //Les identifiants des recepteurs et sources sont construits pour correspondre à l'index des sources et recepteurs dans Tympan.
    TYSourcePonctuelle *sourceP = TYSourcePonctuelle::safeDownCast(sources.at(idSource)->getElement());
    TYPointControl *recepP = TYPointControl::safeDownCast(tabRecepteurs.at(idRecep));
    OMatrix matrice = sources.at(idSource)->getMatrix();
    double CoordSource[3];
    sourceP->getPos()->getCoords(CoordSource);
    TYPoint posSourceGlobal = matrice * (*sourceP->getPos());
    double coordRecepteur[3];
    recepP->getCoords(coordRecepteur);

    //Définition des Evènements.
    TYRayEvent e;

    //Ajout de la source
    e.type = TYSOURCE;
    e.pos = TYPoint(posSourceGlobal);
    events.push_back(e);

    //Création des évènements de diffractions et réflexions
    for(unsigned int j = 0; j < r->getEvents()->size(); j++)
    {
        Evenement *ev = r->getEvents()->at(j);
        switch(ev->getType())
        {
        case REFLEXION :
            e.type = TYREFLEXION;
            break;
        case DIFFRACTION:
            e.type = TYDIFFRACTION;
            break;
        default:
            e.type = TY_NO_TYPE;
            break;
        }
        e.pos = TYPoint(ev->getImpact()->x,ev->getImpact()->y,ev->getImpact()->z);
        events.push_back(e);
    }
    //Ajout du recepteur
    e.type = TYRECEPTEUR;
    e.pos = TYPoint(coordRecepteur[0],coordRecepteur[1],coordRecepteur[2]);
    events.push_back(e);

    //Construction du rayon et envoie au calcul
    ray = new TYRay(sourceP, recepP, events);

    return ray;
}*/
