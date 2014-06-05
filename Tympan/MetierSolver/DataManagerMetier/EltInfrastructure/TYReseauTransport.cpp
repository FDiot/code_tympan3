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
#include "Tympan/GraphicIHM/DataManagerIHM/TYReseauTransportWidget.h"
#include "Tympan/GraphicIHM/DataManagerGraphic/TYReseauTransportGraphic.h"
#endif

#ifdef TYMPAN_USE_PRECOMPILED_HEADER
#include "Tympan/MetierSolver/DataManagerMetier/TYPHMetier.h"
#endif // TYMPAN_USE_PRECOMPILED_HEADER

#include "TYReseauTransport.h"
#include "Tympan/Tools/OMessageManager.h"


TY_EXTENSION_INST(TYReseauTransport);
TY_EXT_GRAPHIC_INST(TYReseauTransport);

TYReseauTransport::TYReseauTransport():
    _nbBrins(2),
    _tension(230),
    _puissance(50),
    _hauteurMoyenne(2)
{
    _name = TYNameManager::get()->generateName(getClassName());

    float r = 0.0f, g = 0.0f, b = 0.0f;

#if TY_USE_IHM
    if (TYPreferenceManager::exists(TYDIRPREFERENCEMANAGER, "TYReseauTransportGraphicColorR"))
    {
        TYPreferenceManager::getColor(TYDIRPREFERENCEMANAGER, "TYReseauTransportGraphicColor", r, g, b);
    }
    else
    {
        TYPreferenceManager::setColor(TYDIRPREFERENCEMANAGER, "TYReseauTransportGraphicColor", r, g, b);
    }
#endif // TY_USE_IHM

    OColor color;
    color.r = r / 255;
    color.g = g / 255;
    color.b = b / 255;

    setColor(color);

    _largeur = 10.0;
}

TYReseauTransport::TYReseauTransport(const TYReseauTransport& other)
{
    *this = other;
}

TYReseauTransport::~TYReseauTransport()
{
}

TYReseauTransport& TYReseauTransport::operator=(const TYReseauTransport& other)
{
    if (this != &other)
    {
        TYAcousticLine::operator =(other);
        _nbBrins = other._nbBrins;
        _tension = other._tension;
        _puissance = other._puissance;
        _hauteurMoyenne = other._hauteurMoyenne;
    }
    return *this;
}

bool TYReseauTransport::operator==(const TYReseauTransport& other) const
{
    if (this != &other)
    {
        if (TYAcousticLine::operator !=(other)) { return false; }
        if (_nbBrins != other._nbBrins) { return false; }
        if (_tension != other._tension) { return false; }
        if (_puissance != other._puissance) { return false; }
        if (_hauteurMoyenne != other._hauteurMoyenne) { return false; }
    }
    return true;
}

bool TYReseauTransport::operator!=(const TYReseauTransport& other) const
{
    return !operator==(other);
}

bool TYReseauTransport::deepCopy(const TYElement* pOther, bool copyId /*=true*/)
{
    if (!TYAcousticLine::deepCopy(pOther, copyId)) { return false; }

    TYReseauTransport* pOtherRes = (TYReseauTransport*) pOther;

    _nbBrins = pOtherRes->_nbBrins;
    _tension = pOtherRes->_tension;
    _puissance = pOtherRes->_puissance;
    _hauteurMoyenne = pOtherRes->_hauteurMoyenne;

    return true;
}

std::string TYReseauTransport::toString() const
{
    return "TYReseauTransport";
}

DOM_Element TYReseauTransport::toXML(DOM_Element& domElement)
{
    DOM_Element domNewElem = TYAcousticLine::toXML(domElement);

    TYXMLTools::addElementIntValue(domNewElem, "nbBrins", _nbBrins);
    TYXMLTools::addElementDoubleValue(domNewElem, "tension", _tension);
    TYXMLTools::addElementDoubleValue(domNewElem, "puissance", _puissance);
    TYXMLTools::addElementDoubleValue(domNewElem, "hauteurMoyenne", _hauteurMoyenne);

    return domNewElem;
}

int TYReseauTransport::fromXML(DOM_Element domElement)
{
    TYAcousticLine::fromXML(domElement);

    bool nbBrinsOk = false;
    bool tensionOk = false;
    bool puissanceOk = false;
    bool hauteurMoyenneOk = false;
    DOM_Element elemCur;

    QDomNodeList childs = domElement.childNodes();
    for (unsigned int i = 0; i < childs.length(); i++)
    {
        elemCur = childs.item(i).toElement();
        TYXMLTools::getElementIntValue(elemCur, "nbBrins", _nbBrins, nbBrinsOk);
        TYXMLTools::getElementDoubleValue(elemCur, "tension", _tension, tensionOk);
        TYXMLTools::getElementDoubleValue(elemCur, "puissance", _puissance, puissanceOk);
        TYXMLTools::getElementDoubleValue(elemCur, "hauteurMoyenne", _hauteurMoyenne, hauteurMoyenneOk);
    }

    return 1;
}

bool TYReseauTransport::updateAcoustic(const bool& force) //force = false
{
    if (_typeDistribution == TY_PUISSANCE_CALCULEE)
    {
    }

    // Distribution des sources et affectation de la puissance acoustique
    TYAcousticLine::updateAcoustic(force);

    return true;
}

