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

#include "NxVec3.h"
NxVec3::NxVec3(NxReal v) : x(v), y(v), z(v)
{
}

NxVec3::NxVec3(NxReal _x, NxReal _y, NxReal _z) : x(_x), y(_y), z(_z)
{
}

NxVec3::NxVec3(const NxReal v[]) : x(v[0]), y(v[1]), z(v[2])
{
}
NxVec3::NxVec3(const NxReal v[], bool normalized) : x(v[0]), y(v[1]), z(v[2])
{
    if (normalized) { this->normalize(); }
}

NxVec3::NxVec3(const NxVec3& v) : x(v.x), y(v.y), z(v.z)
{
}
NxVec3::NxVec3(const NxVec3& v, bool normalized) : x(v.x), y(v.y), z(v.z)
{
    if (normalized) { this->normalize(); }
}

NxVec3::NxVec3()
{
}

const NxVec3& NxVec3::operator=(const NxVec3& v)
{
    x = v.x;
    y = v.y;
    z = v.z;
    return *this;
}

const NxReal* NxVec3::get() const
{
    return &x;
}

NxReal* NxVec3::get()
{
    return &x;
}

void NxVec3::setx(const NxReal& d)
{
    x = d;
}


void NxVec3::sety(const NxReal& d)
{
    y = d;
}

void NxVec3::setz(const NxReal& d)
{
    z = d;
}

bool NxVec3::operator==(const NxVec3& v) const
{
    return ((x == v.x) && (y == v.y) && (z == v.z));
}

bool NxVec3::operator!=(const NxVec3& v) const
{
    return ((x != v.x) || (y != v.y) || (z != v.z));
}

void  NxVec3::set(const NxVec3& v)
{
    x = v.x;
    y = v.y;
    z = v.z;
}

void  NxVec3::setNegative(const NxVec3& v)
{
    x = -v.x;
    y = -v.y;
    z = -v.z;
}


void  NxVec3::setNegative()
{
    x = -x;
    y = -y;
    z = -z;
}


void  NxVec3::set(NxReal x, NxReal y, NxReal z)
{
    this->x = x;
    this->y = y;
    this->z = z;
}


void NxVec3::set(NxReal v)
{
    x = v;
    y = v;
    z = v;
}


void  NxVec3::zero()
{
    x = y = z = 0.0;
}

void  NxVec3::add(const NxVec3& a, const NxVec3& b)
{
    x = a.x + b.x;
    y = a.y + b.y;
    z = a.z + b.z;
}


void  NxVec3::subtract(const NxVec3& a, const NxVec3& b)
{
    x = a.x - b.x;
    y = a.y - b.y;
    z = a.z - b.z;
}


void  NxVec3::arrayMultiply(const NxVec3& a, const NxVec3& b)
{
    x = a.x * b.x;
    y = a.y * b.y;
    z = a.z * b.z;
}


void  NxVec3::multiply(NxReal s,  const NxVec3& a)
{
    x = a.x * s;
    y = a.y * s;
    z = a.z * s;
}


void  NxVec3::multiplyAdd(NxReal s, const NxVec3& a, const NxVec3& b)
{
    x = s * a.x + b.x;
    y = s * a.y + b.y;
    z = s * a.z + b.z;
}


NxReal NxVec3::normalize()
{
    NxReal m = magnitude();
    if (m)
    {
        const NxReal il =  NxReal(1.0) / m;
        x *= il;
        y *= il;
        z *= il;
    }
    return m;
}


void NxVec3::setMagnitude(NxReal length)
{
    NxReal m = magnitude();
    if (m)
    {
        NxReal newLength = length / m;
        x *= newLength;
        y *= newLength;
        z *= newLength;
    }
}


NxReal NxVec3::dot(const NxVec3& v) const
{
    return x * v.x + y * v.y + z * v.z;
}


bool NxVec3::sameDirection(const NxVec3& v) const
{
    return x * v.x + y * v.y + z * v.z >= 0.0f;
}


NxReal NxVec3::magnitude() const
{
    return sqrt(x * x + y * y + z * z);
}


NxReal NxVec3::magnitudeSquared() const
{
    return x * x + y * y + z * z;
}


NxReal NxVec3::distance(const NxVec3& v) const
{
    NxReal dx = x - v.x;
    NxReal dy = y - v.y;
    NxReal dz = z - v.z;
    return sqrt(dx * dx + dy * dy + dz * dz);
}


NxReal NxVec3::distanceSquared(const NxVec3& v) const
{
    NxReal dx = x - v.x;
    NxReal dy = y - v.y;
    NxReal dz = z - v.z;
    return dx * dx + dy * dy + dz * dz;
}


void NxVec3::cross(const NxVec3& left, const NxVec3& right)     //prefered version, w/o temp object.
{
    NxReal a = (left.y * right.z) - (left.z * right.y);
    NxReal b = (left.z * right.x) - (left.x * right.z);
    NxReal c = (left.x * right.y) - (left.y * right.x);

    x = a;
    y = b;
    z = c;
}

void NxVec3::cross(const NxVec3& left, const NxVec3& right, bool normalized)    //prefered version, w/o temp object.
{
    NxReal a = (left.y * right.z) - (left.z * right.y);
    NxReal b = (left.z * right.x) - (left.x * right.z);
    NxReal c = (left.x * right.y) - (left.y * right.x);

    x = a;
    y = b;
    z = c;

    if (normalized) { this->normalize(); }
}


NxVec3 NxVec3::operator -() const
{
    return NxVec3(-x, -y, -z);
}


NxVec3 NxVec3::operator +(const NxVec3& v) const
{
    return NxVec3(x + v.x, y + v.y, z + v.z);
}


NxVec3 NxVec3::operator -(const NxVec3& v) const
{
    return NxVec3(x - v.x, y - v.y, z - v.z);
}



NxVec3 NxVec3::operator *(NxReal f) const
{
    return NxVec3(x * f, y * f, z * f);
}


NxVec3 NxVec3::operator /(NxReal f) const
{
    f = NxReal(1.0) / f; return NxVec3(x * f, y * f, z * f);
}


NxVec3& NxVec3::operator +=(const NxVec3& v)
{
    x += v.x;
    y += v.y;
    z += v.z;
    return *this;
}


NxVec3& NxVec3::operator -=(const NxVec3& v)
{
    x -= v.x;
    y -= v.y;
    z -= v.z;
    return *this;
}


NxVec3& NxVec3::operator *=(NxReal f)
{
    x *= f;
    y *= f;
    z *= f;
    return *this;
}


NxVec3& NxVec3::operator /=(NxReal f)
{
    f = 1.0f / f;
    x *= f;
    y *= f;
    z *= f;
    return *this;
}


NxVec3 NxVec3::cross(const NxVec3& v) const
{
    NxVec3 temp;
    temp.cross(*this, v);
    return temp;
}


NxVec3 NxVec3::operator^(const NxVec3& v) const
{
    NxVec3 temp;
    temp.cross(*this, v);
    return temp;
}


NxReal NxVec3::operator|(const NxVec3& v) const
{
    return x * v.x + y * v.y + z * v.z;
}

NxVec3 operator *(NxReal f, const NxVec3& v)
{
    return NxVec3(f * v.x, f * v.y, f * v.z);
}
NxReal s, c, xy, ys, xz, yz, xs, zs, un_c;
void NxVec3::rotate(NxReal angle, const NxVec3& axe)
{
    s     = sin(angle);
    c     = cos(angle);
    xy    = axe.x * axe.y;
    ys    = axe.y * s;
    xz    = axe.x * axe.z;
    yz    = axe.y * axe.z;
    xs    = axe.x * s;
    zs    = axe.z * s;
    un_c  = 1 - c;
    NxReal  dst_x = (axe.x * axe.x * un_c + c) * x + (xy * un_c - zs) * y + (xz * un_c + ys) * z,
            dst_y = (xy * un_c + zs) * x + (axe.y * axe.y * un_c + c) * y + (yz * un_c - xs) * z,
            dst_z = (xz * un_c - ys) * x + (yz * un_c + xs) * y + (axe.z * axe.z * un_c + c) * z;
    x = dst_x; y = dst_y; z = dst_z;

    /*
    from http://www.opengl.org/documentation/specs/man_pages/hardcopy/GL/html/gl/rotate.html
    ( xx(1-c)+c xy(1-c)-zs  xz(1-c)+ys)
    | yx(1-c)+zs    yy(1-c)+c   yz(1-c)-xs|
    | xz(1-c)-ys    yz(1-c)+xs  zz(1-c)+c |
    Where   c = cos(angle), s = sine(angle), and ||( x,y,z )|| = 1
    */
}

void NxVec3::rotate(NxReal sin_angle, NxReal cos_angle, const NxVec3& axe)
{
    s = sin_angle;
    c = cos_angle;
    xy    = axe.x * axe.y;
    ys    = axe.y * s;
    xz    = axe.x * axe.z;
    yz    = axe.y * axe.z;
    xs    = axe.x * s;
    zs    = axe.z * s;
    un_c  = 1 - c;
    NxReal  dst_x = (axe.x * axe.x * un_c + c) * x + (xy * un_c - zs) * y + (xz * un_c + ys) * z,
            dst_y = (xy * un_c + zs) * x + (axe.y * axe.y * un_c + c) * y + (yz * un_c - xs) * z,
            dst_z = (xz * un_c - ys) * x + (yz * un_c + xs) * y + (axe.z * axe.z * un_c + c) * z;
    x = dst_x; y = dst_y; z = dst_z;

    /*
    from http://www.opengl.org/documentation/specs/man_pages/hardcopy/GL/html/gl/rotate.html
    ( xx(1-c)+c xy(1-c)-zs  xz(1-c)+ys)
    | yx(1-c)+zs    yy(1-c)+c   yz(1-c)-xs|
    | xz(1-c)-ys    yz(1-c)+xs  zz(1-c)+c |
    Where   c = cos(angle), s = sine(angle), and ||( x,y,z )|| = 1
    */
}

void  NxVec3::rotate(const NxVec3& axe)
{
    NxReal dst_x = (axe.x * axe.x * un_c + c) * x + (xy * un_c - zs) * y + (xz * un_c + ys) * z;
    NxReal dst_y = (xy * un_c + zs) * x + (axe.y * axe.y * un_c + c) * y + (yz * un_c - xs) * z;
    NxReal dst_z = (xz * un_c - ys) * x + (yz * un_c + xs) * y + (axe.z * axe.z * un_c + c) * z;
    x = dst_x; y = dst_y; z = dst_z;

    /*
    from http://www.opengl.org/documentation/specs/man_pages/hardcopy/GL/html/gl/rotate.html
    ( xx(1-c)+c xy(1-c)-zs  xz(1-c)+ys)
    | yx(1-c)+zs    yy(1-c)+c   yz(1-c)-xs|
    | xz(1-c)-ys    yz(1-c)+xs  zz(1-c)+c |
    Where   c = cos(angle), s = sine(angle), and ||( x,y,z )|| = 1
    */

}
