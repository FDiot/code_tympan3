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


#ifndef __OGL_SPHERE_ELEMENT
#define __OGL_SPHERE_ELEMENT

#include "OGLElement.h"
#include "Tympan/Tools/OColor.h"
#include "Tympan/models/common/3d.h"

#include <qstring.h>

class OGLSphere;

/**
 * Classe de sphere OGL
 */
class OGLSphereElement : public OGLElement
{
public:
    /**
     * Constructeur.
     */
    OGLSphereElement();

    /**
     * Destructeur.
     */
    virtual ~OGLSphereElement();

    virtual int render();

    void setColor(const OColor& oColor);

    void setCenter(const OPoint3D& center) { m_oCenter = center; };
    double* getCenter() { return m_oCenter.getCoords(); };

    void setRadius(float radius) { m_fRadius = radius;}

protected:
    OColor      m_oColor;
    OPoint3D    m_oCenter;
    float       m_fRadius;
    OGLSphere*  m_pOGLSphereToDisplay;
};

#endif //__OGL_SPHERE_ELEMENT
