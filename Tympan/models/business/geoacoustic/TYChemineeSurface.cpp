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


#include "Tympan/core/logging.h"
#if TY_USE_IHM
  #include "Tympan/gui/widgets/TYChemineeSurfaceWidget.h"
#endif


#include "TYChemineeSurface.h"

TY_EXTENSION_INST(TYChemineeSurface);

TYChemineeSurface::TYChemineeSurface()
{
    _name = TYNameManager::get()->generateName(getClassName());

    distriSrcs();

    // Couleur par default
    setColor(OColor(1.0f, 0.5f, 0.0f));
}

TYChemineeSurface::TYChemineeSurface(const TYChemineeSurface& other)
{
    *this = other;
}

TYChemineeSurface::~TYChemineeSurface()
{
}

int TYChemineeSurface::fromXML(DOM_Element domElement)
{
    int retVal = TYAcousticSurface::fromXML(domElement);

    // On recupere le rectangle de la surface pour l'affecter a la source bafflee
    //    LPTYAcousticRectangle pRectangle = new TYAcousticRectangle();
    LPTYRectangle pRectangle = new TYRectangle();
    //    *pRectangle->getBoundingRect() = *getBoundingRect();
    *pRectangle = *getBoundingRect();

    LPTYSourceCheminee pSrcCheminee = this->getSourceCheminee();
    if (pSrcCheminee == NULL)
    {
        pSrcCheminee = new TYSourceCheminee();
        pSrcCheminee->setParent(this);
    }

    pSrcCheminee->setAcousticRectangle(pRectangle);

    return retVal;
}

TYChemineeSurface& TYChemineeSurface::operator=(const TYChemineeSurface& other)
{
    if (this != &other)
    {
        TYAcousticRectangle::operator =(other);
    }
    return *this;
}

bool TYChemineeSurface::operator==(const TYChemineeSurface& other) const
{
    if (this != &other)
    {
        if (TYAcousticRectangle::operator !=(other)) { return false; }
    }
    return true;
}

bool TYChemineeSurface::operator!=(const TYChemineeSurface& other) const
{
    return !operator==(other);
}

std::string TYChemineeSurface::toString() const
{
    return "TYChemineeSurface";
}

void TYChemineeSurface::distriSrcs()
{
    if (surface() <= 1.E-8)
    {
        _pSrcSurf->purge();
        return; // securite : pas de surface nulle
    }

    // La source surfacique est composee d'une seule
    // source de type TYSourceCheminee
    // Si la source n'est pas trouvee
    if (!getSourceCheminee())
    {
        // On creait la source, cela devrait etre fait qu'une seule fois a l'init...
        setSourceCheminee(new TYSourceCheminee());
    }
    // To define directivity
    double specificSize = getBoundingRect()->getCircleEqDiameter() / 2.0;
    OVector3D faceNormal = normal();
    faceNormal.normalize();
    int type = TYComputedDirectivity::Chimney;
    getSourceCheminee()->setDirectivity( new TYComputedDirectivity(faceNormal, type, specificSize) );
}

LPTYSourceCheminee TYChemineeSurface::getSourceCheminee()
{
    LPTYSourceCheminee pSrc = NULL;

    if (_pSrcSurf->getNbSrcs())
    {
        pSrc = TYSourceCheminee::safeDownCast(_pSrcSurf->getSrc(0));
    }

    return pSrc;
}

void TYChemineeSurface::setSourceCheminee(LPTYSourceCheminee pSrc)
{
    // On s'assure du type
    assert(TYSourceCheminee::safeDownCast(pSrc));

    // On vide, il doit y avoir qu'une seule face a l'index 0
    _pSrcSurf->remAllSrc();


    // Ajout
    LPTYPoint pPoint = pSrc->getPos();
    pPoint->_z+=_offsetSources;
    _pSrcSurf->addSrc(pSrc);

    // Cette face est son parent
    pSrc->setParent(this);

    // Cette face est aussi le support de la source bafflee associee
    // Uniquement si 'this' est un smart pointer...
    if (getRefCount() > 0)
    {
        //        pSrc->setAcousticRectangle(this);
        pSrc->setAcousticRectangle(this->getBoundingRect());
    }
    else
    {
        //        LPTYAcousticRectangle temp = new TYAcousticRectangle(*this);
        LPTYRectangle temp = new TYRectangle(*((*this).getBoundingRect()));
        pSrc->setAcousticRectangle(temp);
    }
}
