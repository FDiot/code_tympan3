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
 
#include <stack>
#include <list>
#include <algorithm>
#include <QTime>
#include <vector>

#include "Geometry/mathlib.h"
#include "Acoustic/Event.h"
#include "Ray/Ray.h"
#include "Tools/PostFilter.h"
#include "Tools/CloseEventPostFilter.h"

#include "DefaultEngine.h"
#include "Tympan/Tools/TYProgressManager.h"


struct CompareVec
{
    bool operator()(std::vector<unsigned int> list1, std::vector<unsigned int> list2) const
    {
        //On compare jusqu'a ce qu'on atteigne le bout du plus petit vecteur
        int minSize = min(list1.size(), list2.size());
        for (int i = 0; i < minSize; i++)
        {
            if (list1.at(i) < list2.at(i))
            {
                return true;
            }
            else if (list1.at(i) == list2.at(i))
            {
                continue;
            }
            else
            {
                return false;
            }
        }
        //Si les vecteurs ont la meme taille, c'est qu'ils sont egaux
        if (list1.size() == list2.size())
        {
            return false;
        }
        //La list avec la plus grande taille est superieure a l'autre
        if (list1.size() < list2.size())
        {
            return true;
        }
        return false;
    }
} myCompare ;

bool DefaultEngine::process()
{

    //std::cout<<"Lancement du process."<<std::endl;

    //Compte le nombre total de rayon a traiter
    std::size_t ray_to_process(0);

    for (unsigned int i = 0; i < sources->size(); i++)
    {

        ray_to_process += sources->at(i).getNbRayLeft();
    }

    std::size_t nb_event(0);
    bool find_intersection;
    std::size_t max_size(0);

    QTime time;
    time.start();
    nbRayonsTraites = 0;

    //Generation des rayons en direction des recepteurs pour chaque source
    for (vector<Source>::iterator itsource = sources->begin(); itsource != sources->end(); itsource++)
    {
        for (vector<Recepteur>::iterator itrecp = recepteurs->begin(); itrecp != recepteurs->end(); itrecp++)
        {
            Ray* new_ray = new Ray();
            new_ray->constructId = rayCounter;
            rayCounter++;
            new_ray->source = (&(*itsource));
            new_ray->position = itsource->getPosition();
            vec3 psource = itsource->getPosition();
            vec3 precp = itrecp->getPosition();
            new_ray->direction = precp - psource;
            new_ray->direction.normalize();
            new_ray->mint = 0.;
            new_ray->maxt = 2000.0;
            //std::cout<<"Emission d'un rayon d'origine ("<<new_ray->position.x<<","<<new_ray->position.y<<","<<new_ray->position.z;
            //std::cout<<") et direction ("<<new_ray->direction.x<<","<<new_ray->direction.y<<","<<new_ray->direction.z<<")."<<std::endl;
            pile_traitement.push(new_ray);
        }
    }


    unsigned int last_pourcent = 0;
    unsigned int total_ray = ray_to_process;
#if TY_USE_IHM
    TYProgressManager::setMessage("Calcule les trajets et appelle le solveur");
    TYProgressManager::set(static_cast<int>(total_ray));
#endif
    //Traitement des rayons diffractes ainsi que les rayons tires aleatoirement
    while (1)
    {
        //Pile de traitement vide, on prend le suivant dans la liste des sources
        if (pile_traitement.empty())
        {
            Ray* newRay = genRay();
            if (newRay)
            {
                ray_to_process--;
                pile_traitement.push(newRay);
            }
            unsigned int current_pourcent = (unsigned int)(((double)(total_ray - ray_to_process) / (double)(total_ray)) * 100);
            //TYProgressManager::step(); Erreur de compilation lors du link avec un solver utilisant RayTracer et Tools
            if (current_pourcent >= last_pourcent)
            {
                //std::cout << "Avancement : " << current_pourcent << "%." << endl;
                last_pourcent++;
            }

            //Aucun rayon genere a partir des sources, fin du traitement
            if (pile_traitement.empty())
            {
                solver->finish();
                //std::cout << "Tous les rayon des sources ont ete traites" << endl;
                //std::cout << "Duree de la simulation : " << (double)(time.elapsed()) / 1000.0 << endl;
                //std::cout << "Il y a " << solver->getValidRays()->size() << " rayons valides. " << endl;
                //std::cout << nb_rayon_traite << " rayons ont ete traites." << endl;

                //std::cout << "Statistiques de benchmark pour le traitement : " << endl;
                //std::cout << "Nombre de primitives : " << scene.getPrimitives()->size() << endl;
                //std::cout << "Duree de la simulation : " << ((int)(time.elapsed())) << endl;
                //std::cout << "Nombre de rayons traites : " << nbRayonsTraites << endl;
                //std::cout << "Nombre  de traverser de rayon : " << totalTraversalStep << endl;
                //std::cout << "Vitesse de traitement : " << ((double)nbRayonsTraites / (double)(time.elapsed())) * 1000. << endl;
                //std::cout << "Nombre d'intersection moyen avec les boites : " << (double)totalIntersecNode / (double)totalRay << endl;
                //std::cout << "Nombre d'intersection moyen avec les primitives : " << (double)totalIntersecPrimitive / (double)totalRay << endl;
                //std::cout << "Calcul d'intersection par rapport au brut force : " << ((double) (((double)totalIntersecNode / (double)totalRay) + ((double)totalIntersecPrimitive / (double)totalRay)) / (double) (scene.getPrimitives()->size())) * 100;

// BEGIN : DTn20130507 - Commented cause code under does nothing
                //vector<vector<unsigned int> > historique = std::vector<std::vector<unsigned int> >();
                //for (unsigned int i = 0; i < solver->getValidRays()->size(); i++)
                //{
                //    historique.push_back(solver->getValidRays()->at(i)->getPrimitiveHistory());
                //}

                //std::sort(historique.begin(), historique.end(), myCompare);
                ////std::cout << "Fin de la Simulation. " << solver->getValidRays()->size() << " rayons ont ete trouves." << endl;
                ////std::cout << "Historique des rayons : " << endl;
                //for (unsigned int i = 0; i < historique.size(); i++)
                //{
                //    vector<unsigned int> path = historique.at(i);
                //    //std::cout << "Historique du rayon " << i << " : [";
                //    for (unsigned int j = 0; j < path.size(); j++)
                //    {
                //        //std::cout << path.at(j) << ",";
                //    }
                //    //std::cout << "]." << endl;
                //}
// END

				// Post filtering of the rays
				closeEventPostFilter cepf(getSolver()->getValidRays());
				unsigned int suppressed = cepf.Process();
                return true;
            }
        }
        else
        {
            Ray* current_ray = pile_traitement.top();
            pile_traitement.pop();
            //nb_rayon_traite++;
            nb_event = current_ray->events.size();
            std::list<validRay> result;
            find_intersection = traitementRay(current_ray, result);

            unsigned int compteurValidation(0);
            for (std::list<validRay>::iterator it = result.begin(); it != result.end(); it++)
            {
                bool currentValid = it->valid;
                Ray* currentValidRay = it->r;
                if (!currentValid)
                {
                    solver->invalidRayon(currentValidRay);
                }
                else
                {
                    pile_traitement.push(currentValidRay);
                    if (pile_traitement.size() > max_size)
                    {
                        max_size = pile_traitement.size();
                        //std::cout << "Nouvelle taille maximale : " << max_size << std::endl;
                    }
                }
            }
            //cout << compteurValidation << " primitives ont ete valide." << endl;
        }
    }
}

Ray* DefaultEngine::genRay()
{
    //progressionInfo progInfo(sources.size());
    for (unsigned int i = 0; i < sources->size(); i++)
    {

        if (sources->at(i).getNbRayLeft() > 0)
        {
            Ray* new_ray = new Ray();
            new_ray->constructId = rayCounter;
            rayCounter++;
            new_ray->source = (&(sources->at(i)));
            new_ray->position = sources->at(i).getPosition();
            sources->at(i).getDirection(new_ray->direction);
            //std::cout<<"Direction du nouveau rayon : ("<<new_ray->direction.x<<","<<new_ray->direction.y<<","<<new_ray->direction.z<<") : "<<new_ray->direction.length()<<std::endl;
            new_ray->mint = 0.;
            new_ray->maxt = 2000.;
            return new_ray;
        }
    }

    return (Ray*)NULL;
}

bool DefaultEngine::traitementRay(Ray* r, std::list<validRay> &result)
{

    //std::cout<<"Traitement d'un rayon."<<std::endl;
    bool valide;
    nbRayonsTraites++; //Nombre de rayons traites au cours de la simulation

    //Si le dernier evenement du rayon peut generer plusieurs rayons secondaires, on genere
    //un rayon secondaire, puis on copie le rayon restant et on le met dans la pile de traitement.
    //Si le dernier evenement n'a plus de rayon a generer, le rayon n'est pas traite
    if (!(r->events.empty()))
    {
        //Le rayon ne peut plus generer de rayons secondaires
        /*if(!r->events.back()->isReponseLeft())
            return false;
            r->events.back()->getResponse(r->direction);*/
        if (r->events.back()->isReponseLeft())
        {
            //Copie de r->getEvents()->size() Evenements
            //Copie d'un rayon ayant rencontre une diffraction...
            Ray* copie = new Ray(r);
            copie->constructId = rayCounter;
            rayCounter++;
            r->events.back()->getResponse(copie->direction);
            pile_traitement.push(copie);
            //Copie achevee
        }
    }

    decimal tmin = -1.0f;

    //Recuperation des structures acceleratrices pour le Solver
    Shape* firstPrimitive(NULL);
    Accelerator* accelerator = scene->getAccelerator();
    std::list<Intersection> foundPrims;

    //Appel du Solver pour le choix de la methode de traverser de la structure
    tmin =  accelerator->traverse(r, foundPrims);

    //std::cout<<"L'accelerator a trouve "<<foundPrims.size()<<" intersections."<<std::endl;
    if (!foundPrims.empty())
    {
        firstPrimitive = foundPrims.begin()->p;
    }
    //cout << "Recherche pour des recepteurs..." << endl;
    for (unsigned int i = 0; i < recepteurs->size(); i++)
    {
        Intersection result;
        if (recepteurs->at(i).intersectionRecepteur(r->position, r->direction, tmin, result))
        {
            //Cas ou le rayon touche un recepteur
            Ray* valide_ray = new Ray(r);
            valide_ray->constructId = rayCounter;
            rayCounter++;
            valide_ray->recepteur = (&(recepteurs->at(i)));
            valide_ray->finalPosition = valide_ray->position + valide_ray->direction * result.t;
            valide_ray->computeLongueur();
            solver->valideRayon(valide_ray);
        }
    }


    //Validation des rayons en generant un evenement. Les premiers rayons valides sont des copies de l'original, le dernier est valide sans copie.
    //De cette maniere on peut valider separement des diffractions et une reflexion a partir d'un seul rayon initial.
    //La copie est necessaire pour ne pas valider 2 fois le meme evenement. Si le rayon ne peut pas etre valide, il sera delete dans la fonction traitement()
    unsigned int compteurValide(0);
    for (std::list<Intersection>::iterator it = foundPrims.begin(); it != foundPrims.end(); it++)
    {
        Ray* ray;
        if (it != foundPrims.begin())
        {
            ray = new Ray(r);
        }
        else { ray = r; }
        firstPrimitive = it->p;
        valide = false;
        //if (tmin < 0 || (tmin > 0 && foundPrims.at(i).tsect <= tmin ))
        //valide = firstPrimitive->valideIntersection(ray);
        valide = solver->valideIntersection(ray, &(*it));
        if (valide) { compteurValide++; }
        validRay resultRay;
        resultRay.r = ray;
        resultRay.valid = valide;
        result.push_back(resultRay);
    }

    if (result.empty()) //Aucune primitive rencontree, le rayon n'est pas valide.
    {
        validRay resultRay;
        resultRay.r = r;
        resultRay.valid = false;
        result.push_back(resultRay);
    }
    //cout << "La traversee a valide " << compteurValide << " primitives." << endl;

    return true;

}

void DefaultEngine::runStructureBenchmark()
{

    unsigned int nbVec = 1000000;

    //Generation du buffer de vecteur
    BBox sceneBox = scene->getGlobalBox();
    /*std::vector<vec3> buffer = std::vector<vec3>(2*nbVec);
    for(unsigned int i = 0; i < nbVec; i++){
        vec3 newDir = vec3((double)rand() / (double)RAND_MAX, (double)rand() / (double)RAND_MAX, (double)rand() / (double)RAND_MAX);
        newDir.normalize();
        buffer.push_back(newDir);
        vec3 newPos = vec3(((double)rand() / (double)RAND_MAX) * (sceneBox.pMax.x - sceneBox.pMin.x) + sceneBox.pMin.x,
                            ((double)rand() / (double)RAND_MAX) * (sceneBox.pMax.y - sceneBox.pMin.y) + sceneBox.pMin.y,
                            ((double)rand() / (double)RAND_MAX) * (sceneBox.pMax.z - sceneBox.pMin.z) + sceneBox.pMin.z);
        buffer.push_back(newPos);
    }*/

    QTime time;
    time.start();

    for (unsigned int i = 0; i < nbVec; i++)
    {
        Ray r;
        r.mint = 0.00001f;
        r.maxt = 2000.;
        r.direction = vec3((double)rand() / (double)RAND_MAX, (double)rand() / (double)RAND_MAX, (double)rand() / (double)RAND_MAX);
        r.direction.normalize();
        r.position = vec3(((double)rand() / (double)RAND_MAX) * (sceneBox.pMax.x - sceneBox.pMin.x) + sceneBox.pMin.x,
                          ((double)rand() / (double)RAND_MAX) * (sceneBox.pMax.y - sceneBox.pMin.y) + sceneBox.pMin.y,
                          ((double)rand() / (double)RAND_MAX) * (sceneBox.pMax.z - sceneBox.pMin.z) + sceneBox.pMin.z);
        Accelerator* accel = scene->getAccelerator();
        std::list<Intersection> foundPrims;
        accel->traverse(&r, foundPrims);
    }

    int totalTime = time.elapsed();
    //std::cout << "Fin du benchmark." << std::endl;
    //std::cout << "Nombre de rayons traites : " << nbVec << std::endl;
    //std::cout << "Temps ecoule : " << totalTime << " ms." << std::endl;
    //std::cout << "Vitesse de traitement : " << nbVec / totalTime << " rays/ms." << std::endl;
}
