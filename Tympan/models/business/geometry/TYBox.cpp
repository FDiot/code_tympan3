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


#include "Tympan/core/logging.h"
#include "Tympan/models/common/3d.h"
#include "Tympan/models/business/geometry/TYRectangle.h" 
#if TY_USE_IHM
  #include "Tympan/gui/widgets/TYBoxWidget.h"
#endif
#include "TYBox.h"

TY_EXTENSION_INST(TYBox);

TYBox::TYBox():
    _sizeX(1),
    _sizeY(1),
    _sizeZ(1)
{
    _name = TYNameManager::get()->generateName(getClassName());

    _position = TYPoint(0, 0, 0);
}

TYBox::TYBox(const TYBox& other)
{
    *this = other;
}

TYBox::~TYBox()
{
}

TYBox& TYBox::operator=(const TYBox& other)
{
    if (this != &other)
    {
        TYElement::operator =(other);
        TYColorInterface::operator =(other);
        _sizeX = other._sizeX;
        _sizeY = other._sizeY;
        _sizeZ = other._sizeZ;
        _position = other._position;
    }
    return *this;
}

bool TYBox::operator==(const TYBox& other) const
{
    if (this != &other)
    {
        if (TYElement::operator !=(other)) { return false; }
        if (TYColorInterface::operator !=(other)) { return false; }
        if (_sizeX != other._sizeX) { return false; }
        if (_sizeY != other._sizeY) { return false; }
        if (_sizeZ != other._sizeZ) { return false; }
        if (_position != other._position) { return false; }
    }
    return true;
}

bool TYBox::operator!=(const TYBox& other) const
{
    return !operator==(other);
}

bool TYBox::deepCopy(const TYElement* pOther, bool copyId /*=true*/)
{
    if (!TYElement::deepCopy(pOther, copyId)) { return false; }

    TYColorInterface::deepCopy((TYColorInterface*)pOther, copyId);

    TYBox* pOtherBox = (TYBox*) pOther;

    _sizeX = pOtherBox->_sizeX;
    _sizeY = pOtherBox->_sizeY;
    _sizeZ = pOtherBox->_sizeZ;
    _position = pOtherBox->_position;

    return true;
}

std::string TYBox::toString() const
{
    std::string str = "TYBox: X=" + doubleToStr(_sizeX) + " Y=" + doubleToStr(_sizeY) + " Z=" + doubleToStr(_sizeZ);
    return str;
}

DOM_Element TYBox::toXML(DOM_Element& domElement)
{
    DOM_Element domNewElem = TYElement::toXML(domElement);

    TYColorInterface::toXML(domNewElem);

    domNewElem.setAttribute("sizeX", doubleToStr(_sizeX).data());
    domNewElem.setAttribute("sizeY", doubleToStr(_sizeY).data());
    domNewElem.setAttribute("sizeZ", doubleToStr(_sizeZ).data());

    _position.toXML(domNewElem);

    return domNewElem;
}

int TYBox::fromXML(DOM_Element domElement)
{
    TYElement::fromXML(domElement);

    TYColorInterface::fromXML(domElement);

    _sizeX = TYXMLTools::getElementAttributeToDouble(domElement, "sizeX");
    _sizeY = TYXMLTools::getElementAttributeToDouble(domElement, "sizeY");
    _sizeZ = TYXMLTools::getElementAttributeToDouble(domElement, "sizeZ");

    DOM_Element elemCur;

    QDomNodeList childs = domElement.childNodes();

    for (unsigned int i = 0; i < childs.length(); i++)
    {
        elemCur = childs.item(i).toElement();

        _position.callFromXMLIfEqual(elemCur);
    }

    return 1;
}

int TYBox::intersects(const OSegment3D& seg, TYTabPoint& ptList) const
{
    int res = INTERS_NULLE;
    OPoint3D pt;
    int nbIntersects = 0;

    // Construction des faces
    TYPoint pt1, pt2, pt3, pt4;
    TYRectangle faces[6];
    double demiSizeX = _sizeX / 2.0;
    double demiSizeY = _sizeY / 2.0;
    double demiSizeZ = _sizeZ / 2.0;

    pt1.set(+demiSizeX, -demiSizeY, +demiSizeZ); pt2.set(+demiSizeX, +demiSizeY, +demiSizeZ);
    pt3.set(+demiSizeX, +demiSizeY, -demiSizeZ); pt4.set(+demiSizeX, -demiSizeY, -demiSizeZ);
    faces[0] = TYRectangle(pt1, pt2, pt3, pt4);

    pt1.set(-demiSizeX, -demiSizeY, +demiSizeZ); pt2.set(+demiSizeX, -demiSizeY, +demiSizeZ);
    pt3.set(+demiSizeX, -demiSizeY, -demiSizeZ); pt4.set(-demiSizeX, -demiSizeY, -demiSizeZ);
    faces[1] = TYRectangle(pt1, pt2, pt3, pt4);

    pt1.set(-demiSizeX, +demiSizeY, +demiSizeZ); pt2.set(-demiSizeX, -demiSizeY, +demiSizeZ);
    pt3.set(-demiSizeX, -demiSizeY, -demiSizeZ); pt4.set(-demiSizeX, +demiSizeY, -demiSizeZ);
    faces[2] = TYRectangle(pt1, pt2, pt3, pt4);

    pt1.set(+demiSizeX, +demiSizeY, +demiSizeZ); pt2.set(-demiSizeX, +demiSizeY, +demiSizeZ);
    pt3.set(-demiSizeX, +demiSizeY, -demiSizeZ); pt4.set(+demiSizeX, +demiSizeY, -demiSizeZ);
    faces[3] = TYRectangle(pt1, pt2, pt3, pt4);

    pt1.set(-demiSizeX, -demiSizeY, -demiSizeZ); pt2.set(+demiSizeX, -demiSizeY, -demiSizeZ);
    pt3.set(+demiSizeX, +demiSizeY, -demiSizeZ); pt4.set(-demiSizeX, +demiSizeY, -demiSizeZ);
    faces[4] = TYRectangle(pt1, pt2, pt3, pt4);

    pt1.set(-demiSizeX, +demiSizeY, +demiSizeZ); pt2.set(+demiSizeX, +demiSizeY, +demiSizeZ);
    pt3.set(+demiSizeX, -demiSizeY, +demiSizeZ); pt4.set(-demiSizeX, -demiSizeY, +demiSizeZ);
    faces[5] = TYRectangle(pt1, pt2, pt3, pt4);

    // Passage en coord locales
    OPoint3D ptTmp1 = OVector3D(seg._ptA) - OVector3D(_position);
    OPoint3D ptTmp2 = OVector3D(seg._ptB) - OVector3D(_position);
    OSegment3D segTmp(ptTmp1, ptTmp2);

    // Pour chaque face
    for (int i = 0; i < 6; i++)
    {
        // Recherche de l'intersection du segment avec la face
        if (faces[i].intersects(segTmp, pt) != INTERS_NULLE)
        {
            // On ramene le point en coord d'origine
            pt = OVector3D(pt) + OVector3D(_position);

            // Ajout du pt d'intersection trouve a la liste
            ptList.push_back(pt);

            // Increment du nb d'intersections trouvees
            nbIntersects++;
        }
    }

    // Une intersection a-t-elle ete trouvee ?
    if (nbIntersects > 0)
    {
        res = INTERS_OUI;
    }

    return res;
}

int TYBox::isInside(const TYPoint& pt) const
{
    int res = INTERS_NULLE;
    double demiSizeX = _sizeX / 2.0;
    double demiSizeY = _sizeY / 2.0;
    double demiSizeZ = _sizeZ / 2.0;

    TYPoint ptTmp = OVector3D(pt) - OVector3D(_position);

    if ((ptTmp._x >= -demiSizeX) && (ptTmp._x <= demiSizeX) &&
        (ptTmp._y >= -demiSizeY) && (ptTmp._y <= demiSizeY) &&
        (ptTmp._z >= -demiSizeZ) && (ptTmp._z <= demiSizeZ))
    {
        res = INTERS_OUI;
    }

    return res;
}
