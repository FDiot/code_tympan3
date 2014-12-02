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




#include "Tympan/models/common/3d.h"

#include "Tympan/core/logging.h"


#include "TYSourceBafflee.h"
TYSourceBafflee::TYSourceBafflee()
{
    _name = TYNameManager::get()->generateName(getClassName());

    _type = TypeBouche;

    _pAcousticRectangle = new TYRectangle();
}

TYSourceBafflee::TYSourceBafflee(const TYSourceBafflee& other)
{
    *this = other;
}

TYSourceBafflee::~TYSourceBafflee()
{
}

TYSourceBafflee& TYSourceBafflee::operator=(const TYSourceBafflee& other)
{
    if (this != &other)
    {
        TYSourcePonctuelle::operator =(other);
        _pAcousticRectangle = other._pAcousticRectangle;
    }
    return *this;
}

bool TYSourceBafflee::operator==(const TYSourceBafflee& other) const
{
    if (this != &other)
    {
        if (TYSourcePonctuelle::operator !=(other)) { return false; }
        if (_pAcousticRectangle != other._pAcousticRectangle) { return false; }
    }
    return true;
}

bool TYSourceBafflee::operator!=(const TYSourceBafflee& other) const
{
    return !operator==(other);
}

bool TYSourceBafflee::deepCopy(const TYElement* pOther, bool copyId /*=true*/)
{
    if (!TYSource::deepCopy(pOther, copyId)) { return false; }

    TYSourceBafflee* pOtherSrc = (TYSourceBafflee*) pOther;

    if (_pAcousticRectangle && pOtherSrc->_pAcousticRectangle)
    {
        _pAcousticRectangle->deepCopy(pOtherSrc->_pAcousticRectangle, copyId);
    }
    else
    {
        _pAcousticRectangle = NULL;
    }

    return true;
}

std::string TYSourceBafflee::toString() const
{
    return "TYSourceBafflee";
}

DOM_Element TYSourceBafflee::toXML(DOM_Element& domElement)
{
    DOM_Element domNewElem = TYSourcePonctuelle::toXML(domElement);

    if (_pAcousticRectangle)
    {
        // Mise en commentaire DT20061228 - car cette ligne devrait etre inutile
        //      TYXMLTools::addElementStringValue(domNewElem, "accRectId", _pAcousticRectangle->getID().toString().c_str());
    }

    return domNewElem;
}

int TYSourceBafflee::fromXML(DOM_Element domElement)
{
    TYSourcePonctuelle::fromXML(domElement);

    _pAcousticRectangle = NULL;

    QString id;
    DOM_Element elemCur;

    QDomNodeList childs = domElement.childNodes();

    for (unsigned int i = 0; i < childs.length(); i++)
    {
        elemCur = childs.item(i).toElement();

        if (TYXMLTools::getElementStringValue(elemCur, "accRectId", id))
        {
            _pAcousticRectangle = TYRectangle::safeDownCast(TYElement::getInstance(id));
        }
    }

    return 1;
}

void TYSourceBafflee::setAcousticRectangle(LPTYRectangle pAcousticRectangle)
{

    _pAcousticRectangle = pAcousticRectangle;
    _pAcousticRectangle->setParent(this);
    setIsAcousticModified(true);
}

