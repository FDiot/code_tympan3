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
 * \file TYPolyLineGraphic.h
 * \brief Representation graphique d'une polyligne (fichier header)
 * \author Projet_Tympan
 *
 *
 *
 *
 *
 */

#ifndef __TY_POLY_LINE_GRAPHIC__
#define __TY_POLY_LINE_GRAPHIC__


#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include "Tympan/MetierSolver/DataManagerCore/TYDefines.h"
#include "TYElementGraphic.h"
#include "Tympan/MetierSolver/DataManagerMetier/ComposantGeometrique/TYPoint.h"


/**
 * \class TYPolyLineGraphic
 * \brief classe graphique pour representer une polyligne a partir d'un tableau de points.
 */
class TYPolyLineGraphic : public TYElementGraphic
{
    TY_DECL_METIER_GRAPHIC(TYElement)

    // Methodes
public:
    /// Default line width in screen pixels
    /// (cf OpenGL doc for more details of meaning when coupled with anti-aliasing)
    static const unsigned int default_width_px = 2;

    TYPolyLineGraphic(bool closed = false);
    virtual ~TYPolyLineGraphic();

    virtual void update(bool force = false);
    virtual void display(GLenum mode = GL_RENDER);
    virtual void computeBoundingBox();

    void setTabPoint(const TYTabPoint& tabPts) { _tabPts = tabPts; computeBoundingBox(); }
    TYTabPoint& getTabPoint() { return _tabPts; }
    const TYTabPoint& getTabPoint() const { return _tabPts; }

    // Membres
protected:
    TYTabPoint _tabPts;
    bool _closed;

    GLfloat width; ///< Line width - cf OpenGL doc for exact meaning
};


///Smart Pointer sur TYPolyLineGraphic.
typedef SmartPtr<TYPolyLineGraphic> LPTYPolyLineGraphic;


#endif // __TY_POLY_LINE_GRAPHIC__
