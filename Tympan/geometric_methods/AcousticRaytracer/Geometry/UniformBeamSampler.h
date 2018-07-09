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
#include "Tools/UnitConverter.h"
#include "Repere.h"
#include "Geometry/mathlib.h"

#ifndef UNIFORM_BEAM_SAMPLER
#define UNIFORM_BEAM_SAMPLER


using namespace std;

/**
 * \brief A Sampler class for sampling rays uniformely from a cone
 * 
 * Principle : Sample points on the spherical cap resulting from the intersection between the unit sphere and the cone of axis z-axis and opening angle alpha.
 * Then rotate the samples so that the axis of the cone points in the direction of the directivity
 * 
 * The cap is divided into horizontal slices on which points are sampled. 
 * The angle between consecutive slices is phi. The angle between consecutive samples on the same slice is theta.
 * Theta is computed so that the arc length between neighboring samples on the same slice is equal to the arc length between neihboring slices (hence the uniformity of the samples)
 * This means that the phi angle directly controls the number of slices and indirectly controls the number samples on each one of them.
 * This gives us a nice uniform distribution of the samples but it prevents us from chosing exactly how many samples are generated.
 *
 * The phi angle (and therefore the number of slices) is choosen so that the real number of samples is at least equal to the desired number of rays (nbRays)
 * 
 * Note : The first sample is always (0,0,1) 
 * Therefore, if we launch at least one ray, the first ray is always collinear with the directivity of the source
 *
 * Ref : This approach is refered as Rusin's discoball
 * Some more details on evenly distributed points on a sphere : http://web.archive.org/web/20120107030109/http://cgafaq.info/wiki/Evenly_distributed_points_on_sphere
 * Comparison with the "golden sector" and the "Saff & Kuijlaar" approaches : https://bendwavy.org/pack/pack.htm
 */

class UniformBeamSampler: public Sampler
{
public:
	/// Constructors
    UniformBeamSampler(const unsigned int& nbRays = 0,
					   const decimal& alpha = (decimal) M_PI,
					   const vec3& directivity = vec3(0,0,1)) : Sampler(nbRays, (decimal)M_PI, (decimal)M_2PI),
        _real_nb_rays(0),
        _slices(0),																																																
		_slice_rays(0),																	
		_radius(0.),																	
		_alpha(alpha/2),																
		_directivity(directivity),														
        _i(0),
        _j(0),
		_repere(Repere(vec3(1.,0.,0.),vec3(0.,1.,0.),vec3(0.,0.,1.),vec3(0.,0.,0.)))
    {
        init();
    }

    UniformBeamSampler(const UniformBeamSampler& other) : Sampler(other)
    {
        _real_nb_rays = 0;
        _slices = 0;
		_slice_rays = 0;
		_radius = other._radius;
		_alpha = other._alpha;
		_directivity = other._directivity;
        _i = 0;
        _j = 0;
		_repere = Repere(vec3(1.,0.,0.),vec3(0.,1.,0.),vec3(0.,0.,1.),vec3(0.,0.,0.));
        init();
    }

    UniformBeamSampler(UniformBeamSampler* sampler) : Sampler(sampler)
    {
        _real_nb_rays = 0;
        _slices = 0;
		_slice_rays = 0;
		_radius = sampler->_radius;
		_alpha = sampler->_alpha;
		_directivity = sampler->_directivity;
        _i = 0;
        _j = 0;
		_repere = Repere(vec3(1.,0.,0.),vec3(0.,1.,0.),vec3(0.,0.,1.),vec3(0.,0.,0.));
        init();
    }

    virtual Sampler* Clone()
    {
        Sampler* sampler = new UniformBeamSampler(this);
        return sampler;
    }
    /// Destructor
    virtual ~UniformBeamSampler() { }

	// Move to next slice
	void next_slice(){
		_i++;														// Increment the slice index
		_radius= sin(_i*_phi);										// Radius of the new slice
		decimal perimeter= static_cast<decimal>(2*M_PI*_radius);	// Perimeter of the new slice
		_slice_rays=static_cast<int>(floor(perimeter/_phi));		// Number of rays on the new slice
		_theta = static_cast<decimal>(2*M_PI/_slice_rays);			// Angle between consecutive samples on the new slice
		_j=0;														// Reset the sample index
	}

    virtual vec3 getSample()
    {

		if(_i>_slices || _nb_rays == 0) // if all samples have been drawn
			return vec3(0.,0.,0.);		

		vec3 res(0.,0.,1.); // first sample is points in direction of the z-axis <=> points in the direction of the directivity after the rotation

        if (_i != 0) {  
			res=vec3(cos(_j*_theta)*_radius,  //x
					 sin(_j*_theta)*_radius,  //y
					 cos(_i*_phi));			  //z
			_j++;
		
		}

		if( _j>=_slice_rays){ // if _j as reached the number of samples on the current slice
			next_slice();     // move to the next slice
		}

		return _repere.vectorFromLocalToGlobal(res); //rotate the sample according to the rotation between the z-axis and the directivity
    }

	// v is acceptable if it lies in the beam's cone
	virtual bool isAcceptableSample(vec3 v) { return acos(v*_directivity)<=(_alpha+EPSILON_5) ;}	

    virtual void init()
    {
	   find_number_of_slices(); // determine the minimum number of slices so that _real_nb_rays >= _nb_rays

	   // Compute the axises of the global repere
	   vec3 ax1=vec3(0.,0.,1.); 
	   ax1.cross(_directivity); // the first axis is orthogonal to the z_axis and to the directivity
	   ax1.normalize();

	   vec3 ax2=_directivity; 
	   ax2.cross(ax1);			// the second axis is orthogonal to the first one and to the directivity
	   ax2.normalize();

	   // Mapping from the local repere to the global one (the third axis is the directivity and corresponds to the local z_axis)
	   _repere = Repere(ax1,ax2,_directivity,vec3(0.,0.,0.));

    }


    // Return the real number of launched rays 
    unsigned int getRealNbRays() const { return _real_nb_rays; }

	//Set the opening angle of the beam (input angle in degrees)
	void setOpeningAngle(const decimal& Alpha) { _alpha= Alpha * M_PIDIV180/2; init(); }

	//Get the opening angle of the beam (radians)
	decimal getOpeningAngle() { return _alpha*2; }
	
	
	//Set the directivity of the sampler
	void setDirectivity(const vec3& Directivity) { _directivity= Directivity; init(); }

	//Set the directivity of the sampler
	vec3 getDirectivity() { return _directivity; }

    virtual unsigned int computeDiffractionNbr(const decimal& thetaCalcul)
    {
	
		decimal p=static_cast<decimal>(2*_radius*M_PI);		// perimeter of the slice
        return static_cast<int>(floor(p/_phi));				// number of samples on the slice
    }

private :
    inline void find_number_of_slices()
    {
		_slices=0;
		_real_nb_rays=_nb_rays;
		// increase the number of slices until the desired number of rays is reached
		for(unsigned int i = 1;i<_nb_rays;i++){

			_real_nb_rays=1;
			_slices=i;
			decimal phi=static_cast<decimal>(_alpha/_slices); // phi is equal to the opening angle divided by the number of slices

			for(unsigned int j=1;j<=_slices;j++){
				
				decimal r=static_cast<decimal>(sin(j*phi));		// radius of the slice
				decimal p=static_cast<decimal>(2*r*M_PI);		// perimeter of the slice
				_real_nb_rays+=static_cast<int>(floor(p/phi));  // add the number of rays on the current slice to the total _real_nb_rays
			}
		

			if(_real_nb_rays>=_nb_rays){
				_phi=static_cast<decimal>(_alpha/_slices); // phi is equal to the opening angle divided by the number of slices
				break; // break if the desired number of rays has been reached
			}
		}

    }

 
private :
    unsigned int _real_nb_rays;  	//!< Real number of rays launched
    unsigned int _slices;           //!< Number of slices
	unsigned int _slice_rays;		//!< Number of remaining rays to launch for the current slice
	decimal _radius;				//!< radius of the current slice
	decimal _alpha;					//!< Opening angle of the cone
	vec3 _directivity;				//!< Directivity of the beam
    unsigned int _i, _j;        	//!< Current indices ( respectively slice index and sample index of the current slice)
	Repere _repere;					//!< Repere used to rotate the beam in direction of the directivity
};

#endif
