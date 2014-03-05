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


#include "TYAcousticModel.h"
#include "Tympan/MetierSolver/DataManagerMetier/Commun/TYCalcul.h"
#include "Tympan/MetierSolver/DataManagerMetier/Commun/TYTrajet.h"
#include "Tympan/MetierSolver/DataManagerMetier/Site/TYSiteNode.h"
#include "TYSolver.h"

TYAcousticModel::TYAcousticModel(TYSolver& solver)
    : _expGeo(1),
      _useSol(true),
      _useEcran(true),
      _useReflex(false),
      _conditionFav(false),
      _useAtmo(true),
      _bCalculTrajetsHorizontaux(true),
      _interference(false),
      _seuilConfondus(0.01),
      _paramH(10.0),
      _pAtmo(NULL),
      _pTopographie(NULL),
      _solver(solver)
{
    _absoNulle = OSpectre::getEmptyLinSpectre(1.0);
    _absoNulle.setType(SPECTRE_TYPE_ABSO); // Spectre d'absorption
}

TYAcousticModel::~TYAcousticModel()
{

}

void TYAcousticModel::init(const TYSiteNode& site, const TYCalcul& calcul)
{
    // Calcul de l'expansion geometrique
    _expGeo = calcul.getExpansGeo();


    // Calcul avec sol reel
    _useSol = calcul.getUseSol();


    // Calcul avec reflexion sur les parois verticales
    _useReflex = calcul.getUseReflexion();


    // Calcul en conditions favorables
    _conditionFav = calcul.getCondFav();


    // Definit l'atmosphere courante du site
    _pAtmo = calcul.getAtmosphere();


    // Calcul avec prise en compte des ecrans
    _useEcran = calcul.getUseEcran();

    // Prise en compte de l'atmosphere (divergence geometrique)
    _useAtmo = calcul.getUseAtmosphere();


    // Calcul avec interference
    _interference = calcul.getInterference();

    // On calcul tout de suite le spectre de longueur d'onde
    _lambda = OSpectre::getLambda(_pAtmo->getVitSon());

    // Distance minimale entre deux points consideres comme confondus
    _seuilConfondus = calcul.getSeuilConfondu();


    /// Calcul des trajets horizontaux
    _bCalculTrajetsHorizontaux = calcul.getCalculTrajetsHorizontaux();


    // Topographie du site
    _pTopographie = site.getTopographie();


    // Coefficient multiplicateur pour le calcul des reflexions supplementaires en condition favorable
    _paramH = calcul.getParamH();

	// Maintien des TYRay à la fin des calcul
	_bSaveRay = calcul.getSaveRay();
}


void TYAcousticModel::compute(std::vector<TYSIntersection*>& tabIntersect, OSegment3D& rayon, TYSourcePonctuelle* pSrc, TYPointCalcul* pRcpt, std::vector<TYRay*>& tabRays, OSpectre& spectrum)
{
	std::vector<OSpectreComplex> tabSpectres;

	// On traite les rayons un par un
	for (size_t i=0; i<tabRays.size(); i++)
	{
		// On affecte la source et le récepteur à chaque rayon
		tabRays[i]->setSource(pSrc);
		tabRays[i]->setRecepteur(pRcpt);
		tabSpectres.push_back(computeRay(tabRays[i], tabIntersect));
	}

    // Calcul la pression cumulee de tous les chemins au point de reception du trajet
    solve(rayon, pSrc, tabRays, tabSpectres, spectrum);

	// Nettoyage des rayons si demandé
	if (!_bSaveRay)
	{
		for (unsigned int i=0; i<tabRays.size(); i++)
		{
				delete tabRays[i];
				tabRays[i] = NULL;
		}

		tabRays.clear();
	}
}

OSpectreComplex TYAcousticModel::computeRay(TYRay* ray, std::vector<TYSIntersection*>& tabIntersect)
{
	OSpectreComplex spectrum(1.0, 0.0), 
					lwSrc(1.0, 0.0),
					attDiffraction(1.0, 0.0),
					attReflexSol(1.0, 0.0),
					attReflex(1.0, 0.0);

	TYTabRayEvent& tabEv = ray->getEvents();

	for (size_t i=0 ; i<tabEv.size(); i++)
	{
		switch (tabEv[i]->type)
		{
		case TYSOURCE :
			lwSrc = computeLwSource(ray, tabEv[i]->pos, tabEv[i+1]->pos);
			break;
		case TYDIFFRACTION :
			attDiffraction = computeAttDiffraction(ray, i);
			deleteOtherDiffractions(ray, i); // Ce solveur traite globalement l'ensemble des diffractions
			break;
		case TYREFLEXION :
			attReflex = computeAttReflexion(ray, tabIntersect, i);
			break;
		case TYREFLEXIONSOL :
			attReflexSol = computeAttReflexionSol(ray, i);
			break;
		case TYRECEPTEUR :
			break;
		default :// Do Nothing
			break;
		}
	}

	return computeAttTotal(ray, lwSrc, attReflexSol, attReflex, attDiffraction);
}

OSpectreComplex TYAcousticModel::computeLwSource(TYRay* ray, const OPoint3D& begin, const OPoint3D& end)
{
	OSegment3D rayon(begin, end);
	return static_cast<OSpectreComplex>(ray->getSource()->lwApparenteSrcDest(rayon, *_pAtmo, _expGeo));
}

OSpectreComplex TYAcousticModel::computeAttTotal(TYRay* ray, OSpectreComplex& S, OSpectreComplex& Qs, OSpectreComplex& Qm, OSpectreComplex& D)
{
	OSpectreComplex spectrum(1.0, 0.0);

	double rr = ray->getLength();

	OSpectre kr = _pAtmo->getKAcoust().mult(rr);

	OSpectreComplex A = _pAtmo->getAtt(rr); // Absorption atmosphérique

	A.setPhase(kr); // A e^(i.kr)

	spectrum = S * A * Qs * Qm / D;

	return spectrum;
}

OSpectreComplex TYAcousticModel::computeAttDiffraction(TYRay* ray, unsigned int& i)
{
	bool vertical = static_cast<bool>(ray->getIdentifiant());

	OSpectreComplex s(1.0, 0.0);

    OSpectre C = calculC(computeScreenThick(ray)); // Facteur correctif lie a l'epaisseur de l'ecran

	// Longueur du chemin direct (rd) et du chemin réellemnt parcouru (re)
	double rd = ray->getPosSourceGlobal().distFrom(ray->getPosReceptGlobal());
	double re = ray->getLength();

   // Dans le cas du calcul en conditions favorables on considere un trajet direct courbe
    if ((_conditionFav) && (vertical))
    {
        double gamma = rd * 8.0;
        gamma = (gamma > 1000 ? gamma : 1000.0);// Rayon minimum 1000 metres

        double alpha = 2 * asin(rd / (2 * gamma));

        rd = gamma * alpha; // Longueur de l'arc de rayon gamma passant par les points extreme du segment de longueur rd
    }

    double delta = re - rd ; // difference de marche
    delta = delta <= 0 ? 0.0 : delta;

    // Attenuation apportee par la diffraction = sqrt(3 + (40 * C * delta)/lambda)

    s = _lambda.invMult(40 * delta); // =40*delta/lambda
    s = s.mult(C); // 40*delta*C/lambda
    s = s.sum(3.0);

    // Si la diffraction a lieu dans le plan vertical (arete horizontale),
    // les attenuations minimales et maximales sont limitees respectivement
    // a 0 dB et 20 ou 25 dB (selon que l'on est en ecran mince ou epais).
    if (vertical)
    {
        s = limAttDiffraction(s, C);
    }

    s.setType(SPECTRE_TYPE_ATT);

	return s.racine();
}

double TYAcousticModel::computeScreenThick(TYRay* ray)
{
	double epaisseur = 0.0;

	//On récupère tous les évènements de type diffraction du rayon
	std::vector<int> tabIndex = ray->getIndexOfEvents(TYDIFFRACTION);

	for (unsigned int i=0; i<tabIndex.size()-1; i++)
	{
		epaisseur += ray->getEvents()[tabIndex[i]]->pos.distFrom(ray->getEvents()[tabIndex[i+1]]->pos);
	}

	return epaisseur;
}

void TYAcousticModel::deleteOtherDiffractions(TYRay* ray, const unsigned int begin)
{
	TYTabRayEvent& tabEvents = ray->getEvents();
	unsigned int i = begin + 1; // Oncommence à la deuxième diffraction
	while (tabEvents[i]->type == TYDIFFRACTION)
	{
		tabEvents[i]->type = TY_NO_TYPE;
		i++;
	}
}

OSpectreComplex TYAcousticModel::computeAttReflexionSol(TYRay* ray, unsigned int& i)
{
	OSpectreComplex spectrum(1.0, 0.0);

	TYTabRayEvent tabEvents = ray->getEvents();

	TYTerrain *pTerrain = NULL;
	TYSol* pSol = NULL;

	double rd = tabEvents[i-1]->pos.distFrom(tabEvents[i+1]->pos);
	double rr = tabEvents[i-1]->pos.distFrom(tabEvents[i]->pos);
	rr += tabEvents[i]->pos.distFrom(tabEvents[i+1]->pos);

	pTerrain = _pTopographie->terrainAt(tabEvents[i]->pos);
	if (pTerrain) // Une zone de terrain a ete definie
	{
		pSol = pTerrain->getSol();
		spectrum = spectrum * pSol->abso(tabEvents[i]->angle, tabEvents[i]->angletheta, *_pAtmo);
	}

	spectrum = spectrum * (rd / rr);

	return spectrum;
}

OSpectreComplex TYAcousticModel::computeAttReflexion(TYRay* ray, std::vector<TYSIntersection*>& tabIntersect, unsigned int& i)
{
	TYTabRayEvent tabEvents = ray->getEvents();

	double rd = tabEvents[i-1]->pos.distFrom(tabEvents[i+1]->pos);
	double rr = tabEvents[i-1]->pos.distFrom(tabEvents[i]->pos);
	rr += tabEvents[i]->pos.distFrom(tabEvents[i+1]->pos);

	TYAcousticSurface* pSurface = tabIntersect[i]->pSurface;
	TYAcousticVolume* pVolume = tabIntersect[i]->pVolume;
	TYMateriauConstruction matFace = getMateriauFace(pSurface, pVolume);

	OSpectre SpectreAbso = matFace.getSpectreAbsoEyring();
    SpectreAbso = SpectreAbso.mult(-1.0).sum(1.0); // coeff reflex = 1 - coefAbso

    //
    // Reflexion sur un cylindre, d'apres ISO9613-2
    //
    TYAcousticCylinder* pCyl = TYAcousticCylinder::safeDownCast(pVolume);
    if (pCyl && (i>0) && (i<=ray->getEvents().size()-1))
    {
		OSegment3D rayonTmp(ray->getEvents()[i-1]->pos, ray->getEvents()[i+1]->pos);
        OPoint3D centre(pCyl->getCenter());
        OVector3D SC(rayonTmp._ptA, centre);
        OVector3D CR(centre, rayonTmp._ptB);
        double diametre = pCyl->getDiameter();
        double dSC = SC.norme(); // Norme du vecteur
        double phi = SC.angle(CR);

        SpectreAbso = SpectreAbso * (diametre * sin(phi / 2) / (2 * dSC));
    }

	SpectreAbso = SpectreAbso * (rd/rr);

	return static_cast<OSpectreComplex>(SpectreAbso);
}

bool TYAcousticModel::solve(OSegment3D& rayon, TYSourcePonctuelle* pSrc, std::vector<TYRay*>& tabRays, std::vector<OSpectreComplex>& tabSpectres, OSpectre& spectrum)
{
    const double PIM4 = 4.0 * M_PI;


	double rD2 = rayon.longueur();


    rD2 = rD2 * rD2 ;


    double divGeom = _pAtmo->getImpedanceSpecifique() / (PIM4 * rD2);


    // W.rho.c / (4pi*rdi¿½)

    spectrum = (*pSrc->getSpectre()).mult(divGeom);



    //  (W.rho.c/4.pi.Rdi¿½)*Attenuations du trajet
    if (_interference)
    {
        spectrum = spectrum.mult(getPInterference(tabRays, tabSpectres));
    }
    else
    {
        spectrum = spectrum.mult(getPEnergetique(tabRays, tabSpectres));
    }

	spectrum.setType(SPECTRE_TYPE_LP); //Le spectre au point est bien un spectre de pression !

	return true;
}

TYMateriauConstruction TYAcousticModel::getMateriauFace(TYAcousticSurface* pSurf, TYAcousticVolume* pVolume) const
{
	assert(pSurf && pVolume);

    TYMateriauConstruction *mat = NULL;
    TYMurElement* pMurElem = TYMurElement::safeDownCast(pSurf);

    if (pMurElem != NULL) // Cas d'une face de bi¿½timent ou d'un ecran
    {
        TYMur* pMur = TYMur::safeDownCast(pMurElem->getParent());
        mat = pMur->getParoi()->getMatFace2();
    }
    else // Cas d'une machine ou d'un acousticVolume quelconque
    {
        mat = pVolume->getMateriau();
    }

    return *mat;
}


OSpectre TYAcousticModel::calculC(const double& epaisseur) const
{
    // C = (1 + (5*lambda/epaisseur)i¿½) / (1/3 + (5*lambda/epaisseur)i¿½)

    OSpectre C = OSpectre::getEmptyLinSpectre();
    OSpectre opLambda;

    if (epaisseur < 1.0E-2)
    {
        C .setDefaultValue(1.0);
    }
    else
    {
        const double unTiers = 1.0 / 3.0;

        opLambda = _lambda.mult(5.0 / epaisseur); // (5*lambda/e)
        opLambda = opLambda.mult(opLambda); // (5*lambda/e)i¿½

        C = opLambda.sum(1.0); // 1 + (5*lambda/e)i¿½
        C = C.div(opLambda.sum(unTiers)); // (1 + (5*lambda/e)i¿½) / (1/3 + (5*lambda/epaisseur)i¿½)
    }

    C.setType(SPECTRE_TYPE_AUTRE);  // Ni Attenuation, ni Absorption

    return C;
}

OSpectre TYAcousticModel::limAttDiffraction(const OSpectre& sNC, const OSpectre& C) const
{
    OSpectre s = OSpectre::getEmptyLinSpectre();
    s.setType(SPECTRE_TYPE_ATT);

    double lim20dB = pow(10.0, (20.0 / 10.0));
    double lim25dB = pow(10.0, (25.0 / 10.0));
    double lim0dB = pow(10.0, (0.0 / 10.0));

    double valeur;

    for (unsigned int i = 0 ; i < sNC.getNbValues() ; i++)
    {
        valeur = sNC.getTabValReel()[i];

        valeur = valeur < lim0dB ? lim0dB : valeur ;  // L'attenuation ne peut etre inferieure a 1

        if ((C.getTabValReel()[i] - 1) <= 1e-2) // Comportement ecran mince
        {
            valeur = valeur > lim20dB ? lim20dB : valeur;
        }
        else    // Comportement ecran epais ou multiple
        {
            valeur = valeur > lim25dB ? lim25dB : valeur;
        }

        s.getTabValReel()[i] = valeur;

    }

    return s;
}

OSpectre TYAcousticModel::getPEnergetique(std::vector<TYRay*>& tabRays, std::vector<OSpectreComplex>& tabSpectres)
{
    unsigned int iNoEcran = tabSpectres.size()-2; // Indice du premier rayon non écran
	unsigned int iCountEcran = 0;
	bool bNotScreenFound = false;

    OSpectre s = OSpectre::getEmptyLinSpectre();

	// Somme des carré sur tous les chemins sauf chemin direct
    for (unsigned int i = 0; i<tabSpectres.size()-2; i++)
    {
        s = s.sum(tabSpectres[i].mult(tabSpectres[i]));
      
        if (tabRays[i]->getNbEventType(TYDIFFRACTION) == 0) 
		{ 
			iNoEcran = bNotScreenFound ? iNoEcran : i; 
			bNotScreenFound = true;
		}
		else
		{
			iCountEcran++;
		}
    }

	// Somme des carrés sur tous les chemins sauf chemins diffractés
    OSpectre sDirect = OSpectre::getEmptyLinSpectre();

    for (unsigned int i = iNoEcran; i<tabSpectres.size(); i++)
    {
        sDirect = sDirect.sum(tabSpectres[i].mult(tabSpectres[i]));
    }

 // Remplacement des fréquences pour lesquelles, le champ direct est plus faible que le champs diffracté
    if (iCountEcran != 0)
    {
        for (unsigned int i = 0 ; i < s.getNbValues() ; i++)
        {
            if (s.getTabValReel()[i] > sDirect.getTabValReel()[i])
            {
                s.getTabValReel()[i] = sDirect.getTabValReel()[i];
            }
			else
			{
                break; // Inutile d'aller plus loin
			}
        }
    }
    else // Pas d'écran
    {
        return sDirect;
    }

    return s;	
}

OSpectre TYAcousticModel::getPInterference(std::vector<TYRay*>& tabRays, std::vector<OSpectreComplex>& tabSpectres)
{
    unsigned int iNoEcran = tabSpectres.size()-2; // Indice du premier rayon non écran
	unsigned int iCountEcran = 0;
	bool bNotScreenFound = false;
    OSpectre s = OSpectre::getEmptyLinSpectre();
    OSpectre sProduitCroise = OSpectre::getEmptyLinSpectre();
    
  // Produit croisé sur tous les chemins sauf chemin direct
    for (unsigned int i = 0; i<tabSpectres.size()-2; i++)
    {
        s = s.sum(tabSpectres[i].mult(tabSpectres[i]));
        for (unsigned j = i+1; j<tabSpectres.size()-2; j++)
        {
            sProduitCroise = sProduitCroise.sum(tabSpectres[i].mult(tabSpectres[j]).mult(2.0).mult(correctTiers(tabSpectres[i], tabSpectres[j], tabRays[i]->getLength(), tabRays[j]->getLength())));
//											   0                   1              1     1   1     1            2                                                     33                       33210
		}
        
		// Recherche de l'indice du premier élément qui ne soit pas un écran
		if (tabRays[i]->getNbEventType(TYDIFFRACTION) == 0) 
		{ 
			iNoEcran = bNotScreenFound ? iNoEcran : i; 
			bNotScreenFound = true;
		}
		else
		{
			iCountEcran++;
		}
    }
 
    s = s.sum(sProduitCroise); 
 
 // Produit croisé de tous les chemins sauf chemins diffractés
    OSpectre sDirect = OSpectre::getEmptyLinSpectre();
    OSpectre sDirectCrossProd = OSpectre::getEmptyLinSpectre();

    for (unsigned int i = iNoEcran; i<tabSpectres.size(); i++)
    {
        sDirect = sDirect.sum(tabSpectres[i].mult(tabSpectres[i]));
        for (unsigned j = i+1; j<tabSpectres.size(); j++)
        {
            sDirectCrossProd = sDirectCrossProd.sum(tabSpectres[i].mult(tabSpectres[j]).mult(2.0).mult(correctTiers(tabSpectres[i], tabSpectres[j], tabRays[i]->getLength(), tabRays[j]->getLength())));
//												   0                   1              1     1   1     1            2                                                     33                       33210 
		}
    }
 
    sDirect = sDirect.sum(sDirectCrossProd);  

 // Remplacement des fréquences pour lesquelles, le champ direct est plus faible que le champs diffracté
    if (iCountEcran != 0)
    {
        for (unsigned int i = 0 ; i < s.getNbValues() ; i++)
        {
            if (s.getTabValReel()[i] > sDirect.getTabValReel()[i])
            {
                s.getTabValReel()[i] = sDirect.getTabValReel()[i];
            }
			else
			{
                break; // Inutile d'aller plus loin
			}
        }
    }
    else // Pas d'écran
    {
        return sDirect;
    }

    return s;
}

OSpectre TYAcousticModel::correctTiers(const OSpectreComplex& si, const OSpectreComplex& sj, const double& ri, const double& rj) const
{
    const double dp6 = pow(2, (1.0 / 6.0));
    const double invdp6 = 1.0 / dp6;
    const double dfSur2f = (dp6 - invdp6) / 2.0;  // df/2f
    OSpectre cosTemp;
    OSpectre s;

    OSpectre sTemp = _pAtmo->getKAcoust().mult(ri - rj); // k(ri-rj)

    if (ri == rj)
    {
        s = (si.getPhase().subst(sj.getPhase()).subst(sTemp)).cos(); // cos(EPS_i - EPS_j)
    }
    else
    {

        s = si.getPhase().subst(sj.getPhase()); // thetaI - thetaJ

        double df = sqrt(1 + dfSur2f * dfSur2f);
        cosTemp = sTemp.mult(df); // k(ri-rj) * sqrt(1 + (df/2f)²)
        cosTemp = cosTemp.sum(s);// k(ri-rj) * sqrt(1 + (df/2f)²) + (thetaI - thetaJ)
        cosTemp = cosTemp.subst(sTemp); // k(ri-rj)*sqrt(1 + (df/2f)²) + EPS_i - EPS_j
        cosTemp = cosTemp.cos(); // cos(k(ri-rj)*sqrt(1 + (df/2f)²) + EPS_i - EPS_j)

        sTemp = sTemp.mult(dfSur2f); // k(ri-rj)*df/2f

        s = sTemp.sin();          // sin(k(ri-rj)*df/2f)
        s = s.div(sTemp);           // sin(k(ri-rj)*df/2f) / (k(ri-rj)*df/2f)
        s = s.mult(cosTemp);        // sin(k(ri-rj)*df/2f) / (k(ri-rj)*df/2f) * cos(k(ri-rj)*sqrt(1 + (df/2f)²) + EPS_i - EPS_j)
    }

    return s;
}

