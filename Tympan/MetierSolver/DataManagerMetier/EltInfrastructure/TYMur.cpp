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
#include "Tympan/GraphicIHM/DataManagerIHM/TYMurWidget.h"
#include "Tympan/GraphicIHM/DataManagerGraphic/TYMurGraphic.h"
#endif

#ifdef TYMPAN_USE_PRECOMPILED_HEADER
#include "Tympan/MetierSolver/DataManagerMetier/TYPHMetier.h"
#endif // TYMPAN_USE_PRECOMPILED_HEADER

#include "TYMur.h"
#include "Tympan/models/common/3d.h"


#include "Tympan/core/logging.h"


TY_EXTENSION_INST(TYMur);
TY_EXT_GRAPHIC_INST(TYMur);

TYMur::TYMur()
{
    _name = TYNameManager::get()->generateName(getClassName());

    // Nettoie l'init fait par le constructeur parent (TYAcousticRectangleNode)
    purge();

    // Couleur du mur par default R=156 G=97 B=1
    float r = 158.0f, g = 158.0f, b = 0.0f;

#if TY_USE_IHM
    if (TYPreferenceManager::exists(TYDIRPREFERENCEMANAGER, "TYMurGraphicColorR"))
    {
        TYPreferenceManager::getColor(TYDIRPREFERENCEMANAGER, "TYMurGraphicColor", r, g, b);
    }
    else
    {
        TYPreferenceManager::setColor(TYDIRPREFERENCEMANAGER, "TYMurGraphicColor", r, g, b);
    }
#endif

    OColor defaultColor;
    defaultColor.r = r / 255.0f;
    defaultColor.g = g / 255.0f;
    defaultColor.b = b / 255.0f;

    setColor(defaultColor);

    _pParoi = new TYParoi();
    _pParoi->setParent(this);
    _pParoi->setEpaisseur(0.5);

    _bParoiLocked = false;

    TYMurElement* pMurElt = new TYMurElement();
    pMurElt->setColor(defaultColor);
    pMurElt->getBoundingRect()->setSize(getSizeX(), getSizeY());
    addAcousticSurf(pMurElt);
}

TYMur::TYMur(const TYMur& other)
{
    *this = other;
}

TYMur::~TYMur()
{
}

TYMur& TYMur::operator=(const TYMur& other)
{
    if (this != &other)
    {
        TYAcousticRectangleNode::operator =(other);
        _pParoi = other._pParoi;
        _bParoiLocked = other._bParoiLocked;
    }
    return *this;
}

bool TYMur::operator==(const TYMur& other) const
{
    if (this != &other)
    {
        if (TYAcousticRectangleNode::operator !=(other)) { return false; }
        if (_pParoi != other._pParoi) { return false; }
    }
    return true;
}

bool TYMur::operator!=(const TYMur& other) const
{
    return !operator==(other);
}

bool TYMur::deepCopy(const TYElement* pOther, bool copyId /*=true*/)
{
    if (!TYAcousticRectangleNode::deepCopy(pOther, copyId)) { return false; }

    TYMur* pOtherMur = (TYMur*) pOther;

    _pParoi->deepCopy(pOtherMur->_pParoi, copyId);

    _bParoiLocked = pOtherMur->_bParoiLocked;

    return true;
}

bool TYMur::makeGrid(int nbCol, int nbRow)
{
    return TYAcousticRectangleNode::makeGrid(nbCol, nbRow, "TYMurElement");
}

void TYMur::updateGrid()
{
    TYAcousticRectangleNode::updateGrid("TYMurElement");
}

std::string TYMur::toString() const
{
    return "TYMur";
}

DOM_Element TYMur::toXML(DOM_Element& domElement)
{
    DOM_Element domNewElem = TYAcousticRectangleNode::toXML(domElement);

    TYXMLTools::addElementBoolValue(domNewElem, "paroiLocked", _bParoiLocked);
    _pParoi->toXML(domNewElem);

    return domNewElem;
}

int TYMur::fromXML(DOM_Element domElement)
{
    bool statParoiOk = false;

    TYAcousticRectangleNode::fromXML(domElement);

    DOM_Element elemCur;

    QDomNodeList childs = domElement.childNodes();
    for (unsigned int i = 0; i < childs.length(); i++)
    {
        elemCur = childs.item(i).toElement();
        TYXMLTools::getElementBoolValue(elemCur, "paroiLocked", _bParoiLocked, statParoiOk);
        _pParoi->callFromXMLIfEqual(elemCur);
    }

    return 1;
}

TYTabSourcePonctuelleGeoNode TYMur::getSrcs() const
{
    return TYAcousticRectangleNode::getSrcs();
}

TYSourcePonctuelle TYMur::srcPonctEquiv() const
{
    return TYAcousticRectangleNode::srcPonctEquiv();
}

void TYMur::distriSrcs()
{
    TYAcousticRectangleNode::distriSrcs();
}

bool TYMur::setSrcsLw()
{
    return TYAcousticRectangleNode::setSrcsLw();
}

double TYMur::surface() const
{
    return TYAcousticRectangleNode::surface();
}

int TYMur::intersects(const TYSurfaceInterface* pSurf, OSegment3D& seg) const
{
    return TYAcousticRectangleNode::intersects(pSurf, seg);
}

int TYMur::intersects(const OSegment3D& seg, OPoint3D& pt) const
{
    return TYAcousticRectangleNode::intersects(seg, pt);
}

bool TYMur::abso(const TYPoint& pt, TYSpectre& spectre) const
{
    printf("TYMur::abso non implemente.\n");
    return false;
}

bool TYMur::transm(const TYPoint& pt, TYSpectre& spectre) const
{
    printf("TYMur::absom non implemente.\n");
    return false;
}

void TYMur::setParoi(const LPTYParoi pParoi)
{
    assert(pParoi);
    assert(_pParoi);

    if (!_bParoiLocked) // Si elle est lockee, on ne peut pas la modifier
    {
        *_pParoi = *pParoi;
        _pParoi->setParent(this);
    }
}

void TYMur::setParoiLocked(const bool& bVal)
{
    _bParoiLocked = bVal;

    if (_pParent) // Inutile de verifier le lockage setParoi y pourvoira
    {
        setParoi(TYEtage::safeDownCast(_pParent)->getParoi());
    }
}

void TYMur::remAllSrcs()
{
    for (unsigned int j = 0; j < _tabAcousticSurf.size(); j++)
    {
        LPTYAcousticSurface pSurf = TYAcousticSurface::safeDownCast(_tabAcousticSurf[j]->getElement());

        pSurf->remAllSrcs();
    }
}

TYMateriauConstruction* TYMur::getMateriau()
{
    return _pParoi->getMatFace2();
}
