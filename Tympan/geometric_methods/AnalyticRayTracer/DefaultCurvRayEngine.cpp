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

#include "Tympan/models/common/mathlib.h"
#include "Tympan/geometric_methods/AcousticRaytracer/Acoustic/Event.h"
#include "Tympan/geometric_methods/AcousticRaytracer/Ray/Ray.h"

#include "DefaultCurvRayEngine.h"

bool DefaultCurvRayEngine::process()
{
    std::size_t max_size(0);

    QTime time;
    time.start();
    nbRayonsTraites = 0;

    //We begin to throw a rays directly from each source to each receptor
	initialReceptorTargeting();

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
            std::list<validRay> result;
            traitementRay(current_ray, result);

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

bool DefaultCurvRayEngine::traitementRay(Ray* r, std::list<validRay> &result)
{
    nbRayonsTraites++; //Nombre de rayons traites au cours de la simulation

    //Si le dernier evenement du rayon peut generer plusieurs rayons secondaires, on genere
    //un rayon secondaire, puis on copie le rayon restant et on le met dans la pile de traitement.
    //Si le dernier evenement n'a plus de rayon a generer, le rayon n'est pas traite
    if ( !(r->events.empty()) && (r->events.back()->isReponseLeft()) )
    {
		copyRayAndAddToStack(r);
    }


	//Recuperation des structures acceleratrices pour le Solver
    Accelerator* accelerator = scene->getAccelerator();
    std::list<Intersection> foundPrims;
	
	while ( true )
	{
		foundPrims.clear(); 
		//Appel du Solver pour le choix de la methode de traverser de la structure
      accelerator->traverse(r, foundPrims);
		// Recherche pour des recepteurs situé à moins de 1 pas de temps;
//		searchForReceptor(delta_x, r);
		
		// Si tmin < delta_x alors on sort de la boucle sinon on avance d'un pas
//		if (tmin < delta_x) { break; }
		
		// OPTION 1 : 
		//	Avancer d'un pas
		//  Ajouter l'évènement REFRACTION (à créer) 
		// OPTION 2 : Avancer de n pas (evite de faire recherche à chaque pas de temps)
		// int nstep = static_cast<int>( tmin / delta_x) / 2 + 1 // Par sécurité on avance de la moitié de la distance
		// for (unsigned int i = 0; i < nstep; i++) { Avancer d'un pas; ajouter lévènement diffraction }
	}

    //Validation des rayons en generant un evenement. Les premiers rayons valides sont des copies de l'original, le dernier est valide sans copie.
    //De cette maniere on peut valider separement des diffractions et une reflexion a partir d'un seul rayon initial.
    //La copie est necessaire pour ne pas valider 2 fois le meme evenement. Si le rayon ne peut pas etre valide, il sera delete dans la fonction traitement()
    bool valide(false);
	unsigned int compteurValide(0);
	Intersection *inter = NULL;
	
	// ! IMPORTANT
	// We suppose, here, that accelarating structures use "leafTreatment::treatment::FIRST:"
	// In this case, accelerating structure return only the closest primitive.
	// If it is not the case, you must browse througth the returned list
	if ( foundPrims.size() > 0 )
	{
		inter = &( *(foundPrims.begin()) );
        valide = solver->valideIntersection(r, inter);
        if (valide) { compteurValide++; }
	}
	// Commented lines under are let intentionnaly for understanding reason only 
	//else // no primitive found. The ray goes to infinity (and beyond)
	//{
	//	valide = false;
	//}

    validRay resultRay;
    resultRay.r = r;
    resultRay.valid = valide;
    result.push_back(resultRay);

    return true;
}
