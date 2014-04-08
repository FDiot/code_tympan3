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

    Diffraction(const vec3& position = vec3(0.0, 0.0, 0.0), const vec3& incomingDirection = vec3(0.0, 0.0, 0.0), Cylindre* c = NULL);

    Diffraction(const Diffraction& other);

	virtual ~Diffraction() {}

    virtual void setNbResponseLeft(int _nbResponseLeft) 
	{ 
		nbResponseLeft = static_cast<unsigned int>( std::floor( static_cast<decimal>(_nbResponseLeft-1) / M_2PI * angleOuverture + 0.5 ) );
		nbResponseLeft = nbResponseLeft >= 4 ? nbResponseLeft : 4; // Limite le nombre à 4
		initialNbResponse = nbResponseLeft;
		computeDTheta(); 
	}

	virtual bool getResponse(vec3& r, bool force = false);


    void setAngleOuverture(decimal angle) { angleOuverture = angle; computeDTheta(); }
    decimal getAngleOuverture() { return angleOuverture; }
	decimal getDeltaTheta() const { return delta_theta; }
    virtual double getAngle() { return angleArrive; }
    virtual bool generateTest(std::vector<vec3>& succededTest, std::vector<vec3>& failTest, unsigned int nbResponses);
	virtual const Repere& getRepere() const { return localRepere; }

	/*!
	 * \fn bool responseValidator(vec3 &T);
	 * \brief Return true if the response is in a correct direction for "standard" diffraction
	 */
	bool responseValidator(vec3 &T) 
	{ 
		decimal FT = from * T;

		if ( ( 1. - FT ) < BARELY_EPSILON ) { return true; } // Vecteur limite tangent au plan de propagation

		if ( FT < 0. ) { return false; }  // Le vecteur sortant est "oppose" au vecteur entrant

		decimal F1 = from * N1;
		decimal F2 = from * N2;

		if ( (F1 * F2) > 0.) { return false; } 

		decimal T1 = T * N1;
		decimal T2 = T * N2;


		if ( (F1 <= 0.) && ( (T1 > BARELY_EPSILON ) || ( (ABS(T2) - ABS(F2)) > BARELY_EPSILON ) ) )
		{ 
			return false; 
		}

		if ( (F2 <= 0.) && ( ( T2 > BARELY_EPSILON ) || ( (ABS(T1) - ABS(F1)) > BARELY_EPSILON ) ) )
		{ 
			return false; 
		}

		return true; 
	}

#ifdef _ALLOW_TARGETING_
    virtual bool isAcceptableResponse(vec3& test);
    virtual bool generateResponse(std::vector<vec3>& responses, unsigned int nbResponses);
    virtual bool appendTarget(vec3 target, bool force = false);
#endif //_ALLOW_TARGETING_


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

	vec3 N1;						/*!< face 1 normal */
	vec3 N2;						/*!< face 2 normal */
};

#endif
