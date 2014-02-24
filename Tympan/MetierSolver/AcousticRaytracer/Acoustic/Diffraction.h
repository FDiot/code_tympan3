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

#include "Event.h"
#include "Tympan/MetierSolver/AcousticRaytracer/Geometry/Repere.h"
#include "Tympan/MetierSolver/AcousticRaytracer/Base.h"


class Cylindre;

class Diffraction : public Event
{
public:

    Diffraction(const vec3& position = vec3(0.0, 0.0, 0.0), const vec3& incomingDirection = vec3(0.0, 0.0, 0.0), Cylindre* c = NULL):
				Event(position, incomingDirection, (Shape*)(c)) 
	{ 
		name = "unknown diffraction"; 
		nbResponseLeft = initialNbResponse = 200; 
		type = DIFFRACTION; 
		buildRepere(); 
		computeAngle();

		computeDTheta();
	}

    Diffraction(const Diffraction& other) : Event(other)
    {
        type = DIFFRACTION;
        buildRepere();
        computeAngle();
		computeDTheta();
    }

    virtual ~Diffraction()
    {

    }

    virtual void setNbResponseLeft(int _nbResponseLeft) 
	{ 
		nbResponseLeft = static_cast<unsigned int>( std::floor( static_cast<decimal>(_nbResponseLeft-1) / M_2PI * angleOuverture + 0.5 ) );
		nbResponseLeft = nbResponseLeft >= 4 ? nbResponseLeft : 4; // Limite le nombre à 4
		initialNbResponse = nbResponseLeft;
		computeDTheta(); 
	}

	virtual bool getResponse(vec3& r, bool force = false);

    virtual bool isAcceptableResponse(vec3& test);

    void setAngleOuverture(decimal angle) { angleOuverture = angle; computeDTheta(); }
    decimal getAngleOuverture() { return angleOuverture; }
	decimal getDeltaTheta() const { return delta_theta; }

    virtual bool generateResponse(std::vector<vec3>& responses, unsigned int nbResponses);
    virtual bool generateTest(std::vector<vec3>& succededTest, std::vector<vec3>& failTest, unsigned int nbResponses);

    virtual bool appendTarget(vec3 target, bool force = false);

    virtual double getAngle() { return angleArrive; }

protected:

    void buildRepere();
    void computeAngle();
	void computeDTheta() 
	{
		delta_theta = angleOuverture / static_cast<decimal>(nbResponseLeft); 
	}

    Repere localRepere;

    decimal angleOuverture;			/*!< Aperture angle of the cone*/
    decimal angleArrive;			/*!< incident ray angle*/

	decimal delta_theta;			/*!< angle step betwwen two rays to send */
};

#endif
