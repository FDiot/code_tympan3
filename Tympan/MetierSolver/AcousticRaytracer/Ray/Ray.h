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
 
#ifndef RAY_H
#define RAY_H

#include <QSharedPointer>
#include "Tympan/MetierSolver/AcousticRaytracer/Acoustic/Source.h"
//#include "Tympan/MetierSolver/AcousticRaytracer/Acoustic/Recepteur.h"
#include "Tympan/MetierSolver/AcousticRaytracer/Acoustic/Event.h"
#include "Tympan/MetierSolver/AcousticRaytracer/Acoustic/Diffraction.h"

#include "Tympan/MetierSolver/AcousticRaytracer/Base.h"
#include "Tympan/MetierSolver/AcousticRaytracer/Geometry/mathlib.h"
using namespace std;

//#ifdef USE_QT
//  #include "RayGraphic.h"
//#endif

/*!
 * brief : signature describes a ray by a pair of unsigend int. The first one gives the source number (in the range 0-4095)
 *         and the receptor the receptor number (in the range 0-1048576) as a bit field.
 *         The second one decribe the sequences of events by their types (user could decide what 1 represent, may be REFLEXION 
 *         or DIFFRACTION)
 */
typedef std::pair<bitSet, bitSet> signature;

class Ray : public Base
{

public:
    //#ifdef USE_QT
    //  //WIDGET_DECL(Sphere)
    //  GRAPHIC_DECL(Ray)
    //#endif

    Ray() : Base(), position(), direction(), mint(0), maxt(100000), source(NULL), recepteur(NULL), nbReflexion(0), nbDiffraction(0) { name = "unknow ray"; }
    Ray(const vec3& _position, const vec3& _direction) : Base(), position(_position), direction(_direction), mint(0), maxt(100000), source(NULL), recepteur(NULL), nbReflexion(0), nbDiffraction(0)
    { name = "unknow ray";}

    Ray(const Ray& other) : Base(other)
    {
        position = vec3(other.position);
        direction = vec3(other.direction);
        mint = other.mint;
        maxt = other.maxt;
        source = other.source;
        recepteur = other.recepteur;
        constructId = other.constructId;
        for (unsigned int i = 0; i < other.events.size(); i++)
        {
            events.push_back(other.events.at(i));
        }

        nbDiffraction = other.nbDiffraction;
        nbReflexion = other.nbReflexion;
    }

    Ray(Ray* other)
    {
        position = vec3(other->position);
        direction = vec3(other->direction);
        mint = other->mint;
        maxt = other->maxt;
        source = other->source;
        recepteur = other->recepteur;
        constructId = other->constructId;
        for (unsigned int i = 0; i < other->events.size(); i++)
        {
            events.push_back(other->events.at(i));
        }

        nbDiffraction = other->nbDiffraction;
        nbReflexion = other->nbReflexion;
    }

    virtual ~Ray() { }

    /*!
    * \fn void computeLongueur()
    * \brief Calcul la distance parcourue par le rayon et place le resultat dans longueur
    */
    void computeLongueur();

	/*!
	 * \fn decimal computeTrueLength(vec3& nearestPoint);
	 * \brief Compute ray lenfth from source to the nearest point of the receptor
	 */
	decimal computeTrueLength(vec3& closestPoint);

	/*!
	 * \fn decimal computePertinentLength(vec3& nearestPoint);
	 * \brief Compute ray lenfth from last pertinent event (i.e. source or last diffraction 
	 *        to the nearest point of the receptor
	 */
	decimal computePertinentLength(vec3& closestPoint);

	/*!
	 * \fn void* getLastPertinentEventPos() const;
	 * \brief return the apointer to the last pertinent event influencing ray thickness
	 *        may be source or last diffraction event
	 */
	Base* getLastPertinentEvent();

    /*!
    * \fn double getLongueur()
    * \brief Renvoie la distance parcourue par le rayon
    * \return Distance parcourue par le rayon.
    */
    double getLongueur() const {return longueur;}

    /*!
    * \fn unsigned int getDiff()
    * \brief Renvoie le nombre de diffractions realisees par le rayon parcourue par le rayon
    * \return Nombre de diffractions realisees par le rayon.
    */
    unsigned int getDiff() { return nbDiffraction; }

    /*!
    * \fn unsigned int getReflex()
    * \brief Renvoie le nombre de reflexions realisees par le rayon parcourue par le rayon
    * \return Nombre de reflexions realisees par le rayon.
    */
    unsigned int getReflex() { return nbReflexion; }

	/*!
	 * \fn unsigned int getNbEvents(); 
	 * \brief Return the total number of events
	 */
	unsigned int getNbEvents() { return nbDiffraction + nbDiffraction; }

    /*!
    * \fn std::vector<QSharedPointer<Event> >* getEvents()
    * \brief Renvoie le tableau des evenements rencontres par le rayon
    * \return Tableau des evenements rencontres par le rayon.
    */
    std::vector<QSharedPointer<Event> >* getEvents() { return &events; }

	/*!
    * \fn const std::vector<QSharedPointer<Event> >* getEvents() const
    * \brief Renvoie le tableau des evenements rencontres par le rayon
    * \return Tableau des evenements rencontres par le rayon.
    */
    const std::vector<QSharedPointer<Event> >* getEvents() const { return &events; }

    /*!
    * \fn float distanceSourceRecepteur()
    * \brief Calcul la distance entre une source et un recepteur
    * \return Distance entre la source et le recepteur associes au rayon
    */
    float distanceSourceRecepteur();

    vector<unsigned int>getFaceHistory();

    vector<unsigned int> getPrimitiveHistory();

    /*!
    * \fn Source* getSource()
    * \brief Renvoie la source d'un rayon
    * \return Pointeur vers la source du rayon
    */
    Source* getSource() { return source; }

    /*!
    * \fn Recepteur* getRecepteur()
    * \brief Renvoie le recepteur d'un rayon.
    * \return Pointeur vers le recepteur rayon. Renvoie NULL si le rayon n'a pas de recepteur associe
    */
    void* getRecepteur() { return recepteur; }

	/*!
	 * \fn signature getSignature(const typeevent& ev = SPECULARREFLEXION);
	 * \brief Compute the signature (i.e. std::pair<unsigned int, unsigned int>) of the ray)
	 */
	signature getSignature(const typeevent& typeEv = SPECULARREFLEXION);

	/*!
	 * \fn decimal getThick( const decimal& distance, const decimal& solid_angle, const unsigned int& nb_rays, bool diffraction = false) const;
	 * \brief Compute thickness of the ray after covering a distance distance for spherical or diffraction source
	 */
	inline decimal getThickness( const decimal& distance)
	{
		bool diffraction(false);
		decimal angle = getSolidAngle( diffraction );

		if ( diffraction )
		{
			return 2 * M_PI * angle * distance;
		}

		return 2. * distance * sqrt( angle / M_PI ); 
	}

	/*!
	 * \fn decimal getSolidAngle( bool& diffraction)
	 * \brief	Compute solid angle associated with the ray
	 *			Set diffraction true if last pertinent event 
	 *			is a diffraction
	 */
	inline decimal getSolidAngle( bool& diffraction)
	{
		unsigned int nb_rays = source->getInitialRayCount();

		Base *last = getLastPertinentEvent();
		Event *e = dynamic_cast<Event*>(last);

		if ( e && ( e->getType() == DIFFRACTION ) )
		{
			diffraction = true;
			return dynamic_cast<Diffraction*>(e)->getDeltaTheta();
		}
		
		diffraction = false;
		return M_4PI / static_cast<decimal>(nb_rays);
	}

	/*!
	 * \fn bitSet getSRBitSet(const unsigned& int source_id, const unsigned int & receptor_id);
	 * \brief compute the bitSet associated with a source and a receptor
	 */
	inline bitSet getSRBitSet(const unsigned int& source_id, const unsigned int & receptor_id)
	{
		// The source id is stored in 12 bits, receptor is stored in 20 bits
		assert( (source_id < 4096) && (receptor_id < 1048576) );
		bitSet SR = source_id;
		SR = SR << 20;
		return SR += receptor_id;
	}

	/*!
	 * \fn bitSet getEventsBitSet(const typeevent& typeEv);
	 * \brief compute the bitSet associated with a list of events of type evType
	 */
	bitSet getEventsBitSet(const typeevent& typeEv);

	/*!
	 * \fn decimal coveredDistance(unsigned int current_indice, unsigned int initial_indice = 0);
	 * \brief Gives the distance covered by the ray between two events
	 *        by default from the source
	 */
	decimal coveredDistance(const unsigned int& current_indice, unsigned int initial_indice = 0, const bool &from_source = true) const;

public:
    vec3 position;                              /*!< Point de depart du rayon */
    vec3 direction;                             /*!< vecteur directeur du rayon a la source */
    vec3 finalPosition;                         /*!< Point d'arrivee du rayon */
    decimal mint;
    decimal maxt;
    Source* source;                             /*!< Pointeur vers la source du rayon */
    void* recepteur;                            /*!< Pointeur vers le recepteur du rayon */
    std::vector<QSharedPointer<Event> > events; /*!< Liste d'evenements subis par le rayon */
    decimal longueur;                           /*!< Distance parcourue par le rayon */
    unsigned long long int constructId;         /*!< Identifiant du rayon */
    unsigned int nbReflexion;                   /*!< Nombre de reflexions subis par le rayon */
    unsigned int nbDiffraction;                 /*!< Nombre de diffractions subis par le rayon */
};


#endif
