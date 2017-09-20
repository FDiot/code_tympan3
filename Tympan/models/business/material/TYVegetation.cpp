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

#if TY_USE_IHM
#include "Tympan/gui/widgets/TYVegetationWidget.h"
#endif

#include "Tympan/core/logging.h"

#include "TYVegetation.h"

TY_EXTENSION_INST(TYVegetation);

QString TYVegetation::_vegeName[] = { "pine", "spruce", "birch", "aspen", "oak" };

TYVegetation::TYVegetation() : _bFoliage(false), _hauteur(10.)
{
    //_name = TYNameManager::get()->generateName(getClassName());
    _name = _vegeName[0];

    _pSpectreAtt = new TYSpectre();
    _pSpectreAtt->setType(SPECTRE_TYPE_ATT);
    _pSpectreAtt->setParent(this);
}

TYVegetation::TYVegetation(const TYVegetation& other)
{
    *this = other;
}

TYVegetation::~TYVegetation()
{
    delete(_pSpectreAtt);
    _pSpectreAtt = NULL;
}

TYVegetation& TYVegetation::operator=(const TYVegetation& other)
{
    if (this != &other)
    {
        TYElement::operator =(other);
        _pSpectreAtt = other._pSpectreAtt;
        _hauteur = other._hauteur;
        _bFoliage = other._bFoliage;
    }
    return *this;
}

bool TYVegetation::operator==(const TYVegetation& other) const
{
    if (this != &other)
    {
        if (TYElement::operator !=(other)) { return false; }
        if (_pSpectreAtt != other._pSpectreAtt) { return false; }
        if (_hauteur != other._hauteur) { return false; }
        if (_bFoliage != other._bFoliage) { return false; }
    }
    return true;
}

bool TYVegetation::operator!=(const TYVegetation& other) const
{
    return !operator==(other);
}

bool TYVegetation::deepCopy(const TYElement* pOther, bool copyId /*=true*/)
{
    if (!TYElement::deepCopy(pOther, copyId)) { return false; }

    TYVegetation* pOtherVeget = (TYVegetation*) pOther;

    _hauteur = pOtherVeget->_hauteur;
    _bFoliage = pOtherVeget->_bFoliage;

    _pSpectreAtt->deepCopy(pOtherVeget->_pSpectreAtt, copyId);

    return true;
}

std::string TYVegetation::toString() const
{
    return "TYVegetation";
}

DOM_Element TYVegetation::toXML(DOM_Element& domElement)
{
    DOM_Element domNewElem = TYElement::toXML(domElement);

    TYXMLTools::addElementBoolValue(domNewElem, "foliage", _bFoliage);
    TYXMLTools::addElementDoubleValue(domNewElem, "hauteur", _hauteur);
    _pSpectreAtt->toXML(domNewElem);

    return domNewElem;
}

int TYVegetation::fromXML(DOM_Element domElement)
{
    TYElement::fromXML(domElement);

    bool hauteurOk = false, foliageOK = false;
    DOM_Element elemCur;
    QDomNodeList childs = domElement.childNodes();
    for (unsigned int i = 0; i < childs.length(); i++)
    {
        elemCur = childs.item(i).toElement();
        TYXMLTools::getElementBoolValue(elemCur, "foliage", _bFoliage, foliageOK);
        TYXMLTools::getElementDoubleValue(elemCur, "hauteur", _hauteur, hauteurOk);
        _pSpectreAtt->callFromXMLIfEqual(elemCur);
    }

    return 1;
}

void TYVegetation::setSpectreAtt(TYSpectre* pAtt)
{
    _pSpectreAtt = pAtt;
    _pSpectreAtt->setParent(this);

    if (_pSpectreAtt->getEtat() != SPECTRE_ETAT_LIN) { *_pSpectreAtt = _pSpectreAtt->toGPhy(); } //Passage en lineaire si necessaire
}

unsigned int TYVegetation::getIndexVegetation(const QString &vegeName)
{
    unsigned int retValue = 0;

    for (unsigned int i=0; i<5; i++)
    {
        if ( vegeName == _vegeName[i] ) { return i; }
    }

    return retValue;
}
