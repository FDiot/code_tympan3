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

#ifndef __TY_POINT__
#define __TY_POINT__

#include "Tympan/models/business/TYElement.h"
//#include "Tympan/models/business/TYColorInterface.h"

/**
 * Classe de definition d'un point.
 */
//class TYPoint: public TYElement, public TYColorInterface, public OPoint3D
class TYPoint: public TYElement, public OPoint3D
{
    OPROTODECL(TYPoint)
    TY_EXTENSION_DECL_ONLY(TYPoint)

    // Methodes
public:
    /**
     * Constructeur par defaut.
     */
    TYPoint(bool PutInInstanceList = false);
    /**
     * Constructeur de copie.
     */
    TYPoint(const TYPoint& other, bool PutInInstanceList = false);
    /**
     * Constructeur de copie.
     */
    TYPoint(const OCoord3D& other, bool PutInInstanceList = false);
    /**
     * Constructeur de copie.
     */
    TYPoint(const OVector3D& other, bool PutInInstanceList = false);
    /**
     * Constructeur.
     */
    TYPoint(double x, double y, double z, bool PutInInstanceList = false);

    /**
     * Destructeur.
     */
    virtual ~TYPoint();

    ///Operateur =.
    TYPoint& operator=(const TYPoint& other);
    ///Operateur ==.
    bool operator==(const TYPoint& other) const;
    ///Operateur !=.
    bool operator!=(const TYPoint& other) const;

    virtual bool deepCopy(const TYElement* pOther, bool copyId = true);

    virtual std::string toString() const;

    virtual DOM_Element toXML(DOM_Element& domElement);
    virtual int fromXML(DOM_Element domElement);

};


#endif // __TY_POINT__
