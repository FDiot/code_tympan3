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

#ifndef __O_GLGRID__
#define __O_GLGRID__

class OColor;
class OPoint3D;

#include "Tympan/core/smartptr.h"

class OGLGrid
{
public:
    OGLGrid();
    virtual ~OGLGrid();

    /**
     * Affiche la grille
     */
    void drawGrid(const float gridDimX, const float gridDimY, const float gridStep, const OColor& oColor, bool bShowGridXY, bool  showGridXZ, bool showGridZY) const;

private:
};

// Smart pointer sur OGLGrid.
typedef SmartPtr<OGLGrid> LPOGLGrid;

#endif // __O_GLGRID__
