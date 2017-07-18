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
#include "Tympan/models/business/material/TYSol.h"
#include "TYTerrain.h"

#if TY_USE_IHM
  #include "Tympan/models/business/TYPreferenceManager.h"
  #include "Tympan/gui/widgets/TYTerrainWidget.h"
  #include "Tympan/gui/gl/TYTerrainGraphic.h"
#endif


TY_EXTENSION_INST(TYTerrain);
TY_EXT_GRAPHIC_INST(TYTerrain);

TYTerrain::TYTerrain() : _vegetActive(false), _pVegetation(nullptr)
{
    _name = TYNameManager::get()->generateName(getClassName());

    _pSol = new TYSol();
    _pSol->setParent(this);

    float r = 0.0f, g = 85.0f, b = 0.0f;

#if TY_USE_IHM
    if (TYPreferenceManager::exists(TYDIRPREFERENCEMANAGER, "TYTerrainGraphicColorR"))
    {
        TYPreferenceManager::getColor(TYDIRPREFERENCEMANAGER, "TYTerrainGraphicColor", r, g, b);
    }
    else
    {
        TYPreferenceManager::setColor(TYDIRPREFERENCEMANAGER, "TYTerrainGraphicColor", r, g, b);
    }
#endif // TY_USE_IHM

    OColor color;
    color.r = r / 255;
    color.g = g / 255;
    color.b = b / 255;

    setColor(color);
}

TYTerrain::TYTerrain(const TYTerrain& other)
{
    *this = other;
}

TYTerrain::~TYTerrain()
{
}

TYTerrain& TYTerrain::operator=(const TYTerrain& other)
{
    if (this != &other)
    {
        TYElement::operator =(other);
        TYColorInterface::operator =(other);
        _pSol = other._pSol;
        _vegetActive = other._vegetActive;
        _pVegetation = other._pVegetation;
        _listPoints = other._listPoints;
    }
    return *this;
}

bool TYTerrain::operator==(const TYTerrain& other) const
{
    if (this != &other)
    {
        if (TYElement::operator !=(other)) { return false; }
        if (TYColorInterface::operator !=(other)) { return false; }
        if (_pSol != other._pSol) { return false; }
        if (_pVegetation != other._pVegetation) { return false; }
        if (_vegetActive != other._vegetActive) { return false; }
        if (!(_listPoints == other._listPoints)) { return false; }
    }
    return true;
}

bool TYTerrain::operator!=(const TYTerrain& other) const
{
    return !operator==(other);
}

bool TYTerrain::deepCopy(const TYElement* pOther, bool copyId /*=true*/)
{
    if (!TYElement::deepCopy(pOther, copyId)) { return false; }

    TYColorInterface::deepCopy((TYColorInterface*)pOther, copyId);

    TYTerrain* pOtherTerrain = (TYTerrain*) pOther;

    _pSol->deepCopy(pOtherTerrain->_pSol, copyId);

    _vegetActive = pOtherTerrain->_vegetActive;

    if (_pVegetation)
    {
        _pVegetation->deepCopy(pOtherTerrain->_pVegetation, copyId);
    }

    _listPoints.clear();
    for (unsigned int i = 0; i < pOtherTerrain->_listPoints.size(); i++)
    {
        TYPoint pt;
        pt.deepCopy(&pOtherTerrain->_listPoints[i], copyId);
        _listPoints.push_back(pt);
    }

    return true;
}

std::string TYTerrain::toString() const
{
    return "TYTerrain";
}

DOM_Element TYTerrain::toXML(DOM_Element& domElement)
{
    DOM_Element domNewElem = TYElement::toXML(domElement);

    TYColorInterface::toXML(domNewElem);

    _pSol->toXML(domNewElem);

    TYXMLTools::addElementIntValue(domNewElem, "active", _vegetActive);

    if (_vegetActive && _pVegetation) { _pVegetation->toXML(domNewElem); }

    TYXMLTools::addElementUIntValue(domNewElem, "nbPoints", _listPoints.size());

    for (unsigned int i = 0; i < _listPoints.size(); i++)
    {
        _listPoints.at(i).toXML(domNewElem);
    }

    return domNewElem;
}

int TYTerrain::fromXML(DOM_Element domElement)
{
    TYElement::fromXML(domElement);

    TYColorInterface::fromXML(domElement);

    _listPoints.clear();
    _vegetActive = false;
    if (_pVegetation) { _pVegetation = nullptr; }
    
    // A new vegetation is created in case of vegetation reading
    // will be destroyed if not used
    _pVegetation = new TYVegetation();

    bool nbPointsOk = false;
    bool bVegetDone = false;
    bool activeOk = false;

    int nbPoints = 0;
    TYPoint pt;
    DOM_Element elemCur;

    TYUUID currentId = this->getID();
    if (TYElement::testId(currentId, this))
    {
        this->regenerateID();
    }

    QDomNodeList childs = domElement.childNodes();
    for (unsigned int i = 0; i < childs.length(); i++)
    {
        elemCur = childs.item(i).toElement();

        _pSol->callFromXMLIfEqual(elemCur);

        TYXMLTools::getElementBoolValue(elemCur, "active", _vegetActive, activeOk);

        if (_vegetActive && !bVegetDone)
        {
            bVegetDone = _pVegetation->callFromXMLIfEqual(elemCur);
        }

        TYXMLTools::getElementIntValue(elemCur, "nbPoints", nbPoints, nbPointsOk);

        if (pt.callFromXMLIfEqual(elemCur))
        {
            _listPoints.push_back(pt);
        }
    }

    if (!_vegetActive && _pVegetation)
    {
        _pVegetation = nullptr;
    }

    return 1;
}

double TYTerrain::surface()
{
    TYPolygon polygon(_listPoints);
    return polygon.surface();
}

void TYTerrain::useVegetation(bool state /*= true*/)
{
    _vegetActive = state;

    if (_vegetActive)
    {
        if (_pVegetation == nullptr)
        {
            _pVegetation = new TYVegetation();
            _pVegetation->setParent(this);
        }
    }
    else
    {
        if (_pVegetation) { _pVegetation = nullptr; }
    }
}

void TYTerrain::setVegetation(LPTYVegetation pVeget) 
{ 
    _pVegetation = pVeget;

    if (_pVegetation != nullptr)
    {
        _vegetActive = true;
        _pVegetation->setParent(this);
    }
    else
    {
        _vegetActive = false;
    }
}
