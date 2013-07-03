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
 
#include "NMPBSolver.h"
#include "ValidRay.h"
#include "PostTreatment.h"
#include "Tympan/MetierSolver/AcousticRaytracer/Tools/FaceSelector.h"
#include "Tympan/MetierSolver/AcousticRaytracer/Tools/LengthSelector.h"
#include "Tympan/MetierSolver/AcousticRaytracer/Tools/Logger.h"

void NMPBSolver::initGlobalValues()
{
    ////////////////////////////
    // General Values
    ////////////////////////////
    globalMaxProfondeur = 2;           //Nombre d'evenements autorises pour un rayon, globalMaxProfondeur inclu
    globalNbRaysPerSource = 10000;     //Nombre de rayons lances par les sources
    globalSizeReceiver = 1;            //Diametre de la sphere representant le recepteur
    globalAccelerator = 1;             //Choix de la structure acceleratrice. 0 : BruteForce, 1 : GridAccelerator, 2 : BVH, 3 : KdTree, other : GridAccelerator
    globalMaxTreeDepth = 12;           //Profondeur maximale autorisee pour le BVH ou KdTree.
    globalKeepDebugRay = false;                //Permet de conserver les rayons qui ont ete invalides pendant la propagation.

    ////////////////////////////
    // NMPB value
    ///////////////////////////
    globalMaxReflexion = 1;            //Nombre de reflexions speculaires autorisees pour un rayon, globalMaxReflexion inclu
    globalMaxDiffraction = 2;          //Nombre de diffractions autorisees pour un rayon, globalMaxDiffraction inclu
    globalMaxLength = 2000;            //Longueur maximale autorisee pour un rayon, globalMaxLength inclu
    globalSampleGround2D = 0.1f;       //Echantillonage sur sol pour la description de la topograohie 2D sous le rayon. (pas encore utilise)
    globalCylindreThick = 0.2f;            //Epaisseur des aretes de diffraction.

    /////////////////////////////
    // Targeting system + NMPB
    /////////////////////////////
    globalEnableTargets = true;        //Active la recherche de cible pour les sources. Pour le moment lie au solver NMPB.
    globalEnableFullTargets = false;   //Active la recherche de cible apres un evenement diffu. Pour le moment lie au solver NMPB.
    globalTargetsDensity = 10;         //Densite pour l'echantillonnage des zones interessantes.

}

bool NMPBSolver::postTreatmentScene(Scene* scene, std::vector<Source>& sources, std::vector<Recepteur>& recepteurs)
{


    selectorManagerIntersection.addSelector(new LengthSelector<Ray>(globalMaxLength));
    selectorManagerValidation.addSelector(new FaceSelector<Ray>(HISTORY_PRIMITIVE));

    PostTreatment::constructEdge(scene);

    if (globalEnableTargets || globalEnableFullTargets) { PostTreatment::findTargetsForNMPB(scene, recepteurs, targetManager, globalTargetsDensity); }
    ss << "Le targetManager contient " << targetManager.getTargets().size() << " cibles." << std::endl;
    if (globalEnableTargets) { PostTreatment::appendDirectionToSources(&targetManager, sources); }
    return true;
}

bool NMPBSolver::valideIntersection(Ray* r, Intersection* inter)
{
    if (r->events.size() > static_cast<unsigned int>(globalMaxProfondeur))
    {
        return false;
    }
    bool isValid = false;
    //if(inter->forme == TRIANGLE && r->events.size() < 3 && !(inter->p->getMaterial()->isNatural)){
    if (inter->forme == TRIANGLE && !(inter->p->getMaterial()->isNatural) && r->nbReflexion <= static_cast<unsigned int>(globalMaxReflexion))
    {
        isValid = ValidRay::validTriangleWithSpecularReflexion(r, inter);
    }
    else if (inter->forme == CYLINDRE && r->nbDiffraction <= static_cast<unsigned int>(globalMaxDiffraction))
    {
        isValid = ValidRay::validCylindreWithDiffraction(r, inter);
        //std::cout<<"Erreur : la forme n'est pas prise en charge."<<std::endl;
    }

    if (isValid && globalEnableFullTargets) { ValidRay::appendDirectionToEvent(r->events.back(), targetManager); }

    return isValid;
    return (isValid && selectorManagerIntersection.appendData(r));
}

bool NMPBSolver::invalidRayon(Ray* r)
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

bool NMPBSolver::valideRayon(Ray* r)
{
    selectorManagerValidation.appendData(r);
    return true;
}

void NMPBSolver::finish()
{
    std::map<unsigned long long int, Ray*> selectedData = selectorManagerValidation.getSelectedData();
    for (std::map<unsigned long long int, Ray*>::iterator it = selectedData.begin(); it != selectedData.end(); it++)
    {
        valid_rays.push_back(it->second);
    }
    return;
}
