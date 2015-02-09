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

#if TY_USE_IHM
#include "Tympan/gui/widgets/TYPointWidget.h"
#endif


#include "Tympan/core/logging.h"


#include "TYPoint.h"

TY_EXTENSION_INST(TYPoint);


TYPoint::TYPoint(bool PutInInstanceList /*= false */): TYElement(NULL, PutInInstanceList), OPoint3D()
{
}

TYPoint::TYPoint(const TYPoint& other, bool PutInInstanceList /*= false */): TYElement(other, false),
    OPoint3D(other)
{
}

TYPoint::TYPoint(const OCoord3D& other, bool PutInInstanceList /*= false */): TYElement(NULL, PutInInstanceList),
    OPoint3D(other)
{
}

TYPoint::TYPoint(const OVector3D& other, bool PutInInstanceList /*= false */): TYElement(NULL, PutInInstanceList),
    OPoint3D(other)
{
}

TYPoint::TYPoint(double x, double y, double z, bool PutInInstanceList /*= false */):
    TYElement(NULL, PutInInstanceList), OPoint3D(x, y, z)
{
}

TYPoint::~TYPoint()
{
}

void TYPoint::set(double x, double y, double z)
{
    _x = x;
    _y = y;
    _z = z;
}

void TYPoint::setFromOGL(float x, float y, float z)
{
    _x =   x;
    _y = - z;
    _z =   y;
}

void TYPoint::setFromOGL(float coords[3])
{
    setFromOGL(coords[0], coords[1], coords[2]);
}

void TYPoint::getToOGL(float& x, float& y, float& z)
{
    x =   _x;
    y =   _z;
    z = - _y;
}

void TYPoint::getToOGL(float coords[3])
{
    getToOGL(coords[0], coords[1], coords[2]);
}

TYPoint& TYPoint::operator=(const TYPoint& other)
{
    if (this != &other)
    {
        TYElement::operator =(other);
        //      TYColorInterface::operator =(other);
        OCoord3D::operator =(other);
    }
    return *this;
}

bool TYPoint::operator==(const TYPoint& other) const
{
    if (this != &other)
    {
        if (TYElement::operator !=(other)) { return false; }
        //      if (TYColorInterface::operator !=(other)) return false;
        if (OCoord3D::operator !=(other)) { return false; }
    }
    return true;
}

bool TYPoint::operator!=(const TYPoint& other) const
{
    return !operator==(other);
}

bool TYPoint::deepCopy(const TYElement* pOther, bool copyId /*=true*/)
{
    if (!TYElement::deepCopy(pOther, copyId)) { return false; }

    //  TYColorInterface::deepCopy((TYColorInterface*)pOther, copyId);

    OCoord3D::operator =(* (TYPoint*) pOther);

    return true;
}

std::string TYPoint::toString() const
{
    std::string str = "TYPoint: X=" + doubleToStr(_x) + " Y=" + doubleToStr(_y) + " Z=" + doubleToStr(_z);
    return str;
}

DOM_Element TYPoint::toXML(DOM_Element& domElement)
{
    DOM_Element domNewElem = TYElement::toXML(domElement);

    //  TYColorInterface::toXML(domNewElem);

    // L'ID et le name d'un point n'ont pas besoin d'etre conserves
    domNewElem.removeAttribute("id");
    domNewElem.removeAttribute("name");

    // On enleve les 4 derniers chiffres, pour garder seulement
    // 2 chiffres apres la virgule
    domNewElem.setAttribute("x", doubleToStrPre(_x, 3).data());
    domNewElem.setAttribute("y", doubleToStrPre(_y, 3).data());
    domNewElem.setAttribute("z", doubleToStrPre(_z, 3).data());

    return domNewElem;
}

int TYPoint::fromXML(DOM_Element domElement)
{
    TYElement::fromXML(domElement);

    //  TYColorInterface::fromXML(domElement);

    // On genere un nouvel ID car l'ID des pts est supprime ! (cf + haut)
    //regenerateID();

    _x = TYXMLTools::getElementAttributeToDouble(domElement, "x");
    _y = TYXMLTools::getElementAttributeToDouble(domElement, "y");
    _z = TYXMLTools::getElementAttributeToDouble(domElement, "z");

    return 1;
}


