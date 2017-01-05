/*
 * Copyright (C) <2012-2014> <EDF-R&D> <FRANCE>
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

#ifndef TY_MC_PLAN
#define TY_MC_PLAN

#include <ostream>
#include <deque>

#include "Tympan/models/common/3d.h"

/**
 * \file plan.h
 * \class OPlan
 * \brief Plan defined by its equation : ax+by+cz+d=0.
 *
 */
class OPlan
{
    // Methods
public:
    /**
     * \brief Default constructor With a=b=c=d=0.
     */
    OPlan();

    /**
     * \brief Copy constructor.
     *
     * \param plan The object to be copied.
     */
    OPlan(const OPlan& plan);

    /**
     * \brief Constructor with the a, b, c and d parameters.
     *
     * \param a The a parameter in the equation ax+by+cz+d=0.
     * \param b The b parameter in the equation ax+by+cz+d=0.
     * \param c The c parameter in the equation ax+by+cz+d=0.
     * \param d The d parameter in the equation ax+by+cz+d=0.
     */
    OPlan(double a, double b, double c, double d);

    /**
     * \brief Constructs a new plan from 3 points.
     *
     *  1. We calculate normal vector N to the plane.
     *  2. All plan vectors are normal to N !
     *     => Scalar product P1P.N = 0
     *      Nx*(Px-P1x) + Ny*(Py-P1y) + Nz*(Pz-P1z) = 0
     *      Can be written as :
     *      a*Px + b*Py + c*Pz + d = 0 == Plane equation
     *
     * \param pt1 Point 1.
     * \param pt2 Point 2.
     * \param pt3 Point 3.
     */
    OPlan(const OPoint3D& pt1, const OPoint3D& pt2, const OPoint3D& pt3);

    /**
     * \brief Constructs a new plan from 1 point and a normal.
     *
     *  All plan vectors are normal to N !
     *  => Scalar product P1P.N = 0
     *      Nx*(Px-P1x) + Ny*(Py-P1y) + Nz*(Pz-P1z) = 0
     *      Can be written as :
     *      a*Px + b*Py + c*Pz + d = 0 == Equation du plan
     *
     * \param pt The point.
     * \param normale The normal.
     */
    OPlan(const OPoint3D& pt, const OVector3D& normale);

    /**
     * \brief Destructor.
     */
    virtual ~OPlan();

    /**
     * \brief Assignment operator.
     *
     * \param plan The source to be assigned.
     */
    OPlan& operator=(const OPlan& plan);

    /**
     * \brief The equality operator.
     *
     * \param plan The object reference with which this object is compared.
     */
    bool operator==(const OPlan& plan) const;

    /**
     * \brief The inequality operator.
     *
     * \param plan The object reference with which this object is compared.
     */
    bool operator!=(const OPlan& plan) const;

    /**
     * \fn void set(double a, double b, double c, double d);
     * \brief Sets a, b, c and d parameters directly.
     *
     * \param a The a parameter in the equation ax+by+cz+d=0.
     * \param b The b parameter in the equation ax+by+cz+d=0.
     * \param c The c parameter in the equation ax+by+cz+d=0.
     * \param d The d parameter in the equation ax+by+cz+d=0.
     */
    void set(double a, double b, double c, double d);

    /**
     * \fn void set(const OPoint3D& pt1, const OPoint3D& pt2, const OPoint3D& pt3);
     * \brief Sets with 3 points.
     *
     *  1. We calculate normal vector N to the plane.
     *  2. All plan vectors are normal to N !
     *     => Scalar product P1P.N = 0
     *      Nx*(Px-P1x) + Ny*(Py-P1y) + Nz*(Pz-P1z) = 0
     *      Can be written as :
     *      a*Px + b*Py + c*Pz + d = 0 == Plane equation
     *
     * \param pt1 Point 1.
     * \param pt2 Point 2.
     * \param pt3 Point 3.
     */
    void set(const OPoint3D& pt1, const OPoint3D& pt2, const OPoint3D& pt3);

    /**
     * \fn void set(const OPoint3D& pt, const OVector3D& normale);
     * \brief Sets with 1 point and a normal.
     *
     *  1. We calculate normal vector N to the plane.
     *  2. All plan vectors are normal to N !
     *     => Scalar product P1P.N = 0
     *      Nx*(Px-P1x) + Ny*(Py-P1y) + Nz*(Pz-P1z) = 0
     *      Can be written as :
     *      a*Px + b*Py + c*Pz + d = 0 == Plane equation
     *
     * \param pt The point.
     * \param normale The normal.
     */
    void set(const OPoint3D& pt, const OVector3D& normale);

    /**
     * \fn int intersectsSegment(const OPoint3D& pt1, const OPoint3D& pt2, OPoint3D& ptIntersec) const;
     * \brief Calculate the intersection of this plane with segment defined by two points.
     *
     * Note :
     *  In the case the segment belongs to the plane, the result
     *  is INTERS_CONFONDU, and the coordinates of the intersection
     *  are not updated.
     *
     * Algorithm :
     *  - The plane is defined by :
     *  .   a*x+b*y+c*z+d=0
     *  - The segment is defined by :
     *  .   x=x1+t*(x2-x1)
     *  .   y=y1+t*(y2-y1)
     *  .   z=z1+t*(z2-z1)
     *  .   with 0 <= t <= 1
     *  - We solve the system :
     *  .   t = -(a*x1+b*y1+c*z1+d)/D
     *  .   with D = a*(x2-x1)+b*(y2-y1)+c*(z2-z1)
     *  .   and D = scalar product between the plane normal and
     *  .   the segment vector, so if D is zero, then
     *  .   the segment is parallel to the plane and we test
     *  .   if a point of the segment belongs to the plane to
     *  .   confirm that the segment belongs to the plane or not...
     *
     * \param pt1 The segment first point.
     * \param pt2 The segment second point.
     * \param ptIntersec The intersection point.
     *
     * \return INTERS_OUI (1) if the intersection happens,
     *         INTERS_CONFONDU (2) if the elements are overlaid,
     *         INTERS_NULLE (0) if no intersection happens.
     */
    int intersectsSegment(const OPoint3D& pt1, const OPoint3D& pt2, OPoint3D& ptIntersec) const;

    /**
     * \fn int intersectsDroite(const OPoint3D& pt, const OVector3D& vector, OPoint3D& ptIntersec);
     * \brief Calculate the intersection of this plane with a line defined by a point and a vector.
     *
     * \param pt A point belonging to the line.
     * \param vector A vector defining the line.
     * \param ptIntersec The intersection point.
     *
     * \return INTERS_OUI (1) if the intersection happens,
     *         INTERS_CONFONDU (2) if the elements are overlaid,
     *         INTERS_NULLE (0) if no intersection happens.
     */
    int intersectsDroite(const OPoint3D& pt, const OVector3D& vector, OPoint3D& ptIntersec);

    /**
     * \fn int intersectsPlan(const OPlan& plan, OVector3D& vectorIntersec);
     * \brief Calculate the intersection of this plane with another plane.
     *
     * \param plan An another plane.
     * \param vectorIntersec The vector defining the intersection.
     *
     * \return INTERS_OUI (1) if the intersection happens,
     *         INTERS_CONFONDU (2) if the elements are overlaid,
     *         INTERS_NULLE (0) if no intersection happens.
     */
    int intersectsPlan(const OPlan& plan, OVector3D& vectorIntersec);

    /**
     * \fn int intersectsSurface(const TabPoint3D& contour, OSegment3D& segment);
     * \brief Compute intersection between a plan and a surface defined by his bounds
     */
    int intersectsSurface(const TabPoint3D& contour, OSegment3D& segment) const;

    /**
     * \fn bool isInPlan(const OPoint3D& pt);
     * \brief Check if a point belongs to a plane.
     */
    bool isInPlan(const OPoint3D& pt);

    /**
     * \brief Check whether the plane is valid.
     *
     * a OPlan object can be invalid, i.e. not actually represent a
     * plane, in case two cases :
     *  - the normal vector (\c _a, \c _b, \c _c) is null.
     *    This is the case when a OPlan is default constructed.
     *  - the normal vector, the origin, ... has not component which is NaN
     *    This can happen when points with NaN component are used.
     */
    bool is_valid();
    bool is_null();
    bool is_NaN();


    /**
     * \fn double angle(const OPlan& plan);
     * \brief Calculation of the angle between this plane and another plane.
     *
     * \param plan Another plane.
     *
     * \return The angle between the 2 planes.
     */
    double angle(const OPlan& plan);

    /**
     * \fn double distance(const OPoint3D& pt);
     * \brief Calculation of the minimal distance between a point and this plane.
     *
     * \param pt A point.
     *
     * \return The minimal distance between a point and this plane.
     */
    double distance(const OPoint3D& pt);


    /*!
     * \fn OPoint3D symPtPlan(const OPoint3D& pt)
     * \brief Calculate the symmetrical of a point relative to the plane
     */
    OPoint3D symPtPlan(const OPoint3D& pt);

    /*!
     * \fn OPoint3D projPtPlan(const OPoint3D& pt)
     * \brief Calculate the projection of a point on the plane
     */
    OPoint3D projPtPlan(const OPoint3D& pt);

    /**
     * \fn bool distancePlanParallel(const OPlan& plan, double& distance);
     * \brief Calculate the distance between this plan and another parallel plane.
     *
     * \param plan Another plane.
     * \param distance The distance between 2 planes if they are parallel.
     *
     * \return <code>true</code> if the two planes are parallel;
     *         <code>false</code> else.
     */
    bool distancePlanParallel(const OPlan& plan, double& distance);

    /**
     * \fn bool isParallel(const OPlan& plan);
     * \brief Check if this plane is parallel with another plane.
     *
     * \param plan Another plane.
     *
     * \return <code>true</code> if two planes are parallel;
     *         <code>false</code> else.
     */
    bool isParallel(const OPlan& plan);

    /**
     * \fn bool isOrthogonal(const OPlan& plan);
     * \brief Check if this plan is perpendicular to another plan.
     *
     * \param plan Another plane.
     *
     * \return <code>true</code> if the 2 planes are perpendicular;
     *         <code>false</code> else.
     */
    bool isOrthogonal(const OPlan& plan);

protected:
    /**
     * \brief updates the implicit representation of the plane
     *
     * We derive a reference frame for the plane (vector u and v) from
     * the implicit equation which is the primary representation of
     * the plane.
     *
     */
    void update_explicit_repr(OVector3D hint = OVector3D(1, 1, 1));

    //Members
public:
    ///The a parameter in the equation ax+by+cz+d=0.
    double _a;
    ///The b parameter in the equation ax+by+cz+d=0.
    double _b;
    ///The c parameter in the equation ax+by+cz+d=0.
    double _c;
    ///The d parameter in the equation ax+by+cz+d=0.
    double _d;
    // Redundant but explicit representation : the base vectors
    ORepere3D rframe;
};

::std::ostream& operator<<(::std::ostream& os, const OPlan& p);

#endif //TY_MC_PLAN
