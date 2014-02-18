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



#include "OGLScalarBarElement.h"


#if _WIN32
#include <windows.h>
#endif //_WIN32

#include <GL/gl.h>

#include "OGLRectangle.h"
#include "OGLFont.h"
#include "Tympan/Tools/OSizeManager.h"


OGLScalarBarElement::OGLScalarBarElement()
{
    m_pOGLRectangleLeftToDisplay = new OGLRectangle();
    m_pOGLRectangleRightToDisplay = new OGLRectangle();
    m_pOGLFont0ToDisplay = new OGLFont();
    m_oPosition = OPoint3D(0.0, 0.0, 0.0);
    m_stdText0 = "";
    m_stdText1 = "";
    m_stdText2 = "";
    m_fWidth = 1;
    m_fHeight = 1;
    m_bIs3D = false;
}

OGLScalarBarElement::~OGLScalarBarElement()
{
    if (m_pOGLRectangleLeftToDisplay)
    {
        delete m_pOGLRectangleLeftToDisplay;
    }
    if (m_pOGLRectangleRightToDisplay)
    {
        delete m_pOGLRectangleRightToDisplay;
    }
    if (m_pOGLFont0ToDisplay)
    {
        delete m_pOGLFont0ToDisplay;
    }
}

void OGLScalarBarElement::initPositions()
{
    m_oPosA._x = m_oPosition._x - (m_fWidth / 2);
    m_oPosA._y = m_oPosition._y - (m_fHeight / 2);
    m_oPosA._z = m_oPosition._z;
    m_oPosB._x = m_oPosition._x;
    m_oPosB._y = m_oPosition._y - (m_fHeight / 2);
    m_oPosB._z = m_oPosition._z;
    m_oPosC._x = m_oPosition._x + (m_fWidth / 2);
    m_oPosC._y = m_oPosition._y - (m_fHeight / 2);
    m_oPosC._z = m_oPosition._z;
    m_oPosD._x = m_oPosition._x + (m_fWidth / 2);
    m_oPosD._y = m_oPosition._y + (m_fHeight / 2);
    m_oPosD._z = m_oPosition._z;
    m_oPosE._x = m_oPosition._x;
    m_oPosE._y = m_oPosition._y + (m_fHeight / 2);
    m_oPosE._z = m_oPosition._z;
    m_oPosF._x = m_oPosition._x - (m_fWidth / 2);
    m_oPosF._y = m_oPosition._y + (m_fHeight / 2);
    m_oPosF._z = m_oPosition._z;
}

int OGLScalarBarElement::render()
{
    if (m_bVisible)
    {
        if (m_bIs3D)
        {
            // Nouvelle vue
            glMatrixMode(GL_MODELVIEW);
            glPushMatrix();
            m_pOGLRectangleLeftToDisplay->drawRectangle(m_oPosA, m_oPosB, m_oPosE, m_oPosF, OColor::WHITE, true);
            m_pOGLRectangleLeftToDisplay->drawRectangle(m_oPosB, m_oPosC, m_oPosD, m_oPosE, OColor::BLACK, true);
            if (m_pOGLFont0ToDisplay->load(m_qsFontPath.toStdString().c_str()))
            {
                m_pOGLFont0ToDisplay->bind();
                m_pOGLFont0ToDisplay->drawText(m_stdText0, m_oFontColor, m_oPosA._x + 1, m_oPosA._y + 20);
                m_pOGLFont0ToDisplay->drawText(m_stdText1, m_oFontColor, m_oPosB._x - 1, m_oPosB._y + 22);
                m_pOGLFont0ToDisplay->drawText(m_stdText2, m_oFontColor, m_oPosC._x - 5, m_oPosC._y + 22);
            }
            // Ancienne vue
            glPopMatrix();
        }
        else
        {
            // Nouvelle projection
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

            m_pOGLRectangleLeftToDisplay->drawRectangle(m_oPosA, m_oPosB, m_oPosE, m_oPosF, OColor::WHITE, true);
            m_pOGLRectangleLeftToDisplay->drawRectangle(m_oPosB, m_oPosC, m_oPosD, m_oPosE, OColor::BLACK, true);
            if (m_pOGLFont0ToDisplay->load(m_qsFontPath.toStdString().c_str()))
            {
                m_pOGLFont0ToDisplay->bind();
                m_pOGLFont0ToDisplay->drawText(m_stdText0, m_oFontColor, m_oPosA._x + 1, m_oPosA._y + 22);
                m_pOGLFont0ToDisplay->drawText(m_stdText1, m_oFontColor, m_oPosB._x - 1, m_oPosB._y + 22);
                m_pOGLFont0ToDisplay->drawText(m_stdText2, m_oFontColor, m_oPosC._x - 5, m_oPosC._y + 22);
            }

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

