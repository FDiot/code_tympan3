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

#ifndef UNIFORM_SPHERIC_SAMPLER
#define UNIFORM_SPHERIC_SAMPLER
                                 
class UniformSphericSampler: public Sampler
{

public:
    UniformSphericSampler() { init(); }
    UniformSphericSampler(const UniformSphericSampler& other) { }
    UniformSphericSampler(UniformSphericSampler* sampler) { }

	// Vector of vec3 : has every single points in memory after the first
	// ray is calculated.
	// Indeed, the first time, the whole sphere is discretized. 
	// It means thsat the other rays will have to look up in this vector
	// to get a new direction.
	// Memory is allocated during the fisrt step.
	std::vector <vec3> points;

    virtual Sampler* Clone()
    {
        Sampler* sampler = new UniformSphericSampler(this);
        return sampler;
    }

    virtual ~UniformSphericSampler() { }

    virtual vec3 getSample()
    {
        decimal U = (decimal)rand() / (decimal)RAND_MAX;
        decimal V = (decimal)rand() / (decimal)RAND_MAX;
        //      decimal tetha = 2. * M_PI * U;
        //      decimal phi = acos(2. * V - 1.);
        decimal theta = acos(2. * U - 1.);
        decimal phi = 2 * M_PI * V;
        vec3 result;
        Tools::fromRadianToCarthesien(theta, phi, result);
        result.normalize();

        return result;
    }

	// This method works in a different way
	// Here we define theta and phi and the number of rays we'd like to send.
	// Then a calculation is made to discretize the source represented by a sphere.
	// The calculation is made on the entire sphere, 
	// using theta = pi/2 and phi = 2pi
	virtual vec3 Discretisation(int indice, int n1, int Nreal, int Nasked)
	{
		int n2 = 0;
		double theta = M_PI/2;
		double phi = 2*M_PI;
		double thetaCalcul;
		double phiCalcul;
		double x, y, z;
		vec3 result;
		vec3 retourne;
		x = 0;
		y = 0;
		z = 0;
		std::vector <double> thetaPts;	
		std::vector <double> phiPts;
		thetaPts.reserve(n1);

		if (indice == Nreal-1)
		{		
			// Reserve space necessary for all directions
			points.reserve(Nreal);

			for(int i = 1; i <= n1 ;i++)
			{
				thetaCalcul = ( n1 - 2*i +1) * theta / n1 ;
				thetaPts.push_back(thetaCalcul);
				n2 = floor ( Nasked * (  sin( (thetaCalcul) + theta /n1 ) - sin( (thetaCalcul) - theta / n1 )  ) / 2  +0.5 );
				
				for(int j = 1; j <= n2 ; j++)
				{		
					phiPts.reserve(n2);
					phiCalcul = j*(phi/n2);
					phiPts.push_back(phiCalcul);
					result.x = cos(thetaCalcul)*cos(phiCalcul);
					result.y = cos(thetaCalcul)*sin(phiCalcul);
					result.z = sin(thetaCalcul);
					points.push_back(result);
				}
			}	
		}
		retourne = points.at((Nreal-1)-indice);

		return retourne;
	}

    virtual bool isAcceptableSample(vec3 v) { return true; }
    virtual void init() { _graine = 3; srand(_graine); }

    unsigned int _graine;
};

#endif