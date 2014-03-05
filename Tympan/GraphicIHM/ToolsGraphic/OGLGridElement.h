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


#ifndef __OGL_GRID_ELEMENT
#define __OGL_GRID_ELEMENT

#include "OGLElement.h"
#include "Tympan/Tools/OColor.h"

#include <qstring.h>

class OGLGrid;

/**
 * Classe pour la grille OGL
 */
class OGLGridElement : public OGLElement
{
public:
    /**
     * Constructeur.
     */
    OGLGridElement();

    /**
     * Destructeur.
     */
    virtual ~OGLGridElement();

    virtual int render();

    void setColor(const OColor& oColor);

    void setGridDimX(const float gridDimX) { m_fGridDimX = gridDimX; };
    void setGridDimY(const float gridDimY) { m_fGridDimY = gridDimY; };
    void setGridStep(const float gridStep) { m_fGridStep = gridStep; };

    void setShowGridXY(bool bShow) { m_bShowGridXY = bShow; };
    void setShowGridXZ(bool bShow) { m_bShowGridXZ = bShow; };
    void setShowGridZY(bool bShow) { m_bShowGridZY = bShow; };

    bool getShowGridXY() { return m_bShowGridXY; };
    bool getShowGridXZ() { return m_bShowGridXZ; };
    bool getShowGridZY() { return m_bShowGridZY; };

    float getGridDimX() { return m_fGridDimX; };
    float getGridDimY() { return m_fGridDimY; };
    float getGridStep() { return m_fGridStep; };

protected:
    OColor      m_oColor;
    float       m_fGridDimX;
    float       m_fGridDimY;
    float       m_fGridStep;
    OGLGrid*    m_pOGLGridToDisplay;
    bool        m_bShowGridXY;
    bool        m_bShowGridXZ;
    bool        m_bShowGridZY;
};

#endif //__OGL_GRID_ELEMENT
