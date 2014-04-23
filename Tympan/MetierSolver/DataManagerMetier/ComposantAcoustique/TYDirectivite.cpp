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
#include "Tympan/GraphicIHM/DataManagerIHM/TYDirectiviteWidget.h"
#endif

#ifdef TYMPAN_USE_PRECOMPILED_HEADER
#include "Tympan/MetierSolver/DataManagerMetier/TYPHMetier.h"
#endif // TYMPAN_USE_PRECOMPILED_HEADER


OPROTOINST(TYDirectivite);
TY_EXTENSION_INST(TYDirectivite);

TYDirectivite::TYDirectivite()
{
    _name = TYNameManager::get()->generateName(getClassName());
}

TYDirectivite::TYDirectivite(const TYDirectivite& other)
{
    *this = other;
}

TYDirectivite::~TYDirectivite()
{
}

TYDirectivite& TYDirectivite::operator=(const TYDirectivite& other)
{
    if (this != &other)
    {
        TYElement::operator =(other);
        _tabAnglesVal = other._tabAnglesVal;
    }
    return *this;
}

bool TYDirectivite::operator==(const TYDirectivite& other) const
{
    if (this != &other)
    {
        if (TYElement::operator!=(other)) { return false; }
        if (!(_tabAnglesVal == other._tabAnglesVal)) { return false; }
    }
    return true;
}

bool TYDirectivite::operator!=(const TYDirectivite& other) const
{
    return !operator==(other);
}

bool TYDirectivite::deepCopy(const TYElement* pOther, bool copyId /*=true*/)
{
    if (!TYElement::deepCopy(pOther, copyId)) { return false; }

    TYDirectivite* pOtherDir = (TYDirectivite*) pOther;

    _tabAnglesVal = pOtherDir->_tabAnglesVal;

    return true;
}

std::string TYDirectivite::toString() const
{
    return "TYDirectivite";
}

DOM_Element TYDirectivite::toXML(DOM_Element& domElement)
{
    DOM_Element domNewElem = TYElement::toXML(domElement);
    QDomDocument domDoc = domElement.ownerDocument();
    for (unsigned int i = 0; i < _tabAnglesVal.size(); i++)
    {
        DOM_Element domChildElem = domDoc.createElement("DirectiviteAnglesValue");
        domNewElem.appendChild(domChildElem);

        domChildElem.setAttribute("theta", doubleToStr(_tabAnglesVal.at(i)._theta).data());
        domChildElem.setAttribute("phi", doubleToStr(_tabAnglesVal.at(i)._phi).data());
        domChildElem.setAttribute("val", doubleToStr(_tabAnglesVal.at(i)._val).data());
    }

    return domNewElem;
}

int TYDirectivite::fromXML(DOM_Element domElement)
{
    TYElement::fromXML(domElement);

    // Reset
    _tabAnglesVal.clear();

    TYDirectiviteAnglesValue dirAnglesVal;
    DOM_Element elemCur;
    QDomNodeList childs = domElement.childNodes();

    for (unsigned int i = 0; i < childs.length(); i++)
    {
        elemCur = childs.item(i).toElement();

        if (elemCur.nodeName() == "DirectiviteAnglesValue")
        {
            dirAnglesVal._theta = TYXMLTools::getElementAttributeToDouble(elemCur, "theta");
            dirAnglesVal._phi = TYXMLTools::getElementAttributeToDouble(elemCur, "phi");
            dirAnglesVal._val = TYXMLTools::getElementAttributeToDouble(elemCur, "val");

            _tabAnglesVal.push_back(dirAnglesVal);
        }
    }

    return 1;
}

inline double TYDirectivite::calcDeriveSeconde(const int& i) const
{
    double dYdroite = 0.0;
    double dYgauche = 0.0;
    const int max = static_cast<int>(_tabAnglesVal.size()) - 1;

    if (_tabAnglesVal.size() >= 3) { return 0.0; }

    if (i == 0)
    {
        dYdroite = (_tabAnglesVal[i + 1]._val - _tabAnglesVal[i]._val) /
                   (_tabAnglesVal[i + 1]._theta - _tabAnglesVal[i]._theta);
        dYgauche = (_tabAnglesVal[i]._val - _tabAnglesVal[max]._val) /
                   (_tabAnglesVal[i]._theta - _tabAnglesVal[max]._theta) ;
    }
    else if (i == max)
    {
        dYdroite = (_tabAnglesVal[0]._val - _tabAnglesVal[i]._val) /
                   (_tabAnglesVal[0]._theta - _tabAnglesVal[i]._theta);
        dYgauche = (_tabAnglesVal[i]._val - _tabAnglesVal[i - 1]._val) /
                   (_tabAnglesVal[i]._theta - _tabAnglesVal[i - 1]._theta) ;
    }
    else
    {
        dYdroite = (_tabAnglesVal[i + 1]._val - _tabAnglesVal[i]._val) /
                   (_tabAnglesVal[i + 1]._theta - _tabAnglesVal[i]._theta);
        dYgauche = (_tabAnglesVal[i]._val - _tabAnglesVal[i - 1]._val) /
                   (_tabAnglesVal[i]._theta - _tabAnglesVal[i - 1]._theta) ;
    }

    return dYdroite - dYgauche;
}

