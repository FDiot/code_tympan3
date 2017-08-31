/*
 * Copyright (C) <2012-2014> <EDF-R&D> <FRANCE>
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

#include <deque>
#include <list>
#include <cmath>
#include "Tympan/core/defines.h"
#include "Tympan/models/solver/config.h"
#include "Tympan/models/common/plan.h"
#include "Tympan/solvers/DefaultSolver/TYTrajet.h"
#include "Tympan/solvers/DefaultSolver/TYSolver.h"
#include "Tympan/models/common/mathlib.h"
#include "Tympan/geometric_methods/AcousticRaytracer/Geometry/Scene.h"
#include "TYAcousticModel.h"
#include "TYSolver.h"

TYAcousticModel::TYAcousticModel(TYSolver& solver)
    : _useSol(true),
      _useReflex(false),
      _conditionFav(false),
      _interference(false),
      _paramH(10.0),
      _solver(solver)
{
    _absoNulle = OSpectreComplex(TYComplex(1.0, 0.0));
    _absoNulle.setType(SPECTRE_TYPE_ABSO); // Spectre d'absorption
}

TYAcousticModel::~TYAcousticModel()
{

}

void TYAcousticModel::init()
{
    tympan::LPSolverConfiguration config = tympan::SolverConfiguration::get();
    // Calcul avec sol reel
    _useSol = config->UseRealGround;
    // Calcul avec reflexion sur les parois verticales
    _useReflex = config->UseReflection;
    // Calcul en conditions favorables
    _conditionFav = config->PropaConditions;
    // Definit l'atmosphere courante du site
    double pression = config->AtmosPressure;
    double temperature = config->AtmosTemperature;
    double hygrometrie = config->AtmosHygrometry;

    pSolverAtmos = std::unique_ptr<AtmosphericConditions> ( new AtmosphericConditions(pression, temperature, hygrometrie) );
    // Calcul avec interference
    _interference = config->ModSummation;
    // On calcul tout de suite le spectre de longueur d'onde
    _lambda = OSpectre::getLambda(pSolverAtmos->compute_c());
    // Coefficient multiplicateur pour le calcul des reflexions supplementaires en condition favorable
    _paramH = config->H1parameter;
}


void TYAcousticModel::compute(  const std::deque<TYSIntersection>& tabIntersect, TYTrajet& trajet, 
                                TabPoint3D& ptsTop, TabPoint3D& ptsLeft,
                                TabPoint3D& ptsRight )
{
    bool vertical = true, horizontal = false;

    // Construction du rayon SR
    OSegment3D rayon;
    trajet.getPtSetPtRfromOSeg3D(rayon);

    // Recuperation de la source
    tympan::AcousticSource& source = trajet.asrc;
    tympan::AcousticReceptor& receptor = trajet.arcpt;

    // Distance de la source au recepteur
    double distance = trajet.getDistance();

    TYTabChemin& tabChemins = trajet.getChemins();

    // Calcul des parcours lateraux
    // 1. Vertical
    computeCheminsAvecEcran(rayon, source, ptsTop, vertical, tabChemins, distance);

    // 2. Horizontal gauche
    computeCheminsAvecEcran(rayon, source, ptsLeft, horizontal, tabChemins, distance);

    // 3. Horizontal droite
    computeCheminsAvecEcran(rayon, source, ptsRight, horizontal, tabChemins, distance);

    if (tabChemins.size() == 0)
    {
        computeCheminSansEcran(rayon, source, tabChemins, distance);
    }

    // Calcul des reflexions si necessaire
    computeCheminReflexion(tabIntersect, rayon, source, tabChemins, distance);

    // On calcule systematiquement le chemin a plat sans obstacle
    // pour limiter l'effet d'ecran a basse frequence
    TYTabChemin& tabCheminsSansEcran = trajet.getCheminsDirect();
    computeCheminAPlat(rayon, source, tabCheminsSansEcran, distance);

    // Calcul la pression cumulee de tous les chemins au point de reception du trajet
    solve(trajet);

    // Le calcul est fini pour ce trajet, on peut effacer les tableaux des chemins
    tabChemins.clear();
    tabCheminsSansEcran.clear();
}

void TYAcousticModel::computeCheminAPlat(const OSegment3D& rayon, const tympan::AcousticSource& source, TYTabChemin& TabChemins, double distance) const
{
    TYTabEtape tabEtapes;

    // Calcul de la pente moyenne sur le trajet source-recepteur
    OSegment3D penteMoyenne;
    meanSlope(rayon, penteMoyenne);

    // Etape directe Source-Recepteur
    TYEtape etape1;
    TYChemin chemin1;

    etape1._pt = rayon._ptA;
	etape1._type = TYSOURCE;
    etape1._Absorption = (source.directivity->lwAdjustment(OVector3D(rayon._ptA, rayon._ptB), rayon.longueur())).racine();

    chemin1.setType(CHEMIN_DIRECT);

    tabEtapes.push_back(etape1);    // Ajout de l'etape directe
    chemin1.setLongueur(distance);  // Dans ce cas, la longueur = la distance source/recepteur
    chemin1.setDistance(distance);
    chemin1.calcAttenuation(tabEtapes, *pSolverAtmos);

    TabChemins.push_back(chemin1);
    tabEtapes.clear(); // Vide le tableau des etapes

    // Liaison avec reflexion
    //              1. Calcul du point de reflexion

    // Ajout du chemin reflechi
    TYEtape etape2;
    TYChemin chemin2;
    chemin2.setType(CHEMIN_SOL);

    etape2.setPoint(rayon._ptA);
	etape2._type = TYSOURCE;

    OPoint3D ptSym;
    int symOK = 0;
    if (penteMoyenne.longueur() > 0) // Si la pente moyenne est definie, on prend le point symetrique
    {
        symOK = penteMoyenne.symetrieOf(rayon._ptA, ptSym);
    }

    if (symOK == 0) // Sinon on prend une simple symetrie par rapport a z
    {
        ptSym = rayon._ptA;
        ptSym._z = 2 * penteMoyenne._ptA._z - ptSym._z;
    }

    // Calcul du point de reflexion
    OPoint3D ptReflex;
    int result = penteMoyenne.intersects(OSegment3D(ptSym, rayon._ptB), ptReflex, TYSEUILCONFONDUS);

    //              2. Etape avant la reflexion
    OSegment3D seg1(rayon._ptA, ptReflex); // On part de la source vers le point de reflexion
    double rr = seg1.longueur();

    // Directivite de la source
    etape2._Absorption = (source.directivity->lwAdjustment(OVector3D(seg1._ptA, seg1._ptB), seg1.longueur())).racine();

    tabEtapes.push_back(etape2); // Ajout de l'etape avant reflexion

    //              3. Etape apres la reflexion

    TYEtape etape3;
    etape3._pt = ptReflex;
	etape3._type = TYREFLEXIONSOL;

    OSegment3D seg2 = OSegment3D(ptReflex, rayon._ptB);// Segment Point de reflexion->Point de reception
    rr += seg2.longueur(); // Longueur parcourue sur le trajet reflechi

    if (_useSol)
    {
        etape3._Absorption = getReflexionSpectrumAt( seg1, rr, penteMoyenne, source);
    }
    else  // Sol totalement reflechissant
    {
        etape3._Absorption = _absoNulle;
    }

    tabEtapes.push_back(etape3); // Ajout de l'etape apres reflexion

    chemin2.setLongueur(rr);
    chemin2.setDistance(distance);
    chemin2.calcAttenuation(tabEtapes, *pSolverAtmos);
    TabChemins.push_back(chemin2);

    tabEtapes.clear(); // Vide le tableau des etapes
}

void TYAcousticModel::computeCheminSansEcran(const OSegment3D& rayon, const tympan::AcousticSource& source, TYTabChemin& TabChemin, double distance) const
{
    /*
        LE CALCUL POUR UN TRAJET SANS OBSTACLE COMPORTE UN CHEMIN DIRECT
        ET DE UN (CONDITIONS NORMALES) A TROIS (CONDITIONS FAVORABLES) TRAJETS
        REFLECHIS
    */
    OSegment3D seg1, seg2;
    TYTabEtape tabEtapes;
    double rr = 0.0; // Longueur du chemin reflechi

    // Calcul de la pente moyenne sur le trajet source-recepteur
    OSegment3D penteMoyenne;
    meanSlope(rayon, penteMoyenne);


    // 1. Conditions homogenes sans vegetation
    TYTabEtape Etapes;
    addEtapesSol(rayon._ptA, rayon._ptB, penteMoyenne, source, true, true, Etapes, rr);

    // Ajout du chemin direct
    TYChemin chemin;
    chemin.setType(CHEMIN_DIRECT);
    tabEtapes.push_back(Etapes[0]); // Ajout de l'etape directe
    chemin.setLongueur(distance);  // Dans ce cas, la longueur = la distance source/recepteur
    chemin.setDistance(distance);
    chemin.calcAttenuation(tabEtapes, *pSolverAtmos);
    TabChemin.push_back(chemin) ; // (4) Ajout du chemin dans le tableau des chemins de la frequence

    tabEtapes.clear(); // Vide le tableau des etapes

    // Ajout du chemin reflechi
    chemin.setType(CHEMIN_SOL);
    tabEtapes.push_back(Etapes[1]); // Ajout de l'etape avant reflexion
    tabEtapes.push_back(Etapes[2]); // Ajout de l'etape apres reflexion
    chemin.setLongueur(rr);
    chemin.setDistance(distance);
    chemin.calcAttenuation(tabEtapes, *pSolverAtmos);
    TabChemin.push_back(chemin);

    tabEtapes.clear(); // Vide le tableau des etapes

    // 2. Conditions faborables
    // on s'assure que les reflexions n'iront pas se faire au dela de la source et
    // du recepteur

    if (_conditionFav)
    {
        OPoint3D ptProj;
		int res;
        double hauteurA = 0.0, hauteurB = 0.0;
        if (penteMoyenne.longueur() > 0)
        {
            res = penteMoyenne.projection(rayon._ptA, ptProj, TYSEUILCONFONDUS);
			if (res == 0) {ptProj = penteMoyenne._ptA;}
            hauteurA = rayon._ptA._z - ptProj._z;
            res = penteMoyenne.projection(rayon._ptB, ptProj, TYSEUILCONFONDUS);
			if (res == 0) {ptProj = penteMoyenne._ptB;}
            hauteurB = rayon._ptB._z - ptProj._z;
        }
        else
        {
            ptProj = rayon._ptA;
            ptProj._z = penteMoyenne._ptA._z;
            hauteurA = rayon._ptA._z - ptProj._z;
            ptProj = rayon._ptB;
            ptProj._z = penteMoyenne._ptB._z;
            hauteurB = rayon._ptB._z - ptProj._z;
        }

        // On s'assure que le calcul en conditions favorable ne va pas provoquer
        //des reflexions au dela de la source et du recepteur
        if (rayon.longueur() > (10 * (hauteurA + hauteurB)))
        {
            // 2.1 Conditions favorables

            // En conditions favorables, le chemin possede deux points de reflexion supplementaires
            // Le premier a n fois la hauteur du point de reception (n = 10 en general) a proximite
            // de la source, le second est aussi a n fois la hauteur du point de reception; cote recepteur


            //          2.1.1 Premier chemin
            chemin.setType(CHEMIN_SOL);

            // Premiere etape
            TYEtape etape; // Etape 1.1
            etape._pt = rayon._ptA;
			etape._type = TYSOURCE;

            // Calcul du point de reflexion
            OPoint3D projA, projB;

            double distRef = _paramH * hauteurA;  //distance =H1*hauteur de la source

            if (penteMoyenne.longueur() > 0)
            {
                res = penteMoyenne.projection(rayon._ptA, projA, TYSEUILCONFONDUS);
				if (res == 0) {projA = penteMoyenne._ptA;}
            }
            else
            {
                projA = rayon._ptA;
                projA._z = penteMoyenne._ptA._z;
            }

            OVector3D vect(projA, penteMoyenne._ptB);
            vect.normalize();
            OPoint3D ptReflex(OVector3D(projA) + vect * distRef);

            seg1 = OSegment3D(rayon._ptA, ptReflex);

            rr = seg1.longueur();  // Longueur du chemin reflechi

            etape._Absorption = (source.directivity->lwAdjustment(OVector3D(rayon._ptA, rayon._ptB), rayon.longueur())).racine();

            tabEtapes.push_back(etape);

            // Deuxieme etape
            etape._pt = ptReflex;
			etape._type = TYREFLEXIONSOL;

            seg2 = OSegment3D(ptReflex, rayon._ptB);
            rr = rr + seg2.longueur(); // Longueur totale du chemin reflechi

            // Prise en compte du terrain au point de reflexion
            // 3 cas :
            if (_useSol)
            {
                etape._Absorption = getReflexionSpectrumAt( seg1, rr, penteMoyenne, source );
            }
            else  // Calcul sol reflechissant
            {
                etape._Absorption = _absoNulle;
            }

            tabEtapes.push_back(etape);

            // Ajout du premier chemin au trajet
            chemin.setLongueur(rr);
            chemin.setDistance(distance);
            chemin.calcAttenuation(tabEtapes, *pSolverAtmos);
            TabChemin.push_back(chemin) ; // (2) Ajout du chemin dans le tableau des chemins de la frequence

            tabEtapes.clear(); // On s'assure que le tableau des etapes est vide

            //          2.1.2. Deuxieme chemin
            chemin.setType(CHEMIN_SOL);

            // Premiere etape
            etape._pt = rayon._ptA;
			etape._type = TYSOURCE;

            // Calcul du point de reflexion
            if (penteMoyenne.longueur() > 0)
            {
                res = penteMoyenne.projection(rayon._ptB, projB, TYSEUILCONFONDUS);
				if (res == 0) {projB = penteMoyenne._ptB;}
            }
            else
            {
                projB = rayon._ptB;
                projB._z = penteMoyenne._ptB._z;
            }

            distRef = _paramH * hauteurB;
            ptReflex = OPoint3D(OVector3D(projB) - vect * distRef);

            seg1 = OSegment3D(rayon._ptA, ptReflex);
            rr = seg1.longueur(); // Longueur du chemin reflechi

            etape._Absorption = (source.directivity->lwAdjustment(OVector3D(rayon._ptA, rayon._ptB), rayon.longueur())).racine();

            tabEtapes.push_back(etape);

            // Deuxieme etape
            etape._pt = ptReflex;
			etape._type = TYREFLEXIONSOL;

            seg2 = OSegment3D(ptReflex, rayon._ptB);
            rr = rr + seg2.longueur(); // Longueur totale du chemin reflechi

            // Prise en compte du terrain au point de reflexion

            // 3 cas :
            if (_useSol)
            {
                etape._Absorption = getReflexionSpectrumAt( seg1, rr, penteMoyenne, source );
            }
            else  // Calcul sol reflechissant
            {
                etape._Absorption = _absoNulle;
            }


            tabEtapes.push_back(etape);

            // Ajout du deuxieme chemin
            chemin.setDistance(distance);
            chemin.setLongueur(rr);
            chemin.calcAttenuation(tabEtapes, *pSolverAtmos);
            TabChemin.push_back(chemin) ; // (3) Ajout du chemin dans le tableau des chemins de la frequence
        }
    }
}


bool TYAcousticModel::computeCheminsAvecEcran(const OSegment3D& rayon, const tympan::AcousticSource& source, const TabPoint3D& pts, const bool vertical, TYTabChemin& TabChemins, double distance) const
{
    /* ============================================================================================================
        07/03/2005 : Suppression du calcul ddes pentes moyennes avant et apres l'obstacle.
                     On utilise uniquement la pente moyenne totale qui est prise comme la projection au sol de la
                     source et du recepteur.
     ==============================================================================================================*/
    if (pts.size() <= 1) { return false; }

    double rr = 0.0; // Longueur parcourue lors de la reflexion sur le sol

    OSegment3D penteMoyenneTotale;//, penteMoyenneAvant, penteMoyenneApres;
    OPoint3D firstPt(pts[1]);
    OPoint3D lastPt(pts[pts.size() - 1]);

    TYTabEtape tabTwoReflex;
    double longTwoReflex = 0.0 ;

    TYTabEtape tabOneReflexBefore;
    double longOneReflexBefore = 0.0;

    TYTabEtape tabOneReflexAfter;
    double longOneReflexAfter = 0.0;

    TYTabEtape tabNoReflex;
    double longNoReflex = 0.0;

    //// Calcul de la pente moyenne sur le trajet source-recepteur
    meanSlope(rayon, penteMoyenneTotale);

    //                              /*--- AVANT L'OBSTACLE ---*/

    TYTabEtape Etapes;
    OSegment3D segCourant(rayon._ptA, firstPt);
    double tempLong = segCourant.longueur();

    bool bCheminOk = addEtapesSol(rayon._ptA, firstPt, penteMoyenneTotale, source, true, false, Etapes, rr); // Calcul des etapes avant l'obstacle

    // Si le parcours du rayon rencontre le sol (hors des reflexions), on ne continue pas la creation du chemin
    if (!bCheminOk) { return true; }

    tabNoReflex.push_back(Etapes[0]); // Ajoute le trajet direct au chemin sans reflexion
    longNoReflex += tempLong;

    tabOneReflexAfter.push_back(Etapes[0]); // Ajoute le trajet direct au chemin une reflexion apres
    longOneReflexAfter += tempLong;

    tabOneReflexBefore.push_back(Etapes[1]); // Ajoute les trajets reflechis
    tabOneReflexBefore.push_back(Etapes[2]);
    longOneReflexBefore += rr;

    tabTwoReflex.push_back(Etapes[1]); // Ajoute les trajets reflechis
    tabTwoReflex.push_back(Etapes[2]);
    longTwoReflex += rr;

    Etapes.clear(); // Efface le contenu de Etapes

    /*--- CONTOURNEMENT DE L'OBSTACLE ---*/

    double epaisseur = 0.0;
    TYEtape Etape;

    for (unsigned int i = 1; i < pts.size() - 1; i++)
    {
        epaisseur += (OSegment3D(pts[i], pts[i + 1])).longueur();

        Etape._pt = pts[i];
		Etape._type = TYDIFFRACTION;
        Etape._Absorption = _absoNulle;

        tabTwoReflex.push_back(Etape);
        tabOneReflexBefore.push_back(Etape);
        tabOneReflexAfter.push_back(Etape);
        tabNoReflex.push_back(Etape);
    }

    longNoReflex += epaisseur;
    longOneReflexAfter += epaisseur;
    longOneReflexBefore += epaisseur;
    longTwoReflex += epaisseur;

    /*--- APRES L'OBSTACLE ---*/
    segCourant =  OSegment3D(lastPt, rayon._ptB);
    tempLong = segCourant.longueur();

    addEtapesSol(lastPt, rayon._ptB, penteMoyenneTotale, source, false, true, Etapes, rr);

    tabNoReflex.push_back(Etapes[0]);
    longNoReflex += tempLong;

    tabOneReflexBefore.push_back(Etapes[0]);
    longOneReflexBefore += tempLong;

    tabOneReflexAfter.push_back(Etapes[1]);
    tabOneReflexAfter.push_back(Etapes[2]);
    longOneReflexAfter += rr;

    tabTwoReflex.push_back(Etapes[1]);
    tabTwoReflex.push_back(Etapes[2]);
    longTwoReflex += rr;

    Etapes.clear();

    /*--- PRISE EN COMPTE DE L'EFFET DE DIFFRACTION APPORTE PAR L'ECRAN SUR CHAQUE CHEMIN ---*/

    OSpectre Diff;
    bool bDiffOk = true; // Sera mis a false si la difference de marche devient <=0

    Etape._pt = rayon._ptB;
    Etape._Absorption = _absoNulle;

    //      4.1. Chemin sans reflexion
    Diff = calculAttDiffraction(rayon, penteMoyenneTotale, false, longNoReflex, epaisseur, vertical, false, bDiffOk);
    Etape._Attenuation = Diff;
    tabNoReflex.push_back(Etape);

    //      4.2. Chemin 2 reflexions
    Diff = calculAttDiffraction(rayon, penteMoyenneTotale, false, longTwoReflex, epaisseur, vertical, false, bDiffOk);
    Etape._Attenuation = Diff;
    tabTwoReflex.push_back(Etape);

    //      4.3. Chemin une reflexion avant
    Diff = calculAttDiffraction(rayon, penteMoyenneTotale, true, longOneReflexBefore, epaisseur, vertical, false, bDiffOk);
    Etape._Attenuation = Diff;
    tabOneReflexBefore.push_back(Etape);

    //      4.4. Chemin une reflexion apres
    Diff = calculAttDiffraction(rayon, penteMoyenneTotale, true, longOneReflexAfter, epaisseur, vertical, true, bDiffOk);
    Etape._Attenuation = Diff;
    tabOneReflexAfter.push_back(Etape);

    /*--- AJOUT DES CHEMINS AU au tableau des chemins ---*/

    TYChemin chemin;
    chemin.setType(CHEMIN_ECRAN);
    chemin.setDistance(distance);

    // Chemin reflexion au sol avant et apres l'obstacle
    chemin.setLongueur(longTwoReflex);
    chemin.calcAttenuation(tabTwoReflex, *pSolverAtmos);
    TabChemins.push_back(chemin);

    // Chemin avec une reflexion avant
    chemin.setLongueur(longOneReflexBefore);
    chemin.calcAttenuation(tabOneReflexBefore, *pSolverAtmos);
    TabChemins.push_back(chemin);

    // Chemin avec une reflexion apres
    chemin.setLongueur(longOneReflexAfter);
    chemin.calcAttenuation(tabOneReflexAfter, *pSolverAtmos);
    TabChemins.push_back(chemin);

    //Chemin sans reflexion sur le sol
    chemin.setLongueur(longNoReflex);
    chemin.calcAttenuation(tabNoReflex, *pSolverAtmos);
    TabChemins.push_back(chemin);

    return true;
}

bool TYAcousticModel::addEtapesSol(const OPoint3D& ptDebut, const OPoint3D& ptFin, const OSegment3D& penteMoyenne, const tympan::AcousticSource& source, const bool& fromSource, const bool& toRecepteur, TYTabEtape& Etapes, double& rr) const
{
    /* =========================================================================================
        0001 : 10/03/2005 : Modification du calcul des points symetriques
       ========================================================================================= */
    bool res = true;

    OSegment3D segDirect(ptDebut, ptFin);
    OPoint3D ptSym, ptReflex, pt3;

    TYEtape EtapeCourante;

    // === CONSTRUCTION DU TRAJET DIRECT ptDebut-ptFin
    EtapeCourante._pt = ptDebut;

    if (fromSource)   // Si on part d'une source, on tient compte de la directivite de celle-ci
    {
	    EtapeCourante._type = TYSOURCE;
        EtapeCourante._Absorption = (source.directivity->lwAdjustment(OVector3D(ptDebut, ptFin), ptDebut.distFrom(ptFin))).racine();
    }
    else
    {
	    EtapeCourante._type = TYDIFFRACTION;
        EtapeCourante._Absorption = _absoNulle;
    }

    Etapes.push_back(EtapeCourante);

    // === CONSTRUCTION DU TRAJET REFLECHI

    // Construction du plan correspondant a la pente moyenne.
    OPoint3D pt1(penteMoyenne._ptA);
    OPoint3D pt2(penteMoyenne._ptB);
    pt3._z = pt1._z;

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

    OPlan planPenteMoyenne(pt1, pt2, pt3);

    // On construit le segment correspondant a la projection des points sur le plan
    OPoint3D ptDebutProj; // PtDebut projete sur le plan
    planPenteMoyenne.intersectsSegment(OPoint3D(ptDebut._x, ptDebut._y, +100000), OPoint3D(ptDebut._x, ptDebut._y, -100000), ptDebutProj);

    OPoint3D ptFinProj; // PtFin projete sur le plan
    planPenteMoyenne.intersectsSegment(OPoint3D(ptFin._x, ptFin._y, +100000), OPoint3D(ptFin._x, ptFin._y, -100000), ptFinProj);

    OSegment3D segPente(ptDebutProj, ptFinProj);


    // Liaison avec reflexion
    //              Calcul du point de reflexion
    int symOK = 0;

    EtapeCourante._pt = ptDebut;
    if (segPente.longueur() > 0) // Si la pente moyenne est definie, on prend le point symetrique
    {
        symOK = segPente.symetrieOf(ptDebut, ptSym);
    }

    if ( symOK == 0 ) // Sinon on prend une simple symetrie par rapport a z
    {
        ptSym = ptDebut;
        ptSym._z = 2 * segPente._ptA._z - ptSym._z;
    }

    int result = segPente.intersects(OSegment3D(ptSym, ptFin), ptReflex, TYSEUILCONFONDUS);

    if (result == 0) // Si pas d'intersection trouvee, on passe au plan B
    {
        OPoint3D ptSymFin;
        symOK = 0;

        if (segPente.longueur() > 0) // Si la pente moyenne est definie, on prend le point symetrique
        {
            symOK = segPente.symetrieOf(ptFin, ptSymFin);
        }

        if ( symOK == 0 ) // Sinon on prend une simple symetrie par rapport a z
        {
            ptSymFin = ptFin;
            ptSymFin._z = 2 * segPente._ptB._z - ptSymFin._z;
        }

        // Calcul du coefficient lie au rapport des hauteurs
        // Distance de du point symetrique a la droite de pente moyenne
        double d1 = ptSym.distFrom(segPente._ptA);
        double d2 = ptSymFin.distFrom(segPente._ptB);

        double coefH = (d1 + d2) != 0 ? d1 / (d2 + d1) : 0.0;

        // Calcul des coordommees du point de reflexion
        ptReflex._x = (ptSymFin._x - ptSym._x) * coefH + ptSym._x;
        ptReflex._y = (ptSymFin._y - ptSym._y) * coefH + ptSym._y;
        ptReflex._z = (ptSymFin._z - ptSym._z) * coefH + ptSym._z;
    }


    // On traie deux cas :
    //          1/ le depart et l'arrivee ne sont pas sur le sol
    //          2/ le depart ou l'arrivee sont sur le sol et sont soit la source, soit le recepteur
    //          3/ ni 1, ni 2, dans ce cas, les segments ne sont pas produits
    if ((ptSym.distFrom(ptReflex) > TYSEUILCONFONDUS) && (ptFin.distFrom(ptReflex) > TYSEUILCONFONDUS))
    {
        //              Etape avant la reflexion
        OSegment3D seg1(ptDebut, ptReflex);

        rr = seg1.longueur();

        if (fromSource)   // Si on part d'une source, on tient compte de la directivite de celle-ci
        {
		    EtapeCourante._type = TYSOURCE;
            EtapeCourante._Absorption = (source.directivity->lwAdjustment(OVector3D(ptDebut, ptReflex), ptDebut.distFrom(ptReflex))).racine();
        }
        else
        {
		    EtapeCourante._type = TYDIFFRACTION;
            EtapeCourante._Absorption = _absoNulle;
        }

        Etapes.push_back(EtapeCourante);

        //              Etape Apres reflexion
        EtapeCourante._pt = ptReflex;
		EtapeCourante._type = TYREFLEXIONSOL;

        OSegment3D seg2 = OSegment3D(ptReflex, ptFin);// Segment Point de reflexion->Point de reception
        rr = rr + seg2.longueur(); // Longueur parcourue sur le trajet reflechi

        // 3 cas :
        if (_useSol)
        {
            EtapeCourante._Absorption = getReflexionSpectrumAt( seg1, rr , segPente, source);
        }
        else  // Sol totalement reflechissant
        {
            EtapeCourante._Absorption = _absoNulle;
        }

        Etapes.push_back(EtapeCourante);
    }
    else if (fromSource || toRecepteur)
    {
        //              Etape avant la reflexion
        OSegment3D seg1(ptDebut, ptReflex);
        rr = seg1.longueur();

        EtapeCourante._Absorption = _absoNulle;

        Etapes.push_back(EtapeCourante);

        //              Etape Apres reflexion
        EtapeCourante._pt = ptReflex;
		EtapeCourante._type = TYREFLEXIONSOL;

        OSegment3D seg2 = OSegment3D(ptReflex, ptFin);// Segment Point de reflexion->Point de reception
        rr = rr + seg2.longueur(); // Longueur parcourue sur le trajet reflechi

        // 3 cas :
        if (_useSol)
        {
            EtapeCourante._Absorption = getReflexionSpectrumAt( seg1, rr, segPente, source );
        }
        else  // Sol totalement reflechissant
        {
            EtapeCourante._Absorption = _absoNulle;
        }

        Etapes.push_back(EtapeCourante);
    }
    else
    {
        Etapes.clear();
        res = false;
    }

    return res;
}

void TYAcousticModel::computeCheminReflexion(   const std::deque<TYSIntersection>& tabIntersect, const OSegment3D& rayon,
                                                const tympan::AcousticSource& source, TYTabChemin& TabChemins,
                                                double distance ) const
{
    if (!_useReflex) { return; }

    OSegment3D segInter;
    OSegment3D rayonTmp;
    OPoint3D ptSym;
    OSpectre SpectreAbso;

    OSegment3D seg; // Segment source image->recepteur
    OSegment3D segMontant; // Segment source-> point de reflexion
    OSegment3D segDescendant; // Segment point de reflexion->recepteur

    OPoint3D pt; // Point d'intersection

    size_t nbFaces = tabIntersect.size();

    // Pour chaque face test de la reflexion
    for (unsigned int i = 0; i < nbFaces; i++)
    {
        TYSIntersection inter = tabIntersect[i];

        // Si la face ne peut interagir on passe a la suivante
        if ( (!inter.isInfra) || !(inter.bIntersect[1]) ) { continue; }

        segInter = inter.segInter[1];

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

            bool intersect = false;
            size_t j = 0;

            // Si on traverse un autre ecran, qui peut etre de la topo, le chemin de reflexion n'est pas pris en compte
            while ((j < nbFaces) && (!intersect))
            {
                if (j == i)
                {
                    j++;
                    continue; // Si la face ne peut interagir on passe a la suivante
                }

                segInter = tabIntersect[j].segInter[1];

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

            // Si le chemin reflechi n'est pas coupe, on peut calculer la reflexion
            if (!intersect)
            {
                SpectreAbso = dynamic_cast<tympan::AcousticBuildingMaterial*>(inter.material)->asEyring();
                SpectreAbso = SpectreAbso.mult(-1.0).sum(1.0);

                //// DO NOT REMOVE -- MAY BE REUSED
                //// Reflexion sur un cylindre, d'apres ISO9613-2
                ////
                //TYAcousticCylinder* pCyl = NULL;
                //if (pSurfaceGeoNode) { pCyl = dynamic_cast<TYAcousticCylinder*>(pSurfaceGeoNode->getParent()); }
                //rayonTmp = rayon * SI.matInv;
                //if (pCyl)
                //{
                //    OPoint3D centre(pCyl->getCenter());
                //    OVector3D SC(rayonTmp._ptA, centre);
                //    OVector3D CR(centre, rayonTmp._ptB);
                //    double diametre = pCyl->getDiameter();
                //    double dSC = SC.norme(); // Norme du vecteur
                //    double phi = SC.angle(CR);

                //    SpectreAbso = SpectreAbso.mult(diametre * sin(phi / 2) / (2 * dSC));
                //}

                // Premiere etape : du debut du rayon au point de reflexion sur la face
                TYTabEtape tabEtapes;

                double rr = segMontant.longueur() + segDescendant.longueur();

                TYEtape Etape;
                Etape._pt = rayon._ptA;
				Etape._type = TYSOURCE;
                Etape._Absorption = (source.directivity->lwAdjustment(OVector3D(segMontant._ptA, segMontant._ptB), segMontant.longueur())).racine();

                tabEtapes.push_back(Etape);

                // Deuxieme etape : du point de reflexion a la fin du rayon
                Etape._pt = segDescendant._ptA;
				Etape._type = TYREFLEXION;
                Etape._Absorption = SpectreAbso;

                tabEtapes.push_back(Etape);

                TYChemin Chemin;
                Chemin.setType(CHEMIN_REFLEX);
                Chemin.setLongueur(rr);
                Chemin.setDistance(distance);
                Chemin.calcAttenuation(tabEtapes, *pSolverAtmos);

                TabChemins.push_back(Chemin); // Mise en place du chemin dans la table des chemins
            }
        }
    }
}



OSpectre TYAcousticModel::calculC(const double& epaisseur) const
{
    // C = (1 + (5*lambda/epaisseur)i申) / (1/3 + (5*lambda/epaisseur)i申)

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
        opLambda = opLambda.mult(opLambda); // (5*lambda/e)i申

        C = opLambda.sum(1.0); // 1 + (5*lambda/e)i申
        C = C.div(opLambda.sum(unTiers)); // (1 + (5*lambda/e)i申) / (1/3 + (5*lambda/epaisseur)i申)
    }

    C.setType(SPECTRE_TYPE_AUTRE);  // Ni Attenuation, ni Absorption

    return C;
}

OSpectre TYAcousticModel::calculAttDiffraction(const OSegment3D& rayon, const OSegment3D& penteMoyenne, const bool& miroir, const double& re, const double& epaisseur, const bool& vertical, const bool& avantApres, bool& bDiffOk) const
{
    double rd;

    OSpectre s;

    OSpectre C = calculC(epaisseur); // Facteur correctif lie a l'epaisseur de l'ecran

    // Si le chemin comporte une reflexion sur le sol (et une seule), on prend le trajet source image recepteur
    if (miroir)
    {
        OPoint3D ptSym;
        if (!avantApres) // On est avant l'obstacle, on calcul le symetrique du point A
        {
            if (penteMoyenne.longueur() > 0) // On peut calculer la symetrie
            {
                penteMoyenne.symetrieOf(rayon._ptA, ptSym);
            }
            else // On se contente de prendre le symetrique par rapport a z
            {
                ptSym = rayon._ptA;
                ptSym._z = 2 * penteMoyenne._ptA._z - ptSym._z;
            }

            OSegment3D segReflex(ptSym, rayon._ptB);
            rd = segReflex.longueur();
        }
        else // On est apres l'obstacle, on calcule le symetrique du point B
        {
            if (penteMoyenne.longueur() > 0) // On peut calculer la symetrie
            {
                penteMoyenne.symetrieOf(rayon._ptB, ptSym);
            }
            else // On se contente de prendre le symetrique par rapport a z
            {
                ptSym = rayon._ptB;
                ptSym._z = 2 * penteMoyenne._ptB._z - ptSym._z;
            }

            OSegment3D segReflex(rayon._ptA, ptSym);
            rd = segReflex.longueur();
        }
    }
    else
    {
        rd = rayon.longueur();
    }

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

bool TYAcousticModel::solve(TYTrajet& trajet)
{
    const double PIM4 = 4.0 * M_PI;

    double rD2 = trajet.getDistance();

    rD2 = rD2 * rD2 ;

    double divGeom = pSolverAtmos->compute_z() / (PIM4 * rD2);

    OSpectre& SLp = trajet.getSpectre();

    // W.rho.c / (4pi*rdi申)
    SLp = trajet.asrc.spectrum.mult(divGeom);

    //  (W.rho.c/4.pi.Rdi申)*Attenuations du trajet
    if (_interference)
    {
        SLp = SLp.mult(trajet.getPInterference(*pSolverAtmos));
    }
    else
    {
        SLp = SLp.mult(trajet.getPEnergetique(*pSolverAtmos));
    }
    SLp.setType(SPECTRE_TYPE_LP); //Le spectre au point est bien un spectre de pression !

    return true;
}

OSpectreComplex TYAcousticModel::getReflexionSpectrumAt(const OSegment3D& incident, double length, const OSegment3D& segPente, const tympan::AcousticSource& source) const
{
    OSpectreComplex spectre;

    // Search for material at reflexion point
    // Set position of ray at the same high as the source
    vec3 start = OPoint3Dtovec3(incident._ptB);
    start.z = incident._ptA._z;
    Ray ray1( start, vec3(0., 0., -1.) );
    ray1.maxt = 20000;

    std::list<Intersection> LI;

    double distance1 = static_cast<double>( _solver.getScene()->getAccelerator()->traverse( &ray1, LI ) );

    if(LI.empty())
    {
        start.z = incident._ptB._z + 1000;
        Ray ray1( start, vec3(0., 0., -1.) );
        ray1.maxt = 20000;
        distance1 = static_cast<double>( _solver.getScene()->getAccelerator()->traverse(&ray1, LI));
    }

    assert(!LI.empty());
    unsigned int indexFace = LI.begin()->p->getPrimitiveId();
    tympan::AcousticMaterialBase *mat = _solver.getTabPolygon()[indexFace].material;

    // Avoid cases where the reflexion point is below a "floating" volumic source
    while(_solver.getTabPolygon()[indexFace].is_infra() && source.volume_id == _solver.getTabPolygon()[indexFace].volume_id )
    {
        start.z = _solver.getTabPolygon()[indexFace].tabPoint[0]._z;
        Ray ray(start, vec3(0,0,-1));
        ray.maxt = 20000;
        std::list<Intersection> LI2;
        distance1 = static_cast<double>( _solver.getScene()->getAccelerator()->traverse( &ray, LI2 ) );
        assert( !LI2.empty() );
        indexFace = LI2.begin()->p->getPrimitiveId();
        mat = _solver.getTabPolygon()[indexFace].material;
    }

    // Angle estimation
    OVector3D direction(incident._ptA, incident._ptB);
    direction.normalize();

    // This is kept commented for the time being, even though it's the best solution
    //double angle = ( direction * -1 ).angle( _solver.getTabPolygon()[indexFace].normal );
    //angle = ABS(M_PI/2. - angle);
    double angle = (direction*-1).angle(OVector3D(incident._ptB, segPente._ptA));
    // Compute reflexion spectrum
    spectre = mat->get_absorption(angle, length);

    return spectre;
}

void TYAcousticModel::meanSlope(const OSegment3D& director, OSegment3D& slope) const
{
    // Search for primitives under the two segment extremities

    // To begin : initialize slope
    slope = director;

    // first one
    OPoint3D pt = director._ptA;
    pt._z += 1000.;
    vec3 start = OPoint3Dtovec3(pt);
    Ray ray1( start, vec3(0., 0., -1.) );

    std::list<Intersection> LI;

    double distance1 = static_cast<double>( _solver.getScene()->getAccelerator()->traverse( &ray1, LI ) );
    assert( distance1 > 0. );
    assert(!LI.empty());

    unsigned int indexFace = LI.begin()->p->getPrimitiveId();

    // Avoid cases where the extremities are above infrastructure elements
    while(_solver.getTabPolygon()[indexFace].is_infra()){
        start.z = _solver.getTabPolygon()[indexFace].tabPoint[0]._z;
        Ray ray(start, vec3(0,0,-1));
        ray.maxt = 20000;
        std::list<Intersection> LI2;
        double distance = static_cast<double>( _solver.getScene()->getAccelerator()->traverse( &ray, LI2 ) );
        assert(distance > 0.);
        if (LI2.empty())
            break;
        distance1 += distance;
        indexFace = LI2.begin()->p->getPrimitiveId();
	}

    // Second one
    LI.clear();
    pt = director._ptB;
    pt._z += 1000.;
    start = OPoint3Dtovec3(pt);
    Ray ray2( start, vec3(0., 0., -1.) );

    double distance2 = static_cast<double>( _solver.getScene()->getAccelerator()->traverse( &ray2, LI ) );
    // An error can occur if some elements are outside of the grip (emprise)
    assert( distance2 > 0. );
    assert(!LI.empty());

    indexFace = LI.begin()->p->getPrimitiveId();

    // Avoid cases where the extremities are above infrastructure elements
    while(_solver.getTabPolygon()[indexFace].is_infra()){
        start.z = _solver.getTabPolygon()[indexFace].tabPoint[0]._z;
        Ray ray(start, vec3(0,0,-1));
        ray.maxt = 20000;
        std::list<Intersection> LI2;
        double distance = static_cast<double>( _solver.getScene()->getAccelerator()->traverse( &ray, LI2 ) );
        assert(distance1 > 0.);
        if (LI2.empty())
            break;
        distance1 += distance;
        indexFace = LI2.begin()->p->getPrimitiveId();
        }

    // Compute projection on the ground of segment points suppose sol is under the points ...
    slope._ptA._z = director._ptA._z - (distance1-1000.);
    slope._ptB._z = director._ptB._z - (distance2-1000.);
}

