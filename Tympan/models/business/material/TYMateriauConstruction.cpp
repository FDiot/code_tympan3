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
#include "Tympan/gui/widgets/TYMateriauConstructionWidget.h"
#endif


#include "Tympan/core/logging.h"


#include "TYMateriauConstruction.h"

TYSpectre* TYMateriauConstruction::_defSpectreTrans = NULL;
TYSpectre* TYMateriauConstruction::_defSpectreAbso = NULL;

TY_EXTENSION_INST(TYMateriauConstruction);

TYSpectre* TYMateriauConstruction::getDefSpectreTrans()
{
    if (_defSpectreTrans)
    {
        return _defSpectreTrans;
    }
    else
    {
        _defSpectreTrans = new TYSpectre();
        *_defSpectreTrans = _getDefSpectreTrans();
        return _defSpectreTrans;
    }
}

TYSpectre* TYMateriauConstruction::getDefSpectreAbso()
{
    if (_defSpectreAbso)
    {
        return _defSpectreAbso;
    }
    else
    {
        _defSpectreAbso = new TYSpectre();
        *_defSpectreAbso = _getDefSpectreAbso();
        return _defSpectreAbso;
    }
}


TYMateriauConstruction::TYMateriauConstruction()
{
    _name = TYNameManager::get()->generateName(getClassName());

    _masseVol = 2500;
    _spectreTransm = *getDefSpectreTrans();
    _spectreTransm.setType(SPECTRE_TYPE_ATT);
    _spectreTransm.setParent(this);

    _spectreAbso = *getDefSpectreAbso();
    _spectreAbso.setType(SPECTRE_TYPE_ABSO);
    _spectreAbso.setParent(this);
}

TYMateriauConstruction::TYMateriauConstruction(const TYMateriauConstruction& other)
{
    *this = other;
}

TYMateriauConstruction::~TYMateriauConstruction()
{
}

/*static*/ TYSpectre TYMateriauConstruction::_getDefSpectreTrans()
{
    TYSpectre s;
    s.setDefaultValue(15.0);

#if TY_USE_IHM
    if (TYPreferenceManager::exists(TYDIRPREFERENCEMANAGER, "SpectreTransmDefault0"))
    {
        s = *TYPreferenceManager::getSpectre(TYDIRPREFERENCEMANAGER, "SpectreTransmDefault");
    }
    else
    {
        TYPreferenceManager::setSpectre(TYDIRPREFERENCEMANAGER, "SpectreTransmDefault", &s);
    }
#endif

    return s;
}

/*static*/ TYSpectre TYMateriauConstruction::_getDefSpectreAbso()
{
    TYSpectre s;
    s.setDefaultValue(0.2);

#if TY_USE_IHM
    if (TYPreferenceManager::exists(TYDIRPREFERENCEMANAGER, "SpectreAbsoDefault0"))
    {
        s = *TYPreferenceManager::getSpectre(TYDIRPREFERENCEMANAGER, "SpectreAbsoDefault");
    }
    else
    {
        TYPreferenceManager::setSpectre(TYDIRPREFERENCEMANAGER, "SpectreAbsoDefault", &s);
    }
#endif

    return s;
}

TYMateriauConstruction& TYMateriauConstruction::operator=(const TYMateriauConstruction& other)
{
    if (this != &other)
    {
        TYElement::operator =(other);
        _masseVol = other._masseVol;
        _spectreTransm = other._spectreTransm;
        _spectreAbso = other._spectreAbso;
    }
    return *this;
}

bool TYMateriauConstruction::operator==(const TYMateriauConstruction& other) const
{
    if (this != &other)
    {
        if (TYElement::operator !=(other)) { return false; }
        if (_masseVol != other._masseVol) { return false; }
        if (_spectreTransm != other._spectreTransm) { return false; }
        if (_spectreAbso != other._spectreAbso) { return false; }
    }
    return true;
}

bool TYMateriauConstruction::operator!=(const TYMateriauConstruction& other) const
{
    return !operator==(other);
}

bool TYMateriauConstruction::deepCopy(const TYElement* pOther, bool copyId /*=true*/)
{
    if (!TYElement::deepCopy(pOther, copyId)) { return false; }

    TYMateriauConstruction* pOtherMat = (TYMateriauConstruction*) pOther;

    _masseVol = pOtherMat->_masseVol;

    _spectreTransm.deepCopy(&pOtherMat->_spectreTransm, copyId);
    _spectreAbso.deepCopy(&pOtherMat->_spectreAbso, copyId);

    return true;
}

std::string TYMateriauConstruction::toString() const
{
    return "TYMateriauConstruction";
}

DOM_Element TYMateriauConstruction::toXML(DOM_Element& domElement)
{

    DOM_Element domNewElem = TYElement::toXML(domElement);
    TYXMLTools::addElementDoubleValue(domNewElem, "masseVol", _masseVol);

    TYSpectre::setXMLPrecision(3); // sauvegarde du spectre avec 3 decimales

    _spectreTransm.toXML(domNewElem);
    _spectreAbso.toXML(domNewElem);

    TYSpectre::setXMLPrecision(); // retour a la precision par defaut

    domElement.appendChild(domNewElem);

    return domNewElem;
}

int TYMateriauConstruction::fromXML(DOM_Element domElement)
{
    TYElement::fromXML(domElement);

    bool masseVolOk = false;
    int spectreNb = 1;
    DOM_Element elemCur;
    QDomNodeList childs = domElement.childNodes();
    for (unsigned int i = 0; i < childs.length(); i++)
    {
        elemCur = childs.item(i).toElement();
        TYXMLTools::getElementDoubleValue(elemCur, "masseVol", _masseVol, masseVolOk);
        if (elemCur.nodeName() == "Spectre")
        {
            if (spectreNb == 1)
            {
                _spectreTransm.fromXML(elemCur);
            }
            else if (spectreNb == 2)
            {
                _spectreAbso.fromXML(elemCur);
            }
            spectreNb++;
        }
    }

    _spectreTransm.setType(SPECTRE_TYPE_ATT);
    _spectreAbso.setType(SPECTRE_TYPE_ABSO);

    return 1;
}

TYSpectre TYMateriauConstruction::getSpectreAbsoEyring()const
{
    TYSpectre Eyring(_spectreAbso);

    // 1-e^(-alphaS)
    Eyring = Eyring.exp(-1.0).mult(-1.0).sum(1.0);

    return Eyring;
}

