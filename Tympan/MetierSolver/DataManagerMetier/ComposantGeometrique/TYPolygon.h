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
 *
 */

#ifndef __TY_POLYGON__
#define __TY_POLYGON__


#include "TYRepere.h"
#include "TYSurfaceInterface.h"

#if TY_USE_IHM
#include "Tympan/GraphicIHM/DataManagerIHM/TYPolygonWidget.h"
#include "Tympan/GraphicIHM/DataManagerGraphic/TYPolygonGraphic.h"
#endif


/**
 * Classe de definition d'un polygone.
 *
 * @author Projet_Tympan
 *
 */
class TYPolygon: public TYElement, public TYSurfaceInterface
{
    OPROTOSUPERDECL(TYPolygon, TYElement)
    TY_EXTENSION_DECL(TYPolygon)
    TY_EXT_GRAPHIC_DECL(TYPolygon)

    // Methodes
public:
    /**
     * Constructeur par defaut.
     */
    TYPolygon();
    /**
     * Constructeur par copie.
     */
    TYPolygon(const TYPolygon& other);
    /**
     * Constructeur a partir d'un tableau de points.
     */
    TYPolygon(const TYTabPoint& pts);

    /**
     * Destructeur.
     */
    virtual ~TYPolygon();

    ///Operateur =.
    TYPolygon& operator=(const TYPolygon& other);
    ///Operateur ==.
    bool operator==(const TYPolygon& other) const;
    ///Operateur !=.
    bool operator!=(const TYPolygon& other) const;

    virtual bool deepCopy(const TYElement* pOther, bool copyId = true);

    virtual std::string toString() const;

    virtual DOM_Element toXML(DOM_Element& domElement);
    virtual int fromXML(DOM_Element domElement);

    virtual double surface() const;
    virtual OVector3D normal() const;
    virtual OPlan plan() const;
    virtual TYTabPoint getContour(int n = -1) const;
	virtual TYTabPoint3D getOContour(int n = -1) const;

    // TODO Check and tests those methods for numerical stability
    virtual int intersects(const TYSurfaceInterface* pSurf, OSegment3D& seg) const;
    virtual int intersects(const OSegment3D& seg, OPoint3D& pt) const;
    virtual int intersects(const OSegment3D& seg, OPoint3D& pt, bool insideTest) const;

    //XXX This method is not numerically stable !
    virtual int intersects(const OPoint3D& pt) const;

    /**
     * Vide le tableau de points.
     */
    void purge() { _pts.clear(); _bConvex = false; updateNormal(); updateBox(); setIsGeometryModified(true); }

    /**
     * Retourne le nombre de points composants ce polygone.
     */
    size_t getNbPts() const { return _pts.size(); }

    /**
     * Set/Get de la collection de points.
     */
    void setPoints(const TYTabPoint& pts);
    /**
     * Return true if the all the points of the polygon are coplanar.
     */
    bool checkCoplanar() const;
    /**
     * Set/Get de la collection de points.
     */
    const TYTabPoint& getPoints() const { return _pts; }
    TYTabPoint& getPoints() { return _pts; }
    /**
     * Get d'un point dans collection de point
     */
    OPoint3D getPoint(size_t index) const { return _pts[index]; }
    /**
     * Transforme les points du polygone avec la matrice passee en parametre.
     */
    void transform(const OMatrix& matrix);
    /**
     * Set de l'attribut de convexite.
     */
    void setConvex(bool bConvex)    { _bConvex = bConvex; }
    /**
     * Get de l'attribut de convexite.
     */
    bool isConvex() const           { return _bConvex; }
    /**
     * Teste si le point se trouve dans la boite du polygone.
     */
    bool isInBox(const OPoint3D& pt) const     { return _box.isInside(pt); }

    /**
     * Teste si, vu de dessus, le point se trouve dans la boite du polygone.
     */
    bool isInBox2D(const OPoint3D& pt) const       { return _box.isInside2D(pt); }

    OPoint3D getCenter() const
    {
        OPoint3D p1;
        p1._x = (_box._max._x + _box._min._x) / 2;
        p1._y = (_box._max._y + _box._min._y) / 2;
        p1._z = (_box._max._z + _box._min._z) / 2;
        return p1;
    }


    /**
     * Test si ce polygone est valide.
     * Les points de ce polygone doivent appartenir au meme plan.
     *
     * @return <code>true</code> si le polygon est valide;
     *         <code>false</code> sinon.
     */
    bool isValid() const;

    /**
     * Calcule le repere local a ce polygone par rapport au repere global.
     */
    ORepere3D getRepere() const;

    /**
     * Calcul le rectangle englobant ce polygone.
     *
     * @return Le rectangle englobant ce polygone.
     */
    TYRectangle getBoundingRect() const;

    /**
     * retourne le plan associe a cette surface
     */
    virtual const OPlan& getPlan() const { return _plan; }
    void updateNormal();
    void updateBox();

    virtual void inverseNormale();

    /**
     * @brief Export the surface as a triangular mesh
     *
     * NB : This function expect empty deques and will clear the deque passed.
     *
     * @param points output argument filled with the vertices of the triangulation
     * @param triangles output argument filled with the faces of the triangulation
     */
    void
    exportMesh(
    		std::deque<OPoint3D>& points,
    		std::deque<OTriangle>& triangles,
                const TYGeometryNode& geonode) const;



    // Membres
private:
    ///Sommets.
    TYTabPoint      _pts; // TODO change this to TYTabPoint3D

    OPlan           _plan;

    OBox            _box;

    bool            _bConvex;


    int isInpolyXY(const TYTabPoint& poly, int npoints, double xt, double yt) const;
    int isInpolyXZ(const TYTabPoint& poly, int npoints, double xt, double yt) const;
    int isInpolyYZ(const TYTabPoint& poly, int npoints, double xt, double yt) const;
};


#endif // __TY_POLYGON__
