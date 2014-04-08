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
#include "Tools/DiffractionPathPostFilter.h"
#include "Tools/FermatPostFilter.h"

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
    //Compte le nombre total de rayon a traiter
//    std::size_t ray_to_process(0);

    //for (unsigned int i = 0; i < sources->size(); i++)
    //{

    //    ray_to_process += sources->at(i).getNbRayLeft();
    //}

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
            new_ray->maxt = 10000.;
            pile_traitement.push(new_ray);
        }
    }


    //unsigned int last_pourcent = 0;
    //unsigned int total_ray = ray_to_process;

	//Traitement des rayons diffractes ainsi que les rayons tires aleatoirement
    while (1)
    {
        //Pile de traitement vide, on prend le suivant dans la liste des sources
        if (pile_traitement.empty())
        {
            Ray* newRay = genRay();
            if (newRay)
            {
                //ray_to_process--;
                pile_traitement.push(newRay);
            }

            //unsigned int current_pourcent = (unsigned int)(((double)(total_ray - ray_to_process) / (double)(total_ray)) * 100);
            //if (current_pourcent >= last_pourcent)
            //{
            //    last_pourcent++;
            //}

            //Aucun rayon genere a partir des sources, fin du traitement
            if (pile_traitement.empty())
            {
                solver->finish();
 
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
                    }
                }
            }
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
            new_ray->mint = 0.;
            new_ray->maxt = 10000.;
            return new_ray;
        }
    }

    return (Ray*)NULL;
}

bool DefaultEngine::traitementRay(Ray* r, std::list<validRay> &result)
{
    nbRayonsTraites++; //Nombre de rayons traites au cours de la simulation

    //Si le dernier evenement du rayon peut generer plusieurs rayons secondaires, on genere
    //un rayon secondaire, puis on copie le rayon restant et on le met dans la pile de traitement.
    //Si le dernier evenement n'a plus de rayon a generer, le rayon n'est pas traite
    if ( !(r->events.empty()) && (r->events.back()->isReponseLeft()) )
    {
		copyRayAndAddToStack(r);
    }

    decimal tmin = -1.0f;

	//Recuperation des structures acceleratrices pour le Solver
    Accelerator* accelerator = scene->getAccelerator();
    std::list<Intersection> foundPrims;

    //Appel du Solver pour le choix de la methode de traverser de la structure
    tmin =  accelerator->traverse(r, foundPrims);

    // Recherche pour des recepteurs;
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
    bool valide(false);
	unsigned int compteurValide(0);
	Intersection *inter = NULL;
	
	if ( foundPrims.size() > 0 )
	{
		inter = &( *(foundPrims.begin()) );
        valide = false;
        valide = solver->valideIntersection(r, inter);
        if (valide) { compteurValide++; }
	}
	else // no primitive found. The ray goes to infinity (and beyond)
	{
		valide = false;
	}

    validRay resultRay;
    resultRay.r = r;
    resultRay.valid = valide;
    result.push_back(resultRay);

    //for (std::list<Intersection>::iterator it = foundPrims.begin(); it != foundPrims.end(); it++)
    //{
    //    Ray* ray;
    //    if (it != foundPrims.begin())
    //    {
    //        ray = new Ray(r);
    //    }
    //    else { ray = r; }
    //    //firstPrimitive = it->p;
    //    valide = false;
    //    valide = solver->valideIntersection(ray, &(*it));
    //    if (valide) { compteurValide++; }
    //    validRay resultRay;
    //    resultRay.r = ray;
    //    resultRay.valid = valide;
    //    result.push_back(resultRay);
    //}

    //if (result.empty()) //Aucune primitive rencontree, le rayon n'est pas valide.
    //{
    //    validRay resultRay;
    //    resultRay.r = r;
    //    resultRay.valid = false;
    //    result.push_back(resultRay);
    //}

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
        r.maxt = 10000.;
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
