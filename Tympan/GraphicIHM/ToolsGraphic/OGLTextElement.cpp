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



#include "OGLTextElement.h"
#include "Tympan/Tools/OLocalizator.h"
#include "Tympan/gui/OSizeManager.h"

#include "OGLFont.h"

OGLTextElement::OGLTextElement()
{
    m_pOGLFontToDisplay = new OGLFont();
}

OGLTextElement::~OGLTextElement()
{
    if (m_pOGLFontToDisplay)
    {
        delete m_pOGLFontToDisplay;
    }
}

void OGLTextElement::setTextToDisplay(const QString& qsText)
{
    m_qsText = qsText;
}

void OGLTextElement::setFont(const QString& qsFontPath)
{
    m_qsFontPath = qsFontPath;
}

void OGLTextElement::setColor(const OColor& oColor)
{
    m_oColor = oColor;
}

int OGLTextElement::render()
{
    if (m_bVisible)
    {
        // Nouvelle projection
        glMatrixMode(GL_PROJECTION);
        glPushMatrix();
        glLoadIdentity();
        glOrtho(0, OSizeManager::get()->getWidth(), 0.0, OSizeManager::get()->getHeight(), 0.0, 1.0);

        // Nouvelle vue
        glMatrixMode(GL_MODELVIEW);
        glPushMatrix();
        glLoadIdentity();

        //draw the text
        if (m_pOGLFontToDisplay->load(m_qsFontPath.toStdString().c_str()))
        {
            m_pOGLFontToDisplay->bind();
            m_pOGLFontToDisplay->drawText(m_qsText.toStdString(), m_oColor, m_DisplayPositionX, m_DisplayPositionY + 14);
        }

        // Ancienne projection
        glMatrixMode(GL_PROJECTION);
        glPopMatrix();

        // Ancienne vue
        glMatrixMode(GL_MODELVIEW);
        glPopMatrix();
    }

    return 0;
}


