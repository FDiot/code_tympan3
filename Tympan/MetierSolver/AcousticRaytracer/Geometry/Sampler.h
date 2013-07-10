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
    Sampler() { }
    Sampler(const Sampler& other) { }
    Sampler(Sampler* sampler) { }

    virtual Sampler* Clone()
    {
        Sampler* sampler = new Sampler(this);
        return sampler;
    }

    virtual ~Sampler() { }

    virtual vec3 getSample() { return vec3(0.0, 0.0, 0.0); }
	virtual vec3 Discretisation(int indice, int n1, int Nreal, int Nasked) { return vec3(0.0, 0.0, 0.0); }
    virtual bool isAcceptableSample(vec3 v) { return false; }
    virtual void init() {}
};

#endif