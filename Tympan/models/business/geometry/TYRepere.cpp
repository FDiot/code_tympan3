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
#include "Tympan/gui/widgets/TYRepereWidget.h"
#endif


#include "Tympan/core/logging.h"


#include "TYRepere.h"

TY_EXTENSION_INST(TYRepere);

TYRepere::TYRepere():
    ORepere3D()
{
    _name = TYNameManager::get()->generateName(getClassName());
}

TYRepere::TYRepere(const TYRepere& other)
{
    *this = other;
}

TYRepere::TYRepere(const OPoint3D& origin, const OVector3D& vecX, const OVector3D& vecY, const OVector3D& vecZ):
    ORepere3D(origin, vecX, vecY, vecZ)
{
    _name = TYNameManager::get()->generateName(getClassName());
}


TYRepere::TYRepere(const ORepere3D& repere):
    ORepere3D(repere)
{
    _name = TYNameManager::get()->generateName(getClassName());
}


TYRepere::~TYRepere()
{
}

TYRepere& TYRepere::operator=(const TYRepere& other)
{
    if (this != &other)
    {
        TYElement::operator =(other);
        ORepere3D::operator =(other);
    }
    return *this;
}

bool TYRepere::operator==(const TYRepere& other) const
{
    if (this != &other)
    {
        if (TYElement::operator !=(other)) { return false; }
        if (ORepere3D::operator !=(other)) { return false; }
    }
    return true;
}

bool TYRepere::operator!=(const TYRepere& other) const
{
    return !operator==(other);
}

bool TYRepere::deepCopy(const TYElement* pOther, bool copyId /*=true*/)
{
    if (!TYElement::deepCopy(pOther, copyId)) { return false; }

    ORepere3D::operator=(* (TYRepere*) pOther);

    return true;
}

std::string TYRepere::toString() const
{
    return "TYRepere";
}

DOM_Element TYRepere::toXML(DOM_Element& domElement)
{
    DOM_Element domNewElem = TYElement::toXML(domElement);
    DOM_Document domDoc = domElement.ownerDocument();
    DOM_Element pointElem = domDoc.createElement("Point");
    domNewElem.appendChild(pointElem);
    pointElem.setAttribute("x", doubleToStrPre(_origin._x, 12).data());
    pointElem.setAttribute("y", doubleToStrPre(_origin._y, 12).data());
    pointElem.setAttribute("z", doubleToStrPre(_origin._z, 12).data());

    DOM_Element vectorXElem = domDoc.createElement("Vector");
    domNewElem.appendChild(vectorXElem);
    vectorXElem.setAttribute("x", doubleToStrPre(_vecI._x, 12).data());
    vectorXElem.setAttribute("y", doubleToStrPre(_vecI._y, 12).data());
    vectorXElem.setAttribute("z", doubleToStrPre(_vecI._z, 12).data());

    DOM_Element vectorYElem = domDoc.createElement("Vector");
    domNewElem.appendChild(vectorYElem);
    vectorYElem.setAttribute("x", doubleToStrPre(_vecJ._x, 12).data());
    vectorYElem.setAttribute("y", doubleToStrPre(_vecJ._y, 12).data());
    vectorYElem.setAttribute("z", doubleToStrPre(_vecJ._z, 12).data());

    DOM_Element vectorZElem = domDoc.createElement("Vector");
    domNewElem.appendChild(vectorZElem);
    vectorZElem.setAttribute("x", doubleToStrPre(_vecK._x, 12).data());
    vectorZElem.setAttribute("y", doubleToStrPre(_vecK._y, 12).data());
    vectorZElem.setAttribute("z", doubleToStrPre(_vecK._z, 12).data());

    return domNewElem;
}

int TYRepere::fromXML(DOM_Element domElement)
{
    int vecNb = 0;
    DOM_Element elemCur;

    TYElement::fromXML(domElement);

    QDomNodeList childs = domElement.childNodes();

    for (unsigned int i = 0; i < childs.length(); i++)
    {
        elemCur = childs.item(i).toElement();
        if (elemCur.nodeName() == "Point")
        {
            _origin._x = TYXMLTools::getElementAttributeToDouble(elemCur, "x");
            _origin._y = TYXMLTools::getElementAttributeToDouble(elemCur, "y");
            _origin._z = TYXMLTools::getElementAttributeToDouble(elemCur, "z");
        }

        if (elemCur.nodeName() == "Vector")
        {

            if (vecNb == 0)   // Vector X
            {
                _vecI._x = TYXMLTools::getElementAttributeToDouble(elemCur, "x");
                _vecI._y = TYXMLTools::getElementAttributeToDouble(elemCur, "y");
                _vecI._z = TYXMLTools::getElementAttributeToDouble(elemCur, "z");
            }
            else if (vecNb == 1)     // Vector Y
            {
                _vecJ._x = TYXMLTools::getElementAttributeToDouble(elemCur, "x");
                _vecJ._y = TYXMLTools::getElementAttributeToDouble(elemCur, "y");
                _vecJ._z = TYXMLTools::getElementAttributeToDouble(elemCur, "z");
            }
            else if (vecNb == 2)     // Vector Z
            {
                _vecK._x = TYXMLTools::getElementAttributeToDouble(elemCur, "x");
                _vecK._y = TYXMLTools::getElementAttributeToDouble(elemCur, "y");
                _vecK._z = TYXMLTools::getElementAttributeToDouble(elemCur, "z");
            }

            // Increment l'indice car un vector est trouve
            vecNb++;
        }
    }

    return 1;
}
