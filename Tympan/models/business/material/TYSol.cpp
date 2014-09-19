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
#if TY_USE_IHM
  #include "Tympan/gui/widgets/TYSolWidget.h"
  #include "Tympan/models/business/TYPreferenceManager.h"
#endif
#include "TYSol.h"

TY_EXTENSION_INST(TYSol);

TYSol::TYSol():
    _resistivite(20000),
    _epaisseur(1.0),
    _vegetActive(false)
{
    _name = TYNameManager::get()->generateName(getClassName());

    _pVegetation = NULL;

#if TY_USE_IHM
    if (TYPreferenceManager::exists(TYDIRPREFERENCEMANAGER, "ResisSolDefault"))
    {
        _resistivite = TYPreferenceManager::getDouble(TYDIRPREFERENCEMANAGER, "ResisSolDefault");
    }
    else
    {
        TYPreferenceManager::setDouble(TYDIRPREFERENCEMANAGER, "ResisSolDefault", _resistivite);
    }

    if (TYPreferenceManager::exists(TYDIRPREFERENCEMANAGER, "EpaisSolDefault"))
    {
        _epaisseur = TYPreferenceManager::getDouble(TYDIRPREFERENCEMANAGER, "EpaisSolDefault");
    }
    else
    {
        TYPreferenceManager::setDouble(TYDIRPREFERENCEMANAGER, "EpaisSolDefault", _epaisseur);
    }
#endif
}

TYSol::TYSol(const TYSol& other)
{
    *this = other;
}

TYSol::~TYSol()
{
    if (_pVegetation)
    {
        delete _pVegetation;
    }

    _pVegetation = NULL;
}

TYSol& TYSol::operator=(const TYSol& other)
{
    if (this != &other)
    {
        TYElement::operator =(other);
        _resistivite = other._resistivite;
        _epaisseur = other._epaisseur;
        _vegetActive = other._vegetActive;
        _pVegetation = other._pVegetation;
    }
    return *this;
}

bool TYSol::operator==(const TYSol& other) const
{
    if (this != &other)
    {
        if (TYElement::operator !=(other)) { return false; }
        if (_resistivite != other._resistivite) { return false; }
        if (_epaisseur != other._epaisseur) { return false; }
        if (_pVegetation != other._pVegetation) { return false; }
        if (_vegetActive != other._vegetActive) { return false; }
    }
    return true;
}

bool TYSol::operator!=(const TYSol& other) const
{
    return !operator==(other);
}

bool TYSol::deepCopy(const TYElement* pOther, bool copyId /*=true*/)
{
    if (!TYElement::deepCopy(pOther, copyId)) { return false; }

    TYSol* pOtherSol = (TYSol*) pOther;

    _resistivite = pOtherSol->_resistivite;
    _epaisseur = pOtherSol->_epaisseur;
    _vegetActive = pOtherSol->_vegetActive;

    if (_pVegetation)
    {
        _pVegetation->deepCopy(pOtherSol->_pVegetation, copyId);
    }

    return true;
}

std::string TYSol::toString() const
{
    return "TYSol";
}

DOM_Element TYSol::toXML(DOM_Element& domElement)
{
    DOM_Element domNewElem = TYElement::toXML(domElement);

    TYXMLTools::addElementDoubleValue(domNewElem, "resistivite", _resistivite);
    TYXMLTools::addElementDoubleValue(domNewElem, "epaisseur", _epaisseur);
    TYXMLTools::addElementIntValue(domNewElem, "active", _vegetActive);

    if (_vegetActive && _pVegetation) { _pVegetation->toXML(domNewElem); }

    return domNewElem;
}

int TYSol::fromXML(DOM_Element domElement)
{
    TYElement::fromXML(domElement);

    bool resistiviteOk = false;
    bool epaisseurOk = false;
    bool activeOk = false;
    bool bVegetDone = false;

    if (_pVegetation) { delete _pVegetation; }
    _pVegetation = new TYVegetation();

    DOM_Element elemCur;
    QDomNodeList childs = domElement.childNodes();
    for (unsigned int i = 0; i < childs.length(); i++)
    {
        elemCur = childs.item(i).toElement();

        TYXMLTools::getElementDoubleValue(elemCur, "resistivite", _resistivite, resistiviteOk);
        TYXMLTools::getElementDoubleValue(elemCur, "epaisseur", _epaisseur, epaisseurOk);
        TYXMLTools::getElementBoolValue(elemCur, "active", _vegetActive, activeOk);

        if (_vegetActive && !bVegetDone)
        {
            bVegetDone = _pVegetation->callFromXMLIfEqual(elemCur);
        }
    }

    if (!bVegetDone && _pVegetation)
    {
        delete _pVegetation;
        _pVegetation = NULL;
    }

    return 1;
}

TYVegetation* TYSol::useVegetation(bool state /*= true*/)
{
    _vegetActive = state;

    if (_vegetActive)
    {
        _pVegetation = new TYVegetation();
        _pVegetation->setParent(this);
    }
    else
    {
        if (_pVegetation) { delete _pVegetation; }
        _pVegetation = NULL;
    }

    return _pVegetation;
}
