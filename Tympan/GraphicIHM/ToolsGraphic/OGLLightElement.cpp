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



#include "OGLLightElement.h"


#if _WIN32
#include <windows.h>
#endif //_WIN32

#include <GL/gl.h>

#include "OGLLight.h"

OGLLightElement::OGLLightElement()
{
    m_pOGLLightToDisplay = new OGLLight();
    m_oPosition = OPoint3D(0.0, 0.0, 0.0);
    m_fIntensity = 0.0;
    m_iIndex = 0;
    setVisibility(true);
}

OGLLightElement::~OGLLightElement()
{
    if (m_pOGLLightToDisplay)
    {
        delete m_pOGLLightToDisplay;
    }
}

void OGLLightElement::init(int index, OPoint3D position, float intensity)
{
    m_iIndex = index;
    m_oPosition = position;
    m_fIntensity = intensity;
}

int OGLLightElement::render()
{
    if (m_bVisible)
    {
        m_pOGLLightToDisplay->drawLight(m_iIndex, m_oPosition, m_fIntensity);
    }

    return 0;
}


