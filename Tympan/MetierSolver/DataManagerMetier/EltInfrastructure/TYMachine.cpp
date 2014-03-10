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
#include "Tympan/GraphicIHM/DataManagerIHM/TYMachineWidget.h"
#endif

#ifdef TYMPAN_USE_PRECOMPILED_HEADER
#include "Tympan/MetierSolver/DataManagerMetier/TYPHMetier.h"
#endif // TYMPAN_USE_PRECOMPILED_HEADER

#include "TYMachine.h"
#include "Tympan/Tools/OMessageManager.h"

OPROTOINST(TYMachine);
TY_EXTENSION_INST(TYMachine);

TYMachine::TYMachine()
{
    _name = TYNameManager::get()->generateName(getClassName());

    _categorie = 1;
    _constructeur = "Constructeur";
    _modele = "Modele";
    _commentaire = "Commentaire";
}

TYMachine::TYMachine(const TYMachine& other)
{
    *this = other;
}

TYMachine::~TYMachine()
{
}

TYMachine& TYMachine::operator=(const TYMachine& other)
{
    if (this != &other)
    {
        TYAcousticVolumeNode::operator =(other);
        _categorie = other._categorie;
        _constructeur = other._constructeur;
        _modele = other._modele;
        _commentaire = other._commentaire;
    }
    return *this;
}

bool TYMachine::operator==(const TYMachine& other) const
{
    if (this != &other)
    {
        if (TYAcousticVolumeNode::operator !=(other)) { return false; }
        if (_categorie != other._categorie) { return false; }
        if (_constructeur != other._constructeur) { return false; }
        if (_modele != other._modele) { return false; }
        if (_commentaire != other._commentaire) { return false; }
    }
    return true;
}

bool TYMachine::operator!=(const TYMachine& other) const
{
    return !operator==(other);
}

bool TYMachine::deepCopy(const TYElement* pOther, bool copyId /*=true*/)
{
    if (!TYAcousticVolumeNode::deepCopy(pOther, copyId)) { return false; }

    TYMachine* pOtherMachine = (TYMachine*) pOther;

    _categorie = pOtherMachine->_categorie;
    _constructeur = pOtherMachine->_constructeur;
    _modele = pOtherMachine->_modele;
    _commentaire = pOtherMachine->_commentaire;

    return true;
}

std::string TYMachine::toString() const
{
    return "TYMachine";
}

DOM_Element TYMachine::toXML(DOM_Element& domElement)
{
    DOM_Element domNewElem = TYAcousticVolumeNode::toXML(domElement);

    TYXMLTools::addElementIntValue(domNewElem, "categorie", _categorie);
    TYXMLTools::addElementStringValue(domNewElem, "constructeur", _constructeur);
    TYXMLTools::addElementStringValue(domNewElem, "modele", _modele);
    TYXMLTools::addElementStringValue(domNewElem, "commentaire", _commentaire);

    return domNewElem;
}

int TYMachine::fromXML(DOM_Element domElement)
{
    DOM_Element elemCur;

    QDomNodeList childs = domElement.childNodes();
    for (unsigned int i = 0; i < childs.length(); i++)
    {
        elemCur = childs.item(i).toElement();
        TYXMLTools::getElementIntValue(elemCur, "categorie", _categorie);
        TYXMLTools::getElementStringValue(elemCur, "constructeur", _constructeur);
        TYXMLTools::getElementStringValue(elemCur, "modele", _modele);
        TYXMLTools::getElementStringValue(elemCur, "commentaire", _commentaire);
    }

    return TYAcousticVolumeNode::fromXML(domElement);
}

TYTabSourcePonctuelleGeoNode TYMachine::getSrcs() const
{
    return TYAcousticVolumeNode::getSrcs();
}

TYSourcePonctuelle TYMachine::srcPonctEquiv() const
{
    return TYAcousticVolumeNode::srcPonctEquiv();
}

void TYMachine::distriSrcs()
{
    TYAcousticVolumeNode::distriSrcs();
}

bool TYMachine::setSrcsLw()
{
    return TYAcousticVolumeNode::setSrcsLw();
}

bool TYMachine::updateAcoustic(const bool& force) // force=false
{
    return TYAcousticVolumeNode::updateAcoustic(force);
}

double TYMachine::volume() const
{
    return TYAcousticVolumeNode::volume();
}

double TYMachine::surface() const
{
    return TYAcousticVolumeNode::surface();
}

TYTabVector TYMachine::normals() const
{
    return TYAcousticVolumeNode::normals();
}

TYTabPoint TYMachine::sommets() const
{
    return TYAcousticVolumeNode::sommets();
}

TYTabLPPolygon TYMachine::faces() const
{
    return TYAcousticVolumeNode::faces();
}

TYBox TYMachine::volEnglob() const
{
    return TYAcousticVolumeNode::volEnglob();
}

TYPoint TYMachine::centreGravite() const
{
    return TYAcousticVolumeNode::centreGravite();
}

int TYMachine::intersects(const OSegment3D& seg, TYTabPoint& ptList) const
{
    return TYAcousticVolumeNode::intersects(seg, ptList);
}

int TYMachine::isInside(const TYPoint& pt) const
{
    return TYAcousticVolumeNode::isInside(pt);
}
