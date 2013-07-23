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



#include "OGLGrid.h"

#if _WIN32
#include <windows.h>
#endif //_WIN32


#include <GL/gl.h>

#include "Tympan/Tools/OColor.h"
#include "Tympan/MetierSolver/ToolsMetier/OPoint3D.h"
#include "OGLLine.h"

OGLGrid::OGLGrid()
{

}

OGLGrid::~OGLGrid()
{

}

void OGLGrid::drawGrid(const float gridDimX, const float gridDimY, const float gridStep, const OColor& oColor, bool bShowGridXY, bool  showGridXZ, bool showGridZY) const
{
    int k;
    int resX = int(gridDimX / gridStep);
    int resY = int(gridDimY / gridStep);

    if (bShowGridXY)
    {
        for (k = -int(resX / 2); k <= int(resX / 2); k++)
        {
            OGLLine line1;
            line1.drawLine(OPoint3D(gridStep * k, -gridDimY / 2, 0.0), OPoint3D(gridStep * k, gridDimY / 2, 0.0), oColor, 1.0);
        }

        for (k = -int(resY / 2); k <= int(resY / 2); k++)
        {
            OGLLine line2;
            line2.drawLine(OPoint3D(-gridDimX / 2, gridStep * k, 0.0), OPoint3D(gridDimX / 2, gridStep * k, 0.0), oColor, 1.0);
        }
    }

    if (showGridXZ)
    {
        for (k = -int(resX / 2); k <= int(resX / 2); k++)
        {
            OGLLine line1;
            line1.drawLine(OPoint3D(gridStep * k, 0.0, -gridDimY / 2), OPoint3D(gridStep * k, 0.0, gridDimY / 2), oColor, 1.0);
        }

        for (k = -int(resY / 2); k <= int(resY / 2); k++)
        {
            OGLLine line2;
            line2.drawLine(OPoint3D(-gridDimX / 2, 0.0, gridStep * k), OPoint3D(gridDimX / 2, 0.0, gridStep * k), oColor, 1.0);
        }
    }

    if (showGridZY)
    {
        for (k = -int(resX / 2); k <= int(resX / 2); k++)
        {
            OGLLine line1;
            line1.drawLine(OPoint3D(0.0, gridStep * k, -gridDimY / 2), OPoint3D(0.0, gridStep * k, gridDimY / 2), oColor, 1.0);
        }

        for (k = -int(resY / 2); k <= int(resY / 2); k++)
        {
            OGLLine line2;
            line2.drawLine(OPoint3D(0.0, -gridDimX / 2, gridStep * k), OPoint3D(0.0, gridDimX / 2, gridStep * k), oColor, 1.0);
        }
    }
}
