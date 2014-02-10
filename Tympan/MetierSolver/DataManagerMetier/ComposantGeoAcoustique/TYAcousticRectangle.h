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

#ifndef __TY_ACOUSTICRECTANGLE__
#define __TY_ACOUSTICRECTANGLE__

class OPoint3D;
class OSegment3D;


#include "TYAcousticSurface.h"
#include "Tympan/MetierSolver/DataManagerMetier/ComposantGeometrique/TYRectangle.h"

#if TY_USE_IHM
#include "Tympan/GraphicIHM/DataManagerIHM/TYAcousticRectangleWidget.h"
#include "Tympan/GraphicIHM/DataManagerGraphic/TYAcousticRectangleGraphic.h"
#endif


/**
 * Permet de creer des surfaces acoustiques rectangulaires.
 *
 * NB The _pBoundingRect attribute, inherited from TYAcousticSurface
 * is used as the underlying rectangle to which surface related
 * methods are delegated.
 */
class TYAcousticRectangle: public TYAcousticSurface
{
    OPROTOSUPERDECL(TYAcousticRectangle, TYAcousticSurface)
    TY_EXTENSION_DECL(TYAcousticRectangle)
    TY_EXT_GRAPHIC_DECL(TYAcousticRectangle)

    // Methodes
public:
    /**
     * Constructeur.
     */
    TYAcousticRectangle();
    /**
     * Constructeur par copie.
     */
    TYAcousticRectangle(const TYAcousticRectangle& other);
    /**
     * Destructeur.
     */
    virtual ~TYAcousticRectangle();

    ///Operateur =.
    TYAcousticRectangle& operator=(const TYAcousticRectangle& other);
    ///Operateur ==.
    bool operator==(const TYAcousticRectangle& other) const;
    ///Operateur !=.
    bool operator!=(const TYAcousticRectangle& other) const;

    virtual bool deepCopy(const TYElement* pOther, bool copyId = true);

    virtual std::string toString() const;

    virtual DOM_Element toXML(DOM_Element& domElement);
    virtual int fromXML(DOM_Element domElement);

    virtual TYTabSourcePonctuelleGeoNode getSrcs() const;
    virtual TYSourcePonctuelle srcPonctEquiv() const;
    virtual void distriSrcs();
    virtual bool setSrcsLw();

    virtual double surface() const;
    virtual OVector3D normal() const;
    virtual OPlan plan() const;
    virtual TYTabPoint getContour(int n = -1) const;
    virtual TYTabPoint3D getOContour(int n = -1) const;
    virtual int intersects(const TYSurfaceInterface* pSurf, OSegment3D& seg) const;
    virtual int intersects(const OSegment3D& seg, OPoint3D& pt) const;

    /**
     * Set/Get de la geometrie de ce rectangle acoustique.
     */
    TYRectangle* getShape() { return getBoundingRect(); }

    virtual void inverseNormale() { getBoundingRect()->inverseNormale(); }


    /**
     * @brief Export the surface as a triangular mesh
     *
     * NB : This is only a delegate to the underlying TYRectangle method.
     *
     * @param points output argument filled with the vertices of the triangulation
     * @param triangles output argument filled with the faces of the triangulation
     */
    virtual void
    exportMesh(
    		std::deque<OPoint3D>& points,
    		std::deque<OTriangle>& triangles,
                const TYGeometryNode& geonode) const
    { _pBoundingRect->exportMesh(points, triangles, geonode); }


    // Membres
protected:

};


///Noeud geometrique de type TYAcousticRectangle.
typedef TYGeometryNode TYAcousticRectangleGeoNode;
///Smart Pointer sur TYAcousticRectangleGeoNode.
typedef SmartPtr<TYAcousticRectangleGeoNode> LPTYAcousticRectangleGeoNode;
///Collection de noeuds geometriques de type TYAcousticRectangle.
typedef std::vector<LPTYAcousticRectangleGeoNode> TYTabAcousticRectangleGeoNode;


#endif // __TY_ACOUSTICRECTANGLE__
