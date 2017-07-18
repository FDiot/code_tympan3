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

#include <memory>

#include "Base.h"
#include "Geometry/mathlib.h"
#include "Acoustic/Source.h"
#include "Acoustic/Event.h"
#include "Acoustic/Diffraction.h"

using namespace std;


typedef std::pair<bitSet, bitSet> signature;
/**
 * \brief : Describes a ray by a pair of unsigned int. The first one gives the source number (in the range 0-4095)
 *         and the receptor number (in the range 0-1048576) as a bit field.
 *         The second one describes the sequences of events by their types (user could decide what 1 represent, may be REFLEXION
 *         or DIFFRACTION)
 */
class Ray : public Base
{

public:
	/// Constructors
    Ray() : Base(), position(), direction(), mint(0), maxt(100000), source(NULL), recepteur(NULL), nbReflexion(0), nbDiffraction(0), cumulDistance(0.), cumulDelta(0.)
	{ 
		name = "unknow ray"; 
	}

    Ray(const vec3& _position, const vec3& _direction) : Base(), position(_position), direction(_direction), mint(0), maxt(100000), source(NULL), recepteur(NULL), nbReflexion(0), nbDiffraction(0), cumulDistance(0.), cumulDelta(0.)
    { 
		name = "unknow ray";
	}
    /// Copy constructors
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
    /// Destructor
    virtual ~Ray() { }

	/*!
	 * \fn decimal computeEventsSequenceLength()
	 * \brief Compute the length of the sequence of events
	 */
	decimal computeEventsSequenceLength();

	/*!
    * \fn void computeLongueur()
    * \brief Compute the distance traveled (length) by the ray and the result is set into the longueur attribute
    */
    void computeLongueur();

    /*!
     * \fn decimal computeTrueLength(const vec3& ref, const vec3& lastPos, vec3& closestPoint);
     * \brief	Compute ray length from source to the nearest point
     *			of the "event" located at ref position
     * \param ref
     * \param lastPos
     * \param closestPoint
     */
	decimal computeTrueLength(const vec3& ref, const vec3& lastPos, vec3& closestPoint);

    /*!
     * \fn decimal computePertinentLength(const vec3& ref, const vec3& lastPos, vec3& closestPoint);
     * \brief Compute ray length from last pertinent event (i.e. source or last diffraction
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
    * \brief Return the traveled distance by the ray
    * \return Traveled distance by the ray.
    */
    double getLongueur() const {return longueur;}

    /*!
    * \fn unsigned int getDiff()
    * \brief Return the diffractions number encountered by the ray.
    * \return Reflections diffractions encountered by the ray.
    */
    unsigned int getDiff() { return nbDiffraction; }

    /*!
    * \fn unsigned int getReflex()
    * \brief Return the reflections number encountered by the ray.
    * \return Reflections number encountered by the ray.
    */
    unsigned int getReflex() { return nbReflexion; }

    /*!
     * \fn unsigned int getNbEvents();
     * \brief Return the total number of events
     */
    unsigned int getNbEvents() { return nbDiffraction + nbReflexion; }

    /*!
    * \fn std::vector<std::shared_ptr<Event> >* getEvents()
    * \brief Return the events array encountered by the ray
    * \return Events array encountered by the ray.
    */
    std::vector<std::shared_ptr<Event> >* getEvents() { return &events; }

    /*!
    * \fn const std::vector<std::shared_ptr<Event> >* getEvents() const
    * \brief Return the events array encountered by the ray
    * \return Events array encountered by the ray.
    */
    const std::vector<std::shared_ptr<Event> >* getEvents() const { return &events; }

    /**
     * \fn getFaceHistory()
     * \brief Return the array of faces id encountered by the ray
     */
    vector<unsigned int>getFaceHistory();

    /**
     * \fn getPrimitiveHistory()
     * \brief Return the array of primitives id encountered by the ray
     */
    vector<unsigned int> getPrimitiveHistory();

    /*!
    * \fn Source* getSource()
    * \brief Return the ray source
    * \return Pointer to the source
    */
    Source* getSource() { return source; }

    /*!
    * \fn Recepteur* getRecepteur()
    * \brief Return the ray receptor.
    * \return Pointer to the ray receptor. NULL if the ray has no associated receptor
    */
    void* getRecepteur() { return recepteur; }

    /*!
     * \fn signature getSignature(const typeevent& ev = SPECULARREFLEXION);
     * \brief Compute the signature (i.e. std::pair<unsigned int, unsigned int>) of the ray)
     */
    signature getSignature(const typeevent& typeEv = SPECULARREFLEXION);

    /*!
     * \fn decimal getThickness( const decimal& distance, bool diffraction);
     * \brief Compute thickness of the ray after covering a distance for spherical or diffraction source
     */
	decimal getThickness( const decimal& distance, bool diffraction);

    /*!
     * \fn decimal getSolidAngle( bool &diffraction)
     * \brief   Compute solid angle associated with the ray
     * \param diffraction Set diffraction true if last pertinent event is a diffraction
     */
	decimal getSolidAngle( bool &diffraction );

    /*!
     * \fn bitSet getSRBitSet(const unsigned& int source_id, const unsigned int & receptor_id);
     * \brief Compute the bitSet associated with a source and a receptor
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
     * \brief Compute the bitSet associated with a list of events of type evType
     */
    bitSet getEventsBitSet(const typeevent& typeEv);

public:
    vec3 position;                              //!< Starting point ray
    vec3 direction;                             //!< Direction vector for the ray at the source
    vec3 finalPosition;                         //!< Ending point of the ray
    decimal mint;
    decimal maxt;
    Source* source;                             //!< Pointer to the source of the ray
    void* recepteur;                            //!< Pointer to the receptor of the ray
    std::vector<std::shared_ptr<Event> > events; //!< Events list for the ray
    decimal longueur;                           //!< Distance traveled by the ray
    unsigned long long int constructId;         //!< Ray id
    unsigned int nbReflexion;                   //!< Reflections number for the ray
    unsigned int nbDiffraction;                 //!< Diffractions number for the ray
    decimal cumulDistance;                      //!< Cumulative distance by the ray computed at each step
    decimal cumulDelta;                         //!< Cumulative walking step difference by the ray computed at each step
};


#endif
