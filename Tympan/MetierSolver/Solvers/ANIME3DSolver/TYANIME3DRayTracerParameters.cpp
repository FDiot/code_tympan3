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
 
#include "TYANIME3DRayTracerParameters.h"

//#include "ValidationEvenement.h"

#include <iostream>
#include <fstream>

#include "Tympan/MetierSolver/AcousticRaytracer/Acoustic/ValidRay.h"
#include "Tympan/MetierSolver/AcousticRaytracer/Acoustic/PostTreatment.h"
#include "Tympan/MetierSolver/AcousticRaytracer/Tools/FaceSelector.h"
#include "Tympan/MetierSolver/AcousticRaytracer/Tools/LengthSelector.h"
#include "Tympan/MetierSolver/AcousticRaytracer/Tools/DiffractionSelector.h"
#include "Tympan/MetierSolver/AcousticRaytracer/Tools/ReflectionSelector.h"
#include "Tympan/MetierSolver/AcousticRaytracer/Tools/Logger.h"

void TYANIME3DRayTracerParameters::initGlobalValues()
{
    ////////////////////////////
    // General Values
    ////////////////////////////
    globalMaxProfondeur = 2;            //Nombre d'evenements autorises pour un rayon, globalMaxProfondeur inclu
    globalNbRaysPerSource = 80000;      //Nombre de rayons lances par les sources
    globalSizeReceiver = 5.0f;          //Diametre de la sphere representant le recepteur
    globalAccelerator = 3;              //Choix de la structure acceleratrice. 0 : BruteForce, 1 : GridAccelerator, 2 : BVH, 3 : KdTree, other : GridAccelerator
    globalMaxTreeDepth = 12;            //Profondeur maximale autorisee pour le BVH ou KdTree.
    globalUseSol = true;                // Utilisation du sol pour les reflexions
    globalKeepDebugRay = false;     //Permet de conserver les rayons qui ont ete invalides pendant la propagation.

    ////////////////////////////
    // NMPB value
    ///////////////////////////
    globalMaxReflexion = 2;         //Nombre de reflexions speculaires autorisees pour un rayon, globalMaxReflexion inclu
    globalMaxDiffraction = 2;           //Nombre de diffractions autorisees pour un rayon, globalMaxDiffraction inclu
    globalMaxLength = 2000;         //Longueur maximale autorisee pour un rayon, globalMaxLength inclu
    globalSampleGround2D = 0.1f;        //Echantillonage sur sol pour la description de la topograohie 2D sous le rayon. (pas encore utilise)
    globalCylindreThick = 0.2f;     //Epaisseur des aretes de diffraction.

    /////////////////////////////
    // Targeting system + NMPB
    /////////////////////////////
    globalEnableTargets = false;        //Active la recherche de cible pour les sources. Pour le moment lie au solver NMPB.
    globalEnableFullTargets = false;    //Active la recherche de cible apres un evenement diffu. Pour le moment lie au solver NMPB.
    globalTargetsDensity = 10;          //Densite pour l'echantillonnage des zones interessantes.

    //////////////////////////////
    // Meteo Extensions
    /////////////////////////////
    globalAngleDiffMin = 5;             // Angle minimal (exprime en degre) a prendre en compte entre 2 faces pour ajouter une arrete de diffraction
    globalNbRayWithDiffraction = 200;   // Nombre de rayons relance lors d'un evenement diffraction
    globalRayTracingOrder = 0;          // [0-2]Sens de traitement des rayons source-recepteur ou inverse (0 = SR / 1 =RS / 2 = auto)

    globalUseMeteo = true;              // Prise en compte (ou non) de la meteo
    globalAnalyticDMax = 2000.0f;       // Distance de propagation maximale des rayons courbes
    globalAnalyticTMax = 3.0f;          // Temps de propagation maximal des rayons courbes
    globalAnalyticH = 0.01f;            // Pas de temps de calcul pour la propagation des rayons courbes
    globalAnalyticNbRay = 10;           // Nombre de rayons tires pour le lancer de rayons courbes

    globalAnalyticGradC = 0.1f;     // Gradient vertical de celerite
    globalAnalyticGradV = 0.15f;    // Gradient vertical de vitesse de vent
    globalAnalyticC0 = 340.0f;      // Celerite du son initiale
    globalAnalyticTypeTransfo = 1;  // Methode de transformation -- TOUJOURS = 1 -- pas d'autre methode definie
//    globalRestitModifiedGeom = 1;   // Indique si l'on souhaite recuperer la geometrie transformee
    globalOverSampleD = 3;          // [0 +[ (0 pas de surechantillonnage) Indique le taux de surechantillonnage des rayons

    // Chargement des parametres de calcul
    loadParameters();
}

bool TYANIME3DRayTracerParameters::loadParameters()
{
    bool bRes = true;
    const char fileName[31] = "ANIME3DRayTracerParameters.txt";

    // Ouverture du fichier
    ifstream params(fileName, ios::in);

    if (!params) { return false; }

    char ligne[132];

    //Nombre d'evenements autorises pour un rayon, globalMaxProfondeur inclu
    if (params.getline(ligne, 132)) { globalMaxProfondeur = getParam(ligne); }

    //Nombre de rayons lances par les sources
    if (params.getline(ligne, 132)) { globalNbRaysPerSource = getParam(ligne); }

    //Diametre de la sphere representant le recepteur
    if (params.getline(ligne, 132)) { globalSizeReceiver = getParam(ligne); }

    //Choix de la structure acceleratrice. 0 : BruteForce, 1 : GridAccelerator, 2 : BVH, 3 : KdTree, other : GridAccelerator
    if (params.getline(ligne, 132)) { globalAccelerator = getParam(ligne); }

    //Profondeur maximale autorisee pour le BVH ou KdTree
    if (params.getline(ligne, 132)) { globalMaxTreeDepth = getParam(ligne); }

    // Utilisation du sol pour les reflexions
    if (params.getline(ligne, 132)) { globalUseSol = getParam(ligne); }

    //Permet de conserver les rayons qui ont ete invalides pendant la propagation.
    if (params.getline(ligne, 132)) { globalKeepDebugRay = getParam(ligne); }

    //Nombre de reflexions speculaires autorisees pour un rayon, globalMaxReflexion inclu
    if (params.getline(ligne, 132)) { globalMaxReflexion = getParam(ligne); }

    //Nombre de diffractions autorisees pour un rayon, globalMaxDiffraction inclu
    if (params.getline(ligne, 132)) { globalMaxDiffraction = getParam(ligne); }

    //Longueur maximale autorisee pour un rayon, globalMaxLength inclu
    if (params.getline(ligne, 132)) { globalMaxLength = getParam(ligne); }

    //Echantillonage sur sol pour la description de la topographie 2D sous le rayon. (pas encore utilise)
    if (params.getline(ligne, 132)) { globalSampleGround2D = getParam(ligne); }

    //Epaisseur des aretes de diffraction.
    if (params.getline(ligne, 132)) { globalCylindreThick = getParam(ligne); }

    //Active la recherche de cible pour les sources. Pour le moment lie au solver NMPB.
    if (params.getline(ligne, 132)) { globalEnableTargets = getParam(ligne); }

    //Active la recherche de cible apres un evenement diffu. Pour le moment lie au solver NMPB.
    if (params.getline(ligne, 132)) { globalEnableFullTargets = getParam(ligne); }

    //Densite pour l'echantillonnage des zones interessantes.
    if (params.getline(ligne, 132)) { globalTargetsDensity = getParam(ligne); }

    //Angle minimal (exprime en degre) a prendre en compte entre 2 faces pour ajouter une arrete de diffraction
    if (params.getline(ligne, 132)) { globalAngleDiffMin = getParam(ligne); }

    // Nombre de rayons relance lors d'un evenement diffraction
    if (params.getline(ligne, 132)) { globalNbRayWithDiffraction = getParam(ligne); }

    //[0-2]Sens de traitement des rayon source-recepteur ou inverse (0 = SR / 1 =RS / 2 = auto)
    if (params.getline(ligne, 132)) { globalRayTracingOrder = getParam(ligne); }

    // Prise en compte (ou non) de la meteo
    if (params.getline(ligne, 132)) { globalUseMeteo = getParam(ligne); }

    // Distance de propagation maximale des rayons courbes
    if (params.getline(ligne, 132)) { globalAnalyticDMax = getParam(ligne); }

    // Temps de propagation maximal des rayons courbes
    if (params.getline(ligne, 132)) { globalAnalyticTMax = getParam(ligne); }

    // Pas de temps de calcul pour la propagation des rayons courbes
    if (params.getline(ligne, 132)) { globalAnalyticH = getParam(ligne); }

    // Nombre de rayons tires pour le lancer de rayons courbes
    if (params.getline(ligne, 132)) { globalAnalyticNbRay = getParam(ligne); }

    // Gradient vertical de celerite
    if (params.getline(ligne, 132)) { globalAnalyticGradC = getParam(ligne); }

    // Gradient vertical de vitesse de vent
    if (params.getline(ligne, 132)) { globalAnalyticGradV = getParam(ligne); }

    // Celerite du son initiale
    if (params.getline(ligne, 132)) { globalAnalyticC0 = getParam(ligne); }

    // Methode de transformation -- TOUJOURS = 1 -- pas d'autre methode definie
    if (params.getline(ligne, 132)) { globalAnalyticTypeTransfo = getParam(ligne); }

    // Indique si l'on souhaite recuperer la geometrie transformee
//    if (params.getline(ligne, 132)) { globalRestitModifiedGeom = getParam(ligne); }

    // [0 +[ (0 pas de surechantillonnage) Indique le taux de surechantillonnage des rayons
    if (params.getline(ligne, 132)) { globalOverSampleD = getParam(ligne); }

    params.close();

    return bRes;
}

bool TYANIME3DRayTracerParameters::postTreatmentScene(Scene* scene, std::vector<Source>& sources, std::vector<Recepteur>& recepteurs)
{
	selectorManagerValidation.addSelector(new LengthSelector<Ray>(globalMaxLength));
    //  selectorManagerIntersection.addSelector(new LengthSelector<Ray>(globalMaxLength));
	//
    selectorManagerIntersection.addSelector(new DiffractionSelector<Ray>(globalMaxDiffraction));
    selectorManagerIntersection.addSelector(new ReflectionSelector<Ray>(globalMaxReflexion, globalUseSol));
    //  selectorManagerValidation.addSelector(new FaceSelector<Ray>(HISTORY_PRIMITIVE));

    // Ajoute des cylindres sur les arretes diffractantes
    PostTreatment::constructEdge(scene);

    return true;
}

bool TYANIME3DRayTracerParameters::valideIntersection(Ray* r, Intersection* inter)
{
    if (r->events.size() > static_cast<unsigned int>(globalMaxProfondeur)) { return false; }

    bool isValid = false;

    // cas d'un triangle (sol)
    if (inter->forme == TRIANGLE && !(inter->p->getMaterial()->isNatural) && r->nbReflexion <= static_cast<unsigned int>(globalMaxReflexion))
    {
        isValid = ValidRay::validTriangleWithSpecularReflexion(r, inter);
    }

    // cas du cylindre (arrete de diffraction)
    else if (inter->forme == CYLINDRE && r->nbDiffraction <= static_cast<unsigned int>(globalMaxDiffraction))
    {
        isValid = ValidRay::validCylindreWithDiffraction(r, inter);
    }

    if (isValid && globalEnableFullTargets) { ValidRay::appendDirectionToEvent(r->events.back(), targetManager); }

    return (isValid && selectorManagerIntersection.appendData(r));
}

bool TYANIME3DRayTracerParameters::valideRayon(Ray* r)
{
    selectorManagerValidation.appendData(r);
    return true;
}

bool TYANIME3DRayTracerParameters::invalidRayon(Ray* r)
{
    if (!globalKeepDebugRay)
    {
        delete r;
    }
    else
    {
        debug_rays.push_back(r);
    }

    return true;
}

/*void TYANIME3DRayTracerParameters::clean()
{
    while(!valid_rays.empty())
    {
        Ray *r = valid_rays.back();
        valid_rays.pop_back();
        delete r;
    }
}*/

void TYANIME3DRayTracerParameters::finish()
{
    std::map<unsigned long long int, Ray*> selectedData = selectorManagerValidation.getSelectedData();

    for (std::map<unsigned long long, Ray*>::iterator it = selectedData.begin(); it != selectedData.end(); it++)
    {
        valid_rays.push_back(it->second);
    }

    return;
}
