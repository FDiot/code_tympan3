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
#include <vector>
#include <string>
#include <math.h>
#include "Sampler.h"
#include "Tympan/MetierSolver/AcousticRaytracer/Tools/UnitConverter.h"

#ifndef LONGITUDE_2D_SAMPLER
#define LONGITUDE_2D_SAMPLER
                                 
class Longitude2DSampler: public Sampler
{

public:
    Longitude2DSampler( const unsigned int& nbRays = 0, 
						  const decimal& Theta = (decimal) M_PIDIV2, 
						  const decimal& Phi = (decimal) M_2PI		) :	Sampler(nbRays, Theta, Phi), 
																		_startTheta(0),
																		_endTheta(M_PI),
																		_startPhi(0),
																		_endPhi(M_PI),
																		_deltaTheta(0.), 
																		_nMin( vec3(0., 0., 0.) ),
																		_i(0)
	{ 
		init(); 
	}

    Longitude2DSampler(const Longitude2DSampler& other) : Sampler(other) 
	{
		_startTheta = other._startTheta;
		_endTheta = other._endTheta;
		_startPhi = other._startPhi;
		_endPhi = other._endPhi;
		init();
	}
    
	Longitude2DSampler(Longitude2DSampler* sampler) : Sampler(sampler)
	{
		_startTheta = sampler->_startTheta;
		_endTheta = sampler->_endTheta;
		_startPhi = sampler->_startPhi;
		_endPhi = sampler->_endPhi;
		init();
	}

    virtual Sampler* Clone()
    {
        Sampler* sampler = new Longitude2DSampler(this);
        return sampler;
    }

    virtual ~Longitude2DSampler() { }

    virtual vec3 getSample()
    {
		// angle entre l'axe des y et nMin
		decimal Theta = std::asin( _nMin * vec3(0, 0, 1) ) * _deltaTheta * _i;
		_i++;
	
        vec3 result;
        Tools::fromRadianToCarthesien(Theta, _startPhi, result);
		
		return result;
    }

    virtual bool isAcceptableSample(vec3 v) { return true; }
    virtual void init() 
	{ 
		vec3 _nMin(cos(_startTheta), 0, sin(_startTheta));
		vec3 nMax(cos(_endTheta), 0,  sin(_endTheta));

		// angle de variation des rayons
		_deltaTheta = -std::acos( _nMin * nMax ) / nb_rays;
		
		_i = 0; 
	}

	void setStartTheta(const decimal& startTheta) { _startTheta = startTheta * M_PIDIV180; init(); }
	unsigned int getStartTheta() const { return _startTheta * M_180DIVPI; }

	void setEndTheta(const decimal& endTheta) { _endTheta = endTheta * M_PIDIV180; init(); }
	unsigned int getEndTheta() const { return _endTheta * M_180DIVPI; }

	void setStartPhi(const decimal& startPhi) { _startPhi = startPhi * M_PIDIV180; init(); }
	unsigned int getStartPhi() const { return _startPhi * M_180DIVPI; }
	
	void setEndPhi(const decimal& endPhi) { _endPhi = endPhi * M_PIDIV180; init(); }
	unsigned int getEndPhi() const { return _endPhi * M_180DIVPI; }

private :
	decimal _startTheta;
	decimal _endTheta;
	decimal _startPhi;
	decimal _endPhi;
	decimal _deltaTheta;
	vec3 _nMin;
	unsigned int _i;
};

#endif //LONGITUDE_2D_SAMPLER