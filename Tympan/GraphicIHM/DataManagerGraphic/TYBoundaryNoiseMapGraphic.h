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

/**
 * \file TYBoundaryNoiseMapGraphic.h
 * \brief Graphical representation of the BoundaryNoiseMap entity (header file)
 * \author Projet_Tympan
 *
 *
 *
 *
 *
 */

#ifndef __TY_BOUNDARY_NOISE_MAP_GRAPHIC__
#define __TY_BOUNDARY_NOISE_MAP_GRAPHIC__


#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Tympan/MetierSolver/DataManagerCore/TYDefines.h"

#include "Tympan/models/business/TYMaillage.h"

#include "TYPolyLineGraphic.h"

/**
 * \class TYBoundaryNoiseMapGraphic
 * \brief Rendering class for the BoundaryNoiseMap entity
 */
class TYBoundaryNoiseMapGraphic : public TYElementGraphic
{
    TY_DECL_METIER_GRAPHIC(TYBoundaryNoiseMap)

public:
    enum RenderingMode { RENDER_SURF, RENDER_LINES, RENDER_MIXED } _Rendering;

    /**
     * \brief Constructor.
     */
    TYBoundaryNoiseMapGraphic(TYBoundaryNoiseMap* pElement);

    /**
     * \brief Destructor.
     */
    virtual ~TYBoundaryNoiseMapGraphic() { }

    /**
     * \brief Update the mesh and the isocurve.
     * \param force If we force the update
     */
    virtual void update(bool force = false);

    /**
     * \brief Display the object.
     * \param mode The mode (selection mode vs display mode)
     */
    virtual void display(GLenum mode = GL_RENDER);

    /**
     * \brief Set the children.
     * \param childs The container that contains the children
     * \param recursive If a recursive check is performed
     */
    virtual void getChilds(TYListPtrTYElementGraphic& childs, bool recursive = true);

    /**
     * \brief Compute the bounding box of the object.
     */
    virtual void computeBoundingBox();

    /**
     * \brief Set the rendering mode (surface, isocurve or both).
     * \param mode The rendering mode
     */
    virtual void setRendering(int mode) { _Rendering = (RenderingMode) mode; };

protected:
    /**
     * \brief Display both the surface and the isocurve.
     * \param opacity The transparency
     */
    virtual void displayMixed(float opacity = 1.f);

    /**
     * \brief Display the surface of the mesh.
     * \param opacity The transparency
     */
    virtual void displaySurface(float opacity = 1.f);

    /**
     * \brief Display the isocurve.
     * \param opacity The transparency
     * \param invertColors If the color are inverted
     */
    virtual void displayLines(float opacity = 1.f, bool invertColors = false);

    /**
     * \brief What we have to display when the entity is highlighted.
     * \param mode The rendering mode
     */
    void displayOnHighlight(int mode);

protected:
    /// The polyline rendering.
    LPTYPolyLineGraphic _pPolyLineGraphic;

    /// The mesh.
    std::vector<MTriangle> _mesh;

    /// The iso curve
    std::vector<MPoint> _isoCurve;

    // XXX Since we never change these values, maybe we could use constants instead
    // (they also appears in TYRectangularMaillageGraphic...)
    /// The position of the palette in x.
    double _paletteX;

    /// The position of the palette in y.
    double _paletteY;

    /// The width of the palette.
    double _paletteW;

    /// The height of the palette.
    double _paletteH;

    /// The width of the panel.
    double _panelW;

    /// The height of the panel.
    double _panelH;
};


#endif // __TY_BOUNDARY_NOISE_MAP_GRAPHIC__
