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


#ifndef __OGL_TEXT_ELEMENT
#define __OGL_TEXT_ELEMENT

#include "OGLElement.h"
#include "Tympan/Tools/OColor.h"

#include <qstring.h>

class OGLFont;
/**
 * Classe Text OGL
 */
class OGLTextElement : public OGLElement
{
public:
    /**
     * Constructeur.
     */
    OGLTextElement();

    /**
     * Destructeur.
     */
    virtual ~OGLTextElement();

    virtual int render();

    void setTextToDisplay(const QString& qsText);

    void setColor(const OColor& oColor);

    void setFont(const QString& qsFontPath);

    QString getText() {return m_qsText;};

protected:
    QString     m_qsText;
    QString     m_qsFontPath;
    OGLFont*    m_pOGLFontToDisplay;
    OColor      m_oColor;
};

#endif //__OGL_TEXT_ELEMENT
