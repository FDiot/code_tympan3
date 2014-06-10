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
#include "Tympan/GraphicIHM/DataManagerIHM/TYBoucheSurfaceWidget.h"
#endif

#ifdef TYMPAN_USE_PRECOMPILED_HEADER
#include "Tympan/MetierSolver/DataManagerMetier/TYPHMetier.h"
#endif // TYMPAN_USE_PRECOMPILED_HEADER

#include "Tympan/Tools/TYProgressManager.h"

#include "Tympan/Tools/OMessageManager.h"


TY_EXTENSION_INST(TYBoucheSurface);

TYBoucheSurface::TYBoucheSurface()
{
    _name = TYNameManager::get()->generateName(getClassName());

    distriSrcs();

    // Couleur par default
    setColor(OColor(0.5f, 0.0f, 0.0f));
}

TYBoucheSurface::TYBoucheSurface(const TYBoucheSurface& other)
{
    *this = other;
}

TYBoucheSurface::~TYBoucheSurface()
{
}

int TYBoucheSurface::fromXML(DOM_Element domElement)
{
    int retVal = TYAcousticSurface::fromXML(domElement);

    // On recupere le rectangle de la surface pour l'affecter a la source bafflee
    //    LPTYAcousticRectangle pRectangle = new TYAcousticRectangle();
    LPTYRectangle pRectangle = new TYRectangle();
    //    *pRectangle->getBoundingRect() = *getBoundingRect();
    *pRectangle = *getBoundingRect();
    LPTYSourceBafflee pSourceBafflee = this->getSourceBafflee();
    if (pSourceBafflee == NULL)
    {
        pSourceBafflee = new TYSourceBafflee();
        pSourceBafflee->setParent(this);
    }

    pSourceBafflee->setAcousticRectangle(pRectangle);

    return retVal;
}

TYBoucheSurface& TYBoucheSurface::operator=(const TYBoucheSurface& other)
{
    if (this != &other)
    {
        TYAcousticRectangle::operator =(other);
    }
    return *this;
}

bool TYBoucheSurface::operator==(const TYBoucheSurface& other) const
{
    if (this != &other)
    {
        if (TYAcousticRectangle::operator !=(other)) { return false; }
    }
    return true;
}

bool TYBoucheSurface::operator!=(const TYBoucheSurface& other) const
{
    return !operator==(other);
}

std::string TYBoucheSurface::toString() const
{
    return "TYBoucheSurface";
}

void TYBoucheSurface::distriSrcs()
{
    if (surface() <= 1.E-8)
    {
        _pSrcSurf->purge();
        return; // securite : pas de surface nulle
    }

    // La source surfacique est composee d'une seule
    // source de type TYSourceBafflee
    // Si la source n'est pas trouvee
    if (!getSourceBafflee())
    {
        // On creait la source, cela devrait etre fait qu'une seule fois a l'init...
        setSourceBafflee(new TYSourceBafflee());
    }
}

LPTYSourceBafflee TYBoucheSurface::getSourceBafflee()
{
    LPTYSourceBafflee pSrc = NULL;

    if (_pSrcSurf->getNbSrcs())
    {
        pSrc = TYSourceBafflee::safeDownCast(_pSrcSurf->getSrc(0));
    }

    return pSrc;
}

void TYBoucheSurface::setSourceBafflee(LPTYSourceBafflee pSrc)
{
    // On s'assure du type
    assert(TYSourceBafflee::safeDownCast(pSrc));

    // On vide, il doit y avoir qu'une seule face a l'index 0
    _pSrcSurf->remAllSrc();

    // Ajout
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
