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

#include "Tympan/solvers/AcousticRaytracer/Base.h"
#include "Tympan/models/common/mathlib.h"
#include "Tympan/solvers/AcousticRaytracer/Acoustic/Source.h"
#include "Tympan/solvers/AcousticRaytracer/Acoustic/Event.h"
#include "Tympan/solvers/AcousticRaytracer/Acoustic/Diffraction.h"

using namespace std;


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
    Ray() : Base(), position(), direction(), mint(0), maxt(100000), source(NULL), recepteur(NULL), nbReflexion(0), nbDiffraction(0), cumulDistance(0.), cumulDelta(0.)
	{ 
		name = "unknow ray"; 
	}

    Ray(const vec3& _position, const vec3& _direction) : Base(), position(_position), direction(_direction), mint(0), maxt(100000), source(NULL), recepteur(NULL), nbReflexion(0), nbDiffraction(0), cumulDistance(0.), cumulDelta(0.)
    { 
		name = "unknow ray";
	}

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
		cumulDistance = other.cumulDistance;
		cumulDelta = other.cumulDelta;
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
		cumulDistance = other->cumulDistance;
		cumulDelta = other->cumulDelta;

    }

    virtual ~Ray() { }

	/*!
	 * \fn decimal computeEventsSequenceLength()
	 * \brief compute the length of the sequence of events 
	 */
	decimal computeEventsSequenceLength();

	/*!
    * \fn void computeLongueur()
    * \brief Calcul la distance parcourue par le rayon et place le resultat dans longueur
    */
    void computeLongueur();

    /*!
     * \fn decimal computeTrueLength(vec3& nearestPoint);
	 * \brief	Compute ray lenfth from source to the nearest point 
	 *			of the "event" located at ref position
     */
	decimal computeTrueLength(const vec3& ref, const vec3& lastPos, vec3& closestPoint);

    /*!
     * \fn decimal computePertinentLength(vec3& nearestPoint);
     * \brief Compute ray lenfth from last pertinent event (i.e. source or last diffraction
	 *        to the nearest point of the "event" located at ref position
     */
	decimal computePertinentLength(const vec3& ref, const vec3& lastPos, vec3& closestPoint);

    /*!
	 * \fn void* getLastPertinentEventOrSource(typeevent evType = DIFFRACTION) const;
	 * \brief Return a pointer to the last event of type evType or source if none
     */
	Base* getLastPertinentEventOrSource(typeevent evType = DIFFRACTION);

	/*!
	 * \fn vec3 computeLocalOrigin( Base *ev);
	 * \brief Return position of event found by getLastPertinentEventOrSource()
	 */
	inline vec3 computeLocalOrigin( Base *ev)
	{
		if ( dynamic_cast<Source*>(ev) )
		{
			return dynamic_cast<Source*>(ev)->getPosition();
		}
		else // that's a standard event
		{
			return dynamic_cast<Event*>(ev)->getPosition();
		}
	}

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
	 * \fn decimal getThick( const decimal& distance, bool diffraction) const;
     * \brief Compute thickness of the ray after covering a distance distance for spherical or diffraction source
     */
	decimal getThickness( const decimal& distance, bool diffraction);

    /*!
	 * \fn decimal getSolidAngle( bool &diffraction)
     * \brief   Compute solid angle associated with the ray
     *          Set diffraction true if last pertinent event
     *          is a diffraction
     */
	decimal getSolidAngle( bool &diffraction );

    /*!
     * \fn bitSet getSRBitSet(const unsigned& int source_id, const unsigned int & receptor_id);
     * \brief compute the bitSet associated with a source and a receptor
     */
    inline bitSet getSRBitSet(const unsigned int& source_id, const unsigned int& receptor_id)
    {
        // The source id is stored in 12 bits, receptor is stored in 20 bits
        assert((source_id < 4096) && (receptor_id < 1048576));
        bitSet SR = source_id;
        SR = SR << 20;
        return SR += receptor_id;
    }

    /*!
     * \fn bitSet getEventsBitSet(const typeevent& typeEv);
     * \brief compute the bitSet associated with a list of events of type evType
     */
    bitSet getEventsBitSet(const typeevent& typeEv);

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
	decimal cumulDistance;						/*!< Distance cumulee parcourue par le rayon calculee à chaque étape */
	decimal cumulDelta;							/*!< Difference de marche cumulé par le rayon calculee à chaque etape */
};


#endif
