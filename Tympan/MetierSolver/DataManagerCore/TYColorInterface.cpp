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
 */



#ifdef TYMPAN_USE_PRECOMPILED_HEADER
#include "TYPHCore.h"
#endif // TYMPAN_USE_PRECOMPILED_HEADER

TYColorInterface::TYColorInterface() : _color(OColor::WHITE)
{

}

TYColorInterface::TYColorInterface(const TYColorInterface& other)
{
    *this = other;
}

TYColorInterface::~TYColorInterface()
{

}

TYColorInterface& TYColorInterface::operator=(const TYColorInterface& other)
{
    if (this != &other)
    {
        _color = other._color;
    }
    return *this;
}

bool TYColorInterface::operator==(const TYColorInterface& other) const
{
    if (this != &other)
    {
        if (_color != other._color)
        {
            return false;
        }
    }
    return true;
}

bool TYColorInterface::operator!=(const TYColorInterface& other) const
{
    return !operator==(other);
}

void TYColorInterface::deepCopy(const TYColorInterface* pOther, bool copyId /*= true*/)
{
    _color = pOther->_color;
}

DOM_Element TYColorInterface::toXML(DOM_Element& domElement)
{
    QDomDocument domDoc = domElement.ownerDocument();
    QDomElement domNewElem = domDoc.createElement("Color");

    domNewElem.setAttribute("r", _color.r);
    domNewElem.setAttribute("g", _color.g);
    domNewElem.setAttribute("b", _color.b);

    domElement.appendChild(domNewElem);

    return domElement;
}

int TYColorInterface::fromXML(DOM_Element domElement)
{
    DOM_Element elemCur;
    QString tagName = "";
    elemCur = domElement.firstChild().toElement();
    if (!elemCur.isNull())
    {
        tagName = elemCur.tagName();
    }

    if (tagName == "Color")
    {
        _color.r = TYXMLTools::getElementAttributeToDouble(elemCur, "r");
        _color.g = TYXMLTools::getElementAttributeToDouble(elemCur, "g");
        _color.b = TYXMLTools::getElementAttributeToDouble(elemCur, "b");

        return 1;
    }

    return 0;
}


void TYColorInterface::setColor(const OColor& color)
{
    _color = color;
}

OColor TYColorInterface::getColor() const
{
    return _color;
}
