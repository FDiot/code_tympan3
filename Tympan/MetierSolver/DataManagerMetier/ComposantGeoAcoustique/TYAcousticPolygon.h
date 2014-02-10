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

#ifndef __TY_ACOUSTIC_POLYGON__
#define __TY_ACOUSTIC_POLYGON__


#include "Tympan/MetierSolver/DataManagerMetier/ComposantGeometrique/TYPolygon.h"
#include "TYAcousticSurface.h"

#if TY_USE_IHM
#include "Tympan/GraphicIHM/DataManagerIHM/TYAcousticPolygonWidget.h"
#include "Tympan/GraphicIHM/DataManagerGraphic/TYAcousticPolygonGraphic.h"
#endif


/**
 * Classe de definition d'un polygon acoustique.
 * La partie geometrie est deleguee a un TYPolygon membre.
 */
class TYAcousticPolygon: public TYAcousticSurface
{
    OPROTOSUPERDECL(TYAcousticPolygon, TYAcousticSurface)
    TY_EXTENSION_DECL(TYAcousticPolygon)
    TY_EXT_GRAPHIC_DECL(TYAcousticPolygon)

    // Methodes
public:
    /**
     * Constructeur par defaut.
     */
    TYAcousticPolygon();
    /**
     * Constructeur par copie.
     */
    TYAcousticPolygon(const TYAcousticPolygon& other);
    /**
     * Destructeur.
     */
    virtual ~TYAcousticPolygon();

    ///Operateur =.
    TYAcousticPolygon& operator=(const TYAcousticPolygon& other);
    ///Operateur ==.
    bool operator==(const TYAcousticPolygon& other) const;
    ///Operateur !=.
    bool operator!=(const TYAcousticPolygon& other) const;

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
     * Set/Get du polygon.
     */
    virtual LPTYPolygon getPolygon() { return _pPolygon; }
    /**
     * Set/Get du polygon.
     */
    const LPTYPolygon getPolygon() const { return _pPolygon; }

    virtual void inverseNormale() { _pPolygon->inverseNormale(); }

    // Simple direct delegate
    virtual void
    exportMesh(
    		std::deque<OPoint3D>& points,
    		std::deque<OTriangle>& triangles,
                const TYGeometryNode& geonode) const
    { return _pPolygon->exportMesh(points, triangles, geonode); }

    // Membres
protected:
    ///Le polygone definissant la geometrie de cet acoustic polygon.
    LPTYPolygon _pPolygon;
};

typedef SmartPtr<TYAcousticPolygon> LPTYAcousticPolygon;

#endif // __TY_ACOUSTIC_POLYGON__
