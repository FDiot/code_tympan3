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


#ifndef __OGL_LINE_ELEMENT
#define __OGL_LINE_ELEMENT

#include "OGLElement.h"
#include "Tympan/Tools/OColor.h"
#include "Tympan/MetierSolver/ToolsMetier/OPoint3D.h"

#include <qstring.h>

class OGLLine;

/**
 * Classe de ligne OGL
 *
 * @author Projet_Tympan.
 */
class OGLLineElement : public OGLElement
{
public:
    /**
     * Constructeur.
     */
    OGLLineElement();

    /**
     * Destructeur.
     */
    virtual ~OGLLineElement();

    virtual int render();

    void setColor(const OColor& oColor);

    void setPoint1(const OPoint3D& point1) { m_oPoint1 = point1; };
    void setPoint2(const OPoint3D& point2) { m_oPoint2 = point2; };

    void setLineWidth(float lineWidth) { m_fLineWidth = lineWidth; };

    double* getPoint1() { return m_oPoint1.getCoords(); };
    double* getPoint2() { return m_oPoint2.getCoords(); };

    double dist() {return m_oPoint1.dist2DFrom(m_oPoint2);};

protected:
    OColor      m_oColor;
    OPoint3D    m_oPoint1;
    OPoint3D    m_oPoint2;
    OGLLine*    m_pOGLLineToDisplay;
    float       m_fLineWidth;
};

#endif //__OGL_LINE_ELEMENT
