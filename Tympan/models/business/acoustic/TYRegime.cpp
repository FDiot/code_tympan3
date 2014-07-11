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
#include "Tympan/GraphicIHM/DataManagerIHM/TYRegimeWidget.h"
#endif

#ifdef TYMPAN_USE_PRECOMPILED_HEADER
#include "Tympan/models/business/TYPHMetier.h"
#endif // TYMPAN_USE_PRECOMPILED_HEADER

TY_EXTENSION_INST(TYRegime)

TYRegime::TYRegime() :  _isRayonnant(true),
    _useAtt(false),
    _typeDistribution(0)
{
    _name = TYNameManager::get()->generateName(getClassName());

    _spectre.setType(SPECTRE_TYPE_LW);
    _pAtt = NULL;
}

TYRegime::TYRegime(const TYRegime& other)
{
    *this = other;
}

TYRegime::~TYRegime()
{
    _pAtt = NULL;
}

TYRegime& TYRegime::operator=(const TYRegime& other)
{
    if (this != &other)
    {
        TYElement::operator =(other);
        _isRayonnant = other._isRayonnant;
        _useAtt = other._useAtt;
        _typeDistribution = other._typeDistribution;
        _spectre = other._spectre;  // Recopie du contenu du spectre

        // Recopie de l'attenuateur s'il existe
        if (other._pAtt)
        {
            _pAtt = new TYAttenuateur();
            _pAtt->deepCopy(other._pAtt);
        }
    }
    return *this;
}

bool TYRegime::operator==(const TYRegime& other) const
{
    if (this != &other)
    {
        if (TYElement::operator!=(other)) { return false; }
        if (_isRayonnant != other._isRayonnant) { return false; }
        if (_useAtt != other._useAtt) { return false; }
        if (_spectre != other._spectre) { return false; }
        if (_pAtt != other._pAtt) { return false; }
    }
    return true;
}

bool TYRegime::operator!=(const TYRegime& other) const
{
    return !operator==(other);
}

bool TYRegime::deepCopy(const TYElement* pOther, bool copyId /*=true*/)
{
    if (!TYElement::deepCopy(pOther, copyId)) { return false; }

    //  TYRegime * pOtherRegime = (TYRegime *) pOther;

    _isRayonnant = ((TYRegime*)(pOther))->_isRayonnant;
    _useAtt = ((TYRegime*)(pOther))->_useAtt;
    _typeDistribution = ((TYRegime*)(pOther))->_typeDistribution;
    _spectre.deepCopy(&((TYRegime*)(pOther))->_spectre, copyId);    // Recopie du contenu du spectre

    if (((TYRegime*)(pOther))->_pAtt)
    {
        _pAtt = new TYAttenuateur();
        _pAtt->deepCopy(((TYRegime*)(pOther))->_pAtt, copyId);
    }

    return true;
}

std::string TYRegime::toString() const
{
    return "TYRegime";
}

DOM_Element TYRegime::toXML(DOM_Element& domElement)
{
    DOM_Element domNewElem = TYElement::toXML(domElement);

    TYXMLTools::addElementIntValue(domNewElem, "isRayonnant", _isRayonnant);
    TYXMLTools::addElementIntValue(domNewElem, "useAtt", _useAtt);
    TYXMLTools::addElementIntValue(domNewElem, "typeDistribution", _typeDistribution);

    if (_typeDistribution == TY_PUISSANCE_IMPOSEE) { _spectre.toXML(domNewElem); }

    if (_useAtt && (_pAtt != NULL)) { _pAtt->toXML(domNewElem); }

    return domNewElem;
}

int TYRegime::fromXML(DOM_Element domElement)
{
    TYElement::fromXML(domElement);

    DOM_Element elemCur;

    QDomNodeList childs = domElement.childNodes();

    bool typeDistributionOk = false;
    bool useAttOk = false;
    bool isRayonnantOk = false;

    LPTYAttenuateur pTempAtt = new TYAttenuateur();

    for (unsigned int i = 0; i < childs.length(); i++)
    {
        elemCur = childs.item(i).toElement();

        TYXMLTools::getElementBoolValue(elemCur, "isRayonnant", _isRayonnant, isRayonnantOk);
        TYXMLTools::getElementBoolValue(elemCur, "useAtt", _useAtt, useAttOk);
        TYXMLTools::getElementIntValue(elemCur, "typeDistribution", _typeDistribution, typeDistributionOk);

        _spectre.callFromXMLIfEqual(elemCur); // Relecture du spectre s'il a ete enregistre

        if (pTempAtt->callFromXMLIfEqual(elemCur))
        {
            _pAtt = pTempAtt;
        }
    }
    return 1;
}
