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

#ifdef TYMPAN_USE_PRECOMPILED_HEADER
#include "Tympan/MetierSolver/DataManagerMetier/TYPHMetier.h"
#endif // TYMPAN_USE_PRECOMPILED_HEADER

#include "Tympan/core/logging.h"
#include "Tympan/models/common/3d.h"
#if TY_USE_IHM
  #include "Tympan/GraphicIHM/DataManagerIHM/TYAcousticRectangleWidget.h"
  #include "Tympan/GraphicIHM/DataManagerGraphic/TYAcousticRectangleGraphic.h"
#endif


TY_EXTENSION_INST(TYAcousticRectangle);
TY_EXT_GRAPHIC_INST(TYAcousticRectangle);

TYAcousticRectangle::TYAcousticRectangle()
{
    _name = TYNameManager::get()->generateName(getClassName());
}

TYAcousticRectangle::TYAcousticRectangle(const TYAcousticRectangle& other)
{
    *this = other;
}

TYAcousticRectangle::~TYAcousticRectangle()
{
}

TYAcousticRectangle& TYAcousticRectangle::operator=(const TYAcousticRectangle& other)
{
    if (this != &other)
    {
        TYAcousticSurface::operator =(other);
    }
    return *this;
}

bool TYAcousticRectangle::operator==(const TYAcousticRectangle& other) const
{
    if (this != &other)
    {
        if (TYAcousticSurface::operator !=(other)) { return false; }
    }
    return true;
}

bool TYAcousticRectangle::operator!=(const TYAcousticRectangle& other) const
{
    return !operator==(other);
}

bool TYAcousticRectangle::deepCopy(const TYElement* pOther, bool copyId /*=true*/)
{
    return TYAcousticSurface::deepCopy(pOther, copyId);
}

std::string TYAcousticRectangle::toString() const
{
    return "TYAcousticRectangle";
}

DOM_Element TYAcousticRectangle::toXML(DOM_Element& domElement)
{
    DOM_Element domNewElem = TYAcousticSurface::toXML(domElement);
    return domNewElem;
}

int TYAcousticRectangle::fromXML(DOM_Element domElement)
{
    return TYAcousticSurface::fromXML(domElement);
}

TYTabSourcePonctuelleGeoNode TYAcousticRectangle::getSrcs() const
{
    return TYAcousticSurface::getSrcs();
}

TYSourcePonctuelle TYAcousticRectangle::srcPonctEquiv() const
{
    return TYSourcePonctuelle();
}

void TYAcousticRectangle::distriSrcs()
{
    // Suppression des sources existantes
    _pSrcSurf->purge();

    if (!_isRayonnant || surface() <= 1.E-8)  // Face non rayonnante
    {
        // Pas de calcul
        return;
    }

    // On cherche les vecteurs des cotes H et V du rectangle
    OVector3D vecH(_pBoundingRect->_pts[0], _pBoundingRect->_pts[1]);
    OVector3D vecV(_pBoundingRect->_pts[0], _pBoundingRect->_pts[3]);

    // On calcul le nb de source en H et V
    int nbSrcsH = (int)(vecH.norme() / _pSrcSurf->getDensiteSrcsH());
    int nbSrcsV = (int)(vecV.norme() / _pSrcSurf->getDensiteSrcsV());

    // On essaye de se rapprocher au mieux de la densite totale (H*V)
    // (On veut au moins 1 source)
    if ((nbSrcsH * nbSrcsV) != (surface() * _pSrcSurf->getDensiteSrcs()))
    {
        nbSrcsV ? nbSrcsV : nbSrcsV++;
        nbSrcsH ? nbSrcsH : nbSrcsH++;
    }

    // Nombre d'iterations (nbSrcsH * nbSrcsV);

    // Calcul des vecteurs pas en H et V (ecart entre chaque src)
    vecH = vecH * (1.0 / (double) nbSrcsH);
    vecV = vecV * (1.0 / (double) nbSrcsV);

    // Calcul du pt de depart pour le placement des srcs
    OVector3D startPt = OVector3D(_pBoundingRect->_pts[0]) + (vecH * 0.5) + (vecV * 0.5);

    // Scan V
    for (int iV = 0; (iV < nbSrcsV); iV++)
    {
        // Petite economie de calcul
        OVector3D offsetV = startPt + (vecV * iV);

        // Scan H
        for (int iH = 0; (iH < nbSrcsH); iH++)
        {

            // Creation d'une source ponctuelle
            TYSourcePonctuelle* pSrc = new TYSourcePonctuelle();

            // Positionnement, on "parcours" la surface par des multiples
            // des vecteurs pas
            *pSrc->getPos() = offsetV + (vecH * iH);

            // On decale les sources de la surface pour le lancer de rayons
            pSrc->getPos()->_z = _offsetSources;

            // Attribution du parent
            pSrc->setParent(this);

            // Definition des sources comme sources a directivite calculee
            pSrc->setTypeRaynt(CALCULATED);

            // Ajout de la src ponct a la src surf
            _pSrcSurf->addSrc(pSrc);
        }
    }

}

bool TYAcousticRectangle::setSrcsLw()
{
    return TYAcousticSurface::setSrcsLw(); // Appel de la methode generale
}


double TYAcousticRectangle::surface() const
{
    return _pBoundingRect->surface();
}

OVector3D TYAcousticRectangle::normal() const
{
    return _pBoundingRect->normal();
}

OPlan TYAcousticRectangle::plan() const
{
    return _pBoundingRect->plan();
}

TYTabPoint TYAcousticRectangle::getContour(int n /*=-1*/) const
{
    return _pBoundingRect->getContour(n);
}

TYTabPoint3D TYAcousticRectangle::getOContour(int n /*=-1*/) const
{
    return _pBoundingRect->getOContour(n);
}

int TYAcousticRectangle::intersects(const TYSurfaceInterface* pSurf, OSegment3D& seg) const
{
    return _pBoundingRect->intersects(pSurf, seg);
}

int TYAcousticRectangle::intersects(const OSegment3D& seg, OPoint3D& pt) const
{
    return _pBoundingRect->intersects(seg, pt);
}
