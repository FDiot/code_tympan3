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

#ifndef __O_GLSPHERE__
#define __O_GLSPHERE__

class OColor;
class OPoint3D;

#include "Tympan/Tools/OSmartPtr.h"

class OGLSphere
{
public:
    OGLSphere();
    virtual ~OGLSphere();

    /**
     * Affiche une sphere
     */
    void drawSphere(const OPoint3D& center, float raduis, const OColor& oColor) const;

private:
};

// Smart pointer sur OGLSphere.
typedef SmartPtr<OGLSphere> LPOGLLine;

#endif // __O_GLSPHERE__
