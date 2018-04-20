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

#ifndef DEFAULT_ENGINE_H
#define DEFAULT_ENGINE_H

#include "Engine.h"
#include <stack>

/**
 * \brief Default Engine class
 */
class DefaultEngine : public Engine
{
#ifdef TEST_ACCELERATION_RECEPTORS
public:
	/// Constructors
    DefaultEngine() : Engine() { nbRayonsTraites = 0;}

    DefaultEngine(Scene* _scene, std::vector<Source> *_sources, Solver* _solver, Scene *_recepteurs)
        : Engine(_scene, _sources, _solver, _recepteurs) {  }    
    /// Copy constructor
    DefaultEngine(const DefaultEngine& other)
    {
        scene = other.scene;
        sources = other.sources;
        solver = other.solver;
        recepteurs = other.recepteurs;
    }
    /// Destructor
    virtual ~DefaultEngine() { }

    virtual bool process();

    virtual void runStructureBenchmark();

    Ray* genRay();                                                  //!< Create rays from the sources


protected:
    /**
     * \brief Search if a ray intersects a receptor before traveling a tmin distance 
     * (NB: this is the only place in the code where a ray is added to the solver's list of valid rays)
     * @param tmin maximum distance the ray can travel before hiting a receptor for the intersection to be considered valid
     * @param r The ray
     */
	void searchForReceptor(const decimal &tmin, Ray *r)
	{
        //Get the receptors' accelerating structure
        Accelerator* accelerator = recepteurs->getAccelerator();
        std::list<Intersection> foundPrims;

        //Find intersections with receptors
        accelerator->traverse(r, foundPrims);
        std::list<Intersection>::iterator iter;

        //For each found intersections
        for (iter=foundPrims.begin(); iter != foundPrims.end(); iter++)
        {

            Intersection its = (*iter);
   			Intersection result;
	
            vec3 position=r->getPosition();
   			vec3 direction=r->getDirection();

   			//If the ray intersect the receptor before traveling a tmin distance
            if ( dynamic_cast<Recepteur*>(its.p)->intersectionRecepteur(position, direction, tmin, result) )
            {
				// Create a copy of the ray
				Ray* valide_ray = new Ray(r);
				valide_ray->setConstructId (rayCounter);
				
				rayCounter++;

				// Set the ray's receptor
				valide_ray->setRecepteur ( dynamic_cast<Recepteur*>(its.p));

				//Compute the final position of the ray based on its direction and the distance traveled
				valide_ray->setFinalPosition ( valide_ray->getPosition() + valide_ray->getDirection() * result.t);
				valide_ray->computeLongueur();

				//Add the ray to the solver's list of valid rays
				solver->valideRayon(valide_ray);
            }            
        }
    }
    /**
     * \brief Initialize the rays treatment stack by a loop on the receptors
     */
	void initialReceptorTargeting()
	{
		for (vector<Source>::iterator itsource = sources->begin(); itsource != sources->end(); itsource++)
		{
            std::vector<Shape*> *tabReceptors = recepteurs->getShapes();
			for (std::vector<Shape*>::iterator itrecp = tabReceptors->begin(); itrecp != tabReceptors->end(); itrecp++)
			{
				Ray* new_ray = new Ray();
				new_ray->setConstructId ( rayCounter);
				rayCounter++;
				new_ray->setSource ( (&(*itsource)) );
				new_ray->setPosition ( itsource->getPosition());
				vec3 psource = itsource->getPosition();
				vec3 precp = dynamic_cast<Sphere*>( (*itrecp) )->getPosition();
				vec3 direction=precp - psource;
				direction.normalize();
				new_ray->setDirection(direction);
				new_ray->setMint ( 0. );
				new_ray->setMaxt ( 10000.);
				pile_traitement.push(new_ray);
			}
		}	
	}
#else
public:
    DefaultEngine() : Engine() { nbRayonsTraites = 0;}

    DefaultEngine(Scene* _scene, std::vector<Source> *_sources, Solver* _solver, std::vector<Recepteur> *_recepteurs)
        : Engine(_scene, _sources, _solver, _recepteurs) {  }

    DefaultEngine(const DefaultEngine& other)
    {
        scene = other.scene;
        sources = other.sources;
        solver = other.solver;
        recepteurs = other.recepteurs;
    }

    virtual ~DefaultEngine() { }

    virtual bool process();

    virtual void runStructureBenchmark();
    
    Ray* genRay();                                                  //!< Create rays from the sources


protected :
    void searchForReceptor(const decimal &tmin, Ray *r)
	{
		for (unsigned int i = 0; i < recepteurs->size(); i++)
		{
			Intersection result;
			//if the ray hits a receptor before traveling tmin
			if (recepteurs->at(i).intersectionRecepteur(r->position, r->direction, tmin, result))
			{
				
				// Create a copy of the ray
				Ray* valide_ray = new Ray(r);
				valide_ray->setConstructId (rayCounter);

				rayCounter++;

				// Set the ray's receptor
				valide_ray->setRecepteur ( dynamic_cast<Recepteur*>(its.p));

				//Compute the final position of the ray based on its direction and the distance traveled
				valide_ray->setFinalPosition ( valide_ray->getPosition() + valide_ray->getDirection() * result.t);
				valide_ray->computeLongueur();

				//Add the ray to the solver's list of valid rays
				solver->valideRayon(valide_ray);
			}
		}
	}

	void initialReceptorTargeting()
	{
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
	}
#endif
protected :
	/**
	 * \brief Copy a ray and use its last event to generate a response to use as the copy's direction
	 * (used to handle the generation of rays by diffraction events)
	 * @param r Return modified ray
	 */
	void copyRayAndAddToStack(Ray *r)
	{
        //Copy ray
        Ray* copie = new Ray(r);
        copie->setConstructId ( rayCounter );
        rayCounter++;

        //Generate a response with the event and use it has the copy's direction
        vec3 response;
        r->getEvents()->back()->getResponse(response);
        copie->setDirection(response);

        //Add the copy to the stack
        pile_traitement.push(copie);
	}

     /**
      * \brief Ray treatment method
      * @param r Ray to treat
      * @param result Updated valid rays list
      * @return
      */
    virtual bool traitementRay(Ray* r, std::list<validRay> &result);

    std::stack< Ray*, std::deque <Ray*> > pile_traitement;			//!< Treatment stack containing the rays to treat

    unsigned long long int nbRayonsTraites;							//!< Treated rays number
};

#endif
