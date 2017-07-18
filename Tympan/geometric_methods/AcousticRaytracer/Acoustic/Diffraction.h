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

#ifndef DIFFRACTION_H
#define DIFFRACTION_H

#include <cmath>

#include "Event.h"
#include "Geometry/Repere.h"
#include "Base.h"

class Cylindre;

/**
 * \brief Diffraction class Event
 */
class Diffraction : public Event
{
public:
	/// Constructor
    Diffraction(const vec3& position = vec3(0.0, 0.0, 0.0), const vec3& incomingDirection = vec3(0.0, 0.0, 0.0), Cylindre* c = NULL);
    /// Copy constructor
    Diffraction(const Diffraction& other);
    /// Destructor
	virtual ~Diffraction() {}

    virtual void setNbResponseLeft(int _nbResponseLeft) 
	{ 
		nbResponseLeft = static_cast<unsigned int>( std::floor( static_cast<decimal>(_nbResponseLeft-1) / M_2PI * angleOuverture + 0.5 ) );
		nbResponseLeft = nbResponseLeft >= 4 ? nbResponseLeft : 4; // Limite le nombre � 4
		initialNbResponse = nbResponseLeft;
		computeDTheta(); 
	}

	virtual bool getResponse(vec3& r, bool force = false);

	/// Set/Get aperture angle
    void setAngleOuverture(decimal angle) { angleOuverture = angle; computeDTheta(); }
    decimal getAngleOuverture() { return angleOuverture; }
    /// Get the angle between 2 rays to send
	decimal getDeltaTheta() const { return delta_theta; }
	/// Get the incident angle
    virtual double getAngle() { return angleArrive; }
    virtual bool generateTest(std::vector<vec3>& succededTest, std::vector<vec3>& failTest, unsigned int nbResponses);
    /// Get the local frame
	virtual const Repere& getRepere() const { return localRepere; }

#ifdef _ALLOW_TARGETING_
    virtual bool isAcceptableResponse(vec3& test);
    virtual bool generateResponse(std::vector<vec3>& responses, unsigned int nbResponses);
    virtual bool appendTarget(vec3 target, bool force = false);
#endif //_ALLOW_TARGETING_

protected:
    /// Filter generated response (or not)
	bool (*responseValidator) (const vec3&, const vec3&, const vec3&, vec3 &);
	/// Get theta
	void (*getTheta) (const decimal&, const decimal&, const decimal&, decimal&);
	/// Build local frame
    void buildRepere();
    /// Compute incident ray angle
    void computeAngle();
    /// Compute the angle step between two rays to send
	void computeDTheta() 
	{
		delta_theta = angleOuverture / static_cast<decimal>(nbResponseLeft); 
	}

    Repere localRepere;				//!< Local frame

    decimal angleOuverture;			//!< Aperture angle of the cone
    decimal angleArrive;			//!< Incident ray angle

	decimal delta_theta;			//!< Angle step between two rays to send

	vec3 N1;						//!< Face 1 normal
	vec3 N2;						//!< Face 2 normal

};

#endif
