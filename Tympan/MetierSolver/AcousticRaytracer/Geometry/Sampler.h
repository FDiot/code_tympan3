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
 
#ifndef SAMPLER_H
#define SAMPLER_H

#include "mathlib.h"

class Sampler
{
public:
    Sampler( const unsigned int& nbRays = 0, 
			 const decimal& Theta = (decimal) M_PIDIV2, 
			 const decimal& Phi = (decimal) M_2PI		) : nb_rays( nbRays ),
															theta( Theta ),
															phi( Phi )
	{ }

    Sampler(const Sampler& other)
	{ 
		theta = other.theta;
		phi = other.phi;

		nb_rays = other.nb_rays; 
	}

    Sampler(Sampler* sampler) 
	{ 
		theta = sampler->theta;
		phi = sampler->phi;

		nb_rays = sampler->nb_rays; 
	}

    virtual Sampler* Clone()
    {
        Sampler* sampler = new Sampler(this);
        return sampler;
    }

    virtual ~Sampler() { }

    virtual vec3 getSample() { return vec3(0.0, 0.0, 0.0); }
    virtual bool isAcceptableSample(vec3 v) { return false; }
    virtual void init() {}

	virtual unsigned int getNbRays() const { return nb_rays; }
	virtual void setNbRays(const unsigned int& nbRays) { nb_rays = nbRays; init(); }

	decimal getTheta() const { return theta; }
	void setTheta(const decimal& Theta) { theta = Theta; init(); }

	decimal getPhi() const { return phi; }
	void setPhi(const decimal& Phi) { phi = Phi ; init(); }

protected :
	unsigned int nb_rays; /*! Number of rays to lauch */
	decimal theta;		  /*! global equatorial angle */
	decimal phi;		  /*! global polar angle */
};

#endif