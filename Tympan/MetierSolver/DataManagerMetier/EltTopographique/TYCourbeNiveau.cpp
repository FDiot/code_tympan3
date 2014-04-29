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
#include "Tympan/GraphicIHM/DataManagerIHM/TYCourbeNiveauWidget.h"
#include "Tympan/GraphicIHM/DataManagerGraphic/TYCourbeNiveauGraphic.h"
#endif

#ifdef TYMPAN_USE_PRECOMPILED_HEADER
#include "Tympan/MetierSolver/DataManagerMetier/TYPHMetier.h"
#endif // TYMPAN_USE_PRECOMPILED_HEADER

#include "Tympan/Tools/OMessageManager.h"


OPROTOINST(TYCourbeNiveau);
TY_EXTENSION_INST(TYCourbeNiveau);
TY_EXT_GRAPHIC_INST(TYCourbeNiveau);

/*static*/ double TYCourbeNiveau::_defaultDistMax = setDefaultDistMax();

TYCourbeNiveau::TYCourbeNiveau() :  _isDMaxDefault(false),
    _altitude(0.0)

{
    _name = TYNameManager::get()->generateName(getClassName());

    _distMax = _defaultDistMax;

    updateColor();
}

TYCourbeNiveau::TYCourbeNiveau(const TYCourbeNiveau& other)
{
    *this = other;
}

TYCourbeNiveau::TYCourbeNiveau(const TYTabPoint& pts, double alt)
{
    setListPoints(pts);
    setAltitude(alt);
    _distMax = _defaultDistMax;
    _isDMaxDefault = false;

    updateColor();
}

TYCourbeNiveau::~TYCourbeNiveau()
{
}

TYCourbeNiveau& TYCourbeNiveau::operator=(const TYCourbeNiveau& other)
{
    if (this != &other)
    {
        TYElement::operator =(other);
        TYColorInterface::operator =(other);
        _altitude = other._altitude;
        _listPoints = other._listPoints;
    }
    return *this;
}

bool TYCourbeNiveau::operator==(const TYCourbeNiveau& other) const
{
    if (this != &other)
    {
        if (TYElement::operator !=(other)) { return false; }
        if (TYColorInterface::operator !=(other)) { return false; }
        if (_altitude != other._altitude) { return false; }
        if (!(_listPoints == other._listPoints)) { return false; }
    }
    return true;
}

bool TYCourbeNiveau::operator!=(const TYCourbeNiveau& other) const
{
    return !operator==(other);
}

bool TYCourbeNiveau::deepCopy(const TYElement* pOther, bool copyId /*=true*/)
{
    if (!TYElement::deepCopy(pOther, copyId)) { return false; }

    TYColorInterface::deepCopy((TYColorInterface*)pOther, copyId);

    TYCourbeNiveau* pOtherCrbNiv = (TYCourbeNiveau*) pOther;

    _altitude = pOtherCrbNiv->_altitude;

    _listPoints.clear();
    for (unsigned int i = 0; i < pOtherCrbNiv->_listPoints.size(); i++)
    {
        TYPoint pt;
        pt.deepCopy(&pOtherCrbNiv->_listPoints[i], copyId);
        _listPoints.push_back(pt);
    }

    return true;
}

std::string TYCourbeNiveau::toString() const
{
    return "TYCourbeNiveau";
}

DOM_Element TYCourbeNiveau::toXML(DOM_Element& domElement)
{
    DOM_Element domNewElem = TYElement::toXML(domElement);

    TYColorInterface::toXML(domNewElem);

    TYXMLTools::addElementDoubleValue(domNewElem, "altitude", _altitude);

    if (_isDMaxDefault == true) // Si on utilise une valeur specifique, on l'enregistre
    {
        TYXMLTools::addElementDoubleValue(domNewElem, "distmaxpts", _distMax);
    }

    TYXMLTools::addElementUIntValue(domNewElem, "nbPoints", _listPoints.size());

    for (unsigned int i = 0; i < _listPoints.size(); i++)
    {
        _listPoints.at(i).toXML(domNewElem);
    }

    return domNewElem;
}

int TYCourbeNiveau::fromXML(DOM_Element domElement)
{
    TYElement::fromXML(domElement);

    TYColorInterface::fromXML(domElement);

    _listPoints.clear();

    bool altitudeOk = false;
    bool nbPointsOk = false;
    bool distMaxOk = false;
    int nbPoints = 0;
    TYPoint pt;
    DOM_Element elemCur;

    QDomNodeList childs = domElement.childNodes();
    for (unsigned int i = 0; i < childs.length(); i++)
    {
        elemCur = childs.item(i).toElement();
        TYXMLTools::getElementDoubleValue(elemCur, "altitude", _altitude, altitudeOk);
        TYXMLTools::getElementDoubleValue(elemCur, "distmaxpts", _distMax, distMaxOk);
        TYXMLTools::getElementIntValue(elemCur, "nbPoints", nbPoints, nbPointsOk);
        if (nbPointsOk)
        {
            _listPoints.reserve(nbPoints);
        }

        if (pt.callFromXMLIfEqual(elemCur))
        {
            _listPoints.push_back(pt);
        }
    }

    if (distMaxOk) { _isDMaxDefault = true; } // Si une distance entre points est trouvee, on met le flag

    applyAlitudeToPoints();

    return 1;
}

void TYCourbeNiveau::setIsGeometryModified(bool isModified)
{
    TYElement::setIsGeometryModified(isModified);

    if (_pParent) { _pParent->setIsGeometryModified(isModified); }
}

void TYCourbeNiveau::setListPoints(const TYTabPoint& pts)
{
    _listPoints = pts;

    setIsGeometryModified(true);

    // Tant qu'il y a un parent
    TYElement* pParent = getParent();
    while (pParent)
    {
        // Tentative de cast du parent en Topo
        TYTopographie* pTopo = TYTopographie::safeDownCast(pParent);

        // Si le parent est effectivement une Topo
        if (pTopo)
        {
            // L'alti n'est plus a jour non plus...
            pTopo->getAltimetrie()->setIsGeometryModified(true);
            break;
        }

        // Parent du parent...
        pParent = pParent->getParent();
    }
}

void TYCourbeNiveau::close()
{
    if (_listPoints.size() == 0) { return; }

    // Fermeture de la courbe en ajoutant le premier point a la fin de la liste
    _listPoints.push_back(_listPoints[0]);
}

void TYCourbeNiveau::setAltitude(double alt)
{
    _altitude = alt;

    applyAlitudeToPoints();
}

void TYCourbeNiveau::applyAlitudeToPoints()
{
    for (unsigned int i = 0; i < _listPoints.size(); i++)
    {
        _listPoints[i]._z = _altitude;
    }

    setIsGeometryModified(true);

    // Tant qu'il y a un parent
    TYElement* pParent = getParent();
    while (pParent)
    {
        // Tentative de cast du parent en Topo
        TYTopographie* pTopo = TYTopographie::safeDownCast(pParent);

        // Si le parent est effectivement une Topo
        if (pTopo)
        {
            // L'alti n'est plus a jour non plus...
            pTopo->getAltimetrie()->setIsGeometryModified(true);
            break;
        }

        // Parent du parent...
        pParent = pParent->getParent();
    }
}


/*static*/ double TYCourbeNiveau::setDefaultDistMax()
{
    double valeur = 200.0;
#if TY_USE_IHM
    if (TYPreferenceManager::exists(TYDIRPREFERENCEMANAGER, "DistMinPtCrbNiv"))
    {
        valeur = TYPreferenceManager::getDouble(TYDIRPREFERENCEMANAGER, "DistMinPtCrbNiv");
    }
    else
    {
        TYPreferenceManager::setDouble(TYDIRPREFERENCEMANAGER, "DistMinPtCrbNiv", valeur);
    }
#endif

    return valeur; // Valeur par defaut si parametre non defini
}

double TYCourbeNiveau::getDistMax()
{
    updateDistMax(); // Il faut mettre a jour car le projet peut avoir change la valeurs par defaut
    return _distMax;
}

void TYCourbeNiveau::updateDistMax()
{
    // Mise a jour de la valeur par defaut (si necessaire)
    _distMax = _isDMaxDefault ? _distMax : _defaultDistMax;

}

void TYCourbeNiveau::setIsDMaxDefault(const bool& etat)
{
    _isDMaxDefault = etat;

    if (!_isDMaxDefault)
    {
        _distMax = _defaultDistMax;
    }
}

/*virtual*/ void TYCourbeNiveau::updateColor()
{
    // Couleur par default
    float r = 255.0f, g = 0.0f, b = 0.0f;

#if TY_USE_IHM
    if (TYPreferenceManager::exists(TYDIRPREFERENCEMANAGER, "TYCourbeNiveauGraphicColorR"))
    {
        TYPreferenceManager::getColor(TYDIRPREFERENCEMANAGER, "TYCourbeNiveauGraphicColor", r, g, b);
    }
    else
    {
        TYPreferenceManager::setColor(TYDIRPREFERENCEMANAGER, "TYCourbeNiveauGraphicColor", r, g, b);
    }
#endif

    OColor color;
    color.r = r / 255;
    color.g = g / 255;
    color.b = b / 255;

    setColor(color);
}

