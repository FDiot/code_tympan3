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


OPROTOINST(TYAcousticSurface);


TYAcousticSurface::TYAcousticSurface()
{
    _name = TYNameManager::get()->generateName(getClassName());

    _pSrcSurf = new TYSourceSurfacic();
    _pSrcSurf->setParent(this);

    _pBoundingRect = new TYRectangle();
    _pBoundingRect->setParent(this);

    _isSub = false;
}

TYAcousticSurface::TYAcousticSurface(const TYAcousticSurface& other)
{
    *this = other;
}

TYAcousticSurface::~TYAcousticSurface()
{
}

TYAcousticSurface& TYAcousticSurface::operator=(const TYAcousticSurface& other)
{
    if (this != &other)
    {
        TYElement::operator =(other);
        TYColorInterface::operator =(other);
        TYAcousticInterface::operator =(other);
        _pSrcSurf = other._pSrcSurf;
        _pBoundingRect = other._pBoundingRect;
        _isSub = other._isSub;
    }
    return *this;
}

bool TYAcousticSurface::operator==(const TYAcousticSurface& other) const
{
    if (this != &other)
    {
        if (TYElement::operator !=(other)) { return false; }
        if (TYColorInterface::operator !=(other)) { return false; }
        if (TYAcousticInterface::operator !=(other)) { return false; }
        if (_pSrcSurf != other._pSrcSurf) { return false; }
        if (_pBoundingRect != other._pBoundingRect) { return false; }
        if (_isSub != other._isSub) { return false; }
    }
    return true;
}

bool TYAcousticSurface::operator!=(const TYAcousticSurface& other) const
{
    return !operator==(other);
}

bool TYAcousticSurface::deepCopy(const TYElement* pOther, bool copyId /*=true*/)
{
    if (!TYElement::deepCopy(pOther, copyId)) { return false; }

    TYAcousticSurface* pOtherAccSurf = (TYAcousticSurface*) pOther;

    TYColorInterface::deepCopy((TYColorInterface*)pOtherAccSurf, copyId);

    TYAcousticInterface::deepCopy(pOtherAccSurf, copyId);

    _pSrcSurf->deepCopy(pOtherAccSurf->_pSrcSurf, copyId);
    _pBoundingRect->deepCopy(pOtherAccSurf->_pBoundingRect, copyId);

    _isSub = pOtherAccSurf->_isSub;

    return true;
}

std::string TYAcousticSurface::toString() const
{
    return "TYAcousticSurface";
}

DOM_Element TYAcousticSurface::toXML(DOM_Element& domElement)
{
    DOM_Element domNewElem = TYElement::toXML(domElement);
    TYColorInterface::toXML(domNewElem);
    TYAcousticInterface::toXML(domNewElem);

    TYXMLTools::addElementBoolValue(domNewElem, "isSub", _isSub);

    _pBoundingRect->toXML(domNewElem);
    _pSrcSurf->toXML(domNewElem);


    return domNewElem;
}

int TYAcousticSurface::fromXML(DOM_Element domElement)
{
    TYElement::fromXML(domElement);
    TYColorInterface::fromXML(domElement);
    TYAcousticInterface::fromXML(domElement);

    bool isSubOk = false;
    DOM_Element elemCur;
    QDomNodeList childs = domElement.childNodes();
    for (unsigned int i = 0; i < childs.length(); i++)
    {
        elemCur = childs.item(i).toElement();
        TYXMLTools::getElementBoolValue(elemCur, "isSub", _isSub, isSubOk);

        _pSrcSurf->callFromXMLIfEqual(elemCur);

        _pBoundingRect->callFromXMLIfEqual(elemCur);
    }

    return 1;
}

void TYAcousticSurface::setIsAcousticModified(bool isModified)
{
    TYElement::setIsAcousticModified(isModified);

    if (_pParent) { _pParent->setIsAcousticModified(isModified); }
}

void TYAcousticSurface::setDensiteSrcsH(double densite, bool recursif /*=true*/)
{
    TYAcousticInterface::setDensiteSrcsH(densite, recursif);
    _pSrcSurf->setDensiteSrcsH(densite);
}

void TYAcousticSurface::setDensiteSrcsV(double densite, bool recursif /*=true*/)
{
    TYAcousticInterface::setDensiteSrcsV(densite, recursif);
    _pSrcSurf->setDensiteSrcsV(densite);
}

TYTabSourcePonctuelleGeoNode TYAcousticSurface::getSrcs() const
{
    TYTabSourcePonctuelleGeoNode tabRet;
    TYTabLPSourcePonctuelle tab = _pSrcSurf->getSrcs();

    for (unsigned int i = 0; i < tab.size(); i++)
    {
        tabRet.push_back(new TYSourcePonctuelleGeoNode((LPTYElement)tab[i]));
    }

    return tabRet;
}

TYSourcePonctuelle TYAcousticSurface::srcPonctEquiv() const
{
    return TYSourcePonctuelle();
}

void TYAcousticSurface::propagateRegime()
{
    TYAcousticInterface::propagateRegime();
}

void TYAcousticSurface::setCurRegime(int regime)
{
    TYAcousticInterface::setCurRegime(regime);

    setIsAcousticModified(true);
}

void TYAcousticSurface::propagateAtt(LPTYAttenuateur pAtt)
{
    setIsAcousticModified(true);

    TYAcousticInterface::propagateAtt(pAtt);
}

void TYAcousticSurface::distriSrcs()
{
}

void TYAcousticSurface::remAllSrcs()
{
    _pSrcSurf->remAllSrc();
}


bool TYAcousticSurface::setSrcsLw()
{
    double surface = this->surface();

    const TYTabSourcePonctuelleGeoNode& tabSrcs = this->getSrcs();
    size_t nbSrcs = tabSrcs.size();

    TYSpectre s = TYSpectre::getEmptyLinSpectre(1.0);

    if (this->getUseAtt() && this->_pAtt != NULL)  // Si un attenuateur est defini, on l'applique
    {
        s = s.div(_pAtt->getSpectreAtt().toGPhy());
    }

    double surfSource = surface / nbSrcs;

    s = s.mult(getCurrentSpectre().toGPhy().mult(surfSource / surface));

    for (size_t i = 0 ; i < nbSrcs ; i++)
    {
        //Affectation du spectre a la source i pour le regime j
        TYSourcePonctuelle::safeDownCast(tabSrcs[i]->getElement())->setRegime(s);
    }

    return true;
}

bool TYAcousticSurface::updateAcoustic(const bool& force) // force = false
{
    return true;
}

double TYAcousticSurface::surface() const
{
    // XXX This is a dummy default implementation
    return _pBoundingRect->surface();
}

OVector3D TYAcousticSurface::normal() const
{
    // XXX This is a dummy default implementation
    return _pBoundingRect->normal();
}

OPlan TYAcousticSurface::plan() const
{
    // XXX This is a dummy default implementation
    return _pBoundingRect->plan();
}

TYTabPoint TYAcousticSurface::getContour(int n /*=-1*/) const
{
    // XXX This is a dummy default implementation
    return _pBoundingRect->getContour(n);
}

TYTabPoint3D TYAcousticSurface::getOContour(int n /*=-1*/) const
{
    // XXX This is a dummy default implementation
    return _pBoundingRect->getOContour(n);
}

int TYAcousticSurface::intersects(const TYSurfaceInterface* pSurf, OSegment3D& seg) const
{
    // XXX This is a dummy default implementation
    return _pBoundingRect->intersects(pSurf, seg);
}

int TYAcousticSurface::intersects(const OSegment3D& seg, OPoint3D& pt) const
{
    // XXX This is a dummy default implementation
    return _pBoundingRect->intersects(seg, pt);
}

int TYAcousticSurface::intersects(const OPoint3D& pt) const
{
    // XXX This is a dummy default implementation
    return _pBoundingRect->intersects(pt);
}

LPTYMateriauConstruction TYAcousticSurface::getMateriau()
{
    LPTYMateriauConstruction pMat = NULL;

    TYElement* pParent = getParent();

    // Tant qu'il y a un parent
    while (pParent)
    {
        // Tentative de cast du parent en AcousticVolume
        TYAcousticVolume* pAccVol = TYAcousticVolume::safeDownCast(pParent);

        // Si le parent est effectivement un AcousticVolume
        if (pAccVol)
        {
            pMat = pAccVol->getMateriau();
            break;
        }

        // Parent du parent...
        pParent = pParent->getParent();
    }

    return pMat;
}

const LPTYMateriauConstruction TYAcousticSurface::getMateriau() const
{
    return ((TYAcousticSurface*) this)->getMateriau();
}

TYSpectre TYAcousticSurface::setGlobalLW(const TYSpectre& spectre, const double& surfGlobale, const int& regime/*=-1*/)
{
    TYSpectre LWv;

    // 2 cas
    if (_typeDistribution == TY_PUISSANCE_CALCULEE)
    {
        if (surfGlobale == 0)
        {
            LWv = TYSpectre::getEmptyLinSpectre();
        }
        else
        {
            LWv = spectre.toGPhy().mult(surface() / surfGlobale).toDB(); // Spectre pondere de la surface de la face
        }

        setRegime(LWv, regime, false);
    }

    return getCurrentSpectre().toGPhy();
}

LPTYSpectre TYAcousticSurface::getRealPowerSpectrum()
{
    // Si la surface n'est pas rayonnante on renvoi un spectre vide
    if (!_isRayonnant) { return new TYSpectre(); }

    // on prend en compte l'eventuel attenuateur
    if (_useAtt && (_pAtt != NULL))
    {
        OSpectre temp = _pCurrentSpectre.subst(_pAtt->getSpectreAtt());
        return new TYSpectre(temp);
    }

    return new TYSpectre(_pCurrentSpectre);

}

void TYAcousticSurface::setIsRayonnant(bool rayonnant, bool recursif)
{
    TYAcousticInterface::setIsRayonnant(rayonnant);

    if (_isSub && rayonnant)
    {
        // Si une face rayonne son parent rayonne
        TYAcousticSurfaceNode* pSurfNode = TYAcousticSurfaceNode::safeDownCast(this->getParent());
        if (pSurfNode) { pSurfNode->setIsRayonnant(rayonnant, recursif); }
    }
}




void TYAcousticSurface::exportCSV(std::ofstream& ofs)
{
    // Export du nom de l'objet
    ofs << getName().toAscii().data() << '\n';

    // Export du type de l'objet
    ofs << toString() << '\n';
    TYAcousticInterface::exportCSV(ofs);
}
