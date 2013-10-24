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
 
#ifndef NXVEC3_H
#define NXVEC3_H
#include <cmath>

#define NxReal double

class NxVec3
{
public:
    NxVec3();
    NxVec3(NxReal a);
    NxVec3(NxReal nx, NxReal ny, NxReal nz);
    NxVec3(const NxReal v[]);
    NxVec3(const NxReal v[], bool normalized);
    NxVec3(const NxVec3& v);
    NxVec3(const NxVec3& v, bool normalized);
    const   NxVec3& operator=(const NxVec3&);
    const   NxReal* get() const;
    NxReal* get();
    bool    operator< (const NxVec3&) const;
    bool    operator==(const NxVec3&) const;
    bool    operator!=(const NxVec3&) const;
    void    set(const NxVec3&);
    void    setx(const NxReal& d);
    void    sety(const NxReal& d);
    void    setz(const NxReal& d);
    void    setNegative(const NxVec3& a);
    void    setNegative();
    void    set(NxReal, NxReal, NxReal);
    void    set(NxReal);
    void    zero();
    void    setPlusInfinity();
    void    setMinusInfinity();

    void    add(const NxVec3& a, const NxVec3& b);
    void    subtract(const NxVec3& a, const NxVec3& b);
    void    multiply(NxReal s,  const NxVec3& a);
    void    arrayMultiply(const NxVec3& a, const NxVec3& b);
    void    multiplyAdd(NxReal s, const NxVec3& a, const NxVec3& b);
    NxReal  normalize();
    void    setMagnitude(NxReal);
    bool    isFinite() const;
    NxReal  dot(const NxVec3& other) const;
    bool    sameDirection(const NxVec3&) const;
    NxReal  magnitude() const;
    NxReal  magnitudeSquared() const;
    NxReal  distance(const NxVec3&) const;
    NxReal  distanceSquared(const NxVec3& v) const;
    void    cross(const NxVec3& left, const NxVec3& right);
    void    cross(const NxVec3& left, const NxVec3& right, bool normalized);
    bool    equals(const NxVec3&, NxReal epsilon) const;
    void    rotate(NxReal angle, const NxVec3& axe);
    void    rotate(NxReal sin_angle, NxReal cos_angle, const NxVec3& axe);
    void    rotate(const NxVec3& axe);
    NxVec3  operator -() const;
    NxVec3  operator +(const NxVec3& v) const;
    NxVec3  operator -(const NxVec3& v) const;
    NxVec3  operator *(NxReal f) const;
    NxVec3  operator /(NxReal f) const;
    NxVec3&  operator +=(const NxVec3& v);
    NxVec3&  operator -=(const NxVec3& v);
    NxVec3&  operator *=(NxReal f);
    NxVec3&  operator /=(NxReal f);
    NxVec3  cross(const NxVec3& v) const;
    NxVec3  operator^(const NxVec3& v) const;
    NxReal  operator|(const NxVec3& v) const;
    NxReal  x, y, z;

};


#endif
