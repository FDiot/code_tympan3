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
#include "Tympan/gui/widgets/TYPlanEauWidget.h"
#include "Tympan/gui/gl/TYPlanEauGraphic.h"
#endif


#include "Tympan/core/logging.h"



TY_EXTENSION_INST(TYPlanEau);
TY_EXT_GRAPHIC_INST(TYPlanEau);

TYPlanEau::TYPlanEau()
{
    _name = TYNameManager::get()->generateName(getClassName());

    // Couleur par default
    float r = 0.0, g = 0.0, b = 255.0;

#if TY_USE_IHM
    if (TYPreferenceManager::exists(TYDIRPREFERENCEMANAGER, "TYPlanEauGraphicColorR"))
    {
        TYPreferenceManager::getColor(TYDIRPREFERENCEMANAGER, "TYPlanEauGraphicColor", r, g, b);
    }
    else
    {
        TYPreferenceManager::setColor(TYDIRPREFERENCEMANAGER, "TYPlanEauGraphicColor", r, g, b);
    }
#endif

    setColor(OColor(r / 255, g / 255, b / 255));

    _pCrbNiv = new TYCourbeNiveau();
    _pCrbNiv->setParent(this);

    _pSol->setEpaisseur(1);
    _pSol->setMiroir(true);
}

TYPlanEau::TYPlanEau(const TYPlanEau& other)
{
    *this = other;
}

TYPlanEau::TYPlanEau(const TYTabPoint& pts, double alt)
{
    _pCrbNiv = new TYCourbeNiveau();
    _pCrbNiv->setListPoints(pts);
    _pCrbNiv->setAltitude(alt);
    _pSol->setMiroir(true);
}

TYPlanEau::~TYPlanEau()
{
}

TYPlanEau& TYPlanEau::operator=(const TYPlanEau& other)
{
    if (this != &other)
    {
        TYTerrain::operator =(other);
        _pCrbNiv = other._pCrbNiv;
    }
    return *this;
}

bool TYPlanEau::operator==(const TYPlanEau& other) const
{
    if (this != &other)
    {
        if (TYTerrain::operator !=(other)) { return false; }
        if (_pCrbNiv != other._pCrbNiv) { return false; }
    }
    return true;
}

bool TYPlanEau::operator!=(const TYPlanEau& other) const
{
    return !operator==(other);
}

bool TYPlanEau::deepCopy(const TYElement* pOther, bool copyId /*=true*/)
{
    if (!TYTerrain::deepCopy(pOther, copyId)) { return false; }

    TYPlanEau* pOtherPlanEau = (TYPlanEau*) pOther;

    _pCrbNiv->deepCopy(pOtherPlanEau->_pCrbNiv, copyId);

    return true;
}

std::string TYPlanEau::toString() const
{
    return "TYPlanEau";
}

DOM_Element TYPlanEau::toXML(DOM_Element& domElement)
{
    DOM_Element domNewElem = TYTerrain::toXML(domElement);

    _pCrbNiv->toXML(domNewElem);

    return domNewElem;
}

int TYPlanEau::fromXML(DOM_Element domElement)
{
    TYTerrain::fromXML(domElement);

    DOM_Element elemCur;

    QDomNodeList childs = domElement.childNodes();
    for (unsigned int i = 0; i < childs.length(); i++)
    {
        elemCur = childs.item(i).toElement();
        _pCrbNiv->callFromXMLIfEqual(elemCur);
    }

    // On force le "sol" comme reflechissant pour les fichiers anciens
    _pSol->setMiroir(true);

    return 1;
}

void TYPlanEau::setIsGeometryModified(bool isModified)
{
    TYElement::setIsGeometryModified(isModified);

    if (_pParent) { _pParent->setIsGeometryModified(isModified); }
}

