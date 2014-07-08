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
#include "Tympan/gui/widgets/TYBatimentWidget.h"
#include "Tympan/gui/gl/TYBatimentGraphic.h"
#endif

#ifdef TYMPAN_USE_PRECOMPILED_HEADER
#include "Tympan/models/business/TYPHMetier.h"
#endif // TYMPAN_USE_PRECOMPILED_HEADER

#include "TYBatiment.h"
#include "Tympan/core/logging.h"


TY_EXTENSION_INST(TYBatiment);
TY_EXT_GRAPHIC_INST(TYBatiment);

TYBatiment::TYBatiment()
{
    _name = TYNameManager::get()->generateName(getClassName());
}

TYBatiment::TYBatiment(const TYBatiment& other)
{
    *this = other;
}

TYBatiment::~TYBatiment()
{
}

TYBatiment& TYBatiment::operator=(const TYBatiment& other)
{
    if (this != &other)
    {
        TYAcousticVolumeNode::operator =(other);
    }
    return *this;
}

bool TYBatiment::operator==(const TYBatiment& other) const
{
    if (this != &other)
    {
        if (TYAcousticVolumeNode::operator !=(other)) { return false; }
    }
    return true;
}

bool TYBatiment::operator!=(const TYBatiment& other) const
{
    return !operator==(other);
}

std::string TYBatiment::toString() const
{
    return "TYBatiment";
}

DOM_Element TYBatiment::toXML(DOM_Element& domElement)
{
    DOM_Element domNewElem = TYAcousticVolumeNode::toXML(domElement);
    return domNewElem;
}

int TYBatiment::fromXML(DOM_Element domElement)
{
    TYAcousticVolumeNode::fromXML(domElement);

    return 1;
}

void TYBatiment::setDensiteSrcsH(double densite, bool recursif)
{
    TYAcousticInterface::setDensiteSrcsH(densite, recursif);

    if (recursif)
    {
        for (int i = 0; i < getNbChild(); i++)
        {
            LPTYAcousticVolume pEtage = TYAcousticVolume::safeDownCast(_tabAcousticVol[i]->getElement());
            assert(pEtage);

            // Densite H de srcs ponct
            pEtage->setDensiteSrcsH(getDensiteSrcsH(), recursif);
        }
    }

}

void TYBatiment::setDensiteSrcsV(double densite, bool recursif)
{
    TYAcousticInterface::setDensiteSrcsV(densite, recursif);

    if (recursif)
    {
        for (int i = 0; i < getNbChild(); i++)
        {
            LPTYAcousticVolume pEtage = TYAcousticVolume::safeDownCast(_tabAcousticVol[i]->getElement());
            assert(pEtage);

            // Densite V de srcs ponct
            pEtage->setDensiteSrcsV(getDensiteSrcsV(), recursif);
        }
    }

}

TYTabSourcePonctuelleGeoNode TYBatiment::getSrcs() const
{
    return TYAcousticVolumeNode::getSrcs();
}

TYSourcePonctuelle TYBatiment::srcPonctEquiv() const
{
    return TYAcousticVolumeNode::srcPonctEquiv();
}

void TYBatiment::distriSrcs()
{
    TYAcousticVolumeNode::distriSrcs();
}

bool TYBatiment::setSrcsLw()
{
    return TYAcousticVolumeNode::setSrcsLw();
}

double TYBatiment::volume() const
{
    return TYAcousticVolumeNode::volume();
}

double TYBatiment::surface() const
{
    return TYAcousticVolumeNode::surface();
}

TYTabVector TYBatiment::normals() const
{
    return TYAcousticVolumeNode::normals();
}

TYTabPoint TYBatiment::sommets() const
{
    return TYAcousticVolumeNode::sommets();
}

TYTabLPPolygon TYBatiment::faces() const
{
    return TYAcousticVolumeNode::faces();
}

TYBox TYBatiment::volEnglob() const
{
    return TYAcousticVolumeNode::volEnglob();
}

TYPoint TYBatiment::centreGravite() const
{
    return TYAcousticVolumeNode::centreGravite();
}

int TYBatiment::intersects(const OSegment3D& seg, TYTabPoint& ptList) const
{
    return TYAcousticVolumeNode::intersects(seg, ptList);
}

int TYBatiment::isInside(const TYPoint& pt) const
{
    return TYAcousticVolumeNode::isInside(pt);
}

bool TYBatiment::updateAcoustic(const bool& force) // force = false
{
    if (!getIsAcousticModified() && !force) { return true; }   // Le batiment est i¿½ jour

    register unsigned int i;
    bool ret = true;
    TYSpectre LW = TYSpectre::getEmptyLinSpectre();
    LW.setType(SPECTRE_TYPE_LW);

    remAllSrcs();

    setCurRegime(_curRegime);

    if (this->_typeDistribution == TY_PUISSANCE_IMPOSEE)
    {
        TYAcousticVolumeNode::updateAcoustic();
        LW = getCurrentSpectre();
    }
    else // TY_PUISSANCE_CALCULEE
    {
        for (i = 0; i < _tabAcousticVol.size(); i++)
        {
            TYAcousticVolume* pEtage = TYAcousticVolume::safeDownCast(_tabAcousticVol[i]->getElement());
            ret &= pEtage->updateAcoustic();
            LW = LW.sum(pEtage->getCurrentSpectre().toGPhy());
        }
    }

    TYSpectre aTYSpectre(LW.toDB());
    setRegime(aTYSpectre, -1, false);

    if (ret) { setIsAcousticModified(false); } // L'acoustique est i¿½ jour

    return ret;
}

LPTYSpectre TYBatiment::getRealPowerSpectrum()
{
    LPTYElementArray childs;
    getChilds(childs, true);
    TYSpectre* sp = new TYSpectre(-200.0);
    sp->setType(SPECTRE_TYPE_LW);
    LPTYAcousticSurface pSurf = NULL;
    for (unsigned int i = 0; i < childs.size(); i++)
    {
        pSurf = dynamic_cast<TYAcousticSurface*>(childs[i]._pObj);
        if (pSurf._pObj != nullptr)
        {
            // On s'assure que la surface appartient bien a l'etage et non a une machine a l'interieur
            TYElement* pParent = pSurf->getParent();
            while (pParent)
            {
                if (pParent->isA("TYMachine"))
                {
                    break;
                }
                else if (pParent->isA("TYBatiment"))
                {
                    *sp = sp->sumdB(*pSurf->getRealPowerSpectrum());
                    break;
                }

                pParent = pParent->getParent();
            }

        }
    }

    return sp;
}
