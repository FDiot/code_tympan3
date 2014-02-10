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


#ifndef __OGL_RECTANGLE_ELEMENT
#define __OGL_RECTANGLE_ELEMENT

#include "OGLElement.h"
#include "Tympan/Tools/OColor.h"
#include "Tympan/MetierSolver/ToolsMetier/OPoint3D.h"

#include <qstring.h>

class OGLRectangle;

/**
 * Classe de rectangle OGL
 *
 * @author Projet_Tympan.
 */
class OGLRectangleElement : public OGLElement
{
public:
    /**
     * Constructeur.
     */
    OGLRectangleElement();

    /**
     * Destructeur.
     */
    virtual ~OGLRectangleElement();

    virtual int render();

    void setColor(const OColor& oColor);

    void setPoint0(const OPoint3D& point0) { m_oPoint0 = point0; };
    void setPoint1(const OPoint3D& point1) { m_oPoint1 = point1; };
    void setPoint2(const OPoint3D& point2) { m_oPoint2 = point2; };
    void setPoint3(const OPoint3D& point3) { m_oPoint3 = point3; };

    double* getPoint0() { return m_oPoint0.getCoords(); };
    double* getPoint1() { return m_oPoint1.getCoords(); };
    double* getPoint2() { return m_oPoint2.getCoords(); };
    double* getPoint3() { return m_oPoint3.getCoords(); };

    void setFull(bool bFull) { m_bFull = bFull;};

protected:
    OColor          m_oColor;
    OPoint3D        m_oPoint0;
    OPoint3D        m_oPoint1;
    OPoint3D        m_oPoint2;
    OPoint3D        m_oPoint3;
    bool            m_bFull;
    OGLRectangle*   m_pOGLRectangleToDisplay;
};

#endif //__OGL_RECTANGLE_ELEMENT
