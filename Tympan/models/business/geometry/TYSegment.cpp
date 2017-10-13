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

#include <math.h>

#include "Tympan/core/logging.h"
#include "Tympan/core/defines.h"
#if TY_USE_IHM
  #include "Tympan/gui/widgets/TYSegmentWidget.h"
  #include "Tympan/gui/gl/TYSegmentGraphic.h"
#endif
#include "TYSegment.h"

TY_EXTENSION_INST(TYSegment);
TY_EXT_GRAPHIC_INST(TYSegment);

TYSegment::TYSegment()
{
    _name = TYNameManager::get()->generateName(getClassName());

    _ptA._x = 0; _ptA._y = 0; _ptA._z = 0;
    _ptB._x = 0; _ptB._y = 1; _ptB._z = 0;
}

TYSegment::TYSegment(const TYSegment& other)
{
    *this = other;
}

TYSegment::TYSegment(TYPoint ptA, TYPoint ptB)
{
    _name = TYNameManager::get()->generateName(getClassName());

    _ptA = ptA;
    _ptB = ptB;
}

TYSegment::~TYSegment()
{
}

TYSegment& TYSegment::operator=(const TYSegment& other)
{
    if (this != &other)
    {
        TYElement::operator =(other);
        TYColorInterface::operator =(other);
        _ptA = other._ptA;
        _ptB = other._ptB;
    }
    return *this;
}

bool TYSegment::operator==(const TYSegment& other) const
{
    if (this != &other)
    {
        if (TYElement::operator !=(other)) { return false; }
        if (TYColorInterface::operator !=(other)) { return false; }
        if (_ptA != other._ptA) { return false; }
        if (_ptB != other._ptB) { return false; }
    }
    return true;
}

bool TYSegment::operator!=(const TYSegment& other) const
{
    return !operator==(other);
}

bool TYSegment::deepCopy(const TYElement* pOther, bool copyId /*=true*/)
{
    if (!TYElement::deepCopy(pOther, copyId)) { return false; }

    TYColorInterface::deepCopy((TYColorInterface*)pOther, copyId);

    TYSegment* pOtherSeg = (TYSegment*) pOther;

    _ptA = pOtherSeg->_ptA;
    _ptB = pOtherSeg->_ptB;

    return true;
}

std::string TYSegment::toString() const
{
    std::string str = "TYSegment";
    return str;
}

DOM_Element TYSegment::toXML(DOM_Element& domElement)
{
    DOM_Element domNewElem = TYElement::toXML(domElement);

    TYColorInterface::toXML(domNewElem);

    TYPoint ptA(_ptA), ptB(_ptB);

    ptA.toXML(domNewElem);
    ptB.toXML(domNewElem);

    return domNewElem;
}

int TYSegment::fromXML(DOM_Element domElement)
{
    int ptNb = 0;
    DOM_Element elemCur;
    TYPoint ptA, ptB;


    TYElement::fromXML(domElement);

    TYColorInterface::fromXML(domElement);

    QDomNodeList childs = domElement.childNodes();

    for (unsigned int i = 0; i < childs.length(); i++)
    {
        elemCur = childs.item(i).toElement();
        if (ptNb == 0)
        {
            // Increment l'indice si le pt A est trouve
            ptA.callFromXMLIfEqual(elemCur) ? ptNb++ : ptNb;
        }
        else if (ptNb == 1)
        {
            // Increment l'indice si le pt B est trouve
            ptB.callFromXMLIfEqual(elemCur) ? ptNb++ : ptNb;
        }
    }

    _ptA = OPoint3D(ptA._x, ptA._y, ptA._z);
    _ptB = OPoint3D(ptB._x, ptB._y, ptB._z);

    return 1;
}

bool TYSegment::unorderedIsEqual(const TYSegment& s1, const TYSegment& s2)
{
    if (&s1 == &s2)
    {
        return true;
    }
    if (s1._ptA == s2._ptA && s1._ptB == s2._ptB)
    {
        return true;
    }
    if (s1._ptA == s2._ptB && s1._ptB == s2._ptA)
    {
        return true;
    }
    return false;
}

