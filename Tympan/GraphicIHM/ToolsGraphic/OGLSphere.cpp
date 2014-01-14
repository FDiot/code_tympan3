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



#include "OGLSphere.h"

#if _WIN32
#include <windows.h>
#endif //_WIN32


#include <GL/gl.h>

#include "Tympan/Tools/OColor.h"
#include "Tympan/MetierSolver/ToolsMetier/OPoint3D.h"

OGLSphere::OGLSphere()
{

}

OGLSphere::~OGLSphere()
{

}

void OGLSphere::drawSphere(const OPoint3D& center, float radius, const OColor& oColor) const
{
    //Antialiasing sur les points
    glEnable(GL_POINT_SMOOTH);
    //Gestion de la transparence
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    // Couleur du point
    glColor3fv(oColor);
    glPointSize(radius);

    glBegin(GL_POINTS);
    glVertex3f(center._x, center._y, center._z);
    glEnd();
}
