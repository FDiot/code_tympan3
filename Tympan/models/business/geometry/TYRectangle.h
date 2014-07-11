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

#ifndef __TY_RECTANGLE__
#define __TY_RECTANGLE__

#include "Tympan/models/business/TYElement.h"
#include "TYPoint.h"
#include "TYRepere.h"
#include "TYPolygon.h"
#include "TYSurfaceInterface.h"

/**
 * Classe de definition d'un rectangle.
 */
class TYRectangle: public TYElement, public TYSurfaceInterface
{
    OPROTODECL(TYRectangle)
    TY_EXTENSION_DECL_ONLY(TYRectangle)
    TY_EXT_GRAPHIC_DECL_ONLY(TYRectangle)

    // Methodes
public:
    /**
     * Constructeur par defaut.
     */
    TYRectangle();
    /**
     * Constructeur par copie.
     */
    TYRectangle(const TYRectangle& other);

    /**
     * Constructeur a partir de 4 points.
     * <pre>
     *              0-----------1
     *              |           |
     *              |           |
     *              |           |
     *              3-----------2
     * </pre>
     *
     * @param pt0 Le point 0 du rectangle.
     * @param pt1 Le point 1 du rectangle.
     * @param pt2 Le point 2 du rectangle.
     * @param pt3 Le point 3 du rectangle.
     */
    TYRectangle(const TYPoint& pt0, const TYPoint& pt1, const TYPoint& pt2, const TYPoint& pt3);

    /**
     * Constructeur a partir du premier segment (n°0, segment directeur) et
     * du second segment (n°1).
     * Le point B du segment 0 et le point A du segment 1 doivent evidement
     * etre confondus.
     * <pre>
     *              ----- 0 -----
     *              |           |
     *              3           1
     *              |           |
     *              ----- 2 -----
     * </pre>
     *
     * @param seg0 Le segment 0 du rectangle (segment directeur).
     * @param seg1 Le segment 1 du rectangle.
     */
    TYRectangle(const TYSegment& seg0, const TYSegment& seg1);

    /**
     * Destructeur.
     */
    virtual ~TYRectangle();

    /**
     * Assigne des valeurs a l'objet.
     *
     * @param pt0 Le point 0 du rectangle.
     * @param pt1 Le point 1 du rectangle.
     * @param pt2 Le point 2 du rectangle.
     * @param pt3 Le point 3 du rectangle.
     */
    void set(TYPoint pt0, TYPoint pt1, TYPoint pt2, TYPoint pt3);

    /**
     * Definit les points de l'objet a partir de ses dimensions.
     *
     * @param lon La longueur du rectangle.
     * @param haut La hauteur du rectangle.
     *
     */
    void setDimension(float lon, float haut);

    /**
     * Idem setDimension().
     * Redimensionne le rectangle node en appliquant le meme facteur
     * d'echelle aux sous rectangles.
     *
     * @param sizeX La nouvelle taille en X.
     * @param sizeY La nouvelle taille en Y.
     */
    void setSize(float sizeX, float sizeY);
    /**
     * Retourne les dimensions de ce rectangle node.
     *
     * @param sizeX La taille en X.
     * @param sizeY La taille en Y.
     */
    void getSize(float& sizeX, float& sizeY);
    /**
     * Retourne la taille en X de ce rectangle node.
     *
     * @return La taille en X.
     */
    float getSizeX();
    /**
     * Retourne la taille en Y de ce rectangle node.
     *
     * @return La taille en Y.
     */
    float getSizeY();

    /**
     * Retourne la valeur min en X
     */
    double getMinX() const;

    /**
     * Retourne la valeur min en Y
     */
    double getMinY() const;

    /**
     * Retourne la valeur max en Y
     */
    double getMaxY() const;

    /**
     * Retourne la longueur de la diagonale du rectangle
     * @return la longueur de la diagonale
     */
    double getDiagSize();

    /**
     * Retourne le diametre du cercle de surface equivalente
     * @return un double
     */
    double getCircleEqDiameter();

    /**
     * Applique un facteur d'echelle au rectangle.
     *
     * @param factorX Le facteur d'echelle en X.
     * @param factorY Le facteur d'echelle en Y.
     */
    void scale(float factorX, float factorY);
    /**
     * Applique un facteur d'echelle au rectangle.
     *
     * @param factor Le facteur d'echelle.
     */
    void scale(float factor) { scale(factor, factor); }

    /**
     * Test l'intersection avec un autre rectangle.
     *
     * @param pRect L'autre rectangle.
     * @param originRect L'origine du repere a appliquer a l'autre rectangle.
     * @param origin L'origine du repere a appliquer au rectangle.
     *
     * @return 1 si l'intersection existe, 0 sinon.
     */
    bool intersectRect(LPTYRectangle pRect, TYPoint originRect = TYPoint(0, 0, 0), TYPoint origin = TYPoint(0, 0, 0));

    ///Operateur =.
    TYRectangle& operator=(const TYRectangle& other);
    ///Operateur ==.
    bool operator==(const TYRectangle& other) const;
    ///Operateur !=.
    bool operator!=(const TYRectangle& other) const;

    virtual bool deepCopy(const TYElement* pOther, bool copyId = true);

    virtual std::string toString() const;

    virtual DOM_Element toXML(DOM_Element& domElement);
    virtual int fromXML(DOM_Element domElement);

    virtual double surface() const;
    virtual OVector3D normal() const;
    virtual OPlan plan() const;
    virtual TYTabPoint getContour(int n = -1) const;
    virtual TYTabPoint3D getOContour(int n = -1) const;
    virtual int intersects(const TYSurfaceInterface* pSurf, OSegment3D& seg) const;
    virtual int intersects(const OSegment3D& seg, OPoint3D& pt) const;
    virtual int intersects(const OPoint3D& pt) const;

    /**
     * Test si ce rectangle est valide.
     * Les 4 points de ce rectangle doivent appartenir au meme plan,
     * et les 4 angles doivent etre droits.
     *
     * @return <code>true</code> si le rectangle est valide;
     *         <code>false</code> sinon.
     */
    bool isValid() const;

    /**
     * Calcule le repere local a ce rectangle par rapport au repere global.
     */
    ORepere3D getORepere3D() const;

    /**
     * Convertit ce rectangle en un polygone.
     * Remqarque : L'appelant est responsable de la gestion de la memoire
     * de l'element retourne.
     *
     * @return Un nouveau polygone representant ce rectangle.
     */
    TYPolygon* toPolygon() const;

    /**
     * retourne le plan associe a cette surface
     */
    virtual const OPlan& getPlan() const { return _plan; }

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
public:
    ///Sommets.
    TYPoint _pts[4];
    OPlan _plan;
    OBox _box;
    OVector3D _normale;

    static const int _nbPts;
};


#endif // __TY_RECTANGLE__
