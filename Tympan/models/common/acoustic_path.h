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

#ifndef TY_MC_ACOUSTIC_PATH
#define TY_MC_ACOUSTIC_PATH

#include "Tympan/models/common/3d.h"

enum ACOUSTIC_EVENT_TYPES
{
    TY_NO_TYPE = 0,
    TYDIFFRACTION = 1,
    TYREFLEXION = 2,
    TYREFLEXIONSOL = 4,
    TYREFRACTION = 8,
    TYSOURCE = 16,
    TYRECEPTEUR = 32,
    TY_ALL_TYPE = TYDIFFRACTION | TYREFLEXION | TYREFLEXIONSOL | TYREFRACTION | TYSOURCE | TYRECEPTEUR
};

class Event;

/*!
* \class acoustic_event
* \brief This class store data and provide functions to manipulate event in the acoustic context
*/
class acoustic_event
{
public:
	/// Default constructor
    acoustic_event();
    /// Constructor from the event position
    acoustic_event(const OPoint3D& pt);
    /// Copy constructor
    acoustic_event(const acoustic_event& ev);
    /// Destructor
    ~acoustic_event();

    acoustic_event& operator=(const acoustic_event& other);

    OPoint3D pos;           		//!< Event position
    double distNextEvent;   		//!< Distance between this event and the next one in TYRay's list of events
    double distEndEvent;    		//!< Distance between this event and the next event needed for calculating (for example, reflection after a diffraction)
    double distPrevNext;    		//!< Distance between event-1 and event +1
    double angle;           		//!< Ray incident angle (for a shooting angle - plan x,z -)
    double angletheta;      		//!< Shooting angle on a horizontal plane (x,y)
    ACOUSTIC_EVENT_TYPES type;      //!< Event type */
    int idFace1;            		//!< Face id on which the event happens (reflection & diffraction)
    int idFace2;            		//!< Face id on which the event happens (diffraction only)
    acoustic_event* previous;   	//!< Pointer to the previous event in TYRay's list of events
    acoustic_event* next;       	//!< Pointer to the next event in TYRay's list of events
    acoustic_event* endEvent;   	//!< Pointer to the next event in TYRay's list of events needed for calculating (for example reflection after a diffraction)
};

typedef std::vector<acoustic_event*> tab_acoustic_events;

class Ray;
class IGeometryModifier;

/*!
* \brief Acoustic path
*/
class acoustic_path
{
public:
	/// Default constructor
	acoustic_path();
	/// Destructor
	virtual ~acoustic_path();

	/*!
    * \fn acoustic_path(unsigned int source_idx, unsigned int receptor_idx, tab_acoustic_events &_events);
    * \brief Build the acoustic path with a source id, a receptor id and
    *        a list of acoustic events positions (reflection,  diffraction, etc...)
    */
    acoustic_path(unsigned int source_idx, unsigned int receptor_idx, tab_acoustic_events &_events);

    /*!
    * \fn acoustic_path( const acoustic_path &ray)
    * \brief Copy constructor of a acoustic path
    */
    acoustic_path(const acoustic_path& ray);

    /*!
     * \fn void cleanEventsTab();
     * \brief clean tab of events
     */
    virtual void cleanEventsTab();

    /*!
     * \fn acoustic_path& operator=(const acoustic_path& other)
     * \brief equal operator
     */
    virtual acoustic_path& operator=(const acoustic_path& other);

    /*!
     * \fn bool deepCopy(acoustic_path *pOther)
     * \brief Deep copy of a ray mainly the events tab
     */
    virtual bool deepCopy(acoustic_path* pOther);

    /*!
     * \fn void tyRayCorrection(IGeometryModifier& transformer);
     * \brief Curve acoustic_path with respect to meteo influence
     *        This is only for watching curved rays on screen
     */
    virtual void tyRayCorrection(IGeometryModifier* transformer);

    /*!
    * \fn void setSource(unsigned int source_idx_, OPoint3D& globalPosition);
    * \brief Set the ray source.
    */
    virtual void setSource(unsigned int source_idx_, OPoint3D& globalPosition);

    /*!
    * \fn void setSource(unsigned int source_idx_)
    * \brief Set the ray source.
    */
    virtual void setSource(unsigned int source_idx_) { source_idx = source_idx_; }


    /*!
    * \fn virtual unsigned int getSource_idx()
    * \brief Get the source id
    */
    virtual unsigned int getSource_idx() {return source_idx;}

    /*!
    * \fn void setRecepteur(unsigned  int receptor_idx_, OPoint3D& globalPosition)
    * \brief Set the ray receptor. The last polyline point is updated
    */
    virtual void setRecepteur(unsigned  int receptor_idx_, OPoint3D& globalPosition);

    /*!
    * \fn void setRecepteur(unsigned  int receptor_idx_)
    * \brief Set the ray receptor.
    */
    virtual void setRecepteur(unsigned  int receptor_idx_) { receptor_idx = receptor_idx_; }


    /*!
    * \fn unsigned int getRecepteur_idx()
    * \brief Get the receptor id.
    */
    virtual unsigned int getRecepteur_idx() {return receptor_idx;}

    /*!
    * \fn void setIdentifiant(unsigned int _id)
    * \brief Set the ray id.
    */
    virtual void setIdentifiant(unsigned int id) {_identifiant = id;}

    /*!
    * \fn int getIdentifiant()
    * \brief Get the ray id.
    */
    virtual unsigned int getIdentifiant() const {return _identifiant;}

    /*!
    * \fn void addEvent(Event &_event)
    * \brief Add an event to the events list of the ray.
    */
    virtual void addEvent(acoustic_event* TYEvent) {_events.push_back(TYEvent);}

    /*!
    * \fn std::vector<Event>& getEvents()
    * \brief Get the events list of the ray.
    */
    virtual tab_acoustic_events& getEvents() {return _events;}
    virtual const tab_acoustic_events& getEvents() const { return _events; }

    /*!
    * \brief Set the events list of the ray.
    * \warning The first acoustic event should match the source and
	* \warning and the last one should match the receptor.
    */
    virtual void setEvents(tab_acoustic_events& tabEvents) { _events = tabEvents; }

    /*!
     * \fn vector<unsigned int> getIndexOfEvents(const int& eventType)
     * \brief return a tab of indexes of events of the same type in a ray
     *        you can merge two types of events (example TYREFLEXION | TYRECEPTEUR)
              vector returned count all events of the two types
     */
    virtual std::vector<int> getIndexOfEvents(const int& eventType) const;

    /*!
     * \fn unsigned int getNbEventType(const ACOUSTIC_EVENT_TYPES& eventType)
     * \brief return the number of events of a type
     */
    virtual unsigned int getNbEventType(const ACOUSTIC_EVENT_TYPES& eventType) { return getIndexOfEvents(eventType).size(); }


    /*!
     * \fn void copyEvents(ACOUSTIC_EVENT_TYPES eventType);
     * \brief copy only events matching eventType to _events tab
     */
    virtual void copyEvents(const acoustic_path* tyRay, ACOUSTIC_EVENT_TYPES eventType);

    /*!
     * \fn void setNextDistance(const ACOUSTIC_EVENT_TYPES& eventType);
     * \brief Compute distance between events of the type "eventType"
     *        and set distNextEvent to each event matching enventType
     */
    virtual void setNextDistance(ACOUSTIC_EVENT_TYPES eventType);

    /*!
     * \fn void setAngles(ACOUSTIC_EVENT_TYPES eventType)
     * \brief Compute angles of incoming ray segment at event point
     */
    virtual void setAngles(ACOUSTIC_EVENT_TYPES eventType);

    /*!
    * \fn double getLength()
    * \brief Return total length of the ray taking account of all events
    */
    virtual double getLength();

    /*!
    * \fn int getnbEvents()
    * \brief Return the number of events of the ray.
    */
    virtual int getnbEvents() {return _events.size();}

    /*!
    * \fn void setPosSourceGlobal(const OPoint3D& pos)
    * \brief Set the source position in the global space
    */
    virtual void setPosSourceGlobal(const OPoint3D& pos) { _posSourceGlobal = pos; }

    /*!
    * \fn OPoint3D& getPosSourceGlobal()
    * \brief Get the source position in the global space
    */
    virtual OPoint3D& getPosSourceGlobal() { return _posSourceGlobal; }

    /*!
    * \fn void setPosReceptGlobal(const OPoint3D& pos)
    * \brief Set the receptor position in the global space
    */
    virtual void setPosReceptGlobal(const OPoint3D& pos) { _posReceptGlobal = pos; }

    /*!
    * \fn OPoint3D& getPosReceptGlobal()
    * \brief Get the receptor position in the global space
    */
    virtual OPoint3D& getPosReceptGlobal() { return _posReceptGlobal; }

    /*!
    * \fn void overSample(TYRay* tyRay);
    * \brief Add events to ray
    */
    virtual void overSample(IGeometryModifier* transformer, const double& dMin);

    /*!
    * \fn void sampleAndCorrection()
    * \brief Computes angle and length correction
    * \ by calling the three previous functions
    * \ Creates two matrix which have corrected lengths and angles
    */
    virtual void sampleAndCorrection(IGeometryModifier* transformer);

    /*!
     * \fn void endLenghtCompute(TYRay *tyRay)
     * \brief compute the length between an event and the next pertinent event
     *        (i.e. between a diffraction and the next reflection or receptor)
     */
    virtual void endLenghtCompute(IGeometryModifier* transformer);

    /*!
     * \fn void angleCompute(TYRay *tyRay)
     * \brief compute the angle between incident ray and the face
     */
    virtual void angleCompute(IGeometryModifier* transformer);

    /*!
     * \fn void nextLenghtCompute(TYRay *tyRay)
     * \brief compute the length between an event and the next event
     */
    virtual void nextLenghtCompute(IGeometryModifier* transformer);

    /*!
     * \fn void prevNextLengthCompute(TYRay *tyRay)
     * \brief Computes the length between event-1 and event+1
     */
    virtual void prevNextLengthCompute(IGeometryModifier* transformer);

    /*!
     * \brief compute the length between an event and the next event
     */
    virtual void eventPosCompute(IGeometryModifier* transformer);

    /*!
    * \fn void lengthCorrection()
    * \brief Computes length correction on path
    * \ Works on over-sampled TYRays
    * \ Creates a vector which has all corrected path lengths
    */
    virtual double lengthCorrection(acoustic_event* ev1, const acoustic_event* ev2, IGeometryModifier* transformer);

    /*!
    * \fn void angleCorrection()
    * \brief Computes angle correction on path
    *        ev1 -> previous event
    *        ev2 -> event to which compute angle
    *        ev3 -> next event
    */
    virtual double angleCorrection(const acoustic_event* ev1, acoustic_event* ev2, const acoustic_event* ev3, IGeometryModifier* transformer);

    /// Set the sampler step
	static void set_sampler_step(double sampler_step_) { sampler_step = sampler_step_; }

    /*!
     * \fn void build_links_between_events();
     * \brief TYRayEvent has to know is direct neighbourg (before and after him)
     * \brief It also has to know his the next REFLEXION event or (if not exist) the RECEPTOR
     */
    void build_links_between_events();

    /*!
     * \fn void compute_shot_angle();
     * \brief Compute shot angle from source
     */
    void compute_shot_angle();

protected:
	static double sampler_step;		//!< max size of step between events after spatial sampling
	unsigned int _identifiant;		//!< Ray id
    unsigned int source_idx;        //!< Source id
    unsigned int receptor_idx;      //!< Receptor id
    OPoint3D _posSourceGlobal;      //!< Source position in the global space
    OPoint3D _posReceptGlobal;      //!< Receptor position in the global space

    tab_acoustic_events _events;	//!< Events vector containing the events list (and their positions) of the associated ray.
};

typedef std::vector<acoustic_path*> tab_acoustic_path;

#endif //TY_MC_ACOUSTIC_PATH
