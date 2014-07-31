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

#if TY_USE_IHM
#include "Tympan/gui/widgets/TYMurElementWidget.h"
#include "Tympan/gui/gl/TYMurElementGraphic.h"
#endif

#include "Tympan/core/logging.h"
#include "Tympan/models/business/infrastructure/TYMur.h"
#include "TYMurElement.h"



TY_EXTENSION_INST(TYMurElement);
TY_EXT_GRAPHIC_INST(TYMurElement);

TYMurElement::TYMurElement()
{
    _name = TYNameManager::get()->generateName(getClassName());

    float r = 170.0f, g = 170.0f, b = 127.0f;
    _color.a = 1.0f;

#if TY_USE_IHM
    if (TYPreferenceManager::exists(TYDIRPREFERENCEMANAGER, "SubRectColorR"))
    {
        TYPreferenceManager::getColor(TYDIRPREFERENCEMANAGER, "SubRectColor", r, g, b);
    }
    else
    {
        TYPreferenceManager::setColor(TYDIRPREFERENCEMANAGER, "SubRectColor", r, g, b);
    }

    if (TYPreferenceManager::exists(TYDIRPREFERENCEMANAGER, "WinOpacity"))
    {
        _color.a = TYPreferenceManager::getDouble(TYDIRPREFERENCEMANAGER, "WinOpacity");
    }
    else
    {
        TYPreferenceManager::setDouble(TYDIRPREFERENCEMANAGER, "WinOpacity", _color.a);
    }
#endif // TY_USE_IHM

    _color.r = r / 255;
    _color.g = g / 255;
    _color.b = b / 255;

    _pParoi = new TYParoi();
    _pParoi->setParent(this);
}

TYMurElement::TYMurElement(const TYMurElement& other)
{
    *this = other;
}

TYMurElement::~TYMurElement()
{
}

TYMurElement& TYMurElement::operator=(const TYMurElement& other)
{
    if (this != &other)
    {
        TYAcousticRectangle::operator =(other);
        _pParoi = other._pParoi;
    }
    return *this;
}

bool TYMurElement::operator==(const TYMurElement& other) const
{
    if (this != &other)
    {
        if (TYAcousticRectangle::operator !=(other)) { return false; }
        if (_pParoi != other._pParoi) { return false; }
    }
    return true;
}

bool TYMurElement::operator!=(const TYMurElement& other) const
{
    return !operator==(other);
}

bool TYMurElement::deepCopy(const TYElement* pOther, bool copyId /*=true*/)
{
    if (!TYAcousticRectangle::deepCopy(pOther, copyId)) { return false; }

    TYMurElement* pOtherMurElt = (TYMurElement*) pOther;

    _pParoi->deepCopy(pOtherMurElt->_pParoi, copyId);

    return true;
}

std::string TYMurElement::toString() const
{
    return "TYMurElement";
}

DOM_Element TYMurElement::toXML(DOM_Element& domElement)
{
    DOM_Element domNewElem = TYAcousticRectangle::toXML(domElement);

    _pParoi->toXML(domNewElem);

    return domNewElem;
}

int TYMurElement::fromXML(DOM_Element domElement)
{
    TYAcousticRectangle::fromXML(domElement);

    DOM_Element elemCur;

    QDomNodeList childs = domElement.childNodes();
    for (unsigned int i = 0; i < childs.length(); i++)
    {
        elemCur = childs.item(i).toElement();
        _pParoi->callFromXMLIfEqual(elemCur);
    }

    return 1;
}

void TYMurElement::setColor(const OColor& color)
{
    TYColorInterface::setColor(color);

    if (getParent())
        if (!getIsSub())
        {
            TYMur::safeDownCast(getParent())->setColor(color);
        }

}

TYTabSourcePonctuelleGeoNode TYMurElement::getSrcs() const
{
    return TYAcousticRectangle::getSrcs();
}

TYSourcePonctuelle TYMurElement::srcPonctEquiv() const
{
    return TYAcousticRectangle::srcPonctEquiv();
}

void TYMurElement::distriSrcs()
{
    TYAcousticRectangle::distriSrcs();
}

bool TYMurElement::setSrcsLw()
{
    return TYAcousticRectangle::setSrcsLw();
}

double TYMurElement::surface() const
{
    return TYAcousticRectangle::surface();
}

int TYMurElement::intersects(const TYSurfaceInterface* pSurf, OSegment3D& seg) const
{
    return TYAcousticRectangle::intersects(pSurf, seg);
}

int TYMurElement::intersects(const OSegment3D& seg, OPoint3D& pt) const
{
    return TYAcousticRectangle::intersects(seg, pt);
}
