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
#include "Tympan/GraphicIHM/DataManagerIHM/TYTraficWidget.h"
#endif

#ifdef TYMPAN_USE_PRECOMPILED_HEADER
#include "Tympan/MetierSolver/DataManagerMetier/TYPHMetier.h"
#endif // TYMPAN_USE_PRECOMPILED_HEADER

#include "TYTrafic.h"

#include "Tympan/core/logging.h"

TY_EXTENSION_INST(TYTrafic);

TYTrafic::TYTrafic()
{
    _name = TYNameManager::get()->generateName(getClassName());

    lv.vehicleType = VehicleType_VL;
    hgv.vehicleType = VehicleType_PL;
    for (unsigned i = 0; i < NB_VEHICLE_TYPES; ++i)
    {
        arr[i].flowType = FlowType_CONST;
        arr[i].trafficFlow = 0;
        arr[i].trafficSpeed = 0;
        assert((arr[i].vehicleType == i + 1) && "Bad consistency assumption wrt vehicles types in Ctor");

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
        for (unsigned i = 0; i < NB_VEHICLE_TYPES; ++i)
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
        for (unsigned i = 0; i < NB_VEHICLE_TYPES; ++i)
        {
            if (arr[i].flowType != other.arr[i].flowType)  {return false;};
            if (arr[i].trafficFlow != other.arr[i].trafficFlow) {return false;};
            if (arr[i].trafficSpeed != other.arr[i].trafficSpeed) {return false;};
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
    DOM_Element domTrafficElem = TYElement::toXML(domElement);

    for (unsigned i = 0; i < NB_VEHICLE_TYPES; ++i)
    {
        const RoadTrafficComponent& rtc = arr[i];
        QDomDocument domDoc = domTrafficElem.ownerDocument();
        QDomElement componentElem = domDoc.createElement("TrafficComponent");
        componentElem.setAttribute("flowType", rtc.flowType);
        componentElem.setAttribute("trafficFlow", rtc.trafficFlow);
        componentElem.setAttribute("trafficSpeed", rtc.trafficSpeed);
        assert((rtc.vehicleType == i + 1) && "Bad consistency assumption wrt vehicles types");
        componentElem.setAttribute("vehicleType", rtc.vehicleType);
        domTrafficElem.appendChild(componentElem);
    }
    return domTrafficElem;
}

int TYTrafic::fromXML(DOM_Element domElement)
{
    TYElement::fromXML(domElement);

    QDomNodeList children = domElement.elementsByTagName("TrafficComponent");
    if (children.size() != NB_VEHICLE_TYPES)
    {
        OMessageManager::get()->error(
            "Loading TYTrafic element %s, "
            "%u TrafficComponent child elements were found but %u were expected",
            str_qt2c(getStringID()), children.size(), NB_VEHICLE_TYPES);
        return 0;
    }

    for (unsigned i = 0; i < NB_VEHICLE_TYPES; ++i)
    {
        QDomElement elem = children.item(i).toElement();
        if (elem.isNull())
        {
            debugXml(children.item(i));
            return 0;
        }
        if (!fromXML_TrafficComponent(elem, arr[i]))
        {
            return 0;
        }
        assert((arr[i].vehicleType == i + 1) && "Inconsistent vehicle types in traffic components");
    }

    return 1;
}

int TYTrafic::fromXML_TrafficComponent(DOM_Element domElement, RoadTrafficComponent& rtc)
{
    QString s;
    bool ok;

    // Deserialise the trafficFlow
    s = domElement.attribute("trafficFlow", QString());
    if (s.isEmpty()) // Attribute not found
    {
        OMessageManager::get()->error(
            "Can not read the TrafficComponent `trafficFlow` attribute for element %s.",
            str_qt2c(getStringID()));
        return 0;
    }
    double trafficFlow = s.toDouble(&ok);
    if (!ok)
    {
        OMessageManager::get()->error(
            "Floating point number expected for attribute `trafficFlow` on element %s, not %s",
            str_qt2c(getStringID()), str_qt2c(s));
        return 0;
    }

    // Deserialise the trafficSpeed
    s = domElement.attribute("trafficSpeed", QString());
    if (s.isEmpty()) // Attribute not found
    {
        OMessageManager::get()->error(
            "Can not read the TrafficComponent `trafficSpeed` attribute for element %s.",
            str_qt2c(getStringID()));
        return 0;
    }
    double trafficSpeed = s.toDouble(&ok);
    if (!ok)
    {
        OMessageManager::get()->error(
            "Floating point number expected for attribute `trafficSpeed` on element %s, not %s",
            str_qt2c(getStringID()), str_qt2c(s));
        return 0;
    }

    // Deserialise the flowType
    s = domElement.attribute("flowType", QString());
    if (s.isEmpty()) // Attribute not found
    {
        OMessageManager::get()->error(
            "Can not read the TrafficComponent `flowType` attribute for element %s.",
            str_qt2c(getStringID()));
        return 0;
    }
    unsigned flowType = s.toUInt(&ok);
    if (!ok)
    {
        OMessageManager::get()->error(
            "Integer expected for attribute `flowType` on element %s, not %s",
            str_qt2c(getStringID()), str_qt2c(s));
        return 0;
    }

    // Deserialise the vehicleType
    s = domElement.attribute("vehicleType", QString());
    if (s.isEmpty()) // Attribute not found
    {
        OMessageManager::get()->error(
            "Can not read the TrafficComponent `vehicleType` attribute for element %s.",
            str_qt2c(getStringID()));
        return 0;
    }
    unsigned vehicleType = s.toUInt(&ok);
    if (!ok)
    {
        OMessageManager::get()->error(
            "Integer expected for attribute `vehicleType` on element %s, not %s",
            str_qt2c(getStringID()), str_qt2c(s));
        return 0;
    }

    rtc.trafficFlow = trafficFlow;
    rtc.trafficSpeed = trafficSpeed;
    rtc.flowType = static_cast<RoadFlowType>(flowType);
    rtc.vehicleType = static_cast<RoadVehicleType>(vehicleType);
    return 1;
}
