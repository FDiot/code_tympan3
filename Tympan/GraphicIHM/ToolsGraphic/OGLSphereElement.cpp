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



#include "OGLSphereElement.h"


#if _WIN32
#include <windows.h>
#endif //_WIN32

#include <GL/gl.h>

#include "OGLSphere.h"
#include "Tympan/gui/OSizeManager.h"


OGLSphereElement::OGLSphereElement()
{
    m_pOGLSphereToDisplay = new OGLSphere();
    m_oCenter = OPoint3D(0.0, 0.0, 0.0);
    m_fRadius = 2.0f;
    m_bIs3D = false;
}

OGLSphereElement::~OGLSphereElement()
{
    if (m_pOGLSphereToDisplay)
    {
        delete m_pOGLSphereToDisplay;
    }
}

void OGLSphereElement::setColor(const OColor& oColor)
{
    m_oColor = oColor;
}

int OGLSphereElement::render()
{
    if (m_bVisible)
    {
        if (m_bIs3D)
        {
            // Nouvelle vue
            glMatrixMode(GL_MODELVIEW);
            glPushMatrix();
            m_pOGLSphereToDisplay->drawSphere(m_oCenter, /*m_fRadius*/5.0f, m_oColor);
            // Ancienne vue
            glPopMatrix();
        }
        else
        {
            //Nouvelle projection
            glMatrixMode(GL_PROJECTION);
            glPushMatrix();
            glLoadIdentity();
            unsigned int width = OSizeManager::get()->getWidth();
            unsigned int height = OSizeManager::get()->getHeight();
            glOrtho(0, width, 0.0, height, 0.0, 1.0);

            // Nouvelle vue
            glMatrixMode(GL_MODELVIEW);
            glPushMatrix();
            glLoadIdentity();

            m_pOGLSphereToDisplay->drawSphere(m_oCenter, m_fRadius, m_oColor);

            // Ancienne projection
            glMatrixMode(GL_PROJECTION);
            glPopMatrix();

            // Ancienne vue
            glMatrixMode(GL_MODELVIEW);
            glPopMatrix();
        }
    }

    return 0;
}


