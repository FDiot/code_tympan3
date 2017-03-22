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

#ifndef EVENT_H
#define EVENT_H


#include <vector>
#include "Tympan/geometric_methods/AcousticRaytracer/Geometry/Sampler.h"
#include "Tympan/geometric_methods/AcousticRaytracer/Base.h"

enum typeevent
{
    SPECULARREFLEXION = 0,
    DIFFRACTION, 
	NOTHING
};

class Shape;

/**
 * \brief Class describing an event (reflection, diffraction, ...)
 */
class Event : public Base
{
public:
	/// Default constructor
    Event(const vec3& position = vec3(0.0, 0.0, 0.0), const vec3& incomingDirection = vec3(0.0, 0.0, 0.0), Shape* _shape = NULL) :
        Base(),
        pos(position),
        from(incomingDirection),
        nbResponseLeft(0),
        initialNbResponse(0),
        sampler(NULL),
        shape(_shape)
    {
        name = "unknown event";
    }
    /// Copy constructor
    Event(const Event& other) : Base(other)
    {
        pos = vec3(other.pos);
        from = vec3(other.pos);
        nbResponseLeft = other.nbResponseLeft;
        initialNbResponse = other.initialNbResponse;
        type = other.type;
        shape = other.shape;
        if (other.sampler) { sampler = new Sampler(*(other.sampler)); }
        else { sampler = NULL; }
    }
    /// Destructor
    virtual ~Event()
    {
        if (sampler) { delete sampler; }
    };

    // Unused function:
    // virtual bool isDiffuse() { return false; }

    /*!
     * \fn const vec3& getPosition() const
     * \brief Return a reference to the event location point.
     * \return Vector with coordinates of the point.
    */
    const vec3& getPosition() const { return pos; }

    /*!
    * \fn void setPosition(vec3 &_pos)
    * \brief Set a new point to the event.
    * \param _pos : New point.
    */
    void setPosition(const vec3& _pos) { pos = vec3(_pos); }

    /*!
    * \fn vec3* getIncomingDirection()
    * \brief Return the direction of the incoming vector.
    * \return The direction of the incoming vector
    */
    vec3 getIncomingDirection() { return from; }

    /*!
    * \fn void setIncomingDirection(const vec3& _from)
    * \brief Set a new incoming vector
    * \param _from : New incoming vector
    */
    void setIncomingDirection(const vec3& _from) { from = vec3(_from); }

    /// Return true if it remains rays to launch
    bool isReponseLeft() { return nbResponseLeft > 0; }
    /// Return the number of remaining rays to launch
    int getNbResponseLeft() { return nbResponseLeft; }
    /// Set the number of remaining rays to launch
    virtual void setNbResponseLeft(int _nbResponseLeft) { nbResponseLeft = _nbResponseLeft; }
    /// Return the number of rays to launch after event
    virtual int getInitialNbResponseLeft() const { return initialNbResponse; }

    /*!
    * \fn Shape* getShape()
    * \brief Return the primitive of the impact
    * \return The impacted primitive of the event
    */
    Shape* getShape() { return shape; }

    const Shape* getShape() const { return shape; }

    /*!
    * \fn void setShape(Shape* _shape)
    * \brief Set the impacted primitive of the event
    */
    void setShape(Shape* _shape) { shape = _shape; }

    /*!
    * \brief Return a direction vector for the impact. Ray direction vector in response of the event.
    * \return New direction vector
    * \warning Should be overloaded by the inherited classes of Event
    */
    virtual bool getResponse(vec3& r, bool force = false) { return false; }

    /*!
    * \fn virtual bool isAcceptableResponse(vec3& test)
    * \brief Return true if the ray direction vector in response of the event is acceptable.
    * \return True if the response is acceptable
    * \warning Should be overloaded by the inherited classes of Event
    */
    virtual bool isAcceptableResponse(vec3& test) { return false; }

    virtual bool generateTest(std::vector<vec3>& succededTest, std::vector<vec3>& failTest, unsigned int nbResponses) { return true; }

    virtual bool generateResponse(std::vector<vec3>& responses, unsigned int nbResponses) { return false; }

    virtual bool appendTarget(vec3 target, bool force = false) { return false; }

    /*!
    * \fn virtual int getType()
    * \brief Return the event type
    * \return Event type
    * \warning Should be overloaded by the inherited classes of Event
    */
    virtual int getType() const { return type; }

    /*!
     * \fn virtual void setType()
     * \brief Added by DTh to make possible simple test of rays
     */
    virtual void setType(const typeevent& _type) { type = _type; }

    /// Get incident angle
    virtual double getAngle() const { return 0.0; }

    /*!
    * \fn const decimal distance(const Event &other) const
    * \brief Return distance from another event
    * \param other : event from which distance must be measured
    */
    const decimal distance(const Event& other) const { return this->pos.distance(other.getPosition()); }


protected:
    vec3 pos;              //!< Location point of the event
    vec3 from;             //!< Direction vector of the incoming ray
    int nbResponseLeft;    //!< Number of remaining rays to launch
    int initialNbResponse; //!< Number of rays to launch after event
    Sampler* sampler;      //!< Pointer to a Sampler
    Shape* shape;          //!< The impact primitive
    typeevent type;        //!< Event type 

#ifdef _ALLOW_TARGETING_
    std::vector<vec3> targets;
#endif
};

#endif
