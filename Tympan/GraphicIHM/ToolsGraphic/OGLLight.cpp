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



#include "OGLLight.h"

#if _WIN32
#include <windows.h>
#endif //_WIN32


#include <GL/gl.h>

#include "Tympan/MetierSolver/ToolsMetier/OPoint3D.h"

OGLLight::OGLLight()
{

}

OGLLight::~OGLLight()
{

}

void OGLLight::drawLight(const int index, const OPoint3D& position, const float intensity) const
{
    // On modifie
    //float lightIntensity[4] = {intensity, intensity, intensity, 1};
    float lightIntensity[4] = {intensity, intensity, intensity, intensity};
    float lightPos[4] = {(float)position._x, (float)position._y, (float)position._z, .0};
    GLenum light = GL_LIGHT0;
    switch (index)
    {
        case 0: light=GL_LIGHT0; break;
        case 1: light=GL_LIGHT1; break;
        case 2: light=GL_LIGHT2; break;
        case 3: light=GL_LIGHT3; break;
        case 4: light=GL_LIGHT4; break;
        case 5: light=GL_LIGHT5; break;
    }
    glEnable(light);
    glLightfv(light, GL_DIFFUSE, lightIntensity);
    glLightfv(light, GL_POSITION, lightPos);
}
