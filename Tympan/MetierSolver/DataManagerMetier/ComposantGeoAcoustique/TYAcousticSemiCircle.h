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

#ifndef __TY_ACOUSTICSEMICIRCLE__
#define __TY_ACOUSTICSEMICIRCLE__


class OPoint3D;
class OSegment3D;

#include "Tympan/MetierSolver/DataManagerMetier/ComposantGeometrique/TYRectangle.h"
#include "TYAcousticSurface.h"

#if TY_USE_IHM
#include "Tympan/GraphicIHM/DataManagerIHM/TYAcousticSemiCircleWidget.h"
#include "Tympan/GraphicIHM/DataManagerGraphic/TYAcousticSemiCircleGraphic.h"
#endif


/**
 * Permet de creer des surfaces acoustiques semi-circulaires.
 * Les points 0 et 1 du rectangle associe correspondent au
 * "diametre de coupe" du demi-cercle.
 *
 * @author Projet_Tympan
 *
 * TOOD This code is mostly a copy-paste of the TYCircle class and
 * needs to be factored.
 */
class TYAcousticSemiCircle: public TYAcousticSurface
{
    OPROTOSUPERDECL(TYAcousticSemiCircle, TYAcousticSurface)
    TY_EXTENSION_DECL(TYAcousticSemiCircle)
    TY_EXT_GRAPHIC_DECL(TYAcousticSemiCircle)

    // Methodes
public:
    /**
     * Constructeur.
     */
    TYAcousticSemiCircle();
    /**
     * Constructeur par copie.
     */
    TYAcousticSemiCircle(const TYAcousticSemiCircle& other);
    /**
     * Destructeur.
     */
    virtual ~TYAcousticSemiCircle();

    ///Operateur =.
    TYAcousticSemiCircle& operator=(const TYAcousticSemiCircle& other);
    ///Operateur ==.
    bool operator==(const TYAcousticSemiCircle& other) const;
    ///Operateur !=.
    bool operator!=(const TYAcousticSemiCircle& other) const;

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
    virtual int intersects(const OPoint3D& pt) const;

    /**
     * Calcul le centre de ce demi-cercle (correspondant au centre
     * du cercle complet).
     *
     * @return Le centre de ce demi-cercle.
     */
    TYPoint getCenter() const;

    /**
     * Calcul le diametre de ce demi-cercle.
     *
     * @return Le diametre de ce demi-cercle.
     */
    double getDiameter() const;
    /**
     * Set le diametre de ce demi-cercle.
     *
     * @param diameter Le diametre de ce demi-cercle.
     */
    void setDiameter(double diameter);

    /**
     * Set/Get du rectangle contenant le cercle.
     */
    TYRectangle* getShape() { return getBoundingRect(); }

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
    		std::deque<OTriangle>& triangles) const;


    // Membres
protected:

};


///Noeud geometrique de type TYAcousticSemiCircle.
typedef TYGeometryNode TYAcousticSemiCircleGeoNode;
///Smart Pointer sur TYAcousticSemiCircleGeoNode.
typedef SmartPtr<TYAcousticSemiCircleGeoNode> LPTYAcousticSemiCircleGeoNode;
///Collection de noeuds geometriques de type TYAcousticSemiCircle.
typedef std::vector<LPTYAcousticSemiCircleGeoNode> TYTabAcousticSemiCircleGeoNode;


#endif // __TY_ACOUSTICSEMICIRCLE__
