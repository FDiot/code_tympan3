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




#ifdef TYMPAN_USE_PRECOMPILED_HEADER
#include "Tympan/MetierSolver/DataManagerMetier/TYPHMetier.h"
#endif // TYMPAN_USE_PRECOMPILED_HEADER


OPROTOINST(TYTrafic);


TYTrafic::TYTrafic():
    _debitVL(0),
    _debitPL(0)
{
    _name = TYNameManager::get()->generateName(getClassName());
}

TYTrafic::TYTrafic(const TYTrafic& other)
{
    *this = other;
}

TYTrafic::TYTrafic(double debitVL, double debitPL):
    _debitVL(debitVL),
    _debitPL(debitPL)
{
    _name = TYNameManager::get()->generateName(getClassName());
}

TYTrafic::~TYTrafic()
{
}

TYTrafic& TYTrafic::operator=(const TYTrafic& other)
{
    if (this != &other)
    {
        TYElement::operator =(other);
        _debitVL = other._debitVL;
        _debitPL = other._debitPL;
    }
    return *this;
}

bool TYTrafic::operator==(const TYTrafic& other) const
{
    if (this != &other)
    {
        if (TYElement::operator !=(other)) { return false; }
        if (_debitVL != other._debitVL) { return false; }
        if (_debitPL != other._debitPL) { return false; }
    }
    return true;
}

bool TYTrafic::operator!=(const TYTrafic& other) const
{
    return !operator==(other);
}

bool TYTrafic::deepCopy(const TYElement* pOther, bool copyId /*=true*/)
{
    if (!TYElement::deepCopy(pOther, copyId)) { return false; }

    TYTrafic* pOtherTrafic = (TYTrafic*) pOther;

    _debitVL = pOtherTrafic->_debitVL;
    _debitPL = pOtherTrafic->_debitPL;

    return true;
}

std::string TYTrafic::toString() const
{
    return "TYTrafic";
}

DOM_Element TYTrafic::toXML(DOM_Element& domElement)
{
    DOM_Element domNewElem = TYElement::toXML(domElement);

    domNewElem.setAttribute("debitVL", doubleToStr(_debitVL).data());
    domNewElem.setAttribute("debitPL", doubleToStr(_debitPL).data());

    return domNewElem;
}

int TYTrafic::fromXML(DOM_Element domElement)
{
    TYElement::fromXML(domElement);

    _debitVL = TYXMLTools::getElementAttributeToDouble(domElement, "debitVL");
    _debitPL = TYXMLTools::getElementAttributeToDouble(domElement, "debitPL");

    return 1;
}
