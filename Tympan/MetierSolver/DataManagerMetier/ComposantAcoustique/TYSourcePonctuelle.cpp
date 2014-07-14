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
#include "Tympan/GraphicIHM/DataManagerIHM/TYSourcePonctuelleWidget.h"
#include "Tympan/GraphicIHM/DataManagerGraphic/TYSourcePonctuelleGraphic.h"
#endif


#include <typeinfo>

#ifdef TYMPAN_USE_PRECOMPILED_HEADER
#include "Tympan/MetierSolver/DataManagerMetier/TYPHMetier.h"
#endif // TYMPAN_USE_PRECOMPILED_HEADER
#include "Tympan/models/common/3d.h"

#include "Tympan/Tools/OMessageManager.h"

#if defined(WIN32)
#include <crtdbg.h>
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new new(_NORMAL_BLOCK, THIS_FILE, __LINE__)
#endif
#endif


TY_EXTENSION_INST(TYSourcePonctuelle);
TY_EXT_GRAPHIC_INST(TYSourcePonctuelle);

TYSourcePonctuelle::TYSourcePonctuelle():
    _type(TypeSurface),
    _object(0),
    _typeRaynt(SPHERIC),
    _hauteur(0.0)
{
    _name = TYNameManager::get()->generateName(getClassName());

    float r = 255.0f, g = 0.0f, b = 0.0f;

#if TY_USE_IHM
    if (TYPreferenceManager::exists(TYDIRPREFERENCEMANAGER, "SrcPonctGraphicColorR"))
    {
        TYPreferenceManager::getColor(TYDIRPREFERENCEMANAGER, "SrcPonctGraphicColor", r, g, b);
    }
    else
    {
        TYPreferenceManager::setColor(TYDIRPREFERENCEMANAGER, "SrcPonctGraphicColor", r, g, b);
    }

#endif

    // Couleur de preference.
    setColor(OColor(r / 255, g / 255, b / 255));

    _pPos = new TYPoint();
}

TYSourcePonctuelle::TYSourcePonctuelle(const TYSourcePonctuelle& other)
{
    *this = other;
}

TYSourcePonctuelle::~TYSourcePonctuelle()
{
}

TYSourcePonctuelle& TYSourcePonctuelle::operator=(const TYSourcePonctuelle& other)
{
    if (this != &other)
    {
        TYSource::operator =(other);
        TYColorInterface::operator =(other);
        _pPos = other._pPos;
        _orientation = other._orientation;
        _type = other._type;
        _typeRaynt = other._typeRaynt;
        _hauteur = other._hauteur;
        _object = other._object;
    }
    return *this;
}

bool TYSourcePonctuelle::operator==(const TYSourcePonctuelle& other) const
{
    if (this != &other)
    {
        if (TYSource::operator !=(other)) { return false; }
        if (TYColorInterface::operator !=(other)) { return false; }
        if (_pPos != other._pPos) { return false; }
        if (_orientation != other._orientation) { return false; }
        if (_type != other._type) { return false; }
        if (_typeRaynt != other._typeRaynt) { return false; }
        if (_hauteur != other._hauteur) { return false; }
        if (_object != other._object) { return false; }
    }
    return true;
}

bool TYSourcePonctuelle::operator!=(const TYSourcePonctuelle& other) const
{
    return !operator==(other);
}

bool TYSourcePonctuelle::deepCopy(const TYElement* pOther, bool copyId /*=true*/)
{
    if (!TYSource::deepCopy(pOther, copyId)) { return false; }

    TYSourcePonctuelle* pOtherSrc = (TYSourcePonctuelle*) pOther;
    TYColorInterface::deepCopy((TYColorInterface*)pOtherSrc, copyId);

    _orientation = pOtherSrc->_orientation;
    _type = pOtherSrc->_type;
    _typeRaynt = pOtherSrc->_typeRaynt;
    _pPos->deepCopy(pOtherSrc->_pPos, copyId);
    _hauteur = pOtherSrc->_hauteur;
    _object = pOtherSrc->_object;

    return true;
}

std::string TYSourcePonctuelle::toString() const
{
    return "TYSourcePonctuelle";
}

DOM_Element TYSourcePonctuelle::toXML(DOM_Element& domElement)
{
    DOM_Element domNewElem = TYSource::toXML(domElement);
    TYColorInterface::toXML(domNewElem);

    _pPos->toXML(domNewElem);

    DOM_Document domDoc = domElement.ownerDocument();

    DOM_Element vectorXElem = domDoc.createElement("Vector");
    domNewElem.appendChild(vectorXElem);
    vectorXElem.setAttribute("x", doubleToStrPre(_orientation._x, 3).data());
    vectorXElem.setAttribute("y", doubleToStrPre(_orientation._y, 3).data());
    vectorXElem.setAttribute("z", doubleToStrPre(_orientation._z, 3).data());

    TYXMLTools::addElementIntValue(domNewElem, "type", _type);

    if (_type == TypeUser)
    {
        TYXMLTools::addElementStringValue(domNewElem, "hauteur", doubleToStrPre(_hauteur, 3).data());
    }

    TYXMLTools::addElementIntValue(domNewElem, "typeRaynt", _typeRaynt);
    TYXMLTools::addElementIntValue(domNewElem, "formeObjet", _object);

    return domNewElem;
}

int TYSourcePonctuelle::fromXML(DOM_Element domElement)
{
    TYSource::fromXML(domElement);
    TYColorInterface::fromXML(domElement);

    // Reset
    bool typeOk = false;
    bool hauteurOk = false;
    bool typeRayntOk = false;
    bool formeObjetOk = false;

    DOM_Element elemCur;
    QDomNodeList childs = domElement.childNodes();

    for (unsigned int i = 0; i < childs.length(); i++)
    {
        elemCur = childs.item(i).toElement();

        _pPos->callFromXMLIfEqual(elemCur);
        if (elemCur.nodeName() == "Vector")
        {
            _orientation._x = TYXMLTools::getElementAttributeToDouble(elemCur, "x");
            _orientation._y = TYXMLTools::getElementAttributeToDouble(elemCur, "y");
            _orientation._z = TYXMLTools::getElementAttributeToDouble(elemCur, "z");
        }

        TYXMLTools::getElementIntValue(elemCur, "type", _type, typeOk);
        TYXMLTools::getElementDoubleValue(elemCur, "hauteur", _hauteur, hauteurOk);
        TYXMLTools::getElementIntValue(elemCur, "typeRaynt", _typeRaynt, typeRayntOk);
        TYXMLTools::getElementIntValue(elemCur, "formeObjet", _object, formeObjetOk);

    }

    return 1;
}

void TYSourcePonctuelle::setType(int type)
{
    _type = type;

    // Si c'est une source utilisateur, il faut lui affecter une orientation
    if (_type == TYSourcePonctuelle::TypeUser)
    {
        _orientation = OVector3D(1, 0, 0);
    }
}

