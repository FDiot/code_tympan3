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

#ifndef __TY_ACOUSTICCIRCLE__
#define __TY_ACOUSTICCIRCLE__


class OSegment3D;
class OPoint3D;

#include "Tympan/MetierSolver/DataManagerMetier/ComposantGeometrique/TYRectangle.h"
#include "TYAcousticSurface.h"

#if TY_USE_IHM
#include "Tympan/GraphicIHM/DataManagerIHM/TYAcousticCircleWidget.h"
#include "Tympan/GraphicIHM/DataManagerGraphic/TYAcousticCircleGraphic.h"
#endif

/**
 * Permet de creer des surfaces acoustiques circulaires.
 */
class TYAcousticCircle: public TYAcousticSurface
{
    OPROTOSUPERDECL(TYAcousticCircle, TYAcousticSurface)
    TY_EXTENSION_DECL(TYAcousticCircle)
    TY_EXT_GRAPHIC_DECL(TYAcousticCircle)

    // Methodes
public:
    /**
     * Constructeur.
     */
    TYAcousticCircle();
    /**
     * Constructeur par copie.
     */
    TYAcousticCircle(const TYAcousticCircle& other);
    /**
     * Destructeur.
     */
    virtual ~TYAcousticCircle();

    ///Operateur =.
    TYAcousticCircle& operator=(const TYAcousticCircle& other);
    ///Operateur ==.
    bool operator==(const TYAcousticCircle& other) const;
    ///Operateur !=.
    bool operator!=(const TYAcousticCircle& other) const;

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
     * Calcul le centre de ce cercle.
     *
     * @return Le centre de ce cercle.
     */
    TYPoint getCenter() const;

    /**
     * Calcul le diametre de ce cercle.
     *
     * @return Le diametre de ce cercle.
     */
    double getDiameter() const;

    /**
     * Set le diametre de ce cercle.
     *
     * @param diameter Le diametre de ce cercle.
     */
    void setDiameter(double diameter);

    /**
     * Set/Get du rectangle contenant le cercle.
     */
    TYRectangle* getShape() { return getBoundingRect(); }


    // Membres
protected:

};


///Noeud geometrique de type TYAcousticCircle.
typedef TYGeometryNode TYAcousticCircleGeoNode;
///Smart Pointer sur TYAcousticCircleGeoNode.
typedef SmartPtr<TYAcousticCircleGeoNode> LPTYAcousticCircleGeoNode;
///Collection de noeuds geometriques de type TYAcousticCircle.
typedef std::vector<LPTYAcousticCircleGeoNode> TYTabAcousticCircleGeoNode;


#endif // __TY_ACOUSTICCIRCLE__
