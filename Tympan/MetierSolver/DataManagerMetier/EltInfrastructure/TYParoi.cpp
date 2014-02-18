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

#include "TYParoi.h"
#include "Tympan/Tools/OMessageManager.h"


OPROTOINST(TYParoi);


TYParoi::TYParoi()
{
    _name = TYNameManager::get()->generateName(getClassName());

    _epaisseur = 1;

    _matStruct = new TYMateriauConstruction();
    _matStruct->setParent(this);
    _matFace1 = new TYMateriauConstruction();
    _matFace1->setParent(this);
    _matFace2 = new TYMateriauConstruction();
    _matFace2->setParent(this);
}

TYParoi::TYParoi(const TYParoi& other)
{
    *this = other;
}

TYParoi::~TYParoi()
{
}

TYParoi& TYParoi::operator=(const TYParoi& other)
{
    if (this != &other)
    {
        TYElement::operator =(other);
        _epaisseur = other._epaisseur;
        _matStruct = other._matStruct;
        _matFace1 = other._matFace1;
        _matFace2 = other._matFace2;
    }
    return *this;
}

bool TYParoi::operator==(const TYParoi& other) const
{
    if (this != &other)
    {
        if (TYElement::operator !=(other)) { return false; }
        if (_epaisseur != other._epaisseur) { return false; }
        if (_matStruct != other._matStruct) { return false; }
        if (_matFace1 != other._matFace1) { return false; }
        if (_matFace2 != other._matFace2) { return false; }
    }
    return true;
}

bool TYParoi::operator!=(const TYParoi& other) const
{
    return !operator==(other);
}

bool TYParoi::deepCopy(const TYElement* pOther, bool copyId /*=true*/)
{
    if (!TYElement::deepCopy(pOther, copyId)) { return false; }

    TYParoi* pOtherParoi = (TYParoi*) pOther;

    _epaisseur = pOtherParoi->_epaisseur;

    _matStruct->deepCopy(pOtherParoi->_matStruct, copyId);
    _matFace1->deepCopy(pOtherParoi->_matFace1, copyId);
    _matFace2->deepCopy(pOtherParoi->_matFace2, copyId);

    return true;
}

std::string TYParoi::toString() const
{
    return "TYParoi";
}

DOM_Element TYParoi::toXML(DOM_Element& domElement)
{
    DOM_Element domNewElem = TYElement::toXML(domElement);

    TYXMLTools::addElementDoubleValue(domNewElem, "epaisseur", _epaisseur);

    _matStruct->toXML(domNewElem);
    _matFace1->toXML(domNewElem);
    _matFace2->toXML(domNewElem);

    return domNewElem;
}

int TYParoi::fromXML(DOM_Element domElement)
{
    TYElement::fromXML(domElement);

    bool epaisseurOk = false;
    bool matStructFound = false;
    bool matFace1Found = false;
    bool matFace2Found = false;

    DOM_Element elemCur;

    QDomNodeList childs = domElement.childNodes();
    for (unsigned int i = 0; i < childs.length(); i++)
    {
        elemCur = childs.item(i).toElement();
        TYXMLTools::getElementDoubleValue(elemCur, "epaisseur", _epaisseur, epaisseurOk);

        if (!matStructFound)
        {
            matStructFound = _matStruct->callFromXMLIfEqual(elemCur);
        }
        else if (!matFace1Found)
        {
            matFace1Found = _matFace1->callFromXMLIfEqual(elemCur);
        }
        else if (!matFace2Found)
        {
            matFace2Found = _matFace2->callFromXMLIfEqual(elemCur);
        }
    }

    return 1;
}
