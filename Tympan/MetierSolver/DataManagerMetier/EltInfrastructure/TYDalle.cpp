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
#include "Tympan/GraphicIHM/DataManagerIHM/TYDalleWidget.h"
#endif

#ifdef TYMPAN_USE_PRECOMPILED_HEADER
#include "Tympan/MetierSolver/DataManagerMetier/TYPHMetier.h"
#endif // TYMPAN_USE_PRECOMPILED_HEADER

#include "TYDalle.h"
#include "Tympan/Tools/OMessageManager.h"


OPROTOINST(TYDalle);
TY_EXTENSION_INST(TYDalle);

TYDalle::TYDalle()
{
    _name = TYNameManager::get()->generateName(getClassName());

    _pParoi = new TYParoi();
    _pParoi->setParent(this);
    _bParoiLocked = false;

    // Couleur par default R=162 G=124 B=1

    float r = 211.0f, g = 211.0f, b = 0.0f;

#if TY_USE_IHM
    if (TYPreferenceManager::exists(TYDIRPREFERENCEMANAGER, "TYDalleGraphicColorR"))
    {
        TYPreferenceManager::getColor(TYDIRPREFERENCEMANAGER, "TYDalleGraphicColor", r, g, b);
    }
    else
    {
        TYPreferenceManager::setColor(TYDIRPREFERENCEMANAGER, "TYDalleGraphicColor", r, g, b);
    }
#endif
    OColor color;
    color.r = r;
    color.g = g;
    color.b = b;

    setColor(color);
}

TYDalle::TYDalle(const TYDalle& other)
{
    *this = other;
}

TYDalle::~TYDalle()
{
}

TYDalle& TYDalle::operator=(const TYDalle& other)
{
    if (this != &other)
    {
        TYAcousticPolygon::operator =(other);
        _pParoi = other._pParoi;
        _bParoiLocked = other._bParoiLocked;
    }
    return *this;
}

bool TYDalle::operator==(const TYDalle& other) const
{
    if (this != &other)
    {
        if (TYAcousticPolygon::operator !=(other)) { return false; }
        if (_pParoi != other._pParoi) { return false; }
    }
    return true;
}

bool TYDalle::operator!=(const TYDalle& other) const
{
    return !operator==(other);
}

bool TYDalle::deepCopy(const TYElement* pOther, bool copyId /*=true*/)
{
    if (!TYAcousticPolygon::deepCopy(pOther, copyId)) { return false; }

    TYDalle* pOtherDalle = (TYDalle*) pOther;

    _pParoi->deepCopy(pOtherDalle->_pParoi, copyId);

    _bParoiLocked = pOtherDalle->_bParoiLocked;

    return true;
}

std::string TYDalle::toString() const
{
    return "TYDalle";
}

DOM_Element TYDalle::toXML(DOM_Element& domElement)
{
    DOM_Element domNewElem = TYAcousticPolygon::toXML(domElement);

    TYXMLTools::addElementBoolValue(domNewElem, "paroiLocked", _bParoiLocked);
    _pParoi->toXML(domNewElem);

    return domNewElem;
}

int TYDalle::fromXML(DOM_Element domElement)
{
    bool statParoiOk = false;

    TYAcousticPolygon::fromXML(domElement);

    DOM_Element elemCur;

    QDomNodeList childs = domElement.childNodes();
    for (unsigned int i = 0; i < childs.length(); i++)
    {
        elemCur = childs.item(i).toElement();

        TYXMLTools::getElementBoolValue(elemCur, "paroiLocked", _bParoiLocked, statParoiOk);
        _pParoi->callFromXMLIfEqual(elemCur);
    }

    return 1;
}

void TYDalle::setParoi(const LPTYParoi pParoi)
{
    assert(pParoi);
    assert(_pParoi);

    if (!_bParoiLocked) // Si elle est lockee, on ne peut pas la modifier
    {
        *_pParoi = *pParoi;
        _pParoi->setParent(this);
    }
}

void TYDalle::setParoiLocked(const bool& bVal)
{
    _bParoiLocked = bVal;

    if (_pParent) // Inutile de verifier le lockage setParoi y pourvoira
    {
        setParoi(TYEtage::safeDownCast(_pParent)->getParoi());
    }
}

TYMateriauConstruction* TYDalle::getMateriau()
{
    return _pParoi->getMatFace2();
}
