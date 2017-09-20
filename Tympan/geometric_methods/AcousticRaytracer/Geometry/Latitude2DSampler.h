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
#include "Tools/UnitConverter.h"

#ifndef LATITUDE_2D_SAMPLER
#define LATITUDE_2D_SAMPLER

/**
 * \brief A Sampler class for latitude sampling
 */
class Latitude2DSampler: public Sampler
{

public:
	/// Constructors
    Latitude2DSampler(const unsigned int& nbRays = 0,
                      const decimal& Theta = (decimal) M_PIDIV2,
                      const decimal& Phi = (decimal) M_2PI) : Sampler(nbRays, Theta, Phi),
        _startTheta(0.f),
        _endTheta( static_cast<float>(M_PI) ),
        _startPhi(0.f),
        _endPhi( static_cast<float>(M_PI) ),
        _deltaPhi(0.f),
        _i(nbRays)
    {
        init();
    }

    Latitude2DSampler(const Latitude2DSampler& other) : Sampler(other)
    {
        _startTheta = other._startTheta;
        _endTheta = other._endTheta;
        _startPhi = other._startPhi;
        _endPhi = other._endPhi;
        init();
    }

    Latitude2DSampler(Latitude2DSampler* sampler) : Sampler(sampler)
    {
        _startTheta = sampler->_startTheta;
        _endTheta = sampler->_endTheta;
        _startPhi = sampler->_startPhi;
        _endPhi = sampler->_endPhi;
        init();
    }

    virtual Sampler* Clone()
    {
        Sampler* sampler = new Latitude2DSampler(this);
        return sampler;
    }
    /// Destructor
    virtual ~Latitude2DSampler() { }

    virtual vec3 getSample()
    {
        decimal Phi = _startPhi + _i * _deltaPhi;
        _i++;
		
        vec3 result;
        Tools::fromRadianToCarthesien(_startTheta, Phi, result);
        return result;
    }

    virtual bool isAcceptableSample(vec3 v) { return true; }
    virtual void init() 
    { 
        int decalage = 1;
        if (abs(_endPhi - M_2PI - _startPhi) < EPSILON_6) // Diffusion a 360�
        {
            decalage = 0;
        }

        _deltaPhi = (_endPhi - _startPhi) / static_cast<decimal>(_nb_rays - decalage); 
        
        _i = 0; 
    }
    /// Set starting angle theta
    void setStartTheta(const decimal& startTheta) { _startTheta = startTheta * M_PIDIV180; init();}
    /// Get starting angle theta
    decimal getStartTheta() const { return _startTheta * M_180DIVPI; }
    /// Set ending angle theta
    void setEndTheta(const decimal& endTheta) { _endTheta = endTheta * M_PIDIV180; init();}
    /// Get ending angle theta
    decimal getEndTheta() const { return _endTheta * M_180DIVPI; }
    /// Set starting angle phi
    void setStartPhi(const decimal& startPhi) { _startPhi = startPhi * M_PIDIV180; init();}
    /// Get starting angle phi
    decimal getStartPhi() const { return _startPhi * M_180DIVPI; }
    /// Set ending angle phi
    void setEndPhi(const decimal& endPhi) { _endPhi = endPhi * M_PIDIV180; init();}
    /// Get ending angle phi
    decimal getEndPhi() const { return _endPhi * M_180DIVPI; }

private :
    decimal _startTheta;
    decimal _endTheta;
    decimal _startPhi;
    decimal _endPhi;
    decimal _deltaPhi;
    unsigned int _i;
};

#endif //LATITUDE_2D_SAMPLER
