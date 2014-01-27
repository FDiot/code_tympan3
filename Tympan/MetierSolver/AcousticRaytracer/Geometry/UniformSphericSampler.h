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
 
// Rajout de libraires pour creation fichier externe de sortie
#include <iostream>
#include <fstream>
#include <sstream>
#include <deque>
#include <stack>
#include <string>
#include <math.h>
#include "Sampler.h"
#include "Tympan/MetierSolver/AcousticRaytracer/Tools/UnitConverter.h"

#ifndef UNIFORM_SPHERIC_SAMPLER
#define UNIFORM_SPHERIC_SAMPLER

using namespace std;
                                 
class UniformSphericSampler: public Sampler
{
public:
    UniformSphericSampler( const unsigned int& nbRays = 0, 
						   const decimal& Theta = (decimal) M_PIDIV2, 
						   const decimal& Phi = (decimal) M_2PI		) :	Sampler(nbRays, Theta, Phi), 
																		real_nb_rays(0),
																		n1(0)
	{ 
		init(); 
	}

    UniformSphericSampler(const UniformSphericSampler& other) : Sampler(other)
	{
		real_nb_rays = 0; 
		n1 = 0;
		init();
	}
    
	UniformSphericSampler(UniformSphericSampler* sampler) : Sampler(sampler)
	{
		real_nb_rays = 0; 
		n1 = 0;
		init();
	}



    virtual Sampler* Clone()
    {
        Sampler* sampler = new UniformSphericSampler(this);
        return sampler;
    }

    virtual ~UniformSphericSampler() { }

    virtual vec3 getSample()
    {
		vec3 res = points.top();
		points.pop();

		return res;
    }

    virtual bool isAcceptableSample(vec3 v) { return true; }

    virtual void init() 
	{
		computeN1(); 

		computeTrueNbRays();

		BuildStack();
	}

	unsigned int getRealNbRays() const { return real_nb_rays; }

private :
	inline void computeN1() 
	{ 	
		n1 = static_cast<unsigned int>( floor( M_PI * sqrt( static_cast<decimal>(nb_rays) ) / 8. + 0.5 ) );
		n1 = 2 * n1;
	}

	inline decimal computeThetaCalcul(unsigned int i)
	{
		return static_cast<decimal>( static_cast<int>(n1) - 2 * static_cast<int>(i) + 1 ) * theta / static_cast<decimal>(n1) ;
	}

	inline unsigned int computeN2(const decimal& thetaCalcul)
	{
		return static_cast<unsigned int>( floor( nb_rays * (  sin( thetaCalcul + theta /n1 ) - sin( thetaCalcul - theta / n1 )  ) / 2.  + 0.5 ) );
	}

	inline void computeTrueNbRays()
	{
		decimal thetaCalcul = 0.;
		unsigned int n2 = 0;  // latitude : nbr of parts, function of n1

		for( unsigned int i = 1; i <= n1 ;i++)
		{
			thetaCalcul = computeThetaCalcul(i);
			n2 = computeN2(thetaCalcul);
			real_nb_rays += n2;
		}
	}

	/*!
	 * \fn void BuildStack()
	 * \brief Build stack that store the initial directions of rays
	 */
	void BuildStack()
	{
		unsigned int n2 = 0;
		decimal thetaCalcul;
		decimal phiCalcul;
		vec3 result;

		for(unsigned int i = 1; i <= n1 ;i++)
		{
			thetaCalcul = computeThetaCalcul(i);
			n2 = computeN2(thetaCalcul);
				
			for(unsigned int j = 1; j <= n2 ; j++)
			{		
				phiCalcul = j * ( phi / n2 );

				vec3 result;
				Tools::fromRadianToCarthesien(thetaCalcul, phiCalcul, result);
				result.normalize();

				points.push(result);
			}
		}	
	}

private :
	/*!
	 * \brief points store every single points in memory
	 */
	std::stack< vec3, std::deque<vec3> > points;

	unsigned int real_nb_rays;  /*! Real number of rays lauched */
	unsigned int n1;			/*! number of ray along equatorius */
};

#endif