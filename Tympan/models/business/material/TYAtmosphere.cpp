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
#include "Tympan/gui/widgets/TYAtmosphereWidget.h"
#endif

#include "Tympan/models/solver/entities.hpp"
#include "Tympan/core/logging.h"


#include "TYAtmosphere.h"

TY_EXTENSION_INST(TYAtmosphere);

TYAtmosphere::TYAtmosphere()
{
    _name = TYNameManager::get()->generateName(getClassName());

    _pression = 101325;
    _temperature = 20.0;
    _hygrometrie = 70.0;

#if TY_USE_IHM
    if (TYPreferenceManager::exists(TYDIRPREFERENCEMANAGER, "PressionAtmoDefault"))
    {
        _pression = TYPreferenceManager::getDouble(TYDIRPREFERENCEMANAGER, "PressionAtmoDefault");
    }
    else
    {
        TYPreferenceManager::setDouble(TYDIRPREFERENCEMANAGER, "PressionAtmoDefault", _pression);
    }

    if (TYPreferenceManager::exists(TYDIRPREFERENCEMANAGER, "TempAtmoDefault"))
    {
        _temperature = TYPreferenceManager::getDouble(TYDIRPREFERENCEMANAGER, "TempAtmoDefault");
    }
    else
    {
        TYPreferenceManager::setDouble(TYDIRPREFERENCEMANAGER, "TempAtmoDefault", _temperature);
    }

    if (TYPreferenceManager::exists(TYDIRPREFERENCEMANAGER, "HygroAtmoDefault"))
    {
        _hygrometrie = TYPreferenceManager::getDouble(TYDIRPREFERENCEMANAGER, "HygroAtmoDefault");
    }
    else
    {
        TYPreferenceManager::setDouble(TYDIRPREFERENCEMANAGER, "HygroAtmoDefault", _hygrometrie);
    }

#endif
}

TYAtmosphere::TYAtmosphere(const TYAtmosphere& other)
{
    *this = other;
}

TYAtmosphere::~TYAtmosphere()
{
}

TYAtmosphere& TYAtmosphere::operator=(const TYAtmosphere& other)
{
    if (this != &other)
    {
        TYElement::operator =(other);
        _pression = other._pression;
        _temperature = other._temperature;
        _hygrometrie = other._hygrometrie;
    }

    return *this;
}

bool TYAtmosphere::operator==(const TYAtmosphere& other) const
{
    if (this != &other)
    {
        if (TYElement::operator !=(other)) { return false; }
        if (_pression != other._pression) { return false; }
        if (_temperature != other._temperature) { return false; }
        if (_hygrometrie != other._hygrometrie) { return false; }
    }
    return true;
}

bool TYAtmosphere::operator!=(const TYAtmosphere& other) const
{
    return !operator==(other);
}

bool TYAtmosphere::deepCopy(const TYElement* pOther, bool copyId /*=true*/)
{
    if (!TYElement::deepCopy(pOther, copyId)) { return false; }

    TYAtmosphere* pOtherAtm = (TYAtmosphere*) pOther;

    return true;
}

std::string TYAtmosphere::toString() const
{
    return "TYAtmosphere";
}

DOM_Element TYAtmosphere::toXML(DOM_Element& domElement)
{
    DOM_Element domNewElem = TYElement::toXML(domElement);

    TYXMLTools::addElementDoubleValue(domNewElem, "pression", _pression);
    TYXMLTools::addElementDoubleValue(domNewElem, "temperature", _temperature);
    TYXMLTools::addElementDoubleValue(domNewElem, "hygrometrie", _hygrometrie);

    return domNewElem;
}

int TYAtmosphere::fromXML(DOM_Element domElement)
{
    TYElement::fromXML(domElement);
    unsigned int i;

    bool getOk[3] = { false, false, false };

    DOM_Element elemCur;

    QDomNodeList childs = domElement.childNodes();
    for (i = 0; i < childs.length(); i++)
    {
        elemCur = childs.item(i).toElement();
        TYXMLTools::getElementDoubleValue(elemCur, "pression", _pression, getOk[0]);
        TYXMLTools::getElementDoubleValue(elemCur, "temperature", _temperature, getOk[1]);
        TYXMLTools::getElementDoubleValue(elemCur, "hygrometrie", _hygrometrie, getOk[2]);
    }

    return 1;
}

double TYAtmosphere::getVitSon() const
{ 
    tympan::AtmosphericConditions atm(_pression, _temperature, _hygrometrie);

    return atm.compute_c(); 
}

TYSpectre TYAtmosphere::getSpectreAtt() 
{ 
    tympan::AtmosphericConditions atm(_pression, _temperature, _hygrometrie);
    atm.compute_absorption_spectrum();
    return atm.get_absorption_spectrum();  
}