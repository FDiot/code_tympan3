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
#include <limits>
#include <iostream>
#include <fstream>
#include <sstream>
#include <deque>
#include <stack>
#include <string>
#include <math.h>
#include "Sampler.h"
#include "Tympan/MetierSolver/AcousticRaytracer/Tools/UnitConverter.h"

#ifndef UNIFORM_SPHERIC_SAMPLER_2
#define UNIFORM_SPHERIC_SAMPLER_2

using namespace std;
                                 
class UniformSphericSampler2: public Sampler
{
public:
    UniformSphericSampler2( const unsigned int& nbRays = 0, 
						   const decimal& Theta = (decimal) M_PIDIV2, 
						   const decimal& Phi = (decimal) M_2PI		) :	Sampler(nbRays, Theta, Phi), 
																		_real_nb_rays(0),
																		_n1(1),
																		_n2(1),
																		_dr(0.),
																		_phi0(0.),
																		_d_theta(0.),
																		_d_phi(0.),
																		_thetaCalcul(0.),
																		_i(1),
																		_j(1)
	{ 
		init(); 
	}

    UniformSphericSampler2(const UniformSphericSampler2& other) : Sampler(other)
	{
		_real_nb_rays = 0; 
		_n1 = 1;
		_n2 = 1;
		_dr = 0.;
		_phi0 = 0.; 
		_d_theta = 0.;
		_d_phi = 0.;
		_i = 1;
		_j = 1;
		_thetaCalcul = 0.;
		init();
	}
    
	UniformSphericSampler2(UniformSphericSampler2* sampler) : Sampler(sampler)
	{
		_real_nb_rays = 0; 
		_n1 = 1;
		_n2 = 1;
		_dr = 0.;
		_phi0 = 0.; 
		_d_theta = 0.;
		_d_phi = 0.;
		_i = 1;
		_j = 1;
		_thetaCalcul = 0.;
		init();
	}

    virtual Sampler* Clone()
    {
        Sampler* sampler = new UniformSphericSampler2(this);
        return sampler;
    }

    virtual ~UniformSphericSampler2() { }

	virtual vec3 getSample()
	{
		vec3 res(0., 0., 0.);
		if (_i > _n1) return res;

		if ( _j > _n2 ) 
		{
			computeThetaCalcul(++_i);
			_phi0 = _d_phi / 2.;
			computeN2();
			_j = 1;
		}

		decimal phiCalcul = _j * _d_phi + _phi0;

		_j++;

		Tools::fromRadianToCarthesien(_thetaCalcul, phiCalcul, res);
		res.normalize();

		return res;
	}

    virtual bool isAcceptableSample(vec3 v) { return true; }

    virtual void init() 
	{
		_dr = sqrt( 4. / static_cast<decimal>(_nb_rays) );
		
		computeN1();

		computeTrueNbRays();

		computeThetaCalcul(_i);

		computeN2();
	}

	unsigned int getRealNbRays() const { return _real_nb_rays; }

	virtual unsigned int computeDiffractionNbr(const decimal &thetaCalcul)
	{
		return static_cast<unsigned int>( floor( M_2PI * cos( thetaCalcul ) / ( 2. * _dr ) + 0.5 ) );
	}

private :
	inline void computeN1() 
	{ 	
		decimal dh = 2 * _dr * sin(M_PI / 3); // elementary high of a slice
		_d_theta = asin(dh);
		_n1 = static_cast<unsigned int>( floor( M_PI / _d_theta + 0.5 ) );
	}

	inline void computeThetaCalcul(unsigned int i)
	{
		_thetaCalcul =  static_cast<decimal>( M_PIDIV2 - i * _d_theta );

		//_thetaCalcul =  static_cast<decimal>( static_cast<int>(_n1) - 2 * static_cast<int>(i) + 1 ) * _theta / static_cast<decimal>(_n1) ;
	}

	inline void computeN2()
	{
		_n2 = static_cast<unsigned int>( floor( M_2PI * cos( _thetaCalcul ) / ( 2. * _dr ) + 0.5 ) );
		_d_phi = _phi / _n2;
		//_n2 = static_cast<unsigned int>( floor( _nb_rays * (  sin( _thetaCalcul + _theta /_n1 ) - sin( _thetaCalcul - _theta / _n1 )  ) / 2.  + 0.5 ) );
	}



	inline void computeTrueNbRays()
	{
		for( unsigned int i = 1; i <= _n1 ;i++)
		{
			computeThetaCalcul(i);
			computeN2();
			_real_nb_rays += _n2;
		}
	}

private :
	unsigned int _real_nb_rays;  /*!< Real number of rays lauched */
	unsigned int _n1;			/*!< number of slices  */
	unsigned int _n2;			/*!< number of rays along a specific slice */
	decimal _dr;				/*!< radius of elementary element of sphere*/
	decimal _phi0;				/*!< angle offset between a slice and the previous on */
	decimal _d_theta;			/*!< angle step along longitude */
	decimal _d_phi;				/*!< angle step along given slice */
	decimal _thetaCalcul;		/*!< current angle along a longitude */
	unsigned int _i, _j;		/*!< current indices */
};

#endif