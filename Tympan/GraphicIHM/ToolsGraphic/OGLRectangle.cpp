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



#include "OGLRectangle.h"

#if _WIN32
#include <windows.h>
#endif //_WIN32


#include <GL/gl.h>

#include "Tympan/Tools/OColor.h"
#include "Tympan/MetierSolver/ToolsMetier/OPoint3D.h"

OGLRectangle::OGLRectangle()
{

}

OGLRectangle::~OGLRectangle()
{

}

void OGLRectangle::drawRectangle(const OPoint3D& point0, const OPoint3D& point1, const OPoint3D& point2, const OPoint3D& point3, const OColor& oColor, bool bFull/*=false*/) const
{
    float tmpColor[4];
    glGetFloatv(GL_CURRENT_COLOR, tmpColor);
    // Couleur de la ligne
    glColor3fv(oColor);

    // On dessine
    if (bFull)
    {
        glBegin(GL_QUADS);
    }
    else
    {
        glBegin(GL_LINES);
    }
    // Top Left --> Top Right
    glVertex3d(point0._x, point0._y, point0._z);
    glVertex3d(point1._x, point1._y, point1._z);

    // Top Right --> Bottom Right
    glVertex3d(point1._x, point1._y, point1._z);
    glVertex3d(point2._x, point2._y, point2._z);

    // Bottom Right --> Bottom Left
    glVertex3d(point2._x, point2._y, point2._z);
    glVertex3d(point3._x, point3._y, point3._z);

    // Bottom Left --> Top Left
    glVertex3d(point3._x, point3._y, point3._z);
    glVertex3d(point0._x, point0._y, point0._z);
    // Fin
    glEnd();
    glColor3f(tmpColor[0], tmpColor[1], tmpColor[2]);
}
