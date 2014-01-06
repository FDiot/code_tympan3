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



#ifdef TYMPAN_USE_PRECOMPILED_HEADER
#include "Tympan/MetierSolver/DataManagerMetier/TYPHMetier.h"
#endif // TYMPAN_USE_PRECOMPILED_HEADER


OPROTOINST(TYAcousticVolume);


TYAcousticVolume::TYAcousticVolume() : _forceNormales(false)
{
    _name = TYNameManager::get()->generateName(getClassName());

    _pMateriau = new TYMateriauConstruction();
    _pMateriau->setParent(this);
}

TYAcousticVolume::TYAcousticVolume(const TYAcousticVolume& other)
{
    *this = other;
}

TYAcousticVolume::~TYAcousticVolume()
{
}

TYAcousticVolume& TYAcousticVolume::operator=(const TYAcousticVolume& other)
{
    if (this != &other)
    {
        TYElement::operator =(other);
        TYColorInterface::operator =(other);
        TYAcousticInterface::operator =(other);
        _pMateriau = other._pMateriau;
        _forceNormales = other._forceNormales;
    }
    return *this;
}

bool TYAcousticVolume::operator==(const TYAcousticVolume& other) const
{
    if (this != &other)
    {
        if (TYElement::operator !=(other)) { return false; }
        if (TYColorInterface::operator !=(other)) { return false; }
        if (TYAcousticInterface::operator !=(other)) { return false; }
        if (_pMateriau != other._pMateriau) { return false; }
        if (_forceNormales != other._forceNormales) { return false; }

    }
    return true;
}

bool TYAcousticVolume::operator!=(const TYAcousticVolume& other) const
{
    return !operator==(other);
}

bool TYAcousticVolume::deepCopy(const TYElement* pOther, bool copyId /*=true*/)
{
    if (!TYElement::deepCopy(pOther, copyId)) { return false; }

    TYAcousticVolume* pVol = (TYAcousticVolume*) pOther;

    TYColorInterface::deepCopy((TYColorInterface*) pOther, copyId);
    TYAcousticInterface::deepCopy((TYAcousticVolume*) pOther, copyId);
    _pMateriau->deepCopy(((TYAcousticVolume*) pOther)->_pMateriau, copyId);
    _forceNormales = pVol->_forceNormales;

    return true;
}

std::string TYAcousticVolume::toString() const
{
    return "TYAcousticVolume";
}

DOM_Element TYAcousticVolume::toXML(DOM_Element& domElement)
{
    DOM_Element domNewElem = TYElement::toXML(domElement);
    TYColorInterface::toXML(domNewElem);
    TYAcousticInterface::toXML(domNewElem);

    if (_pMateriau != NULL)
    {
        _pMateriau->toXML(domNewElem);
    }

    TYXMLTools::addElementBoolValue(domNewElem, "forceNormales", _forceNormales);

    return domNewElem;
}

int TYAcousticVolume::fromXML(DOM_Element domElement)
{
    bool normOk(false);
    TYElement::fromXML(domElement);
    TYColorInterface::fromXML(domElement);
    TYAcousticInterface::fromXML(domElement);

    if (_pMateriau == NULL)
    {
        _pMateriau = new TYMateriauConstruction();
    }

    //  bool isSubOk = false;
    bool matOk = false;
    DOM_Element elemCur;

    QDomNodeList childs = domElement.childNodes();
    for (unsigned int i = 0; i < childs.length(); i++)
    {
        elemCur = childs.item(i).toElement();
        if (!matOk)
        {
            matOk = _pMateriau->callFromXMLIfEqual(elemCur);
        }

        if (!normOk)
        {
            TYXMLTools::getElementBoolValue(elemCur, "forceNormales", _forceNormales, normOk);
        }
    }

    if (!matOk)
    {
        _pMateriau = NULL;
    }

    return 1;
}

void TYAcousticVolume::setIsAcousticModified(bool isModified)
{
    TYElement::setIsAcousticModified(isModified);

    if (_pParent) { _pParent->setIsAcousticModified(isModified); }
}

void TYAcousticVolume::propagateRegime()
{
    TYAcousticInterface::propagateRegime();
}

void TYAcousticVolume::correctNbRegimes()
{
    size_t nbRegimes = _tabRegimes.size();
    TYElementCollection childs;
    getChilds(childs, false);

    LPTYGeometryNode pNode = NULL;
    LPTYAcousticSurfaceNode pSurfNode = NULL;
    for (size_t i = 0; i < childs.size(); i++)
    {
        pNode = TYGeometryNode::safeDownCast(childs[i]);
        if (pNode == NULL) { continue; }

        pSurfNode = TYAcousticSurfaceNode::safeDownCast(pNode->getElement());
        if (pSurfNode)
        {
            if (pSurfNode->getNbRegimes() > nbRegimes)
            {
                pSurfNode->setNbRegimes(static_cast<int>(nbRegimes));
            }

            pSurfNode->correctNbRegimes();
        }
    }
}

void TYAcousticVolume::propagateAtt(LPTYAttenuateur pAtt)
{
    setIsAcousticModified(true);

    TYAcousticInterface::propagateAtt(pAtt);
}

TYTabSourcePonctuelleGeoNode TYAcousticVolume::getSrcs() const
{
    return TYTabSourcePonctuelleGeoNode();
}

TYSourcePonctuelle TYAcousticVolume::srcPonctEquiv() const
{
    return TYSourcePonctuelle();
}

void TYAcousticVolume::distriSrcs()
{
}

void TYAcousticVolume::remAllSrcs()
{
}

bool TYAcousticVolume::setSrcsLw()
{
    return false;
}

bool TYAcousticVolume::updateAcoustic(const bool& force) // force = false
{
    return true;
}

double TYAcousticVolume::volume() const
{
    return 0.0;
}

double TYAcousticVolume::surface() const
{
    return 0.0;
}

double TYAcousticVolume::activeSurface() const
{
    return 0.0;
}

double TYAcousticVolume::activeSurface()
{
    return 0.0;
}


TYTabVector TYAcousticVolume::normals() const
{
    return TYTabVector();
}

TYTabPoint TYAcousticVolume::sommets() const
{
    return TYTabPoint();
}

TYTabLPPolygon TYAcousticVolume::faces() const
{
    return TYTabLPPolygon();
}

TYBox TYAcousticVolume::volEnglob() const
{
    return TYBox();
}

TYPoint TYAcousticVolume::centreGravite() const
{
    return TYPoint();
}

int TYAcousticVolume::intersects(const TYSegment& seg, TYTabPoint& ptList) const
{
    return INTERS_NULLE;
}

int TYAcousticVolume::isInside(const TYPoint& pt) const
{
    return INTERS_NULLE;
}

TYTabAcousticSurfaceGeoNode TYAcousticVolume::acousticFaces()
{
    return TYTabAcousticSurfaceGeoNode();
}

bool TYAcousticVolume::findAcousticSurface(const TYAcousticSurface* pAccSurf, OMatrix* pMatrix /*=0*/)
{
    return false;
}

double TYAcousticVolume::lwApp(const TYSourcePonctuelle* pSrc, TYSegment seg) const
{
    return 0.0f;
}

TYSpectre TYAcousticVolume::setGlobalLW(const TYSpectre& spectre, const double& surfGlobale, const int& regime/*=-1*/)
{
    TYSpectre LWv;

    if (_typeDistribution == TY_PUISSANCE_CALCULEE)
    {
        double surfOfVol = activeSurface();
        LWv = spectre.toGPhy().mult(surfOfVol / surfGlobale).toDB(); // Spectre pondere de la surface du volume

        setRegime(LWv, regime, false);
    }

    return getCurrentSpectre().toGPhy(); // On retourne le spectre obtenu
}

TYTabLPAcousticSurface TYAcousticVolume::getSubFace()
{
    TYTabLPAcousticSurface tabSubFaces;
    return tabSubFaces;
}

void TYAcousticVolume::setMateriau(LPTYMateriauConstruction pMat)
{
    _pMateriau = pMat;

    if (_pMateriau)
    {
        _pMateriau->setParent(this);
    }

    setIsGeometryModified(true);
}


void TYAcousticVolume::setIsRayonnant(bool rayonnant, bool recursif)
{
    TYAcousticInterface::setIsRayonnant(rayonnant);
}
void TYAcousticVolume::setRegimeName(const QString& name)
{
    bool status = true;
    TYRegime& reg = getRegimeNb(_curRegime, status);
    if (status) { reg.setRegimeName(name); }
}

void TYAcousticVolume::inverseNormales()
{
    TYTabAcousticSurfaceGeoNode tabSurf = acousticFaces();
    for (unsigned int i = 0; i < tabSurf.size(); i++)
    {
        TYAcousticSurface* pSurf = TYAcousticSurface::safeDownCast(tabSurf[i]->getElement());
        pSurf->inverseNormale();
    }
}
