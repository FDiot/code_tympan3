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

#include "Tympan/MetierSolver/ToolsMetier/OPoint3D.h"
#include "Tympan/MetierSolver/ToolsMetier/OBox.h"
#include "Tympan/Tools/TYProgressManager.h"

OPROTOINST(TYAcousticPolygon);

TYAcousticPolygon::TYAcousticPolygon()
{
    _name = TYNameManager::get()->generateName(getClassName());

    _pPolygon = new TYPolygon();
    _pPolygon->setParent(this);
}

TYAcousticPolygon::TYAcousticPolygon(const TYAcousticPolygon& other)
{
    *this = other;
}

TYAcousticPolygon::~TYAcousticPolygon()
{
}

TYAcousticPolygon& TYAcousticPolygon::operator=(const TYAcousticPolygon& other)
{
    if (this != &other)
    {
        TYAcousticSurface::operator =(other);
        _pPolygon = other._pPolygon;
    }
    return *this;
}

bool TYAcousticPolygon::operator==(const TYAcousticPolygon& other) const
{
    if (this != &other)
    {
        if (TYAcousticSurface::operator !=(other)) { return false; }
        if (_pPolygon != other._pPolygon) { return false; }
    }
    return true;
}

bool TYAcousticPolygon::operator!=(const TYAcousticPolygon& other) const
{
    return !operator==(other);
}

bool TYAcousticPolygon::deepCopy(const TYElement* pOther, bool copyId /*=true*/)
{
    if (!TYAcousticSurface::deepCopy(pOther, copyId)) { return false; }

    TYAcousticPolygon* pOtherPoly = (TYAcousticPolygon*) pOther;

    _pPolygon->deepCopy(pOtherPoly->_pPolygon, copyId);

    return true;
}

std::string TYAcousticPolygon::toString() const
{
    return "TYAcousticPolygon";
}

DOM_Element TYAcousticPolygon::toXML(DOM_Element& domElement)
{
    DOM_Element domNewElem = TYAcousticSurface::toXML(domElement);

    _pPolygon->toXML(domNewElem);

    return domNewElem;
}

int TYAcousticPolygon::fromXML(DOM_Element domElement)
{
    TYAcousticSurface::fromXML(domElement);

    DOM_Element elemCur;

    QDomNodeList childs = domElement.childNodes();
    for (unsigned int i = 0; i < childs.length(); i++)
    {
        elemCur = childs.item(i).toElement();

        _pPolygon->callFromXMLIfEqual(elemCur);
    }

    return 1;
}

TYTabSourcePonctuelleGeoNode TYAcousticPolygon::getSrcs() const
{
    return TYAcousticSurface::getSrcs();
}

TYSourcePonctuelle TYAcousticPolygon::srcPonctEquiv() const
{
    return TYSourcePonctuelle();
}

void TYAcousticPolygon::distriSrcs()
{
    // Suppression des sources existantes
    _pSrcSurf->purge();

    if ((_isRayonnant == false) || (_pSrcSurf->getDensiteSrcsH() == 0) || (_pSrcSurf->getDensiteSrcsV() == 0))
    {
        // Pas de calcul
        return;
    }

    // Tableau de points
    size_t nbPts = _pPolygon->getNbPts();

    if (nbPts <= 0) { return; }

    OPoint3D* pts = new OPoint3D[nbPts];
    for (size_t i = 0; i < nbPts; i++) { pts[i] = _pPolygon->getPoint(i); }

    // Calcul de l'offset pour la position des sources (utilisation du lancer de rayon)
    OVector3D normale = _pPolygon->normal();
    double offsetZ = _offsetSources * normale._z;

    // Rectangle de travail pour la distribution des sources
    TYRectangle BoundingRect = _pPolygon->getBoundingRect();

    // On cherche les vecteurs des cotes H et V du rectangle
    OVector3D vecH(BoundingRect._pts[0], BoundingRect._pts[1]);
    OVector3D vecV(BoundingRect._pts[0], BoundingRect._pts[3]);

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

    // nombre d'iterations (nbSrcsH * nbSrcsV);

    // Calcul des vecteurs pas en H et V (ecart entre chaque src)
    vecH = vecH * (1.0 / (double) nbSrcsH);
    vecV = vecV * (1.0 / (double) nbSrcsV);

    // Calcul du pt de depart pour le placement des srcs
    OVector3D startPt = OVector3D(BoundingRect._pts[0]) + (vecH * 0.5) + (vecV * 0.5);

    OPoint3D ptMin, ptMax;

    OGeometrie::boundingBox(pts, static_cast<int>(nbPts), ptMin, ptMax);

    OBox box(ptMin, ptMax);


    // Scan V
    for (int iV = 0; (iV < nbSrcsV); iV++)
    {
        // Scan H
        for (int iH = 0; (iH < nbSrcsH); iH++)
        {

            // Positionnement, on "parcours" la surface par des multiples
            // des vecteurs pas
            OPoint3D pos(startPt + (vecH * iH) + (vecV * iV));

            // Il faut ensuite tester si le point trouve
            // est inclu dans le polygone
            if (OGeometrie::pointInPolygonAngleSum(pos, pts, static_cast<int>(nbPts)))
            {
                // Creation d'une source ponctuelle
                TYSourcePonctuelle* pSrc = new TYSourcePonctuelle();

                // Definition de sa position
                pos._z +=  offsetZ; // on decale les sources pour le lancer de rayon selon l'orientation de la normale
                pSrc->setPos(new TYPoint(pos));

                // Attribution du parent
                pSrc->setParent(this);

                // Definition des sources comme sources a directivite calculee
                pSrc->setTypeRaynt(CALCULATED);

                // Ajout de la src ponct a la src surf
                _pSrcSurf->addSrc(pSrc);
            }
        }
    }

    delete [] pts;
}

bool TYAcousticPolygon::setSrcsLw()
{
    return TYAcousticSurface::setSrcsLw(); // Appel de la methode generale
}


double TYAcousticPolygon::surface() const
{
    return _pPolygon->surface();
}

OVector3D TYAcousticPolygon::normal() const
{
    return _pPolygon->normal();
}

OPlan TYAcousticPolygon::plan() const
{
    return _pPolygon->plan();
}

TYTabPoint TYAcousticPolygon::getContour(int n /*=-1*/) const
{
    return _pPolygon->getContour(n);
}

TYTabPoint3D TYAcousticPolygon::getOContour(int n /*=-1*/) const
{
    return _pPolygon->getOContour(n);
}

int TYAcousticPolygon::intersects(const TYSurfaceInterface* pSurf, OSegment3D& seg) const
{
    return _pPolygon->intersects(pSurf, seg);
}

int TYAcousticPolygon::intersects(const OSegment3D& seg, OPoint3D& pt) const
{
    return _pPolygon->intersects(seg, pt);
}

int TYAcousticPolygon::intersects(const OPoint3D& pt) const
{
    return _pPolygon->intersects(pt);
}
