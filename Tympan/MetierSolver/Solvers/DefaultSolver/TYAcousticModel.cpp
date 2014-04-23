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

#include <deque>
#include "TYAcousticModel.h"
#include "Tympan/MetierSolver/DataManagerMetier/Commun/TYCalcul.h"
#include "Tympan/MetierSolver/DataManagerMetier/Commun/TYTrajet.h"
#include "Tympan/MetierSolver/DataManagerMetier/Site/TYTopographie.h"
#include "Tympan/MetierSolver/DataManagerMetier/Site/TYSiteNode.h"
#include "Tympan/MetierSolver/ToolsMetier/Defines.h"
#include "TYSolver.h"

TYAcousticModel::TYAcousticModel(TYSolver& solver)
    : _expGeo(1),
      _typeCalculSol(0),
      _useSol(true),
      _useEcran(true),
      _useReflex(false),
      _conditionFav(false),
      _useAtmo(true),
      _bCalculTrajetsHorizontaux(true),
      _useVegetation(false),
      _interference(false),
      _seuilConfondus(0.01),
      _paramH(10.0),
      _pAtmo(NULL),
      _pTopographie(NULL),
      _solver(solver)
{
    _absoNulle = OSpectreComplex(TYComplex(1.0, 0.0));
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
    _pAtmo = calcul.getAtmosphere()._pObj;


    // Calcul avec prise en compte des ecrans
    _useEcran = calcul.getUseEcran();


    // Prise en compte de la vegetation
    _useVegetation = calcul.getUseVegetation();

    // Prise en compte de l'atmosphere (divergence geometrique)
    _useAtmo = calcul.getUseAtmosphere();


    // Calcul avec interference
    _interference = calcul.getInterference();

    // On calcul tout de suite le spectre de longueur d'onde
    _lambda = OSpectre::getLambda(_pAtmo->getVitSon());

    /// Calcul reflexion sur sol locale ou etendue
    _typeCalculSol = calcul.getTypeCalculSol();


    // Distance minimale entre deux points consideres comme confondus
    _seuilConfondus = calcul.getSeuilConfondu();


    /// Calcul des trajets horizontaux
    _bCalculTrajetsHorizontaux = calcul.getCalculTrajetsHorizontaux();


    // Topographie du site
    _pTopographie = site.getTopographie()._pObj;


    // Coefficient multiplicateur pour le calcul des reflexions supplementaires en condition favorable
    _paramH = calcul.getParamH();
}


void TYAcousticModel::compute(const TYSIntersection* tabIntersect, const OSegment3D& rayon, TYTrajet& trajet, TYTabPoint3D& ptsTop, TYTabPoint3D& ptsLeft, TYTabPoint3D& ptsRight)
{
    bool vertical = true, horizontal = false;

    // Recuperation de la source
    TYSourcePonctuelleGeoNode* pSrcGeoNode = trajet.getSourcePonctuelle();

    // Distance de la source au recepteur
    double distance = trajet.getDistance();

    TYTabChemin& tabChemins = trajet.getChemins();

    // Calcul des parcours lateraux
    // 1. Vertical
    bool bObstacle = computeCheminsAvecEcran(rayon, pSrcGeoNode, ptsTop, vertical, tabChemins, distance);

    // 2. Horizontal gauche
    bObstacle &= computeCheminsAvecEcran(rayon, pSrcGeoNode, ptsLeft, horizontal, tabChemins, distance);

    // 3. Horizontal droite
    bObstacle &= computeCheminsAvecEcran(rayon, pSrcGeoNode, ptsRight, horizontal, tabChemins, distance);

    if (tabChemins.size() == 0)
    {
        computeCheminSansEcran(rayon, pSrcGeoNode, tabChemins, distance);
    }

    // Calcul des reflexions si necessaire
    computeCheminReflexion(tabIntersect, rayon, pSrcGeoNode, tabChemins, distance);

    // On calcule systematiquement le chemin a plat sans obstacle
    // pour limiter l'effet d'ecran a basse frequence
    TYTabChemin& tabCheminsSansEcran = trajet.getCheminsDirect();
    computeCheminAPlat(rayon, pSrcGeoNode, tabCheminsSansEcran, distance);

    // Calcul la pression cumulee de tous les chemins au point de reception du trajet
    solve(trajet);

    // Le calcul est fini pour ce trajet, on peut effacer les tableaux des chemins
    tabChemins.clear();
    tabCheminsSansEcran.clear();
}

void TYAcousticModel::computeCheminAPlat(const OSegment3D& rayon, const TYSourcePonctuelleGeoNode* pSrcGeoNode, TYTabChemin& TabChemins, double distance) const
{
    TYSourcePonctuelle* pSrc = NULL;
    if (pSrcGeoNode) { pSrc = dynamic_cast<TYSourcePonctuelle*>(pSrcGeoNode->getElement()); }

    TYTabEtape tabEtapes;

    // Calcul de la pente moyenne sur le trajet source-recepteur
    OSegment3D penteMoyenne;
    _pTopographie->penteMoy(rayon, penteMoyenne);

    // Etape directe Source-Recepteur
    TYEtape etape1;
    TYChemin chemin1;

    etape1._pt = rayon._ptA;
    if (pSrc) { etape1._Absorption = pSrc->lwApparenteSrcDest(rayon * pSrcGeoNode->getMatrix().getInvert(), *_pAtmo, _expGeo); }

    chemin1.setType(CHEMIN_DIRECT);

    tabEtapes.push_back(etape1);    // Ajout de l'etape directe
    chemin1.setLongueur(distance);  // Dans ce cas, la longueur = la distance source/recepteur
    chemin1.setDistance(distance);
    chemin1.calcAttenuation(tabEtapes, *_pAtmo);

    TabChemins.push_back(chemin1);
    tabEtapes.clear(); // Vide le tableau des etapes

    // Liaison avec reflexion
    //              1. Calcul du point de reflexion

    // Ajout du chemin reflechi
    TYEtape etape2;
    TYChemin chemin2;
    chemin2.setType(CHEMIN_SOL);

    etape2.setPoint(rayon._ptA);

    OPoint3D ptSym;
    if (penteMoyenne.longueur() > 0) // Si la pente moyenne est definie, on prend le point symetrique
    {
        penteMoyenne.symetrieOf(rayon._ptA, ptSym);
    }
    else // Sinon on prend une simple symetrie par rapport a z
    {
        ptSym = rayon._ptA;
        ptSym._z = 2 * penteMoyenne._ptA._z - ptSym._z;
    }

    // Calcul du point de reflexion
    OPoint3D ptReflex;
    penteMoyenne.intersects(OSegment3D(ptSym, rayon._ptB), ptReflex, _seuilConfondus);

    //              2. Etape avant la reflexion
    OSegment3D seg1(rayon._ptA, ptReflex); // On part de la source vers le point de reflexion
    double rr = seg1.longueur();

    // Directivite de la source
    if (pSrc) { etape2._Absorption = pSrc->lwApparenteSrcDest(seg1 * pSrcGeoNode->getMatrix().getInvert(), *_pAtmo, _expGeo); }

    tabEtapes.push_back(etape2); // Ajout de l'etape avant reflexion

    //              3. Etape apres la reflexion

    TYEtape etape3;
    etape3._pt = ptReflex;

    OSegment3D seg2 = OSegment3D(ptReflex, rayon._ptB);// Segment Point de reflexion->Point de reception
    rr += seg2.longueur(); // Longueur parcourue sur le trajet reflechi

    if (_useSol)
    {
        TYTerrain* pTerrain = NULL;
        TYSol* pSol = NULL;
        double angleA = OVector3D(ptReflex, penteMoyenne._ptA).angle(OVector3D(ptReflex, rayon._ptA));// Angle d'incidence

        pTerrain = _pTopographie->terrainAt(ptReflex);
        if (!pTerrain) { pTerrain = _pTopographie->getDefTerrain(); }
        if (pTerrain)
        {
            pSol = pTerrain->getSol();
            if (pSol) { etape3._Absorption = pSol->abso(angleA, rr, *_pAtmo); }
        }
    }
    else  // Sol totalement reflechissant
    {
        etape3._Absorption = _absoNulle;
    }

    tabEtapes.push_back(etape3); // Ajout de l'etape apres reflexion

    chemin2.setLongueur(rr);
    chemin2.setDistance(distance);
    chemin2.calcAttenuation(tabEtapes, *_pAtmo);
    TabChemins.push_back(chemin2);

    tabEtapes.clear(); // Vide le tableau des etapes
}

void TYAcousticModel::computeCheminSansEcran(const OSegment3D& rayon, const TYSourcePonctuelleGeoNode* pSrcGeoNode, TYTabChemin& TabChemin, double distance) const
{
    /*
        LE CALCUL POUR UN TRAJET SANS OBSTACLE COMPORTE UN CHEMIN DIRECT
        ET DE UN (CONDITIONS NORMALES) A TROIS (CONDITIONS FAVORABLES) TRAJETS
        REFLECHIS
    */
    TYSourcePonctuelle* pSrc = NULL;
    if (pSrcGeoNode) { pSrc = dynamic_cast<TYSourcePonctuelle*>(pSrcGeoNode->getElement()); }
    TYTerrain* pTerrain = NULL;
    TYSol* pSol = NULL;

    OSegment3D seg;
    TYTabEtape tabEtapes;
    double rr = 0.0; // Longueur du chemin reflechi

    // Calcul de la pente moyenne sur le trajet source-recepteur
    OSegment3D penteMoyenne;
    _pTopographie->penteMoy(rayon, penteMoyenne);


    // 1. Chemin direct avec vegetation
    if (_useVegetation)
    {
        computeCheminAvecVeg(rayon, pSrcGeoNode, penteMoyenne, TabChemin, distance);

        if (TabChemin.size() > 0) { return; }
    }

    // 1. Conditions homogenes sans vegetation
    TYTabEtape Etapes;
    addEtapesSol(rayon._ptA, rayon._ptB, penteMoyenne, pSrcGeoNode, true, true, Etapes, rr);

    // Ajout du chemin direct
    TYChemin chemin;
    chemin.setType(CHEMIN_DIRECT);
    tabEtapes.push_back(Etapes[0]); // Ajout de l'etape directe
    chemin.setLongueur(distance);  // Dans ce cas, la longueur = la distance source/recepteur
    chemin.setDistance(distance);
    chemin.calcAttenuation(tabEtapes, *_pAtmo);
    TabChemin.push_back(chemin) ; // (4) Ajout du chemin dans le tableau des chemins de la frequence

    tabEtapes.clear(); // Vide le tableau des etapes

    // Ajout du chemin reflechi
    chemin.setType(CHEMIN_SOL);
    tabEtapes.push_back(Etapes[1]); // Ajout de l'etape avant reflexion
    tabEtapes.push_back(Etapes[2]); // Ajout de l'etape apres reflexion
    chemin.setLongueur(rr);
    chemin.setDistance(distance);
    chemin.calcAttenuation(tabEtapes, *_pAtmo);
    TabChemin.push_back(chemin);

    tabEtapes.clear(); // Vide le tableau des etapes

    // 2. Conditions faborables
    // on s'assure que les reflexions n'iront pas se faire au dela de la source et
    // du recepteur

    if (_conditionFav)
    {
        OPoint3D ptProj;
        double hauteurA = 0.0, hauteurB = 0.0;
        if (penteMoyenne.longueur() > 0)
        {
            penteMoyenne.projection(rayon._ptA, ptProj, _seuilConfondus);
            hauteurA = rayon._ptA._z - ptProj._z;
            penteMoyenne.projection(rayon._ptB, ptProj, _seuilConfondus);
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

            // Calcul du point de reflexion
            OPoint3D projA, projB;

            double distRef = _paramH * rayon._ptA._z;  //distance =H1*hauteur de la source

            if (penteMoyenne.longueur() > 0)
            {
                penteMoyenne.projection(rayon._ptA, projA, _seuilConfondus);
            }
            else
            {
                projA = rayon._ptA;
                projA._z = penteMoyenne._ptA._z;
            }

            OVector3D vect(projA, penteMoyenne._ptB);
            vect.normalize();
            OPoint3D ptReflex(OVector3D(projA) + vect * distRef);

            seg = OSegment3D(rayon._ptA, ptReflex);

            rr = seg.longueur();  // Longueur du chemin reflechi

            if (pSrc) { etape._Absorption = pSrc->lwApparenteSrcDest(rayon * pSrcGeoNode->getMatrix().getInvert(), *_pAtmo, _expGeo); }

            tabEtapes.push_back(etape);

            // Deuxieme etape
            etape._pt = ptReflex;

            seg = OSegment3D(ptReflex, rayon._ptB);
            rr = rr + seg.longueur(); // Longueur totale du chemin reflechi

            // Prise en compte du terrain au point de reflexion
            // 3 cas :
            if (_useSol)
            {
                double angle = OVector3D(ptReflex, projA).angle(OVector3D(ptReflex, rayon._ptA));

                pTerrain = _pTopographie->terrainAt(ptReflex);
                if (!pTerrain) { pTerrain = _pTopographie->getDefTerrain(); }
                if (pTerrain) // Une zone de terrain a ete definie
                {
                    pSol = pTerrain->getSol();
                    if (pSol) { etape._Absorption = pSol->abso(angle, rr, *_pAtmo); }
                }
            }
            else  // Calcul sol reflechissant
            {
                etape._Absorption = _absoNulle;
            }

            tabEtapes.push_back(etape);

            // Ajout du premier chemin au trajet
            chemin.setLongueur(rr);
            chemin.setDistance(distance);
            chemin.calcAttenuation(tabEtapes, *_pAtmo);
            TabChemin.push_back(chemin) ; // (2) Ajout du chemin dans le tableau des chemins de la frequence

            tabEtapes.clear(); // On s'assure que le tableau des etapes est vide

            //          2.1.2. Deuxieme chemin
            chemin.setType(CHEMIN_SOL);

            // Premiere etape
            etape._pt = rayon._ptA;

            // Calcul du point de reflexion
            if (penteMoyenne.longueur() > 0)
            {
                penteMoyenne.projection(rayon._ptB, projB, _seuilConfondus);
            }
            else
            {
                projB = rayon._ptB;
                projB._z = penteMoyenne._ptB._z;
            }

            ptReflex = OPoint3D(OVector3D(projB) - vect * distRef);

            seg = OSegment3D(rayon._ptA, ptReflex);
            rr = seg.longueur(); // Longueur du chemin reflechi

            if (pSrc) { etape._Absorption = pSrc->lwApparenteSrcDest(rayon * pSrcGeoNode->getMatrix().getInvert(), *_pAtmo, _expGeo); }

            tabEtapes.push_back(etape);

            // Deuxieme etape
            etape._pt = ptReflex;

            seg = OSegment3D(ptReflex, rayon._ptB);
            rr = rr + seg.longueur(); // Longueur totale du chemin reflechi

            // Prise en compte du terrain au point de reflexion

            // 3 cas :
            if (_useSol)
            {
                double angle = OVector3D(ptReflex, projA).angle(OVector3D(ptReflex, rayon._ptA));

                pTerrain = _pTopographie->terrainAt(ptReflex);
                if (!pTerrain) { pTerrain = _pTopographie->getDefTerrain(); }
                if (pTerrain) // Une zone de terrain a ete definie
                {
                    pSol = pTerrain->getSol();
                    if (pSol) { etape._Absorption = pSol->abso(angle, rr, *_pAtmo); }
                }
            }
            else  // Calcul sol reflechissant
            {
                etape._Absorption = _absoNulle;
            }


            tabEtapes.push_back(etape);

            // Ajout du deuxieme chemin
            chemin.setDistance(distance);
            chemin.setLongueur(rr);
            chemin.calcAttenuation(tabEtapes, *_pAtmo);
            TabChemin.push_back(chemin) ; // (3) Ajout du chemin dans le tableau des chemins de la frequence
        }
    }
}

void TYAcousticModel::computeCheminAvecVeg(const OSegment3D& rayon, const TYSourcePonctuelleGeoNode* pSrcGeoNode, const OSegment3D& penteMoyenne, TYTabChemin& TabChemin, double distance) const
{
    TYSourcePonctuelle* pSrc = NULL;
    if (pSrcGeoNode) { pSrc = dynamic_cast<TYSourcePonctuelle*>(pSrcGeoNode->getElement()); }
    double longueur = 0.0;
    OSegment3D seg;

    TYChemin chemin;

    //  const int expGeo = _pCalcul->getExpansGeo();
    TYTabStructSegLPSp tabSegSp;
    //
    TYTabEtape tabEtapes;

    getSpectreAttVegetation(rayon, penteMoyenne, tabSegSp);

    if (tabSegSp.size() == 0) { return ; }  // Si pas de vegetation detectee, on retourne l'ensemble des chemins sans obstacles

    // Etape 1 : Avant la foret
    seg = OSegment3D(rayon._ptA, tabSegSp[0]._segment._ptA);
    longueur += seg.longueur();

    // Calcul de l'absorption (directivite) de la source
    TYEtape Etape;

    Etape._pt = rayon._ptA;
    if (pSrc) { Etape._Absorption = pSrc->lwApparenteSrcDest(rayon * pSrcGeoNode->getMatrix().getInvert(), *_pAtmo, _expGeo); }

    // On rentre dans la foret, loup y es tu ?
    OPoint3D ext;
    OPoint3D lastPoint(rayon._ptA);
    OSpectre spectreAtt = OSpectre::getEmptyLinSpectre(); // On cumule les attenuations des differents segments

    OPoint3D currentPoint;
    OSegment3D currentSeg;

    for (unsigned int i = 0; i < tabSegSp.size(); i++)
    {
        currentPoint._x = tabSegSp[i]._segment._ptA._x;
        currentPoint._y = tabSegSp[i]._segment._ptA._y;
        currentPoint._z = tabSegSp[i]._segment._ptA._z;

        currentSeg._ptA = lastPoint;
        currentSeg._ptB = currentPoint;
        longueur += currentSeg.longueur();

        // On somme les attenuation vegetation (attenuation atmospherique comprise)
        spectreAtt = spectreAtt.sum(*tabSegSp[i]._spectreAtt);

        if (i != (tabSegSp.size() - 1))
        {
            ext = OPoint3D(tabSegSp[i + 1]._segment._ptA);
        }
        else
        {
            ext = OPoint3D(rayon._ptB);
        }

        if (OPoint3D(tabSegSp[i]._segment._ptB) != ext)
        {
            /*
                        TYEtape Etape;
                        Etape.setPoint(tabSegSp[i]._segment._ptB);
                        tabEtapes.push_back(Etape);
            */
        }

        lastPoint = currentPoint;
    }

    Etape._Attenuation = spectreAtt;
    tabEtapes.push_back(Etape);

    chemin.setType(CHEMIN_DIRECT);
    chemin.setLongueur(longueur);
    chemin.setDistance(distance);
    chemin.calcAttenuation(tabEtapes, *_pAtmo);

    TabChemin.push_back(chemin);

    // Cleaning tab of segments crossing forest
    for (unsigned int i = 0; i < tabSegSp.size(); i++)
    {
        if (tabSegSp[i]._spectreAtt != NULL) { delete tabSegSp[i]._spectreAtt; tabSegSp[i]._spectreAtt = NULL; }
    }
    tabSegSp.clear();
}

void TYAcousticModel::getSpectreAttVegetation(const OSegment3D& rayon, const OSegment3D& penteMoyenne, TYTabStructSegLPSp& tabSegSp) const
{
    TYStructSegLPSp elmt;
    elmt._spectreAtt = NULL;

    double  hveg, hA, hB, l;
    OPoint3D ptCenter, ptMid, projA, projB, pt, ptTmp;
    double R = TY_EDF_FOREST_RAYON;

    // Recuperation des terrains et des segments associes le long du rayon
    TYTabStructSegPtrTer tabSegTer;
    tabSegTer = _pTopographie->sols(rayon);

    // Calcul du centre ptCenter du cercle de rayon R passant par les extremites du rayon
    // 2 cercles correspondent a ce critere,
    // on garde celui dont le centre a l'altitude la plus faible
    ptMid = rayon.centreOf();
    ptCenter = rayon.centerOfCurvedPath(R);
    if (ptCenter._z > ptMid._z)
    {
        ptCenter._z = ptMid._z - (ptCenter._z - ptMid._z);
    }

    // Construction du segment segCenter parallele au rayon passant par ptCenter
    OVector3D vectCenter(ptCenter);
    OVector3D vectMid(rayon._ptA, ptMid);
    OVector3D A(vectCenter - vectMid);
    OVector3D B(vectCenter + vectMid);
    OSegment3D segCenter(A, B);
    OVector3D vectAB(A, B);
    vectAB.normalize();

    for (unsigned int i = 0; i < tabSegTer.size(); i++)
    {
        // Pour chaque terrain traverse qui possede une vegetation
        // on calcule le segment et le spectre associe
        if (tabSegTer[i]._terrain->getSol()->getVegetActive())
        {
            // On considere le segment courant associe a chaque terrain
            OSegment3D segTmp(tabSegTer[i]._segment);
            OSegment3D seg;
            // Calcul de la projection du segment courant sur le rayon
            rayon.projection(segTmp._ptA, seg._ptA, _seuilConfondus);
            rayon.projection(segTmp._ptB, seg._ptB, _seuilConfondus);
            TYVegetation* pVeg = tabSegTer[i]._terrain->getSol()->getVegetation();

            // Calcul de la projection du segment courant sur segCenter
            segCenter.projection(seg._ptA, projA, _seuilConfondus);
            segCenter.projection(seg._ptB, projB, _seuilConfondus);
            // Hauteur de vegetation par rapport a segCenter
            hveg = pVeg->getHauteur() + OVector3D(segTmp._ptA, projA).norme();

            // Hauteur du premier point du segment courant par rapport a segCenter
            hA = sqrt(R * R - OVector3D(projA, ptCenter).norme() * OVector3D(projA, ptCenter).norme());
            // Hauteur du deuxieme point du segment courant par rapport a segCenter
            hB = sqrt(R * R - OVector3D(projB, ptCenter).norme() * OVector3D(projB, ptCenter).norme());
            // Distance par rapport a ptCenter et en projection sur segCenter
            // de l'extremite de la vegetation
            l = sqrt(ABS(R * R - hveg * hveg));

            // Si le premier point est en dessous de la vegetation
            if (hA < hveg)
            {
                // Si le deuxieme point est en dessous de la vegetation
                if (hB < hveg)
                {
                    // Si la vegetation est au dessus du cercle
                    // le segment est entierement dans la vegetation
                    if (hveg > R)
                    {
                        elmt._segment = seg;
                        elmt._spectreAtt = new OSpectre(pVeg->getAtt(seg, *_pAtmo));
                        tabSegSp.push_back(elmt);
                    }
                    else
                    {
                        ptTmp = OPoint3D(vectCenter - vectAB * l);
                        seg.projection(ptTmp, pt, _seuilConfondus);

                        elmt._segment = OSegment3D(seg._ptA, pt);
                        elmt._spectreAtt = new OSpectre(pVeg->getAtt(elmt._segment, *_pAtmo));
                        tabSegSp.push_back(elmt);

                        ptTmp = OPoint3D(vectCenter + vectAB * l);
                        seg.projection(ptTmp, pt, _seuilConfondus);
                        elmt._segment = OSegment3D(pt, seg._ptB);
                        elmt._spectreAtt = new OSpectre(pVeg->getAtt(elmt._segment, *_pAtmo));
                        tabSegSp.push_back(elmt);
                    }
                }
                else
                {
                    ptTmp = OPoint3D(vectCenter - vectAB * l);
                    seg.projection(ptTmp, pt, _seuilConfondus);
                    elmt._segment = OSegment3D(seg._ptA, pt);
                    elmt._spectreAtt = new OSpectre(pVeg->getAtt(elmt._segment, *_pAtmo));
                    tabSegSp.push_back(elmt);
                }
            }
            else if (hB < hveg)
            {
                ptTmp = OPoint3D(vectCenter + vectAB * l);
                seg.projection(ptTmp, pt, _seuilConfondus);
                elmt._segment = OSegment3D(pt, seg._ptB);
                elmt._spectreAtt = new OSpectre(pVeg->getAtt(elmt._segment, *_pAtmo));
                tabSegSp.push_back(elmt);
            }
        }
    }
}

bool TYAcousticModel::computeCheminsAvecEcran(const OSegment3D& rayon, const TYSourcePonctuelleGeoNode* pSrcGeoNode, const TYTabPoint3D& pts, const bool vertical, TYTabChemin& TabChemins, double distance) const
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
    _pTopographie->penteMoy(rayon, penteMoyenneTotale);

    //                              /*--- AVANT L'OBSTACLE ---*/

    TYTabEtape Etapes;
    OSegment3D segCourant(rayon._ptA, firstPt);
    double tempLong = segCourant.longueur();

    bool bCheminOk = addEtapesSol(rayon._ptA, firstPt, penteMoyenneTotale, pSrcGeoNode, true, false, Etapes, rr); // Calcul des etapes avant l'obstacle

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

    addEtapesSol(lastPt, rayon._ptB, penteMoyenneTotale, pSrcGeoNode, false, true, Etapes, rr);

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
    chemin.calcAttenuation(tabTwoReflex, *_pAtmo);
    TabChemins.push_back(chemin);

    // Chemin avec une reflexion avant
    chemin.setLongueur(longOneReflexBefore);
    chemin.calcAttenuation(tabOneReflexBefore, *_pAtmo);
    TabChemins.push_back(chemin);

    // Chemin avec une reflexion apres
    chemin.setLongueur(longOneReflexAfter);
    chemin.calcAttenuation(tabOneReflexAfter, *_pAtmo);
    TabChemins.push_back(chemin);

    //Chemin sans reflexion sur le sol
    chemin.setLongueur(longNoReflex);
    chemin.calcAttenuation(tabNoReflex, *_pAtmo);
    TabChemins.push_back(chemin);

    return true;
}

bool TYAcousticModel::addEtapesSol(const OPoint3D& ptDebut, const OPoint3D& ptFin, const OSegment3D& penteMoyenne, const TYSourcePonctuelleGeoNode* pSrcGeoNode, const bool& fromSource, const bool& toRecepteur, TYTabEtape& Etapes, double& rr) const
{
    /* =========================================================================================
        0001 : 10/03/2005 : Modification du calcul des points symetriques
       ========================================================================================= */
    bool res = true;

    TYSourcePonctuelle* pSrc = NULL;
    if (pSrcGeoNode) { pSrc = dynamic_cast<TYSourcePonctuelle*>(pSrcGeoNode->getElement()); }

    TYSol* pSol = NULL;

    OSegment3D segDirect(ptDebut, ptFin);
    OPoint3D ptSym, ptReflex, pt3;

    TYEtape EtapeCourante;

    // === CONSTRUCTION DU TRAJET DIRECT ptDebut-ptFin
    EtapeCourante._pt = ptDebut;

    if (fromSource && pSrc)   // Si on part d'une source, on tient compte de la directivite de celle-ci
    {
        EtapeCourante._Absorption = pSrc->lwApparenteSrcDest(segDirect * pSrcGeoNode->getMatrix().getInvert(), *_pAtmo, _expGeo);
    }
    else
    {
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
    EtapeCourante._pt = ptDebut;
    if (segPente.longueur() > 0) // Si la pente moyenne est definie, on prend le point symetrique
    {
        segPente.symetrieOf(ptDebut, ptSym);
    }
    else // Sinon on prend une simple symetrie par rapport a z
    {
        ptSym = ptDebut;
        ptSym._z = 2 * segPente._ptA._z - ptSym._z;
    }

    int result = segPente.intersects(OSegment3D(ptSym, ptFin), ptReflex, _seuilConfondus);

    if (result == 0) // Si pas d'intersection trouvee, on passe au plan B
    {
        OPoint3D ptSymFin;

        if (segPente.longueur() > 0) // Si la pente moyenne est definie, on prend le point symetrique
        {
            segPente.symetrieOf(ptFin, ptSymFin);
        }
        else // Sinon on prend une simple symetrie par rapport a z
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
    if ((ptSym.distFrom(ptReflex) > _seuilConfondus) && (ptFin.distFrom(ptReflex) > _seuilConfondus))
    {
        //              Etape avant la reflexion
        OSegment3D seg(ptDebut, ptReflex);

        rr = seg.longueur();

        if (fromSource && pSrc)   // Si on part d'une source, on tient compte de la directivite de celle-ci
        {
            EtapeCourante._Absorption = pSrc->lwApparenteSrcDest(seg * pSrcGeoNode->getMatrix().getInvert(), *_pAtmo, _expGeo); // Directivite de la source
        }
        else
        {
            EtapeCourante._Absorption = _absoNulle;
        }

        Etapes.push_back(EtapeCourante);

        //              Etape Apres reflexion
        EtapeCourante._pt = ptReflex;

        TYTerrain* pTerrain = NULL;

        seg = OSegment3D(ptReflex, ptFin);// Segment Point de reflexion->Point de reception
        rr = rr + seg.longueur(); // Longueur parcourue sur le trajet reflechi

        // 3 cas :
        if (_useSol)
        {
            double angleA = OVector3D(ptReflex, segPente._ptA).angle(OVector3D(ptReflex, ptDebut));// Angle d'incidence

            pTerrain = _pTopographie->terrainAt(ptReflex);
            if (!pTerrain) { pTerrain = _pTopographie->getDefTerrain(); }
            if (pTerrain) // Une zone de terrain a ete definie
            {
                pSol = pTerrain->getSol();
                if (pSol) { EtapeCourante._Absorption = pSol->abso(angleA, rr, *_pAtmo); }
            }
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
        OSegment3D seg(ptDebut, ptReflex);
        rr = seg.longueur();

        EtapeCourante._Absorption = _absoNulle;

        Etapes.push_back(EtapeCourante);

        //              Etape Apres reflexion
        EtapeCourante._pt = ptReflex;

        TYTerrain* pTerrain = NULL;

        seg = OSegment3D(ptReflex, ptFin);// Segment Point de reflexion->Point de reception
        rr = rr + seg.longueur(); // Longueur parcourue sur le trajet reflechi

        // 3 cas :
        if (_useSol)
        {
            double angleA = M_PI;
            if (toRecepteur)
            {
                angleA = OVector3D(ptReflex, segPente._ptA).angle(OVector3D(ptReflex, ptDebut));// Angle d'incidence
            }

            pTerrain = _pTopographie->terrainAt(ptReflex);
            if (!pTerrain) { pTerrain = _pTopographie->getDefTerrain(); }
            if (pTerrain) // Une zone de terrain a ete definie
            {
                pSol = pTerrain->getSol();
                if (pSol) { EtapeCourante._Absorption = pSol->abso(angleA, rr, *_pAtmo); }
            }
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

void TYAcousticModel::computeCheminReflexion(const TYSIntersection* tabIntersect, const OSegment3D& rayon, const TYSourcePonctuelleGeoNode* pSrcGeoNode, TYTabChemin& TabChemins, double distance) const
{
    if (!_useReflex) { return; }

    TYSourcePonctuelle* pSrc = NULL;
    if (pSrcGeoNode) { pSrc = dynamic_cast<TYSourcePonctuelle*>(pSrcGeoNode->getElement()); }
    //  int expGeo = _pCalcul->getExpansGeo();

    OSegment3D segInter;
    OSegment3D rayonTmp;
    OPoint3D ptSym;
    OSpectre SpectreAbso;

    // XBH: remontee des declarations de variables hors de la boucle
    TYAcousticSurfaceGeoNode* pSurfaceGeoNode = NULL;
    TYAcousticSurface* pSurface = NULL;

    OSegment3D seg; // Segment source image->recepteur
    OSegment3D segMontant; // Segment source-> point de reflexion
    OSegment3D segDescendant; // Segment point de reflexion->recepteur

    OPoint3D pt; // Point d'intersection

    unsigned int nbFaces = static_cast<uint32>(_solver.getTabPolygonSize());

    // Pour chaque face test de la reflexion
    for (unsigned int i = 0; i < nbFaces; i++)
    {
        const TYStructSurfIntersect& SI = _solver.getTabPolygon()[i];

        // Si la face ne peut interagir on passe a la suivante
        if ((!SI.isInfra) || !(tabIntersect[i].bIntersect[1]) || (SI.tabPoint.size() == 0)) { continue; }

        segInter = tabIntersect[i].segInter[1];

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
            int j = 0;

            // Si on traverse un autre ecran, qui peut etre de la topo, le chemin de reflexion n'est pas pris en compte
            while ((j < nbFaces) && (!intersect))
            {
                if ((j == i) || !(tabIntersect[j].bIntersect[1]))
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
                rayonTmp = rayon * SI.matInv;

                pSurfaceGeoNode = SI.pSurfGeoNode;
                if (pSurfaceGeoNode) { pSurface = dynamic_cast<TYAcousticSurface*>(pSurfaceGeoNode->getElement()); }

                if (pSurface == NULL) { continue; } // Si la face n'est pas d'infrastructure on passe a la suivante

                TYMateriauConstruction* matFace = getMateriauFace(pSurface, rayonTmp);
                if (matFace)
                {
                    SpectreAbso = matFace->getSpectreAbsoEyring();
                    SpectreAbso = SpectreAbso.mult(-1.0).sum(1.0); // coeff reflex = 1 - coefAbso
                }

                TYAcousticCylinder* pCyl = NULL;
                if (pSurfaceGeoNode) { pCyl = dynamic_cast<TYAcousticCylinder*>(pSurfaceGeoNode->getParent()); }

                //
                // Reflexion sur un cylindre, d'apres ISO9613-2
                //
                if (pCyl)
                {
                    OPoint3D centre(pCyl->getCenter());
                    OVector3D SC(rayonTmp._ptA, centre);
                    OVector3D CR(centre, rayonTmp._ptB);
                    double diametre = pCyl->getDiameter();
                    double dSC = SC.norme(); // Norme du vecteur
                    double phi = SC.angle(CR);

                    SpectreAbso = SpectreAbso.mult(diametre * sin(phi / 2) / (2 * dSC));
                }

                // Premiere etape : du debut du rayon au point de reflexion sur la face
                TYTabEtape tabEtapes;

                double rr = segMontant.longueur() + segDescendant.longueur();

                TYEtape Etape;
                Etape._pt = rayon._ptA;
                if (pSrc) { Etape._Absorption = pSrc->lwApparenteSrcDest(segMontant * pSrcGeoNode->getMatrix().getInvert(), *_pAtmo, _expGeo); }

                tabEtapes.push_back(Etape);

                // Deuxieme etape : du point de reflexion a la fin du rayon
                Etape._pt = segDescendant._ptA;
                Etape._Absorption = SpectreAbso;

                tabEtapes.push_back(Etape);

                TYChemin Chemin;
                Chemin.setType(CHEMIN_REFLEX);
                Chemin.setLongueur(rr);
                Chemin.setDistance(distance);
                Chemin.calcAttenuation(tabEtapes, *_pAtmo);

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
    TYSourcePonctuelleGeoNode* pSrcGeoNode = trajet.getSourcePonctuelle();
    TYSourcePonctuelle* pSrc = NULL;
    if (pSrcGeoNode) { pSrc = dynamic_cast<TYSourcePonctuelle*>(pSrcGeoNode->getElement()); }


    const double PIM4 = 4.0 * M_PI;


    double rD2 = trajet.getDistance();


    rD2 = rD2 * rD2 ;


    double divGeom = _pAtmo->getImpedanceSpecifique() / (PIM4 * rD2);




    OSpectre& SLp = trajet.getSpectre();


    // W.rho.c / (4pi*rdi申)

    SLp = (*pSrc->getSpectre()).mult(divGeom);



    //  (W.rho.c/4.pi.Rdi申)*Attenuations du trajet
    if (_interference)
    {
        SLp = SLp.mult(trajet.getPInterference(*_pAtmo));
    }
    else
    {
        SLp = SLp.mult(trajet.getPEnergetique(*_pAtmo));
    }
    SLp.setType(SPECTRE_TYPE_LP); //Le spectre au point est bien un spectre de pression !

    return true;
}

TYMateriauConstruction* TYAcousticModel::getMateriauFace(TYAcousticSurface* pSurf, const OSegment3D& seg) const
{
    TYMateriauConstruction* mat = NULL;
    TYMurElement* pMurElem = dynamic_cast<TYMurElement*>(pSurf);

    if (pMurElem) // Cas d'une face de bi申timent ou d'un ecran
    {
        TYMur* pMur = dynamic_cast<TYMur*>(pMurElem->getParent());

        OVector3D normale = pSurf->normal();
        OVector3D rayon(seg._ptA, seg._ptB);

        if (normale.scalar(rayon) < 0.0) // Rayon incident de direction oppose a la normale = face exterieure
        {
            mat = pMur->getParoi()->getMatFace2();
        }
        else // Materiau de la face interieure
        {
            mat = pMur->getParoi()->getMatFace1();
        }
    }
    else // Cas d'une machine ou d'un acousticVolume quelconque
    {
        TYElement* pParentSurface = pSurf->getParent();
        TYAcousticVolume* pVolParentSurface = NULL;
        //        while (pParentSurface && !(pParentSurface->inherits("TYAcousticVolume")))
        while (pParentSurface && !(dynamic_cast<TYAcousticVolume*>(pParentSurface)))
        {
            pParentSurface = pParentSurface->getParent();
        }

        pVolParentSurface = dynamic_cast<TYAcousticVolume*>(pParentSurface);
        mat = pVolParentSurface->getMateriau();
    }

    return mat;
}



