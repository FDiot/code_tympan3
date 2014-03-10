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
#include "Tympan/GraphicIHM/DataManagerIHM/TYTerrainWidget.h"
#endif

#ifdef TYMPAN_USE_PRECOMPILED_HEADER
#include "Tympan/MetierSolver/DataManagerMetier/TYPHMetier.h"
#endif // TYMPAN_USE_PRECOMPILED_HEADER

#include "Tympan/Tools/OMessageManager.h"


OPROTOINST(TYTerrain);
TY_EXTENSION_INST(TYTerrain);

TYTerrain::TYTerrain()
{
    _name = TYNameManager::get()->generateName(getClassName());

    //  _type = 1;

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
        //      _type = other._type;
        _pSol = other._pSol;
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
        //      if ( _type != other._type ) return false;
        if (_pSol != other._pSol) { return false; }
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

    //  _type = pOtherTerrain->_type;

    _pSol->deepCopy(pOtherTerrain->_pSol, copyId);

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

    //  TYXMLTools::addElementIntValue(domNewElem, "type", _type);
    _pSol->toXML(domNewElem);
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

    bool typeOk = false;
    bool nbPointsOk = false;
    int nbPoints = 0;
    TYPoint pt;
    DOM_Element elemCur;

    QDomNodeList childs = domElement.childNodes();
    for (unsigned int i = 0; i < childs.length(); i++)
    {
        elemCur = childs.item(i).toElement();
        //      TYXMLTools::getElementIntValue(elemCur, "type", _type, typeOk);

        _pSol->callFromXMLIfEqual(elemCur);

        TYXMLTools::getElementIntValue(elemCur, "nbPoints", nbPoints, nbPointsOk);
        if (nbPointsOk)
        {
            _listPoints.reserve(nbPoints);
        }

        if (pt.callFromXMLIfEqual(elemCur))
        {
            _listPoints.push_back(pt);
        }
    }

    return 1;
}

double TYTerrain::surface()
{
    TYPolygon polygon(_listPoints);
    return polygon.surface();
}
