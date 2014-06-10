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


#ifndef __OGL_SCALARBAR_ELEMENT
#define __OGL_SCALARBAR_ELEMENT

#include "OGLElement.h"
#include "Tympan/Tools/OColor.h"
#include "Tympan/MetierSolver/CommonTools/OPoint3D.h"

#include <qstring.h>

class OGLRectangle;
class OGLFont;

/**
 * Classe de scalarBar OGL
 */
class OGLScalarBarElement : public OGLElement
{
public:
    /**
     * Constructeur.
     */
    OGLScalarBarElement();

    /**
     * Destructeur.
     */
    virtual ~OGLScalarBarElement();

    virtual int render();

    void setFont(const QString& qsFontPath) {m_qsFontPath = qsFontPath; };
    void setFontColor(const OColor& oFontColor) { m_oFontColor = oFontColor; };

    void setPosition(const OPoint3D& position) { m_oPosition = position; };
    void setTexts(const std::string& text0, const std::string& text1, const std::string& text2) { m_stdText0 = text0; m_stdText1 = text1; m_stdText2 = text2;};
    void setSizes(float width, float height) { m_fWidth = width; m_fHeight = height;};
    void initPositions();

    double* getPosition() { return m_oPosition.getCoords(); };

protected:
    OPoint3D        m_oPosition;
    OPoint3D        m_oPosA;  //  A ----- B ------ C
    OPoint3D        m_oPosB;  //  |       |        |
    OPoint3D        m_oPosC;  //  D ----- E ------ F
    OPoint3D        m_oPosD;
    OPoint3D        m_oPosE;
    OPoint3D        m_oPosF;
    std::string     m_stdText0;
    std::string     m_stdText1;
    std::string     m_stdText2;
    float           m_fWidth;
    float           m_fHeight;
    OGLRectangle*   m_pOGLRectangleLeftToDisplay;
    OGLRectangle*   m_pOGLRectangleRightToDisplay;
    OGLFont*        m_pOGLFont0ToDisplay;
    OGLFont*        m_pOGLFont1ToDisplay;
    OGLFont*        m_pOGLFont2ToDisplay;
    QString         m_qsFontPath;
    OColor          m_oFontColor;
};

#endif //__OGL_SCALARBAR_ELEMENT
