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


#include "TYUserSrcRegime.h"


TYUserSrcRegime::TYUserSrcRegime()
{
    _name = TYNameManager::get()->generateName(getClassName());

    _typeDistribution = TY_PUISSANCE_IMPOSEE;
    _pAtt = NULL;
    _pDirectivite = NULL;
}

TYUserSrcRegime::TYUserSrcRegime(const TYUserSrcRegime& other)
{
    *this = other;
}

TYUserSrcRegime::~TYUserSrcRegime()
{
    _pAtt = NULL;
    _pDirectivite = NULL;
}

TYUserSrcRegime& TYUserSrcRegime::operator=(const TYUserSrcRegime& other)
{
    if (this != &other)
    {
        TYRegime::operator =(other);
        if (other._pDirectivite != NULL)
        {
            _pDirectivite = new TYDirectivity(*other._pDirectivite);
        }
        else
        {
            _pDirectivite = NULL;
        }
    }
    return *this;
}

bool TYUserSrcRegime::operator==(const TYUserSrcRegime& other) const
{
    if (this != &other)
    {
        if (TYRegime::operator!=(other)) { return false; }
        if (_pDirectivite != other._pDirectivite) { return false; }
    }
    return true;
}

bool TYUserSrcRegime::operator!=(const TYUserSrcRegime& other) const
{
    return !operator==(other);
}

bool TYUserSrcRegime::deepCopy(const TYElement* pOther, bool copyId /*=true*/)
{
    if (!TYRegime::deepCopy(pOther, copyId)) { return false; }

    //  TYUserSrcRegime * pOtherRegime = (TYUserSrcRegime *) pOther;

    if (((TYUserSrcRegime*) pOther)->_pDirectivite != NULL)
    {
        _pDirectivite = new TYDirectivity();
        _pDirectivite->deepCopy(((TYUserSrcRegime*) pOther)->_pAtt, copyId);
    }
    else
    {
        _pDirectivite = NULL;
    }

    return true;
}

std::string TYUserSrcRegime::toString() const
{
    return "TYUserSrcRegime";
}

DOM_Element TYUserSrcRegime::toXML(DOM_Element& domElement)
{
    DOM_Element domNewElem = TYRegime::toXML(domElement);


    if (_pDirectivite) { _pDirectivite->toXML(domNewElem); }

    return domNewElem;
}

int TYUserSrcRegime::fromXML(DOM_Element domElement)
{
    TYRegime::fromXML(domElement);

    DOM_Element elemCur;

    QDomNodeList childs = domElement.childNodes();

    LPTYDirectivity pDir = new TYDirectivity();

    for (unsigned int i = 0; i < childs.length(); i++)
    {
        elemCur = childs.item(i).toElement();
        if (pDir->callFromXMLIfEqual(elemCur))
        {
            _pDirectivite = pDir;
        }
    }

    return 1;
}
