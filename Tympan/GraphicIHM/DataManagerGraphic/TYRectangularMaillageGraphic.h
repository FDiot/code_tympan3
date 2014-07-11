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
 * \file TYRectangularMaillageGraphic.h
 * \brief Representation graphique d'un maillage rectangulaire (fichier header)
 * \author Projet_Tympan
 *
 *
 *
 *
 *
 */

#ifndef __TY_RECTANGULAR_MAILLAGE_GRAPHIC__
#define __TY_RECTANGULAR_MAILLAGE_GRAPHIC__


#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include "Tympan/MetierSolver/DataManagerCore/TYDefines.h"
#include "TYElementGraphic.h"

#include "Tympan/models/business/TYMaillage.h"

/**
 * \class TYRectangularMaillageGraphic
 * \brief classe graphique pour un maillage rectangulaire
 */
class TYRectangularMaillageGraphic : public TYElementGraphic
{
    TY_DECL_METIER_GRAPHIC(TYRectangularMaillage)

    // Methodes
public:
    /* Enum mode de rendu du maillage */
    enum RenderingMode { RENDER_SURF, RENDER_LINES, RENDER_MIXED } _Rendering;

    TYRectangularMaillageGraphic(TYRectangularMaillage* pElement);
    virtual ~TYRectangularMaillageGraphic();

    virtual void update(bool force = false);
    virtual void display(GLenum mode = GL_RENDER);
    virtual void computeBoundingBox();

    virtual void setRendering(int mode) { _Rendering = (RenderingMode) mode; };

private:
    /**
     * calcul le point suivant du maillage (traitement des maillages irreguliers)
     */
    int nextPoint(const TYTabLPPointCalcul* pPtsCalcul, const int& index1, const int& index2);

protected:
    std::vector<MTriangle> _mesh;
    std::vector<MPoint> _isoCurve;

    double _scalarX;
    double _scalarY;
    double _scalarW;
    double _scalarH;
    double _panelW;
    double _panelH;

    virtual void displaySurface(GLenum mode = GL_RENDER);
    virtual void displayLines(GLenum mode = GL_RENDER, bool invertColors = false);
    virtual void displayMixed(GLenum mode = GL_RENDER);
};


#endif // __TY_RECTANGULAR_MAILLAGE_GRAPHIC__
