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

#ifndef BBOX_H
#define BBOX_H

#include <iostream>
#include "Tympan/geometric_methods/AcousticRaytracer/Geometry/mathlib.h"

/*!
 * \file BBox.h
 * \class BBox
 * \brief Definition of a bounding box which is aligned along the axis (BBox AABB).
 */

using namespace std;

class BBox
{
public:

    //public data
    vec3 pMin;          //!< Lower point of the BBox
    vec3 pMax;          //!< Upper point of the BBox
    vec3 centroid;      //!< Center point of the BBox
    bool isNull;        //!< True if the BBox is initialized, false if not


    //public methods
    /// Default constructor
    BBox() : isNull(true) {}

    /*!
    *  \brief Constructor with initialization of the 3 points of the BBox
    *  \param _pMin : Definition of the lower point
    *  \param _pMax : Definition of the upper point
    */
    BBox(const vec3& _pMin, const vec3& _pMax) : isNull(false) {pMin = _pMin; pMax = _pMax; centroid = (pMin + pMax) / 2.0f;}

    /// Copy constructor
    BBox(const BBox& other) { pMin = vec3(other.pMin); pMax = vec3(other.pMax); centroid = (pMin + pMax) / 2.0f; isNull = other.isNull;}

    /// Return the lower point
    vec3 getBBMin() {return pMin;}

    /// Return the upper point
    vec3 getBBMax() {return pMax;}

    /// Return the center point
    vec3 getCentroid() {return centroid;}

    /// Set the lower point (center point is recomputed)
    void setBBMin(vec3& _pMin) {pMin = _pMin; centroid = (pMin + pMax) / 2.0f;}

    /// Set the upper point (center point is recomputed)
    void setBBMax(vec3& _pMax) {pMax = _pMax; centroid = (pMin + pMax) / 2.0f;}

    /// Return the BBox area (sum of the lateral areas). Used for the SAH calculation of the accelerators
    double SurfaceArea() const
    {
        vec3 d = pMax - pMin;
        return 2 * (d.x * d.y + d.x * d.z + d.y * d.z);
    }

    /*!
    *  \brief Union of a point and a BBox. A new BBox is created.
    *  \param b : Initial BBox (will be not modified)
    *  \param p : Point to insert on the initial BBox
    *  \return Return a new BBox
    */
    BBox Union(const BBox& b, const vec3& p)
    {
        BBox ret = b;
        ret.pMin.x = min(b.pMin.x, p.x);
        ret.pMin.y = min(b.pMin.y, p.y);
        ret.pMin.z = min(b.pMin.z, p.z);
        ret.pMax.x = max(b.pMax.x, p.x);
        ret.pMax.y = max(b.pMax.y, p.y);
        ret.pMax.z = max(b.pMax.z, p.z);
        ret.centroid = (ret.pMin + ret.pMax) * 0.5;
        return ret;
    }

    /*!
    *  \brief Union of a point and the current (this) BBox. A new BBox is created.
    *  \param p : Point to insert on the initial BBox
    *  \return Return a new BBox
    */
    BBox Union(const vec3& p)
    {
        BBox ret;
        ret.isNull = false;
        ret.pMin.x = min(pMin.x, p.x);
        ret.pMin.y = min(pMin.y, p.y);
        ret.pMin.z = min(pMin.z, p.z);
        ret.pMax.x = max(pMax.x, p.x);
        ret.pMax.y = max(pMax.y, p.y);
        ret.pMax.z = max(pMax.z, p.z);
        ret.centroid = (ret.pMin + ret.pMax) * 0.5;
        return ret;
    }

    /*!
    *  \brief Union of a BBox and the current one. A new BBox is created.
    *  \param b2 : A BBox (will be not modified)
    *  \return Return a new BBox
    */
    BBox Union(const BBox& b2)
    {
        BBox ret;
        if (isNull)
        {
            ret = b2;
        }
        else if (b2.isNull)
        {
            ret = *this;
        }
        else
        {
            ret.isNull = false;
            ret.pMin.x = min(pMin.x, b2.pMin.x);
            ret.pMin.y = min(pMin.y, b2.pMin.y);
            ret.pMin.z = min(pMin.z, b2.pMin.z);
            ret.pMax.x = max(pMax.x, b2.pMax.x);
            ret.pMax.y = max(pMax.y, b2.pMax.y);
            ret.pMax.z = max(pMax.z, b2.pMax.z);
            ret.centroid = (ret.pMin + ret.pMax) * 0.5;
        }
        return ret;
    }

    /*!
    *  \brief Union of two BBox. A new BBox is created.
    *  \param b1 : First BBox (will be not modified)
    *  \param b2 : Second BBox (will be not modified)
    *  \return Return a new BBox
    */
    BBox Union(const BBox& b1, const BBox& b2)
    {
        BBox ret = b1;
        ret.pMin.x = min(b1.pMin.x, b2.pMin.x);
        ret.pMin.y = min(b1.pMin.y, b2.pMin.y);
        ret.pMin.z = min(b1.pMin.z, b2.pMin.z);
        ret.pMax.x = max(b1.pMax.x, b2.pMax.x);
        ret.pMax.y = max(b1.pMax.y, b2.pMax.y);
        ret.pMax.z = max(b1.pMax.z, b2.pMax.z);
        ret.centroid = (ret.pMin + ret.pMax) * 0.5;
        return ret;
    }

    /*!
    * \brief Check if a point is inside the BBox
    * \return Return true if the point is inside
    */
    inline bool isInBox(const vec3& p) const
    {
        return p.x >= pMin.x && p.y >= pMin.y && p.z >= pMin.z && p.x <= pMax.x && p.y <= pMax.y && p.z <= pMax.z;
    }

    /*!
    * \brief Test the intersection of a BBox with this one.
    * \return Return true if they intersect. Also true if the BBox is included into the other.
    */
    inline bool intersectBox(const BBox& box)
    {
        if (isNull || box.isNull) { return false; }
        bool x = (pMax.x >= box.pMin.x) && (pMin.x <= box.pMax.x);
        bool y = (pMax.y >= box.pMin.y) && (pMin.y <= box.pMax.y);
        bool z = (pMax.z >= box.pMin.z) && (pMin.z <= box.pMax.z);
        return (x && y && z);
    }

    /// Return the index of the dominant direction (maximal dimension). Index is 0 for x, 1 for y, 2 for z
    int MaximumExtend() const
    {
        vec3 diag = pMax - pMin;
        if (diag.x > diag.y && diag.x > diag.z)
        {
            return 0;
        }
        else if (diag.y > diag.z)
        {
            return 1;
        }
        else
        {
            return 2;
        }
    }

    /*!
    *  \brief Compute the intersection between a Ray and this BBox
    *  \param pos [in] Ray position
    *  \param dir [in] Ray direction
    *  \param hitt0 [out] Pointer to a float storing the t entering point of the ray from this BBox
    *  \param hitt1 [out] Pointer to a float storing the t leaving point of the ray from this BBox
    *  \return Return true if the ray is in contact with this BBox
    */
    bool IntersectP(vec3 pos, vec3 dir, decimal* hitt0 = NULL, decimal* hitt1 = NULL) const
    {
        if (isNull) { return false; }
        //float t0 = ray->mint; float t1 = ray->maxt;
        decimal t0 = 0, t1 = 1000000;
        decimal invRayDir, tNear, tFar;
        vec3 o = pos;//vec3(ray->position);
        vec3 d = dir;//vec3(ray->direction);

        //Composante sur X
        invRayDir = 1 / d.x;
        tNear = (pMin.x - o.x) * invRayDir;
        tFar = (pMax.x - o.x) * invRayDir;

        if (tNear > tFar) { swap(tNear, tFar); }
        t0 = tNear > t0 ? tNear : t0;
        t1 = tFar < t1 ? tFar : t1;
        if (t0 > t1) { return false; }

        //Composante sur Y
        invRayDir = 1 / d.y;
        tNear = (pMin.y - o.y) * invRayDir;
        tFar = (pMax.y - o.y) * invRayDir;

        if (tNear > tFar) { swap(tNear, tFar); }
        t0 = tNear > t0 ? tNear : t0;
        t1 = tFar < t1 ? tFar : t1;
        if (t0 > t1) { return false; }

        //Composante sur Z
        invRayDir = 1 / d.z;
        tNear = (pMin.z - o.z) * invRayDir;
        tFar = (pMax.z - o.z) * invRayDir;

        if (tNear > tFar) { swap(tNear, tFar); }
        t0 = tNear > t0 ? tNear : t0;
        t1 = tFar < t1 ? tFar : t1;
        if (t0 > t1) { return false; }

        if (hitt0) { *hitt0 = t0; }
        if (hitt1) { *hitt1 = t1; }
        return true;
    }

    /* Not used (and seems not coded)
    void print()
    {
        if (isNull)   //std::cout << "Non initialisee." << std::endl;
        {
        }
        else   //std::cout << "Initialisee." << std::endl;
        {
        }
    } */

    /// Return the lower (0) or upper (1) point
    vec3& operator[](int i)
    {
        if (i == 0) { return pMin; }
        else { return pMax; }
    }
    /// Return the lower (0) or upper (1) point
    const vec3& operator[](int i) const
    {
        if (i == 0) { return pMin; }
        else { return pMax; }
    }

    /// Diagonal length of the BBox
    decimal diag() { return sqrt((pMax.x - pMin.x) * (pMax.x - pMin.x) + (pMax.y - pMin.y) * (pMax.y - pMin.y) + (pMax.z - pMin.z) * (pMax.z - pMin.z)); }

    /// Return true if the point pt is inside the BBox
    bool Inside(vec3& pt) const
    {
        return (pt.x >= pMin.x && pt.x <= pMax.x &&
                pt.y >= pMin.y && pt.y <= pMax.y &&
                pt.z >= pMin.z && pt.z <= pMax.z);
    }

};

#endif // BBOX_H
