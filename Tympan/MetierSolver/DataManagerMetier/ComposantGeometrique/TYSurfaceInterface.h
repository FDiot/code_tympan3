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
 */

#ifndef __TY_SURFACEINTERFACE__
#define __TY_SURFACEINTERFACE__

class OSegment3D;
class OPoint3D;
class OPlan;

/**
 * Classe abstraite qui offre une interface utilisable par des composants
 * ayant des proprietes geometriques surfaciques.
 */
class TYSurfaceInterface
{
    // Methodes
public:
    /**
     * @name Construction et Destruction
     */
    //@{

    /**
     * Constructeur par defaut.
     */
    TYSurfaceInterface() {};
    /**
     * Destructeur.
     */
    virtual ~TYSurfaceInterface() {};

    //@}

    /**
     * @name Interface Surface
     */
    //@{

    /**
     * Calcule l'aire de la surface totale.
     */
    virtual double surface() const = 0;

    /**
     * Calcule la normale a cette surface.
     */
    virtual OVector3D normal() const = 0;

    /**
     * Calcule le plan dans lequel cette surface est inscrit.
     */
    virtual OPlan plan() const = 0;

    /**
     * Retourne un tableau de points correspondant au contour de
     * cette surface.
     *
     * @param n Le nombre de portion pour le decoupage.
     *          Si n=-1, n sera calcule a partir de la precision
     *          de distance definie par defaut.
     *          Pour certaine surface, le nombre de portion demande ne
     *          sera pas forcement le nombre de points obtenus.
     *
     * @return Un tableau de points representant le contour de cette
     *         surface.
     */
    virtual TYTabPoint getContour(int n = -1) const = 0;

    virtual TYTabPoint3D getOContour(int n = -1) const = 0;

    /**
     * Test si le point se trouve a l'interieur de la surface, sur le
     * meme plan.
     *
     * @param pt Le point pour lequel on effectue le test.
     *
     * @return INTERS_OUI (1) si le point est a l'interieur de la surface,
     *         INTERS_NULLE (0) si le point est a l'exterieur de la surface.
     */
    virtual int intersects(const OPoint3D& pt) const = 0;

    /**
     * Calcule le point d'intersection avec un segment.
     *
     * @param seg Le segment dont on cherche l'intersection.
     * @param pt Le point resultant de l'intersection.
     *
     * @return INTERS_OUI (1) si l'intersection existe,
     *         INTERS_CONFONDU (2) si les elements sont confondus,
     *         INTERS_NULLE (0) s'il n'y a pas d'intersection.
     */
    virtual int intersects(const OSegment3D& seg, OPoint3D& pt) const = 0;

    /**
     * Calcule le segment d'intersection avec une autre surface.
     *
     * @param pSurf La surface dont on cherche l'intersection.
     * @param seg Le segment resultant de l'intersection.
     *
     * @return INTERS_OUI (1) si l'intersection existe,
     *         INTERS_CONFONDU (2) si les elements sont confondus,
     *         INTERS_NULLE (0) s'il n'y a pas d'intersection.
     */
    virtual int intersects(const TYSurfaceInterface* pSurf, OSegment3D& seg) const;

    /**
     * Calcule le segment d'intersection entre cette surface et un plan.
     *
     * @param plan La surface dont on cherche l'intersection.
     * @param seg Le segment resultant de l'intersection.
     *
     * @return INTERS_OUI (1) si l'intersection existe,
     *         INTERS_CONFONDU (2) si les elements sont confondus,
     *         INTERS_NULLE (0) s'il n'y a pas d'intersection.
     */
    virtual int intersectsPlan(const OPlan& plan, OSegment3D& seg) const;

    /**
     * Inverse la normale a la surface
     */
    virtual void inverseNormale() {};

    //@}
};




#endif // __TY_SURFACEINTERFACE__
