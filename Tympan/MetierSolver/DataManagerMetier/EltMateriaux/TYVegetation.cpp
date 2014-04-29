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
#include "Tympan/GraphicIHM/DataManagerIHM/TYVegetationWidget.h"
#endif

#ifdef TYMPAN_USE_PRECOMPILED_HEADER
#include "Tympan/MetierSolver/DataManagerMetier/TYPHMetier.h"
#endif // TYMPAN_USE_PRECOMPILED_HEADER

#include "Tympan/MetierSolver/ToolsMetier/OSegment3D.h"
#include "Tympan/Tools/OMessageManager.h"


OPROTOINST(TYVegetation);
TY_EXTENSION_INST(TYVegetation);

TYVegetation::TYVegetation()
{
    _name = TYNameManager::get()->generateName(getClassName());

    _hauteur = 10;

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

    TYXMLTools::addElementDoubleValue(domNewElem, "hauteur", _hauteur);
    _pSpectreAtt->toXML(domNewElem);

    return domNewElem;
}

int TYVegetation::fromXML(DOM_Element domElement)
{
    TYElement::fromXML(domElement);

    bool hauteurOk = false;
    DOM_Element elemCur;
    QDomNodeList childs = domElement.childNodes();
    for (unsigned int i = 0; i < childs.length(); i++)
    {
        elemCur = childs.item(i).toElement();
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

OSpectre TYVegetation::getAtt(const OSegment3D& seg, const TYAtmosphere& Atmo) const
{

    return getAtt(seg.longueur(), Atmo);
}

OSpectre TYVegetation::getAtt(const double& distance, const TYAtmosphere& Atmo) const
{
    OSpectre s;
    OSpectre sAttAtmo = Atmo.getAtt(distance); // L'absorption de l'atmosphere s'ajoute a l'absorption de la vegetation

    s.setType(SPECTRE_TYPE_ATT); // Spectre d'attenuation

    if (distance < 10.0)
    {
        s.setDefaultValue(0.0);  // Pas d'attenuation pour les distances inferieures a 10 metres
        return s;
    }

    for (unsigned int i = 0 ; i < _pSpectreAtt->getNbValues(); i++)
    {

        if (distance < 20.0)
        {
            double valeur = (TYSpectre::getTabFreqNorm()[i] < 250 ?  0.0 : 1.0);
            s.getTabValReel()[i] = valeur;
        }

        if (distance < 200.0)
        {
            s = _pSpectreAtt->mult(distance);
        }
        else
        {
            s = _pSpectreAtt->mult(200.0);
        }
    }

    s = s.toGPhy();
    s = s.mult(sAttAtmo); // Prise en compte de l'absorption atmospherique

    return s;
}
