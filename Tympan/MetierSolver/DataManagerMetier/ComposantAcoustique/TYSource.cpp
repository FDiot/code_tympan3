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
#include "Tympan/GraphicIHM/DataManagerIHM/TYSourceWidget.h"
#endif

#ifdef TYMPAN_USE_PRECOMPILED_HEADER
#include "Tympan/MetierSolver/DataManagerMetier/TYPHMetier.h"
#endif // TYMPAN_USE_PRECOMPILED_HEADER


TY_EXTENSION_INST(TYSource);

TYSource::TYSource()
{
    _name = TYNameManager::get()->generateName(getClassName());

    _pSpectre = new TYSpectre();
    _pSpectre->setType(SPECTRE_TYPE_LW);

    //float r = 255.0f, g = 0.0f, b = 0.0f;

    //#if TY_USE_IHM
    //if (TYPreferenceManager::exists(TYDIRPREFERENCEMANAGER, "SrcPonctGraphicColorR"))
    //{
    //  TYPreferenceManager::getColor(TYDIRPREFERENCEMANAGER, "SrcPonctGraphicColor", r, g, b);
    //}
    //else
    //{
    //  TYPreferenceManager::setColor(TYDIRPREFERENCEMANAGER, "SrcPonctGraphicColor", r, g, b);
    //}

    //#endif

    //// Couleur de preference.
    //setColor(OColor(r/255, g/255, b/255));
}

TYSource::TYSource(const TYSource& other)
{
    *this = other;
}

TYSource::~TYSource()
{
}

TYSource& TYSource::operator=(const TYSource& other)
{
    if (this != &other)
    {
        TYElement::operator =(other);
        //      TYColorInterface::operator =(other);
        _pSpectre = other._pSpectre;
    }
    return *this;
}

bool TYSource::operator==(const TYSource& other) const
{
    if (this != &other)
    {
        if (TYElement::operator !=(other)) { return false; }
        //      if ( TYColorInterface::operator !=(other)) return false;
        if (_pSpectre != other._pSpectre) { return false; }
    }
    return true;
}

bool TYSource::operator!=(const TYSource& other) const
{
    return !operator==(other);
}

bool TYSource::deepCopy(const TYElement* pOther, bool copyId /*=true*/)
{
    if (!TYElement::deepCopy(pOther, copyId)) { return false; }

    TYSource* pOtherSrc = (TYSource*) pOther;

    //  TYColorInterface::deepCopy((TYColorInterface*)pOtherSrc, copyId);

    _pSpectre->deepCopy(pOtherSrc->_pSpectre, copyId);

    return true;
}

std::string TYSource::toString() const
{
    return "TYSource";
}

DOM_Element TYSource::toXML(DOM_Element& domElement)
{
    DOM_Element domNewElem = TYElement::toXML(domElement);
    //  TYColorInterface::toXML(domNewElem);

    //  *_pSpectre = _pSpectre->toDB();
    //  _pSpectre->toXML(domNewElem);

    return domNewElem;
}

int TYSource::fromXML(DOM_Element domElement)
{
    TYElement::fromXML(domElement);

    //  TYColorInterface::fromXML(domElement);

    DOM_Element elemCur;
    QDomNodeList childs = domElement.childNodes();

    for (unsigned int i = 0; i < childs.length(); i++)
    {
        elemCur = childs.item(i).toElement();
        if (_pSpectre->callFromXMLIfEqual(elemCur)) { _pSpectre->setParent(this); }
    }

    return 1;
}

void TYSource::setRegime(TYSpectre& Spectre, int regime /*=-1*/)
{
    // Type Puissance
    Spectre.setType(SPECTRE_TYPE_LW);

    // Ajout
    _pSpectre = new TYSpectre(Spectre);
}
