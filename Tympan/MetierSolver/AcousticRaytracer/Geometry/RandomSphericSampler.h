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
#include "Tympan/MetierSolver/AcousticRaytracer/global.h"
#include "Tympan/MetierSolver/AcousticRaytracer/Geometry/Sampler.h"
#include "Tympan/MetierSolver/AcousticRaytracer/Tools/UnitConverter.h"
#include <boost/random/mersenne_twister.hpp>
#include <boost/random/uniform_real.hpp>

#ifndef RANDOM_SPHERIC_SAMPLER
#define RANDOM_SPHERIC_SAMPLER

class RandomSphericSampler: public Sampler
{

public:
    RandomSphericSampler(const unsigned int& nbRays = 0,
                         const decimal& Theta = (decimal) M_PIDIV2,
                         const decimal& Phi = (decimal) M_2PI) : Sampler(nbRays, Theta, Phi),
        bounded_sampler(boost::uniform_real<>(0., 1.))
    {
    }

    RandomSphericSampler(const RandomSphericSampler& other) : Sampler(other), bounded_sampler(other.bounded_sampler)
    {
    }

    RandomSphericSampler(RandomSphericSampler* sampler) : Sampler(sampler), bounded_sampler(sampler->bounded_sampler)
    {
    }

    virtual Sampler* Clone()
    {
        Sampler* sampler = new RandomSphericSampler(this);
        return sampler;
    }

    virtual ~RandomSphericSampler() { }

    virtual vec3 getSample()
    {
        double U = bounded_sampler(random_generator);
        double V = bounded_sampler(random_generator);

        decimal thetaCalcul = acos(2. * U - 1.) - _theta;
        decimal phiCalcul = _phi * V;

        vec3 result;
        Tools::fromRadianToCarthesien(thetaCalcul, phiCalcul, result);
        result.normalize();

        return result;
    }

    virtual bool isAcceptableSample(vec3 v) { return true; }

    virtual unsigned int computeDiffractionNbr(const decimal& thetaCalcul)
    {
        return static_cast<unsigned int>(floor(sqrt(static_cast<decimal>(_nb_rays)) * sin(M_PIDIV2 - thetaCalcul) + 0.5));
    }


private :
    static boost::mt19937 random_generator;

    boost::uniform_real<> bounded_sampler;
};

boost::mt19937 RandomSphericSampler::random_generator = boost::mt19937();

#endif //RANDOM_SPHERIC_SAMPLER