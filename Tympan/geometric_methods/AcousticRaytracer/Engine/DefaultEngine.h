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

protected:
    /**
     * \brief Search if a ray intersects a receptor
     * @param tmin tmin
     * @param r The ray
     */
	void searchForReceptor(const decimal &tmin, Ray *r)
	{
        //Recuperation des structures acceleratrices pour le Solver
        Accelerator* accelerator = recepteurs->getAccelerator();
        std::list<Intersection> foundPrims;

        //Appel du Solver pour le choix de la methode de traverser de la structure
        //decimal tmin_r =  accelerator->traverse(r, foundPrims);
        accelerator->traverse(r, foundPrims);
        std::list<Intersection>::iterator iter;
        for (iter=foundPrims.begin(); iter != foundPrims.end(); iter++)
        {
            Intersection its = (*iter);
   			Intersection result;

            if ( dynamic_cast<Recepteur*>(its.p)->intersectionRecepteur(r->position, r->direction, tmin, result) )
            {
				//Cas ou le rayon touche un recepteur
				Ray* valide_ray = new Ray(r);
				valide_ray->constructId = rayCounter;
				rayCounter++;
				valide_ray->recepteur = dynamic_cast<Recepteur*>(its.p);
				valide_ray->finalPosition = valide_ray->position + valide_ray->direction * result.t;
				valide_ray->computeLongueur();
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
				new_ray->constructId = rayCounter;
				rayCounter++;
				new_ray->source = (&(*itsource));
				new_ray->position = itsource->getPosition();
				vec3 psource = itsource->getPosition();
				vec3 precp = dynamic_cast<Sphere*>( (*itrecp) )->getPosition();
				new_ray->direction = precp - psource;
				new_ray->direction.normalize();
				new_ray->mint = 0.;
				new_ray->maxt = 10000.;
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

protected :
    void searchForReceptor(const decimal &tmin, Ray *r)
	{
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
	 * \brief Add a ray to the treatment stack
	 * @param r Return modified ray
	 */
	void copyRayAndAddToStack(Ray *r)
	{
        //Copie d'un rayon ayant rencontre une diffraction...
        Ray* copie = new Ray(r);
        copie->constructId = rayCounter;
        rayCounter++;
        r->events.back()->getResponse(copie->direction);
        pile_traitement.push(copie);
        //Copie achevee
	}

    Ray* genRay();													//!< Create rays from the sources
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
