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

#include "Tympan/models/common/mathlib.h"

/*!
 * \brief   Sampler class and derivate describe ray generator used in AcousticRayTracer
 *          In these classes :
 *                  + theta is the polar angle where 0 is on equatorius
 *                  + phi is the equatorial angle
 *          by default ray are sent on a sphere
 */

class Sampler
{
public:
    Sampler(const unsigned int& nbRays = 0,
            const decimal& Theta = (decimal) M_PIDIV2,
            const decimal& Phi = (decimal) M_2PI) : _nb_rays(nbRays),
        _theta(Theta),
        _phi(Phi)
    { }

    Sampler(const Sampler& other)
    {
        _theta = other._theta;
        _phi = other._phi;

        _nb_rays = other._nb_rays;
    }

    Sampler(Sampler* sampler)
    {
        _theta = sampler->_theta;
        _phi = sampler->_phi;

        _nb_rays = sampler->_nb_rays;
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

    virtual unsigned int getNbRays() const { return _nb_rays; }
    virtual void setNbRays(const unsigned int& nbRays) {_nb_rays = nbRays; init(); }

    decimal getTheta() const { return _theta; }
    void setTheta(const decimal& Theta) { _theta = Theta; init(); }

    decimal getPhi() const { return _phi; }
    void setPhi(const decimal& Phi) { _phi = Phi ; init(); }

    /*!
     * \fn virtual unsigned int computeDiffractionNbr(const decimal& theta)
     * \brief return the numebr of rays to launch after a diffraction event
     */
    virtual unsigned int computeDiffractionNbr(const decimal& theta) { return 0; }

protected :
    unsigned int _nb_rays; /*! Number of rays to lauch */
    decimal _theta;       /*! global equatorial angle */
    decimal _phi;         /*! global polar angle */
};

#endif