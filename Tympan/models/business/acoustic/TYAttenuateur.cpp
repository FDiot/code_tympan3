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
#include "Tympan/gui/widgets/TYAttenuateurWidget.h"
#endif


#include "Tympan/core/logging.h"


#include "TYAttenuateur.h"

TY_EXTENSION_INST(TYAttenuateur);

TYAttenuateur::TYAttenuateur()
{
    _name = TYNameManager::get()->generateName(getClassName());

    _type = 1;

    _spectreAtt.setType(SPECTRE_TYPE_ATT); // Explicitement spectre d'attenuation
    _spectreAtt.setDefaultValue(0.0);
    _spectreAtt.setParent(this);
}

TYAttenuateur::TYAttenuateur(const TYAttenuateur& other)
{
    *this = other;
}

TYAttenuateur::~TYAttenuateur()
{
}

TYAttenuateur& TYAttenuateur::operator=(const TYAttenuateur& other)
{
    if (this != &other)
    {
        TYElement::operator =(other);
        _type = other._type;
        _spectreAtt = other._spectreAtt;
    }
    return *this;
}

bool TYAttenuateur::operator==(const TYAttenuateur& other) const
{
    if (this != &other)
    {
        if (TYElement::operator !=(other)) { return false; }
        if (_type != other._type) { return false; }
        if (_spectreAtt != other._spectreAtt) { return false; }
    }
    return true;
}

bool TYAttenuateur::operator!=(const TYAttenuateur& other) const
{
    return !operator==(other);
}

bool TYAttenuateur::deepCopy(const TYElement* pOther, bool copyId /*=true*/)
{
    if (!pOther) { return false; }
    if (!TYElement::deepCopy(pOther, copyId)) { return false; }

    TYAttenuateur* pOtherAtt = (TYAttenuateur*) pOther;

    _type = pOtherAtt->_type;

    _spectreAtt.deepCopy(&pOtherAtt->_spectreAtt, copyId);

    return true;
}

std::string TYAttenuateur::toString() const
{
    return "TYAttenuateur";
}

DOM_Element TYAttenuateur::toXML(DOM_Element& domElement)
{
    DOM_Element domNewElem = TYElement::toXML(domElement);

    TYXMLTools::addElementIntValue(domNewElem, "type", _type);

    _spectreAtt.toXML(domNewElem);

    return domNewElem;
}

int TYAttenuateur::fromXML(DOM_Element domElement)
{
    TYElement::fromXML(domElement);

    bool typeOk = false;
    DOM_Element elemCur;

    QDomNodeList childs = domElement.childNodes();

    for (unsigned int i = 0; i < childs.length(); i++)
    {
        elemCur = childs.item(i).toElement();

        TYXMLTools::getElementIntValue(elemCur, "type", _type, typeOk);

        _spectreAtt.callFromXMLIfEqual(elemCur);
    }

    return 1;
}
