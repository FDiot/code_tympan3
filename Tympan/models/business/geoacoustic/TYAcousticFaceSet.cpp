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

#if TY_USE_IHM
  #include "Tympan/gui/widgets/TYAcousticFaceSetWidget.h"
#endif
#include "TYAcousticFaceSet.h"


TY_EXTENSION_INST(TYAcousticFaceSet);

TYAcousticFaceSet::TYAcousticFaceSet()
{
    _name = TYNameManager::get()->generateName(getClassName());

    _pMateriau = new TYMateriauConstruction();
    _pMateriau->setParent(this);
}

TYAcousticFaceSet::TYAcousticFaceSet(const TYAcousticFaceSet& other)
{
    *this = other;
}

TYAcousticFaceSet::~TYAcousticFaceSet()
{
}

TYAcousticFaceSet& TYAcousticFaceSet::operator=(const TYAcousticFaceSet& other)
{
    if (this != &other)
    {
        TYAcousticVolume::operator =(other);
        _faces = other._faces;
    }
    return *this;
}

bool TYAcousticFaceSet::operator==(const TYAcousticFaceSet& other) const
{
    if (this != &other)
    {
        if (TYAcousticVolume::operator !=(other)) { return false; }
        if (_faces != other._faces) { return false; }
    }
    return true;
}

bool TYAcousticFaceSet::operator!=(const TYAcousticFaceSet& other) const
{
    return !operator==(other);
}

bool TYAcousticFaceSet::deepCopy(const TYElement* pOther, bool copyId /*=true*/)
{
    if (!TYAcousticVolume::deepCopy(pOther, copyId)) { return false; }

    _faces.deepCopy(&((TYAcousticFaceSet*) pOther)->_faces, copyId);
    return true;
}

std::string TYAcousticFaceSet::toString() const
{
    return "TYAcousticFaceSet";
}

DOM_Element TYAcousticFaceSet::toXML(DOM_Element& domElement)
{
    DOM_Element domNewElem = TYAcousticVolume::toXML(domElement);
    _faces.toXML(domNewElem); // les faces sont reconstruites depuis le squelette dans TYEcran
    return domNewElem;
}

int TYAcousticFaceSet::fromXML(DOM_Element domElement)
{
    TYAcousticVolume::fromXML(domElement);
    _faces.fromXML(domElement);
    return 1;
}

DOM_Element TYAcousticFaceSet::toXML(DOM_Element& domElement, bool withfaces)
{
    DOM_Element domNewElem = TYAcousticVolume::toXML(domElement);

    if (withfaces) { _faces.toXML(domNewElem); }

    return domNewElem;
}

int TYAcousticFaceSet::fromXML(DOM_Element domElement, bool withfaces)
{
    TYAcousticVolume::fromXML(domElement);

    if (withfaces) { _faces.fromXML(domElement); }

    return 1;
}

void TYAcousticFaceSet::getChilds(LPTYElementArray& childs, bool recursif /*=true*/)
{
}

void TYAcousticFaceSet::propagateRegime()
{
    TYAcousticVolume::propagateRegime();
}

void TYAcousticFaceSet::propagateAtt(LPTYAttenuateur pAtt)
{
    setIsAcousticModified(true);

    TYAcousticVolume::propagateAtt(pAtt);
}

TYTabSourcePonctuelleGeoNode TYAcousticFaceSet::getSrcs() const
{
    return TYTabSourcePonctuelleGeoNode();
}

TYSourcePonctuelle TYAcousticFaceSet::srcPonctEquiv() const
{
    return TYSourcePonctuelle();
}

void TYAcousticFaceSet::distriSrcs()
{
}

void TYAcousticFaceSet::remAllSrcs()
{
}

bool TYAcousticFaceSet::setSrcsLw()
{
    return false;
}

bool TYAcousticFaceSet::updateAcoustic(const bool& force) // force = false
{
    return true;
}

double TYAcousticFaceSet::activeSurface() const
{
    return _faces.activeSurface();
}

double TYAcousticFaceSet::activeSurface()
{
    return _faces.activeSurface();
}

double TYAcousticFaceSet::volume() const
{
    return _faces.volume();
}

double TYAcousticFaceSet::surface() const
{
    return _faces.surface();
}

TYTabVector TYAcousticFaceSet::normals() const
{
    return _faces.normals();
}

TYTabPoint TYAcousticFaceSet::sommets() const
{
    return _faces.sommets();
}

TYTabLPPolygon TYAcousticFaceSet::faces() const
{
    return _faces.faces();
}

TYBox TYAcousticFaceSet::volEnglob() const
{
    return _faces.volEnglob();
}

TYPoint TYAcousticFaceSet::centreGravite() const
{
    return _faces.centreGravite();
}

int TYAcousticFaceSet::intersects(const OSegment3D& seg, TYTabPoint& ptList) const
{
    return _faces.intersects(seg, ptList);
}

int TYAcousticFaceSet::intersects(const TYSegment& seg, TYTabPoint& ptList) const
{
    OPoint3D ptA(seg._ptA._x, seg._ptA._y, seg._ptA._z);
    OPoint3D ptB(seg._ptB._x, seg._ptB._y, seg._ptB._z);
    OSegment3D oSeg(ptA, ptB);
    return _faces.intersects(oSeg, ptList);
}

int TYAcousticFaceSet::isInside(const TYPoint& pt) const
{
    return _faces.isInside(pt);
}

TYTabAcousticSurfaceGeoNode TYAcousticFaceSet::acousticFaces()
{
    return TYTabAcousticSurfaceGeoNode();
}

bool TYAcousticFaceSet::findAcousticSurface(const TYAcousticSurface* pAccSurf, OMatrix* pMatrix /*=0*/)
{
    return false;
}

TYSpectre TYAcousticFaceSet::setGlobalLW(const TYSpectre& spectre, const double& surfGlobale, const int& regime/*=-1*/)
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

TYTabLPAcousticSurface TYAcousticFaceSet::getSubFace()
{
    TYTabLPAcousticSurface tabSubFaces;
    return tabSubFaces;
}

void TYAcousticFaceSet::setMateriau(LPTYMateriauConstruction pMat)
{
    _pMateriau = pMat;

    if (_pMateriau)
    {
        _pMateriau->setParent(this);
    }

    setIsGeometryModified(true);
}


void TYAcousticFaceSet::setIsRayonnant(bool rayonnant, bool recursif)
{
    TYAcousticInterface::setIsRayonnant(rayonnant);
}
