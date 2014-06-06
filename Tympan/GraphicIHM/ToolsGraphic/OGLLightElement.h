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


#ifndef __OGL_LIGHT_ELEMENT
#define __OGL_LIGHT_ELEMENT

#include "OGLElement.h"
#include "Tympan/MetierSolver/CommonTools/OPoint3D.h"

#include <qstring.h>

class OGLLight;

/**
 * Classe de light en tant qu'elemement
 */
class OGLLightElement : public OGLElement
{
public:
    /**
     * Constructeur.
     */
    OGLLightElement();

    /**
     * Destructeur.
     */
    virtual ~OGLLightElement();

    virtual int render();

    void init(int index, OPoint3D   position, float intensity);
    void setPosition(const OPoint3D& position) { m_oPosition = position; };
    void setIntensity(const float intensity) { m_fIntensity = intensity; };

    double* getPosition() { return m_oPosition.getCoords(); };
    float getIntensity() { return m_fIntensity; };

protected:
    OPoint3D    m_oPosition;
    float       m_fIntensity;
    int         m_iIndex;
    OGLLight*   m_pOGLLightToDisplay;
};

#endif //__OGL_LIGHT_ELEMENT
