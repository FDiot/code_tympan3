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


TYTrafic::TYTrafic()
{
    _name = TYNameManager::get()->generateName(getClassName());

    lv.vehicleType = VehicleType_VL;
    hgv.vehicleType = VehicleType_PL;
    for(unsigned i=0; i<NB_VEHICLE_TYPES; ++i)
    {
        arr[i].flowType = FlowType_CONST;
        arr[i].trafficFlow = 0;
        arr[i].trafficSpeed = 0;
    }
}

TYTrafic::TYTrafic(const TYTrafic& other)
{
    *this = other;
}

TYTrafic::~TYTrafic()
{
}

TYTrafic& TYTrafic::operator=(const TYTrafic& other)
{
    if (this != &other)
    {
        TYElement::operator =(other);
        for(unsigned i=0; i<NB_VEHICLE_TYPES; ++i)
        {
            arr[i].flowType = other.arr[i].flowType;
            arr[i].trafficFlow = other.arr[i].trafficFlow;
            arr[i].trafficSpeed = other.arr[i].trafficSpeed;
        }
    }
    return *this;
}

bool TYTrafic::operator==(const TYTrafic& other) const
{
    if (this != &other)
    {
        if (TYElement::operator !=(other)) { return false; }
        for(unsigned i=0; i<NB_VEHICLE_TYPES; ++i)
        {
            if( arr[i].flowType != other.arr[i].flowType)  {return false;};
            if( arr[i].trafficFlow != other.arr[i].trafficFlow) {return false;};
            if( arr[i].trafficSpeed != other.arr[i].trafficSpeed) {return false;};
        }
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

    const TYTrafic* pOtherTrafic = dynamic_cast<const TYTrafic*>(pOther);
    assert(pOtherTrafic && "invalid cast to TYTrafic*");

    *this = *pOtherTrafic;
    return true;
}

std::string TYTrafic::toString() const
{
    return "TYTrafic";
}

DOM_Element TYTrafic::toXML(DOM_Element& domElement)
{
    DOM_Element domNewElem = TYElement::toXML(domElement);

    // TODO update serialisation : cf https://extranet.logilab.fr/ticket/1512503
    domNewElem.setAttribute("debitVL", doubleToStr(getDebitVL()).data());
    domNewElem.setAttribute("debitPL", doubleToStr(getDebitPL()).data());

    return domNewElem;
}

int TYTrafic::fromXML(DOM_Element domElement)
{
    TYElement::fromXML(domElement);

    // TODO update serialisation : cf https://extranet.logilab.fr/ticket/1512503
    setDebitVL(TYXMLTools::getElementAttributeToDouble(domElement, "debitVL"));
    setDebitPL(TYXMLTools::getElementAttributeToDouble(domElement, "debitPL"));

    return 1;
}
