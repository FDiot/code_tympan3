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



#include "OGLLine.h"

#if _WIN32
#include <windows.h>
#endif //_WIN32

#include <GL/gl.h>

#include "Tympan/Tools/OColor.h"
#include "Tympan/MetierSolver/ToolsMetier/OPoint3D.h"

OGLLine::OGLLine()
{

}

OGLLine::~OGLLine()
{

}

void OGLLine::drawLine(const OPoint3D& point1, const OPoint3D& point2, const OColor& oColor, float lineWidth) const
{
    // Couleur de la ligne
    glColor3fv(oColor);

    // Taille de la ligne
    glLineWidth(lineWidth);

    // On dessine
    glBegin(GL_LINES);
    glVertex3d(point1._x, point1._y, point1._z);
    glVertex3d(point2._x, point2._y, point2._z);
    // Fin
    glEnd();
}
