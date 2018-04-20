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

#include "Engine/AcousticRaytracerConfiguration.h"
#include "Geometry/Cylindre.h"
#include "Tools/UnitConverter.h"
#include "Diffraction.h"

// Filter diffraction rays that are not in the shadow zone (i.e area not hit by direct rays)
bool responseAngleLimiter(const vec3& from, const vec3& N1, const vec3& N2, vec3 &T)
{
	decimal FT = from * T;


	// Return true if FROM and TO are colinear and point in the same direction
	if ( ( 1. - FT ) < EPSILON_4 ) { return true; } 

	// Return false if FROM and TO point in opposite directions
	if ( FT < 0. ) { return false; }  

	decimal F1 = from * N1;
	decimal F2 = from * N2;

	// Return false if the ray's incoming direction is such that there is no shadow zone
	if ( (F1 * F2) > 0.) { return false; }

	decimal T1 = T * N1;
	decimal T2 = T * N2;



	// Return false if TO is not in the shadow zone of the obstacle		
	if ( (F1 <= 0.) && ( (T1 > EPSILON_4 ) || ( (ABS(T2) - ABS(F2)) > EPSILON_4 ) ) )
	{
		return false;
	}

	if ( (F2 <= 0.) && ( ( T2 > EPSILON_4 ) || ( (ABS(T1) - ABS(F1)) > EPSILON_4 ) ) )
	{
		return false;
	}

	return true;
}

bool responseAlwaysYes(const vec3& from, const vec3& N1, const vec3& N2, vec3 &T) { return true; }

void getThetaRandom(const decimal& angleOuverture, const decimal & delta_theta, const decimal & nbResponseLeft, decimal& theta)
{
	theta = ((decimal)(rand())) * angleOuverture / ((decimal)RAND_MAX);

	if (theta > angleOuverture / 2.)
	{
		theta += ((decimal)(2 * M_PI) - angleOuverture);
	}
}

void getThetaRegular(const decimal& angleOuverture, const decimal & delta_theta, const decimal & nbResponseLeft, decimal& theta)
{
	//compute next angle around the cone of Keller
	theta = (nbResponseLeft * delta_theta) - (angleOuverture / 2.);
}


Diffraction::Diffraction(const vec3& position, const vec3& incomingDirection, Cylindre* c):
						Event(position, incomingDirection, (Shape*)(c))
{
	name = "unknown diffraction";
	nbResponseLeft = initialNbResponse = 200;
	type = DIFFRACTION;

	// Skip instructions dependant on the Cylindre if c==NULL
	if(c){
		buildRepere();
		computeAngle();
		computeDTheta();

		N1 = dynamic_cast<Cylindre*>(shape)->getFirstShape()->getNormal();
		N2 = dynamic_cast<Cylindre*>(shape)->getSecondShape()->getNormal();
	}

	if (AcousticRaytracerConfiguration::get()->DiffractionFilterRayAtCreation)
	{
		responseValidator = responseAngleLimiter;
	}
	else
	{
		responseValidator = responseAlwaysYes;
	}

	if (AcousticRaytracerConfiguration::get()->DiffractionUseRandomSampler) // Tir des rayons aléatoires sur le cone de Keller
	{
		getTheta = getThetaRandom;
	}
	else
	{
		getTheta = getThetaRegular;
	}

}

Diffraction::Diffraction(const Diffraction& other) : Event(other)
{
    type = DIFFRACTION;
    buildRepere();
    computeAngle();
	computeDTheta();

	N1 = other.N1;
	N2 = other.N2;

	responseValidator = other.responseValidator;
}

bool Diffraction::getResponse(vec3& r, bool force)
{
	bool bRep = false;
	do
	{
		decimal theta = 0.;
		// get the angle of the next response around Keller's cone 
		// (avoids returning an angle that would result in a response pointing inside the obstacle)
		(*getTheta) (angleOuverture, delta_theta, (decimal)nbResponseLeft, theta);

#ifdef _ALLOW_TARGETING_
		if (!force)
		{
			nbResponseLeft--;
			if (nbResponseLeft < 0)
			{
				return false;
			}
		}

		if (!targets.empty())
		{
			r = vec3(targets.back());
			targets.pop_back();
			return true;
		}
#else
		// force is true only when targeting is allowed
		nbResponseLeft--;
		if (nbResponseLeft < 0)
		{
			return false;
		}
#endif

		vec3 localResponse;
		// convert from polar coordinates to carthesian ones
		Tools::fromRadianToCarthesien2( angleArrive, theta, localResponse );

		// transpose the local response into global coordinates
		r = localRepere.vectorFromLocalToGlobal(localResponse);

		// validate response
		bRep = (*responseValidator)(from, N1, N2, r);

	}
	while(!bRep);

	return true;
}

void Diffraction::buildRepere()
{
    Cylindre* c = (Cylindre*)(shape);
    vec3 O = pos;
    vec3 v1 = c->getVertices()->at(c->getLocalVertices()->at(0));
    vec3 v2 = c->getVertices()->at(c->getLocalVertices()->at(1));

    // Z axis is along the diffraction edge
    vec3 Z = v1 - O;
    if (Z.dot(from) < 0)
    {
        Z = v2 - O;
    }
    Z.normalize();

    //X axis is the mean of the two normals
    vec3 X = c->getFirstShape()->getNormal() + c->getSecondShape()->getNormal();
    X.normalize();

    //Y axis is the cross product of Z and X
    vec3 Y;
    Y.cross(Z, X);
    Y.normalize();

    localRepere = Repere(X, Y, Z, O);

}

void Diffraction::computeAngle()
{
    Cylindre* c = (Cylindre*)(shape);

    //The cone's aperture angle
    angleOuverture = c->getAngleOuverture();

    localRepere.vectorFromGlobalToLocal(from);

    //Angle between the incoming ray and the Z axis 
	decimal cosAngleArrive = localRepere.getW().dot(from);

    angleArrive = acos(cosAngleArrive);
}

bool Diffraction::generateTest(std::vector<vec3>& succededTest, std::vector<vec3>& failTest, unsigned int nbResponses)
{
    for (unsigned int i = 0; i < nbResponses; i++)
    {
        vec3 newDir = vec3(((decimal)rand() / (decimal)RAND_MAX) * 2.f - 1.f, ((decimal)rand() / (decimal)RAND_MAX) * 2.f - 1.f, ((decimal)rand() / (decimal)RAND_MAX) * 2.f - 1.f);
        newDir.normalize();
        if (isAcceptableResponse(newDir))
        {
            succededTest.push_back(newDir);
        }
        else
        {
            failTest.push_back(newDir);
        }
    }
    return true;
}

#ifdef _ALLOW_TARGETING_
bool Diffraction::isAcceptableResponse(vec3& test)
{

    vec3 localResponse = localRepere.vectorFromGlobalToLocal(test);

    if (localResponse.z < 0.)
    {
        return false;
    }

    decimal phi = acos(localResponse.z);
    decimal tetha;

    if (localResponse.y >= 0.)
    {
        tetha = acos(localResponse.x / (sqrt(localResponse.x * localResponse.x + localResponse.y * localResponse.y)));
    }
    else
    {
        tetha = 2. * M_PI - acos(localResponse.x / (sqrt(localResponse.x * localResponse.x + localResponse.y * localResponse.y)));
    }

    if (!(tetha < angleOuverture / 2. || tetha > (2. * M_PI - angleOuverture / 2.)))
    {
        return false;
    }

    decimal deltaPhi = fabs(angleArrive - phi);
    if (deltaPhi < M_PI / 18.)
    {
        return true;
    }
    return false;

}

bool Diffraction::generateResponse(std::vector<vec3>& responses, unsigned int nbResponses)
{
    vec3 newDir;
    for (unsigned int i = 0; i < nbResponses; i++)
    {
        getResponse(newDir, true);
        responses.push_back(newDir);
    }

    return true;
}

bool Diffraction::appendTarget(vec3 target, bool force)
{

    if (force)
    {
        vec3 newDir = target - pos;
        newDir.normalize();
        targets.push_back(newDir);
        return true;
    }

    if (targets.size() > static_cast<unsigned int>(nbResponseLeft)) { return false; }
    vec3 newDir = target - pos;
    newDir.normalize();
    if (isAcceptableResponse(newDir))
    {
        targets.push_back(newDir);
        return true;
    }
    return false;
}
#endif //_ALLOW_TARGETING_



