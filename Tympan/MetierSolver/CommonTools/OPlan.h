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

/*
 *
 *
 *
 */

#ifndef __O_PLAN__
#define __O_PLAN__

#include <ostream>
#include <deque>

#include "Tympan/models/common/3d.h"

/**
 * \file OPlan.h
 * \class OPlan
 * \brief Plan defined by its equation : ax+by+cz+d=0.
 * \author Projet_Tympan
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
     *  1. On calcul le vecteur N normal au plan.
     *  2. Tous les vecteurs du plan sont orthogonaux a N !
     *     => Produit scalaire P1P.N = 0
     *      Nx*(Px-P1x) + Ny*(Py-P1y) + Nz*(Pz-P1z) = 0
     *      A mettre sous la forme :
     *      a*Px + b*Py + c*Pz + d = 0 == Equation du plan
     *
     * \param pt1 Le point 1.
     * \param pt2 Le point 2.
     * \param pt3 Le point 3.
     */
    OPlan(const OPoint3D& pt1, const OPoint3D& pt2, const OPoint3D& pt3);

    /**
     * \brief Constructs a new plan from 1 point and a normale.
     *
     *  Tous les vecteurs du plan sont orthogonaux a N !
     *  => Produit scalaire P1P.N = 0
     *      Nx*(Px-P1x) + Ny*(Py-P1y) + Nz*(Pz-P1z) = 0
     *      A mettre sous la forme :
     *      a*Px + b*Py + c*Pz + d = 0 == Equation du plan
     *
     * \param pt Le point.
     * \param normale La normale.
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
     *  1. On calcul le vecteur N normal au plan.
     *  2. Tous les vecteurs du plan sont orthogonaux a N !
     *     => Produit scalaire P1P.N = 0
     *      Nx*(Px-P1x) + Ny*(Py-P1y) + Nz*(Pz-P1z) = 0
     *      A mettre sous la forme :
     *      a*Px + b*Py + c*Pz + d = 0 == Equation du plan
     *
     * \param pt1 Le point 1.
     * \param pt2 Le point 2.
     * \param pt3 Le point 3.
     */
    void set(const OPoint3D& pt1, const OPoint3D& pt2, const OPoint3D& pt3);

    /**
     * \fn void set(const OPoint3D& pt, const OVector3D& normale);
     * \brief Sets with 1 point and a normale.
     *
     *  Tous les vecteurs du plan sont orthogonaux a N !
     *  => Produit scalaire P1P.N = 0
     *      Nx*(Px-P1x) + Ny*(Py-P1y) + Nz*(Pz-P1z) = 0
     *      A mettre sous la forme :
     *      a*Px + b*Py + c*Pz + d = 0 == Equation du plan
     *
     * \param pt Le point.
     * \param normale La normale.
     */
    void set(const OPoint3D& pt, const OVector3D& normale);

    /**
     * \fn int intersectsSegment(const OPoint3D& pt1, const OPoint3D& pt2, OPoint3D& ptIntersec) const;
     * \brief Calcul de l'intersection  avec un plan avec un segment defini par deux points.
     *
     * Remarque :
     *  Dans le cas ou le segment est compris dans le plan, le resultat
     *  est INTERS_CONFONDU, et les coordonnees de l'intersection ne
     *  sont pas mises a jour.
     *
     * Principe :
     *  - Plan defini par :
     *  .   a*x+b*y+c*z+d=0
     *  - Segment defini par :
     *  .   x=x1+t*(x2-x1)
     *  .   y=y1+t*(y2-y1)
     *  .   z=z1+t*(z2-z1)
     *  .   avec 0 <= t <= 1
     *  - On resoud le systeme :
     *  .   t = -(a*x1+b*y1+c*z1+d)/D
     *  .   avec D = a*(x2-x1)+b*(y2-y1)+c*(z2-z1)
     *  .   et D = produit scalaire entre la normale au plan et le
     *  .   vecteur directeur du segment, donc si D est nul, alors
     *  .   le segment est parallele au plan et il suffit de tester
     *  .   si un point du segment est dans le plan pour savoir si
     *  .   le segment est dans le plan ou non...
     *
     * \param pt1 Le 1er point du segment.
     * \param pt2 Le 2eme point du segment.
     * \param ptIntersec Le point correspondant a l'intersection.
     *
     * \return INTERS_OUI (1) si l'intersection existe,
     *         INTERS_CONFONDU (2) si les elements sont confondus,
     *         INTERS_NULLE (0) s'il n'y a pas d'intersection.
     */
    int intersectsSegment(const OPoint3D& pt1, const OPoint3D& pt2, OPoint3D& ptIntersec) const;

    /**
     * \fn int intersectsDroite(const OPoint3D& pt, const OVector3D& vector, OPoint3D& ptIntersec);
     * \brief Calcul de l'intersection de ce plan avec une droite definie par un point et un vecteur.
     *
     * \param pt Un point appartenant a la droite.
     * \param vector Un vecteur definissant la droite.
     * \param ptIntersec Le point correspondant a l'intersection.
     *
     * \return INTERS_OUI (1) si l'intersection existe,
     *         INTERS_CONFONDU (2) si les elements sont confondus,
     *         INTERS_NULLE (0) s'il n'y a pas d'intersection.
     */
    int intersectsDroite(const OPoint3D& pt, const OVector3D& vector, OPoint3D& ptIntersec);

    /**
     * \fn int intersectsPlan(const OPlan& plan, OVector3D& vectorIntersec);
     * \brief Calcul de l'intersection de ce plan avec un autre plan.
     *
     * \param plan Un autre plan.
     * \param vectorIntersec Le vecteur correspondant a l'intersection.
     *
     * \return INTERS_OUI (1) si l'intersection existe,
     *         INTERS_CONFONDU (2) si les elements sont confondus,
     *         INTERS_NULLE (0) s'il n'y a pas d'intersection.
     */
    int intersectsPlan(const OPlan& plan, OVector3D& vectorIntersec);

    /**
     * \fn int intersectsSurface(const std::deque<OPoint3D>& tabPoints, OSegment3D& segment);
     * \brief compute intersection between a plan and a surface defined by his bounds
     */
    int intersectsSurface(const std::deque<OPoint3D>& contour, OSegment3D& segment) const;

    /**
     * \fn bool isInPlan(const OPoint3D& pt);
     * \brief Verifie si un point appartient au plan
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
     *    This can happend when points with NaN component are used.
     */
    bool is_valid();
    bool is_null();
    bool is_NaN();


    /**
     * \fn double angle(const OPlan& plan);
     * \brief Calcul de l'angle entre ce plan et un autre plan.
     *
     * \param plan Un autre plan.
     *
     * \return L'angle entre les 2 plans.
     */
    double angle(const OPlan& plan);

    /**
     * \fn double distance(const OPoint3D& pt);
     * \brief Calcul de la distance minimale entre un point et ce plan.
     *
     * \param pt Un point.
     *
     * \return La distance minimale entre le point et ce plan.
     */
    double distance(const OPoint3D& pt);


    /*!
     * \fn OPoint3D symPtPlan(const OPoint3D& pt)
     * \brief Calcule le symétrique d'un point par rapport au plan
     */
    OPoint3D symPtPlan(const OPoint3D& pt);

    /*!
     * \fn OPoint3D projPtPlan(const OPoint3D& pt)
     * \brief Calcule le projeté d'un point sur le plan
     */
    OPoint3D projPtPlan(const OPoint3D& pt);

    /**
     * \fn bool distancePlanParallel(const OPlan& plan, double& distance);
     * \brief Calcul de la distance entre ce plan et un autre plan parallele a ce plan.
     *
     * \param plan Un autre plan.
     * \param distance La distance entre les 2 plans si ceux-la sont paralleles.
     *
     * \return <code>true</code> si les 2 plans sont bien paralleles;
     *         <code>false</code> sinon.
     */
    bool distancePlanParallel(const OPlan& plan, double& distance);

    /**
     * \fn bool isParallel(const OPlan& plan);
     * \brief Determine si ce plan est parallele avec un autre plan.
     *
     * \param plan Un autre plan.
     *
     * \return <code>true</code> si les 2 plans sont paralleles;
     *         <code>false</code> sinon.
     */
    bool isParallel(const OPlan& plan);

    /**
     * \fn bool isOrthogonal(const OPlan& plan);
     * \brief Determine si ce plan est perpendiculaire avec un autre plan.
     *
     * \param plan Un autre plan.
     *
     * \return <code>true</code> si les 2 plans sont perpendiculaires;
     *         <code>false</code> sinon.
     */
    bool isOrthogonal(const OPlan& plan);

protected:
    /**
     * \brief updates the implicit representation of the plane
     *
     * We derive a reference frame for the plane (vector u and v) from
     * the implicit equation which is the primnary representation of
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

#endif //__O_PLAN__
