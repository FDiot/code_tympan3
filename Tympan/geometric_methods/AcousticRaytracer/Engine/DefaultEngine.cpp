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
#include <vector>

#include "Geometry/mathlib.h"
#include "Acoustic/Event.h"
#include "Ray/Ray.h"
#include "DefaultEngine.h"

/*struct CompareVec
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
} myCompare ;*/

bool DefaultEngine::process()
{
    std::size_t max_size(0);

    //QTime time;
    //time.start();
    nbRayonsTraites = 0;

    //Throw ray from every source to every receptor
	initialReceptorTargeting();

	//Loop until the end of the simulation (i.e. when the stack of rays is empty and the sources cannot generate more rays)
    while (1)
    {
        //Empty stack => we generate a new ray from the sources
        if (pile_traitement.empty())
        {
            Ray* newRay = genRay();
            if (newRay)
            {
                
                pile_traitement.push(newRay);
            }

            //Sources have generated all their rays => finish the simulation
            if (pile_traitement.empty())
            {
                solver->finish();
 
                return true;
            }
        }
        else
        {
        	// Recover the ray a the top of the stack
            Ray* current_ray = pile_traitement.top();
            pile_traitement.pop();

            std::list<validRay> result;

            //Process the current ray and put the resulting rays in result
            traitementRay(current_ray, result);

            //Go through the rays in resulting from the call of traitementRay
            for (std::list<validRay>::iterator it = result.begin(); it != result.end(); it++)
            {
                bool currentValid = it->valid;
                Ray* currentValidRay = it->r;

                //If the ray is not valid we invalid it (delete or put in the debug_rays list if the KeepDebugRay option is set to true)
                if (!currentValid)
                {
                    solver->invalidRayon(currentValidRay);
                }
                else
                {
                	//The ray is valid (but has yet to hit a receptor) => put it back in the stack for further processing
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

    for (unsigned int i = 0; i < sources->size(); i++)
    {
        if (sources->at(i).getNbRayLeft() > 0)
        {
        	// Generate a ray from the current source(i)
            Ray* new_ray = new Ray();
            new_ray->setConstructId ( rayCounter );
            rayCounter++;
            new_ray->setSource ( (&(sources->at(i))) );
            new_ray->setPosition ( sources->at(i).getPosition());
			vec3 direction;
            sources->at(i).getDirection(direction);
			new_ray->setDirection(direction);
            new_ray->setMint ( 0.);
            new_ray->setMaxt ( 10000.);
            return new_ray;
        }
    }

    return (Ray*)NULL;
}

bool DefaultEngine::traitementRay(Ray* r, std::list<validRay> &result)
{
    nbRayonsTraites++; //Number of rays processed during the simulation

    // Handle diffraction events: (reflexion events only have one possible response and it is consumed during their validation in valideIntersection)
    // If the last event of the current ray still has reponses left, copy the ray and use the next response to generate a new direction for the copy and then add the copy to the stack.
    // The reason rays generated by diffraction events are handled in this way instead of generating all responses when the intersection is validated is to avoid flooding the rays' stack.
    // Indeed, in this way, each response is handled individually and needs to be processed before the next response is generated. 
    // Hence, for a particular diffraction event, only one of the rays it generates can be present in the stack at a given time.
    if ( !(r->getEvents()->empty()) && (r->getEvents()->back()->isReponseLeft()) )
    {
		copyRayAndAddToStack(r);
    }

    decimal tmin = -1.0f;

	//Get the solver's accelerating structure
    Accelerator* accelerator = scene->getAccelerator();
    std::list<Intersection> foundPrims;

    // Find intersections with the scene's primitives
    tmin =  accelerator->traverse(r, foundPrims);

    // Check for intersections with receptors (and pass the ray to the selector manager for validation if it hits a receptor)
    searchForReceptor(tmin, r);


    //Intersections' validation. 
  
	if ( foundPrims.size() == 0 ){
	//if no intersection found => return the current ray 'r' as an invalid validRay;
		validRay resultRay;
		resultRay.r = r;
		resultRay.valid = false;
		result.push_back(resultRay);
    
	}else{
	//if some intersections have been found, iterate through them and test their validity
		for (std::list<Intersection,std::allocator<Intersection>>::iterator it=foundPrims.begin();it != foundPrims.end();it++){

			bool valide=false;
			Intersection *inter = NULL;
			Ray* current_ray = NULL;

			if(next(it)!=foundPrims.end()){
			//if the current intersection is NOT the last one, copy the current ray
				current_ray = new Ray(r);
				current_ray->setConstructId ( rayCounter );
				rayCounter++;

			}else{
			//if the current intersection is the last one, validate the current ray 'r' directly without copying it.
				current_ray=r;
			}

			//retrieve the intersection and validate it
			inter = &(*(it));
			valide = solver->valideIntersection(current_ray, inter);

			//create a validRay to return the result of the intersection validation
			validRay resultRay;
			resultRay.r = current_ray;
			resultRay.valid = valide;
			result.push_back(resultRay);

		}
	}


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

    //QTime time;
    //time.start();

    for (unsigned int i = 0; i < nbVec; i++)
    {
        Ray r;
        r.setMint ( 0.00001f);
        r.setMaxt ( 10000.);
        r.setDirection ( vec3((decimal)rand() / (decimal)RAND_MAX, (decimal)rand() / (decimal)RAND_MAX, (decimal)rand() / (decimal)RAND_MAX) );
        r.getDirection().normalize();
        r.setPosition ( vec3(((decimal)rand() / (decimal)RAND_MAX) * (sceneBox.pMax.x - sceneBox.pMin.x) + sceneBox.pMin.x,
                          ((decimal)rand() / (decimal)RAND_MAX) * (sceneBox.pMax.y - sceneBox.pMin.y) + sceneBox.pMin.y,
                          ((decimal)rand() / (decimal)RAND_MAX) * (sceneBox.pMax.z - sceneBox.pMin.z) + sceneBox.pMin.z));
        Accelerator* accel = scene->getAccelerator();
        std::list<Intersection> foundPrims;
        accel->traverse(&r, foundPrims);
    }

    //int totalTime = time.elapsed();
    //std::cout << "Fin du benchmark." << std::endl;
    //std::cout << "Nombre de rayons traites : " << nbVec << std::endl;
    //std::cout << "Temps ecoule : " << totalTime << " ms." << std::endl;
    //std::cout << "Vitesse de traitement : " << nbVec / totalTime << " rays/ms." << std::endl;
}
