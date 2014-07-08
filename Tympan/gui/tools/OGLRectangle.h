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

#ifndef __O_GLRECTANGLE__
#define __O_GLRECTANGLE__

class OColor;
class OPoint3D;

#include "Tympan/core/smartptr.h"

class OGLRectangle
{
public:
    OGLRectangle();
    virtual ~OGLRectangle();

    /**
     * Affiche une ligne
     */
    void drawRectangle(const OPoint3D& point0, const OPoint3D& point1, const OPoint3D& point2, const OPoint3D& point3, const OColor& oColor, bool bFalse = false) const;

private:
};

// Smart pointer sur OGLRectangle.
typedef SmartPtr<OGLRectangle> LPOGLRectangle;

#endif // __O_GLRECTANGLE__
