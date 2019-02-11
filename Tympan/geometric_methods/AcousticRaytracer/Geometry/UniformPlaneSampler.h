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

#ifndef UNIFORM_PLANE_SAMPLER
#define UNIFORM_PLANE_SAMPLER


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

class UniformPlaneSampler: public Sampler
{
public:
	/// Constructors
    UniformPlaneSampler(const vector<vec3> directions,
						const unsigned int& nbRays = 8) : Sampler(nbRays, (decimal)M_2PI, (decimal)M_2PI),
        _real_nb_rays(0),
		_nb_rays_per_direction(0),
		_nb_rays_per_plane(0),
		_nb_directions(0),
        _i(0),
        _j(0),
		_repere(Repere(vec3(1.,0.,0.),vec3(0.,1.,0.),vec3(0.,0.,1.),vec3(0.,0.,0.))),
		_directions(directions)
    {
        init();
    }

    UniformPlaneSampler(const UniformPlaneSampler& other) : Sampler(other)
    {
		_real_nb_rays = other._real_nb_rays;
		_nb_rays_per_direction = other._nb_rays_per_direction;
		_nb_rays_per_plane = other._nb_rays_per_plane;
		_nb_directions = other._nb_directions;
        _i = 0;
        _j = 0;
		_directions = other._directions;
		_repere = other._repere;
     
    }

    UniformPlaneSampler(UniformPlaneSampler* other) : Sampler(other)
    {
        _real_nb_rays = other->_real_nb_rays;
		_nb_rays_per_direction = other->_nb_rays_per_direction;
		_nb_rays_per_plane = other->_nb_rays_per_plane;
		_nb_directions = other->_nb_directions;
        _i = 0;
        _j = 0;
		_directions = other->_directions;
		_repere = other->_repere;
    }

    virtual Sampler* Clone()
    {
        Sampler* sampler = new UniformPlaneSampler(this);
        return sampler;
    }
    /// Destructor
    virtual ~UniformPlaneSampler() { }

	

    virtual vec3 getSample()
    {
		vec3 result(1.,0.,0.);
		if(_i>=_nb_rays_per_direction){
			_i=0;
			_j++;
		}

		if(_j<_nb_directions){
			vec3 dir=_directions.at(_j);
			
			if(_i==0){

			   // Compute the axises of the global repere
			   vec3 ax2=vec3(0.,0.,1.);	// the second axis is orthogonal to the direction and th z-axis
			   ax2.cross(dir);			
			   ax2.normalize();

			   vec3 ax3=dir; 
			   ax3.cross(ax2);			// the second axis is orthogonal to the first one and to the directivity
			   ax3.normalize();

			   // Mapping from the local repere to the global one (the third axis is the directivity and corresponds to the local z_axis)
			   _repere = Repere(dir,ax2,ax3,vec3(0.,0.,0.));
			}

			
			if(_i>=_nb_rays_per_plane){
				if(_i%(_nb_rays_per_plane/2)==0)
					_i++; //skip the rays already launched on the other plane
				Tools::fromRadianToCarthesien((_i-_nb_rays_per_plane)*M_2PI/_nb_rays_per_plane,0, result);
			}else{
				Tools::fromRadianToCarthesien(0,_i*M_2PI/_nb_rays_per_plane, result);
			}
			_i++;
		}
		//return result;
		return _repere.vectorFromLocalToGlobal(result); //rotate the sample according to the rotation between the z-axis and the directivity;
    }

	// v is acceptable if it lies in the beam's cone
	virtual bool isAcceptableSample(vec3 v) { return true ;}	

    virtual void init()
    {

	   _nb_directions = _directions.size();
	   _nb_rays_per_direction = floor(_nb_rays/8.+0.5)*8;
	   _nb_rays_per_plane= _nb_rays_per_direction/2;
	   _real_nb_rays= _nb_directions*(_nb_rays_per_direction-2);   //We remove two rays per direction because the two planes have two rays in common
	 
    }
    // Return the real number of launched rays 
    unsigned int getRealNbRays() const { return _real_nb_rays; }


private :
    unsigned int _real_nb_rays;  			//!< Real number of rays launched
	unsigned int _nb_rays_per_direction;  	//!< Number of rays launched per direction
	unsigned int _nb_rays_per_plane;		//!< Number of rays launched per plane
	unsigned int _nb_directions;  			//!< Number of directions
    unsigned int _i, _j;        			//!< Current indices ( respectively ray index and direction index)
	vector<vec3> _directions;				//!< Directions
	Repere _repere;							//!< Repere used to rotate the beam in direction of the directivity
};

#endif
