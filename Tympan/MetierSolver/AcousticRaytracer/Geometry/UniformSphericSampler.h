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
 
#ifndef UNIFORM_SPHERIC_SAMPLER
#define UNIFORM_SPHEREIC_SAMPLER

#include "Sampler.h"
#include "Tympan/MetierSolver/AcousticRaytracer/Tools/UnitConverter.h"
#include <math.h>

class UniformSphericSampler: public Sampler
{

public:
    UniformSphericSampler() { init(); }
    UniformSphericSampler(const UniformSphericSampler& other) { }
    UniformSphericSampler(UniformSphericSampler* sampler) { }

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
    virtual bool isAcceptableSample(vec3 v) { return true; }
    virtual void init() { _graine = 3; srand(_graine); }

    unsigned int _graine;
};

#endif