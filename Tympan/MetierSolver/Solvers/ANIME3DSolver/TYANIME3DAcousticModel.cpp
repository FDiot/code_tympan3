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

#include "Tympan/MetierSolver/CommonTools/OBox2.h"

#include "Tympan/MetierSolver/DataManagerCore/TYAcousticModelInterface.h"
#include "Tympan/MetierSolver/DataManagerCore/TYSolverInterface.h"

#include "Tympan/MetierSolver/DataManagerMetier/Commun/TYCalcul.h"

#include "Tympan/MetierSolver/CommonTools/acoustic_path.h"

#include "Tympan/MetierSolver/AcousticRaytracer/global.h"

#include "TYANIME3DSolver.h"
#include "TYANIME3DAcousticModel.h"


TYANIME3DAcousticModel::TYANIME3DAcousticModel(TYCalcul& calcul, const TYSiteNode& site,
                                               tab_acoustic_path& tabRayons, TYStructSurfIntersect* tabStruct,
                                               TYTabSourcePonctuelleGeoNode& tabSources, TYTabPointCalculGeoNode& tabRecepteurs) :
    _calcul(calcul),
    _site(site),
    _tabSurfIntersect(tabStruct),
    _tabTYRays(tabRayons),
    _tabSources(tabSources),
    _tabRecepteurs(tabRecepteurs)
{
    _nbRays = _tabTYRays.size();

    OSpectreComplex s1 = OSpectreComplex(OSpectre(1.0));
    OSpectreComplex s0 = OSpectreComplex(OSpectre(0.0));

    _pressAcoustEff = OTabSpectreComplex(_nbRays, s0);
    _absAtm  = OTabSpectreComplex(_nbRays, s1);
    _absRefl = OTabSpectreComplex(_nbRays, s1);
    _absDiff = OTabSpectreComplex(_nbRays, s1);

    _atmos = *(calcul.getAtmosphere());

    _topo = const_cast<TYSiteNode&>(site).getTopographie().getRealPointer();

    // _alti parameter initialized
    _alti = _topo->getAltimetrie().getRealPointer();

    _listeTerrains = _topo->getListTerrain();
    _listeTriangles = (*_topo->getAltimetrie()).getListFaces();

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
    double angle = 0.0, rd = 0.0, rr = 0.0; // incidence angle of acoustic wave, lenght for events computation
    int idFace = 0, rayNbr = 0, reflIndice = 0, nbFacesFresnel = 0;

    acoustic_path* ray = NULL;
    TYSol* pSol = NULL;

    OPoint3D Prefl, Pprec, Psuiv;    //pt de reflexion, pt precedent et suivant

    OTabDouble tabPondFresnel;      // tableau des ponderations de Fresnel
    TYTabPoint3D triangleCentre;    // Contains all triangles centres

    OSpectreComplex spectreAbs;
    OSpectreComplex zero = OSpectreComplex(OSpectre(1.0));
    OSpectreComplex one = OSpectreComplex(OSpectre(1.0));
    OSpectreComplex prod;
    OSpectreComplex sum;
    OSpectreComplex prod1;
    OSpectreComplex sum1;
    OSpectreComplex pond;

    for (int i = 0; i < _nbRays; i++) // boucle sur les rayons
    {
        ray = _tabTYRays[i];
        std::vector<int> tabRefl = ray->getIndexOfEvents(TYREFLEXION | TYREFLEXIONSOL);

        rayNbr = i;

        // Initialisation des spectres
        prod = one;
        sum = zero;
        sum1 = zero;
        pond = zero;

        pSol = NULL;

        for (int j = 0; j < tabRefl.size(); j++)
        {
            reflIndice = tabRefl[j];

            Prefl = ray->getEvents().at(reflIndice)->pos;
            Pprec = ray->getEvents().at(reflIndice)->previous->pos;
            Psuiv = ray->getEvents().at(reflIndice)->next->pos;
            idFace = ray->getEvents().at(reflIndice)->idFace1;
            angle = ray->getEvents().at(reflIndice)->angle;

            // Longueur du trajet reflechi
            rd = ray->getEvents().at(reflIndice)->distPrevNext;
            // Longueur du trajet direct
            rr = Pprec.distFrom(Prefl) + Prefl.distFrom(Psuiv);

            std::cout << "Position de la reflexion : X = " << Prefl._x << " Y = " << Prefl._y << " Z = " << Prefl._z << std::endl;
            std::cout << "Angle d'incidence du rayon = " << angle << std::endl;
            std::cout << "Longueur du rayon direct = " << rd << std::endl;
            std::cout << "Longueur du rayon reflechi = " << rr << std::endl;

            // Initialisation des spectres
            spectreAbs = OSpectreComplex(OSpectre(0.0));


            if (_useFresnelArea) // Avec ponderation de Fresnel
            {
                if (ray->getEvents().at(reflIndice)->type == TYREFLEXIONSOL)
                {
                    std::cout << "We start using Fresnel Area on the ground" << std::endl;

                    triangleCentre.clear();
                    tabPondFresnel.clear();
                    tabPondFresnel = ComputeFresnelWeighting(angle, Pprec, Prefl, Psuiv, rayNbr, reflIndice, triangleCentre);  // calcul des ponderations de Frenel
                    nbFacesFresnel = tabPondFresnel.size();  // nbr de triangles dans le zone de Fresnel

                    std::cout << "il y a N ponderations : " << nbFacesFresnel << std::endl;

                    sum = zero;

                    for (int k = 0; k < nbFacesFresnel; k++)
                    {
                        // boucle sur les faces = intersection plan de l'objet intersecte / ellipsoide de Fresnel
                        pSol = _topo->terrainAt(triangleCentre[k])->getSol();
                        pSol->calculNombreDOnde(_atmos);

                        std::cout << "sol n : " << k << "resistivite = " << pSol->getResistivite() << std::endl;

                        spectreAbs = pSol->abso(angle, rr, _atmos);
                        // TO DO : S'assurer que la somme des pondrations de Fresnel gale 1
                        pond = spectreAbs * tabPondFresnel[k];
                        sum = sum + pond; // calcul du coeff de reflexion moy en ponderant avec les materiaux
                    }

                    prod = prod * sum * (rd / rr);
                }
                else // Reflexion sur une construction
                {
                    std::cout << "We start using Fresnel Area on a building" << std::endl;

                    triangleCentre.clear();
                    tabPondFresnel = ComputeFresnelWeighting(angle, Pprec, Prefl, Psuiv, rayNbr, reflIndice, triangleCentre);  // calcul des ponderations de Frenel
                    nbFacesFresnel = tabPondFresnel.size();  // nbr de triangles dans le zone de Fresnel

                    std::cout << "il y a N ponderations : " << nbFacesFresnel << std::endl;

                    sum = zero;

                    for (int k = 0; k < nbFacesFresnel; k++)
                    {
                        // boucle sur les faces = intersection plan de l'objet intersecte / ellipsoide de Fresnel
                        pond = spectreAbs * tabPondFresnel[k];
                        sum = sum + pond;   // calcul du coeff de reflexion moy en ponderant avec les materiaux
                    }
                    prod = prod * sum;
                }

                std::cout << "End of Fresnel Area" << std::endl;
            }
            else // not use Fresnel Area
            {
                // Cas particulier d'une reflexion sur le sol
                std::cout << "We are not using Fresnel Area on the ground" << std::endl;
                if (ray->getEvents().at(reflIndice)->type == TYREFLEXIONSOL)
                {
                    pSol = _topo->terrainAt(Prefl)->getSol();
                    std::cout << "Impedance sol = " << pSol->getResistivite() << std::endl;
                    pSol->calculNombreDOnde(_atmos);
                    spectreAbs = pSol->abso(angle, rr, _atmos);
                    spectreAbs = spectreAbs * (rd / rr);
                    std::cout << "A 500 Hz, Q = " << spectreAbs.getModule().getValueReal(500) << std::endl;
                }
                else
                {
                    spectreAbs = _tabSurfIntersect[idFace].spectreAbso;  // Recuperation du spectre d'absorption
                }
                // ATTENTION ! : Il semble qu'on ne tienne compte que d'une seule reflexion : La derniere.
                prod = spectreAbs;
            }
        }

        _absRefl[i] = prod;
    }
}

void TYANIME3DAcousticModel::ComputeAbsDiff()
{
    // Delta = diffrence de marche
    // precDiff = distance entre l'evenement precedent et la diffraction
    // diffEnd = distance entre la diffraction et l'evenement pertinent suivant
    // precEnd = distance entre l'evenement precedent et l'evenement pertinent suivant
    // nbF = spectre du nbr de Fresnel de chq diffration
    // absArrete = spectre complexe d'absorption sur chaque arrete
    // prod = spectre produit des absorptions sur chaque arrete
    // kDelta = spectre intermediaire de calcul
    // mod = spectre module du nombre complexe absArrete
    // signe = signe de la diffraction
    // diffIdx = index de l'evenement diffraction courant
    // vDiffPrec = vecteur diffraction / evenement precedent
    // vDiffSuiv = vecteur diffraction evenement suivant
    // n1 = normale  la premire face du diedre diffractant
    // n2 = normale  la seconde face du diedre diffractant
    // normal = n1 + n2



    double delta = 0.0, precDiff = 0.0, diffEnd = 0.0, precEnd = 0.0;
    OSpectre nbF;                // nbr de Fresnel de chq diffration
    OSpectreComplex absArrete;   // absorption sur chaque arrete
    OSpectreComplex prod = OSpectre(1.0); // produit des absorptions sur chaque arrete
    OSpectre kDelta;             // intermediaire de calcul
    OSpectre mod;                // module du nombre complexe absArrete
    int signe = 1, diffIdx = 0;

    OVector3D vDiffPrec, vDiffSuiv, n1, n2, normal;

    acoustic_path* currentRay = NULL;

    for (int i = 0; i < _nbRays; i++) // boucle sur les rayons
    {
        currentRay = _tabTYRays[i];

        prod = OSpectre(1.0);

        std::vector<int> tabDiff = currentRay->getIndexOfEvents(TYDIFFRACTION); // gets a vector where diff occur

        for (int j = 0; j < tabDiff.size(); j++)
        {
            diffIdx = tabDiff[j]; // Index de l'evenement diffraction courant
            acoustic_event* currentEv = currentRay->getEvents().at(diffIdx); // Evenement courant

            precDiff = currentEv->previous->distNextEvent; // Distance de l'venement prcedent  la diffraction
            diffEnd = currentEv->distEndEvent; // de la diffraction  l'vnement pertinent suivant (recepteur ou reflexion)
            precEnd = currentEv->previous->distEndEvent; // Chemin sans diffraction

            vDiffPrec = OVector3D(currentEv->pos, currentEv->previous->pos);
            vDiffSuiv = OVector3D(currentEv->pos, currentEv->next->pos);
            n1 = _tabSurfIntersect[ currentEv->idFace1 ].normal; // normale de la 1ere face
            n2 = _tabSurfIntersect[ currentEv->idFace2 ].normal; // normale de la 2e face
            normal = n1 + n2; // somme des normales

            if ((vDiffPrec.scalar(normal) > 0.0) && (vDiffSuiv.scalar(normal) > 0.0))
            {
                signe = -1;
            }
            else
            {
                signe = 1;
            }

            delta = signe * (precDiff + diffEnd - precEnd);
            kDelta = _K * delta;
            nbF = _lambda.invMult(2.0 * delta);   // 2 * delta / lambda

            if (true) // DTn 20131220 pour forcer l'operation( delta > ( _lambda.div(20) ).valMax() )
            {
                mod = (((nbF * 20.0 + 3.0)).sqrt()).inv(); // 1 / sqrt(20 * nbF + 3)
            }
            else
            {
                mod = 1;
            }

            absArrete = OSpectreComplex(mod, kDelta);// (1 / sqrt(20 * nbF + 3)) exp(j * k * delta)
            prod = prod * absArrete;
        }

        _absDiff[i] = prod;
    }
}

// calcul de la zone de Fresnel pour une diffraction donnee - Approximation : zone = boite englobante de l'ellipsoide de Fresnel
OBox2 TYANIME3DAcousticModel::ComputeFresnelArea(double angle, OPoint3D Pprec, OPoint3D Prefl, OPoint3D Psuiv, int rayNbr, int reflIndice)
{
    OBox2 fresnelArea;  // boundingBox de l'ellispsoide de Fresnel

    // TO DO : Why don't we pass directly the current ray instead of accessing it by its indice in the ray vector ?
    Prefl = _tabTYRays[rayNbr]->getEvents().at(reflIndice)->pos;
    Pprec = _tabTYRays[rayNbr]->getEvents().at(reflIndice)->previous->pos;
    Psuiv = _tabTYRays[rayNbr]->getEvents().at(reflIndice)->next->pos;

    std::cout << "Coordonnees du point de reflexion " << std::endl;
    std::cout << Prefl._x << " " << Prefl._y << " " << Prefl._z << std::endl;

    double distPrefPsuiv = _tabTYRays[rayNbr]->getEvents().at(reflIndice)->distNextEvent;
    double dr = _tabTYRays[rayNbr]->getEvents().at(reflIndice)->previous->distNextEvent + distPrefPsuiv;
    double dd = _tabTYRays[rayNbr]->getEvents().at(reflIndice)->distPrevNext;

    // Calcul de Fc ne prenant en compte que la diffrence de marche
    OSpectre f = OSpectre::getOSpectreFreqExact(); //frequence
    OSpectre fc = computeFc(dd, dr);

    /*
    MIS EN COMMENTAIRE POUR VALIDATION ULTERIEURE

        // Calcul de F lambda officiel (equations 10  15 de H-T63-2010-01193-FR)
        // parametre de l'ellipsoide de Fresnel F_{\lambda} via la formule definie dans la publie de D. van Maercke et J. Defrance (CSTB-2007)
        // Version initiale du calcul de fc prenant en compte le sol rel

        // Le spectre Q sert  rcuprer la phase du trajet rflechi

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

        const OSpectre F = (OSpectre(1.0)-((f*f).div(fc*fc)).exp(1.0))*32.0;

        OSpectre A = (fc*fc)*-1;
        OSpectre B = f*f;
        OSpectre C = B.div(A);
        const OSpectre F = (OSpectre(1.0)-(C.exp()))*32;
    */

    // F fixe a 1 pour eviter une boite trop grande a basse freq
    const OSpectre F = OSpectre(1.0);

    // lF fixe a lambda/2
    OSpectre lF = _lambda.div(2.0) * F;

    // 1/ boite dans le repere (Ox,Oy,Oz)
    // Construction de la bb de l'ellipsoide de Fresnel

    // S' depends on the point where the reflection occurs
    int reflFace =  _tabTYRays[rayNbr]->getEvents().at(reflIndice)->idFace1;
#ifdef _DEBUG
	TYElement* pElm = _tabSurfIntersect[reflFace].pSurfGeoNode->getElement();
#endif
    TYAcousticSurface* face = TYAcousticSurface::safeDownCast(_tabSurfIntersect[reflFace].pSurfGeoNode->getElement());
    OPlan P = face->getPlan();
    OPoint3D SIm = P.symPtPlan(Pprec); // Point image du point prcedent la reflexion
    double dSImR = SIm.distFrom(Psuiv); // Distance de la source image au point suivant la reflexion

    std::cout << "Coordonnees de la source image " << std::endl;
    std::cout << SIm._x << " " << SIm._y << " " << SIm._z << std::endl;


    // On fixe une frequence de travail
    // TO DO : mettre freq en paramtre utilisateur
    float freq = 500.0f;
    double L = 2 * lF.getValueReal(freq) + dSImR;
    double l = sqrt(lF.getValueReal(freq) * (lF.getValueReal(freq) + 2.*dSImR));
    double h = l; // hauteur = largeur (ajout pour la lisibilit du code)


    std::cout << "L = " << L << " l = " << l << std::endl;

    // Boite de Fresnel placee a l'origine du repere
    //    OBox box = OBox( OCoord3D( 0, 0, 0 ), OCoord3D( L, l, h ) );
    fresnelArea = OBox2(L, l, h); // DTn uses new constructor of a box centered on 0, 0, 0

    // Definition de la nouvelle position du centre de la boite // Translation au pt O milieu du segment [SIm R]
    OPoint3D O(0.5 * (SIm._x + Psuiv._x), 0.5 * (SIm._y + Psuiv._y), 0.5 * (SIm._z + Psuiv._z));

    // DTn : define a new vector to change box orientation
    OVector3D vecO(O, Psuiv);

    // DTn : Move and rotate box
    fresnelArea.moveAndRotate(O, vecO);

    return fresnelArea;

    /*
        DTn commented to use new OBox manipulator I defined (20131218)
        // Deplace le centre de la boite au centre du segment S'P (P = point suivant la rflexion)
        const OVector3D vt( ( O._x - (0.5*L) ), ( O._y - ( 0.5*l ) ), ( O._z - ( 0.5*h ) ) );
        fresnelArea.Translate( vt );

    // TO DO : S'assurer que la rotation s'effectue dans le bon repre
        return fresnelArea.boxRotation( O, Psuiv ); // Voir la dfinition de boxRotation : devrai tre fersnelArea.boxRotation(...)
    */
}

// TO DO : Vrifier que c'est la mthode mise en commentaire dans computeFresnelArea
OSpectre TYANIME3DAcousticModel::computeFc(const double& dd, const double& dr)
{
    OSpectre FMin, FMax, fc;
    OSpectre PhiN = _atmos.getKAcoust() * (dr - dd);
    OSpectre Phi0Min(M_PI / 2);
    OSpectre Phi0Max(M_PI);
    OSpectre freq = OSpectre::getOSpectreFreqExact();

    for (unsigned int i = 0; i < TY_SPECTRE_DEFAULT_NB_ELMT - 1; i++)
    {
        double fn = freq.getTabValReel()[i];
        double fnp1 = freq.getTabValReel()[i + 1];
        double phin = PhiN.getTabValReel()[i];
        double phinp1 = PhiN.getTabValReel()[i + 1];
        double phi0 = Phi0Min.getTabValReel()[i];
        FMin.getTabValReel()[i] = fn + ((phi0 - phin) / (phinp1 - phin)) * (fnp1 - fn);
        phi0 = Phi0Max.getTabValReel()[i];
        FMax.getTabValReel()[i] = fn + ((phi0 - phin) / (phinp1 - phin)) * (fnp1 - fn);
    }

    fc = (FMin * FMax).sqrt();

    // Last value is the same as the previous one
    fc.getTabValReel()[TY_SPECTRE_DEFAULT_NB_ELMT - 1] = fc.getTabValReel()[TY_SPECTRE_DEFAULT_NB_ELMT - 2];

    return fc;
}

OTabDouble TYANIME3DAcousticModel::ComputeFresnelWeighting(double angle, OPoint3D Pprec, OPoint3D Prefl, OPoint3D Psuiv, int rayNbr, int reflIndice, TYTabPoint3D& triangleCentre)
{
    OTabDouble tabPond;             // tableau des ponderation de Fresnel
    OTabDouble tabSurface;          // tableau des surfaces des triangles de la zone de Fresnel
    double surfaceTot = 0.0;        // somme des surfaces des triangles de la zone de Fresnel
    const double delaunay = 1e-6;   // Parameter needed to compute triangulation

    std::cout << "Dans ComputeFresnelWeighting :" << std::endl;
    // And bb was born
    OBox2 fresnelArea = ComputeFresnelArea(angle, Pprec, Prefl, Psuiv, rayNbr, reflIndice);

    std::cout << "Coordonnees de l'origine de la boite " << std::endl;
    std::cout << "Center = " << fresnelArea._center._x << " " << fresnelArea._center._y << " " << fresnelArea._center._z << std::endl;

    // XXX Altimetry refactoring impacts here.



    std::cout << "Coordonnees des 8 sommets de la boite " << std::endl;
    std::cout << "A = " << fresnelArea._A._x << " " << fresnelArea._A._y << " " << fresnelArea._A._z << std::endl;
    std::cout << "B = " << fresnelArea._B._x << " " << fresnelArea._B._y << " " << fresnelArea._B._z << std::endl;
    std::cout << "C = " << fresnelArea._C._x << " " << fresnelArea._C._y << " " << fresnelArea._C._z << std::endl;
    std::cout << "D = " << fresnelArea._D._x << " " << fresnelArea._D._y << " " << fresnelArea._D._z << std::endl;


    std::cout << "E = " << fresnelArea._E._x << " " << fresnelArea._E._y << " " << fresnelArea._E._z << std::endl;
    std::cout << "F = " << fresnelArea._F._x << " " << fresnelArea._F._y << " " << fresnelArea._F._z << std::endl;
    std::cout << "G = " << fresnelArea._G._x << " " << fresnelArea._G._y << " " << fresnelArea._G._z << std::endl;
    std::cout << "H = " << fresnelArea._H._x << " " << fresnelArea._H._y << " " << fresnelArea._H._z << std::endl;


    // Remplacer par l'intersection des segments FG, EH, AB, CD
    // avec le plan de la face sur laquelle se trouve le point de rflexion.
    OSegment3D AB(fresnelArea._A, fresnelArea._B);
    OSegment3D CD(fresnelArea._C, fresnelArea._D);
    OSegment3D FG(fresnelArea._F, fresnelArea._G);
    OSegment3D EH(fresnelArea._E, fresnelArea._H);

    LPTYPolygon faceRefl = (*_topo->getAltimetrie()).getFaceUnder(Prefl);

    // Calcul de l'intersection des segments avec le plan de la face de reflexion
    OPlan refPlan(faceRefl->getPoint(0), faceRefl->getPoint(1), faceRefl->getPoint(2)); //->getPlan();

    OPoint3D eProj, fProj, gProj, hProj;
    refPlan.intersectsSegment(AB._ptA, AB._ptB, eProj);
    refPlan.intersectsSegment(CD._ptA, CD._ptB, fProj);
    refPlan.intersectsSegment(EH._ptA, EH._ptB, gProj);
    refPlan.intersectsSegment(FG._ptA, FG._ptB, hProj);


    // ANCIENNE VERSION (PROJECTION DES POINT DU "HAUT" DE LA BOITE
    // fE/fF/fG/fH are the faces under the box corners E/F/G/H
    //LPTYPolygon fE = (*_topo.getAltimetrie()).getFaceUnder(fresnelArea._E);
    //LPTYPolygon fF = (*_topo.getAltimetrie()).getFaceUnder(fresnelArea._F);
    //LPTYPolygon fG = (*_topo.getAltimetrie()).getFaceUnder(fresnelArea._G);
    //LPTYPolygon fH = (*_topo.getAltimetrie()).getFaceUnder(fresnelArea._H);

    // Defines 4 planes from the 4 surfaces
    //OPlan ePlane = fE->plan();
    //OPlan fPlane = fF->plan();
    //OPlan gPlane = fG->plan();
    //OPlan hPlane = fH->plan();

    // These are the four projections onto the planes defined previously
    // This is a way of taking into account the ground altitude
    //OPoint3D eProj = ePlane.projPtPlan(fresnelArea._E);
    //OPoint3D fProj = fPlane.projPtPlan(fresnelArea._F);
    //OPoint3D gProj = gPlane.projPtPlan(fresnelArea._G);
    //OPoint3D hProj = hPlane.projPtPlan(fresnelArea._H);
    // FIN ANCIENNE VERSION

    std::cout << "Coordonnees des 4 projetes des sommets hauts de la boite " << std::endl;
    std::cout << "eProj = " << eProj._x << " " << eProj._y << " " << eProj._z << std::endl;
    std::cout << "fProj = " << fProj._x << " " << fProj._y << " " << fProj._z << std::endl;
    std::cout << "gProj = " << gProj._x << " " << gProj._y << " " << gProj._z << std::endl;
    std::cout << "hProj = " << hProj._x << " " << hProj._y << " " << hProj._z << std::endl;

    // We get back the points inside the zone
    TYTabPoint pointsInside;
    _alti->getPointsInBox(eProj, fProj, gProj, hProj, pointsInside);

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
    unsigned long nbTriangles = _oTriangles.size();
    tabPond.reserve(nbTriangles);
    tabSurface.reserve(nbTriangles);

    // For each triangle, we get the centre and the surface
    for (int k = 0; k < nbTriangles; k++)
    {
        OTriangle triangleK = _oTriangles.at(k);
        tabSurface.push_back(triangleK.getSurface());
        surfaceTot += tabSurface.at(k);
        triangleCentre.push_back(triangleK.getCentre());
    }
    std::cout << "Surface totale = " << surfaceTot << std::endl;

    // This part computes a ponderation depending on the triangle surface
    for (int k = 0; k < nbTriangles; k++)
    {
        tabPond.push_back(tabSurface.at(k) / surfaceTot);
    }

    std::cout << "End of ComputeFresnelWeighting " << std::endl;

    return tabPond;
}

std::vector<OTriangle> TYANIME3DAcousticModel::ComputeTriangulation(const TYTabPoint& points, const double& delaunay)
{

    // This function returns a vector of triangles created within the bounding box
    unsigned int i = 0;  // Indices needed
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
    for (int m = 0; m < nbTriangles; m++)
    {
        listeTriangle.push_back(triangles[m]);  // We get the triangle
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
    OSpectre phase; // phase du nombre complexe _pressAcoustEff[i] pour chq i
    OSpectre mod;   // module du nombre complexe _pressAcoustEff[i] pour chq i
    const double rhoc = _atmos.getImpedanceSpecifique(); //400.0;   // kg/m^2/s
    double c1;      // constante du calcul
    OPoint3D S, P0; // pt de la source et pt du 1er evenement
    OSegment3D seg; // premier segment du rayon
    OSpectre directivite, wSource; // fonction de directivite et spectre de puissance de la source
    TYSourcePonctuelle* source = NULL; // source
    OSpectreComplex prodAbs; // produit des differentes absorptions
    double totalRayLength; // Computes the total ray length including reflections only (diffractions are not included)

    for (int i = 0; i < _nbRays; i++) // boucle sur les rayons
    {
        totalRayLength = 0.0; // Computes the total ray length including reflections only (diffractions are not included)



        // TO DO : Should find source in list known by solver from it's index given by the ray

        //source = _tabTYRays[i]->getSource();

        //--------------------------------------

        totalRayLength = _tabTYRays[i]->getLength();

        c1 = 4.0 * M_PI * totalRayLength * totalRayLength;

        S  = _tabTYRays[i]->getEvents().at(0)->pos;
        P0 = _tabTYRays[i]->getEvents().at(1)->pos;
        seg = OSegment3D(S, P0);

        directivite = source->lwApparenteSrcDest(seg, _atmos);
        wSource = (*source->getSpectre()).toGPhy();

        prodAbs = _absAtm[i] * _absRefl[i] * _absDiff[i];

        // module = dir * W * rhoC / (4 * PI * R) * produit (reflex, diffraction, atmos)
        mod = ((directivite * wSource * rhoc) * (1. / c1)).sqrt() * prodAbs.getModule();
        // phase = exp(j K *L)
        phase = _K.mult(_tabTYRays[i]->getLength()) + prodAbs.getPhase();

        _pressAcoustEff[i] = OSpectreComplex(mod, phase);
    }
}

OTab2DSpectreComplex TYANIME3DAcousticModel::ComputePressionAcoustTotalLevel()
{
    OSpectre C;         // facteur de coherence et son carre
    OSpectre un = OSpectre(1.0);
    OSpectreComplex zero = OSpectreComplex(0.0, 0.0);
    OSpectreComplex sum1;   //somme partielle
    OSpectreComplex sum3;
    OSpectre sum2;          //somme partielle
    OSpectre mod;           // module et module au carre
    const OSpectre K2 = _K * _K;            // nombre d'onde au carre

    double incerRel = globalAnime3DSigma;  // incertitude relative sur la taille du rayon au carree

    // constante pour la definition du facteur de coherence
    double cst = (pow(2., 1. / 6.) - pow(2., -1. / 6.)) * (pow(2., 1. / 6.) - pow(2., -1. / 6.)) / 3.0 + incerRel * incerRel;
    double totalRayLength;

    const int nbSources    = _tabSources.size();           // nbr de sources de la scene
    const int nbRecepteurs = _tabRecepteurs.size();         // nbr de recepteurs de la scene

    TYSourcePonctuelle* source = NULL;
    TYPointCalcul* recept = NULL;

    OTab2DSpectreComplex tabPressionAcoust(nbSources);

    for (int i = 0; i < nbSources; i++)
    {
        tabPressionAcoust[i].resize(nbRecepteurs);
    }

    for (int i = 0; i < nbSources; i++) // boucle sur les sources
    {
        for (int j = 0; j < nbRecepteurs; j++) // boucle sur les recepteurs
        {
            sum1 = zero;
            sum2 = zero;
            sum3 = zero;

            for (int k = 0; k < _nbRays; k++) // boucle sur les rayons allant de la source au recepteur
            {

        // TO DO : Should find source in list known by solver from it's index given by the ray

               // source = _tabTYRays[k]->getSource();

        // ------------------------------------------------

        // TO DO : Should find receptor in list known by solver from it's index given by the ray

                // recept = _tabTYRays[k]->getRecepteur();

        // -------------------------------------------------

                // test si la source est celle en cours idem pour recepteur
 //               if (source == (TYSourcePonctuelle*)(_tabSources[i]->getElement()) && recept == (TYPointCalcul*)(_tabRecepteurs[j]->getElement()))
                {
                    totalRayLength = _tabTYRays[k]->getLength();
                    mod = (_pressAcoustEff[k]).getModule();

                    if (((int) globalAnime3DForceC) == 0)
                    {
                        C = 0.0; // = defaultSolver "energetique"
                    }
                    else if (((int) globalAnime3DForceC) == 1)
                    {
                        C = 1.0; // = defaultSolver "interferences"
                    }
                    else
                    {
                        C = (K2 * totalRayLength * totalRayLength * (-1) * cst).exp();
                    }

                    sum3 = _pressAcoustEff[k] * C;
                    sum1 = sum1 + sum3;
                    sum2 = sum2 + mod * mod * (un - C * C);
                }
            }

            // Be carefull sum of p!= p of sum
            sum1 = sum1.getModule() * sum1.getModule();
            tabPressionAcoust[i][j] = sum1 + sum2;
        }
    }

    return tabPressionAcoust;
}

OTab2DSpectreComplex TYANIME3DAcousticModel::ComputeAcousticModel()
{

    ComputeAbsAtm();// Absorption atmosphrique pour tous les rayons

    ComputeAbsRefl(); // Absorption par reflexion pour tous les rayons

    ComputeAbsDiff(); // Attnuation par diffraction pour tous les rayons

    ComputePressionAcoustEff(); // Pression acoustique efficace pour tous les rayons

    return ComputePressionAcoustTotalLevel(); // Pression acoustique totale pour tous les couples source-recepteur
}
