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
#include "Tympan/MetierSolver/ToolsMetier/OSegment3D.h"

#include "Tympan/Tools/TYProgressManager.h"

#include "Tympan/Tools/OMessageManager.h"


OPROTOINST(TYAcousticSemiCircle);


TYAcousticSemiCircle::TYAcousticSemiCircle()
{
    _name = TYNameManager::get()->generateName(getClassName());

    // Couleur par default
    float r = 255.0f, g = 67.0f, b = 133.0f;

#if TY_USE_IHM
    if (TYPreferenceManager::exists(TYDIRPREFERENCEMANAGER, "TYAcousticCircleGraphicColorR"))
    {
        TYPreferenceManager::getColor(TYDIRPREFERENCEMANAGER, "TYAcousticCircleGraphicColor", r, g, b);
    }
    else
    {
        TYPreferenceManager::setColor(TYDIRPREFERENCEMANAGER, "TYAcousticCircleGraphicColor", r, g, b);
    }
#endif // TY_USE_IHM
    OColor color;
    color.r = r / 255;
    color.g = g / 255;
    color.b = b / 255;

    setColor(color);
}

TYAcousticSemiCircle::TYAcousticSemiCircle(const TYAcousticSemiCircle& other)
{
    *this = other;
}

TYAcousticSemiCircle::~TYAcousticSemiCircle()
{
}

TYAcousticSemiCircle& TYAcousticSemiCircle::operator=(const TYAcousticSemiCircle& other)
{
    if (this != &other)
    {
        TYAcousticSurface::operator =(other);
    }
    return *this;
}

bool TYAcousticSemiCircle::operator==(const TYAcousticSemiCircle& other) const
{
    if (this != &other)
    {
        if (TYAcousticSurface::operator !=(other)) { return false; }
    }
    return true;
}

bool TYAcousticSemiCircle::operator!=(const TYAcousticSemiCircle& other) const
{
    return !operator==(other);
}

bool TYAcousticSemiCircle::deepCopy(const TYElement* pOther, bool copyId /*=true*/)
{
    return TYAcousticSurface::deepCopy(pOther, copyId);
}

std::string TYAcousticSemiCircle::toString() const
{
    return "TYAcousticSemiCircle";
}

DOM_Element TYAcousticSemiCircle::toXML(DOM_Element& domElement)
{
    DOM_Element domNewElem = TYAcousticSurface::toXML(domElement);
    return domNewElem;
}

int TYAcousticSemiCircle::fromXML(DOM_Element domElement)
{
    return TYAcousticSurface::fromXML(domElement);
}

TYTabSourcePonctuelleGeoNode TYAcousticSemiCircle::getSrcs() const
{
    return TYAcousticSurface::getSrcs();
}

TYSourcePonctuelle TYAcousticSemiCircle::srcPonctEquiv() const
{
    return TYSourcePonctuelle();
}

void TYAcousticSemiCircle::distriSrcs()
{
    // Suppression des sources existantes
    _pSrcSurf->purge();

    if ((_isRayonnant == false) || (_pSrcSurf->getDensiteSrcsH() == 0) || (_pSrcSurf->getDensiteSrcsV() == 0))
    {
        // Pas de calcul
        return;
    }

    double rayon = getDiameter() / 2.0;
    // Distance angulaire (sur la circonference de chaque cercle
    // concentrique) et nombre de sources
    int nbSrcsAngulaire = (int)(M_PI * rayon / _pSrcSurf->getDensiteSrcsH());
    nbSrcsAngulaire = nbSrcsAngulaire < 1 ? 1 : nbSrcsAngulaire;  // Au moins 1 source opposees

    double distAngulaire = M_PI / nbSrcsAngulaire;

    // Distance radiale
    int nbSrcsRadiale = (int)(rayon / _pSrcSurf->getDensiteSrcsV());
    nbSrcsRadiale = nbSrcsRadiale < 1 ? 1 : nbSrcsRadiale;  // Au moins 1 source opposees

    double distRayon = rayon / (nbSrcsRadiale + 1);

    OMatrix matrix;
    OCoord3D pt;
    double angle = 0.0;
    double posCercle = distRayon;

    // On recupere le repere local au plan du cercle
    TYRepere repere = _pBoundingRect->getRepere();
    // On place l'origine au centre du cercle pour simplifier les calculs
    repere._origin = getCenter();
    // Matrice de changement de repere
    repere.getMatChangeRep(matrix);

    for (int i = 0 ; i < nbSrcsRadiale ; i++)
    {
        for (int j = 0 ; j < nbSrcsAngulaire ; j++)
        {
            angle = (-M_PI / 2) + ((j + 1) * distAngulaire) - (distAngulaire / 2);
            pt._x = cos(angle) * posCercle;
            pt._y = sin(angle) * posCercle;
            pt._z = _offsetSources;

            // On revient dans le repere d'origine
            pt = matrix * pt;

            // Creation d'une source ponctuelle
            TYSourcePonctuelle* pSrc = new TYSourcePonctuelle();

            // Definition de sa position
            *pSrc->getPos() = pt;

            // Attribution du parent
            pSrc->setParent(this);

            // Definition des sources comme sources a directivite calculee
            pSrc->setTypeRaynt(CALCULATED);

            // Ajout de la src ponct a la src surf
            _pSrcSurf->addSrc(pSrc);

        }
        // Increment du rayon
        posCercle += distRayon;
    }
}

bool TYAcousticSemiCircle::setSrcsLw()
{
    return TYAcousticSurface::setSrcsLw(); // Appel de la methode generale
}

double TYAcousticSemiCircle::surface() const
{
    // La moitie de l'aire d'un cercle
    double rayon = getDiameter() / 2.0;
    return (M_PI * rayon * rayon) / 2.0;
}

OVector3D TYAcousticSemiCircle::normal() const
{
    return _pBoundingRect->normal();
}

OPlan TYAcousticSemiCircle::plan() const
{
    return OPlan(getCenter(), normal());
}

TYTabPoint TYAcousticSemiCircle::getContour(int n /*=-1*/) const
{
    TYTabPoint tab;

    if (n == -1)
    {
        // Prise en compte de la precision
        n = ROUND((getDiameter() / 2.0) / TYPRECISIONCIRCLE);

        // On veut au moins tous les Pi/2 (90i申)
        n = ROUND(n / 4 * 4);
    }
    else if ((getDiameter() == 0) || (n < 3))
    {
        return tab;
    }

    OMatrix matrix;
    double angle = 0.0;
    double rayon = getDiameter() / 2.0;
    double demiPi = M_PI / 2.0;

    // On recupere le repere local au plan du cercle
    TYRepere repere = _pBoundingRect->getRepere();

    // On place l'origine au centre du cercle pour simplifier les calculs
    repere._origin = getCenter();

    // Matrice de changement de repere
    repere.getMatChangeRep(matrix);

    // On connait la taille du tableau de point a retourner
    tab.reserve(n);
    TYPoint pt;

    // On calcul la position de chaque portion
    // (on travaille dans le repere local au plan du cercle)
    // Les calculs sont fait de 0 a Pi(180i申) seulement
    for (int i = 0; i < (n + 1); i++)
    {
        // Increment de la portion (sens horaire)
        angle = ((M_PI * (n - i)) / n) - demiPi;

        // Point correspondant
        pt._x = cos(angle) * rayon;
        pt._y = sin(angle) * rayon;
        pt._z = 0;

        // On revient dans le repere d'origine
        pt = matrix * pt;

        // On ajoute ce point au tableau
        tab.push_back(pt);
    }

    return tab;
}

TYTabPoint3D TYAcousticSemiCircle::getOContour(int n /*=-1*/) const
{
    TYTabPoint3D tab;

    if (n == -1)
    {
        // Prise en compte de la precision
        n = ROUND((getDiameter() / 2.0) / TYPRECISIONCIRCLE);

        // On veut au moins tous les Pi/2 (90i申)
        n = ROUND(n / 4 * 4);
    }
    else if ((getDiameter() == 0) || (n < 3))
    {
        return tab;
    }

    OMatrix matrix;
    double angle = 0.0;
    double rayon = getDiameter() / 2.0;
    double demiPi = M_PI / 2.0;

    // On recupere le repere local au plan du cercle
    ORepere3D repere = _pBoundingRect->getRepere();

    // On place l'origine au centre du cercle pour simplifier les calculs
    repere._origin = getCenter();

    // Matrice de changement de repere
    repere.getMatChangeRep(matrix);

    // On connait la taille du tableau de point a retourner
    OPoint3D pt;

    // On calcul la position de chaque portion
    // (on travaille dans le repere local au plan du cercle)
    // Les calculs sont fait de 0 a Pi(180i申) seulement
    for (int i = 0; i < (n + 1); i++)
    {
        // Increment de la portion (sens horaire)
        angle = ((M_PI * (n - i)) / n) - demiPi;

        // Point correspondant
        pt._x = cos(angle) * rayon;
        pt._y = sin(angle) * rayon;
        pt._z = 0;

        // On revient dans le repere d'origine
        pt = matrix * pt;

        // On ajoute ce point au tableau
        tab.push_back(pt);
    }

    return tab;
}
int TYAcousticSemiCircle::intersects(const TYSurfaceInterface* pSurf, OSegment3D& seg) const
{
    return TYSurfaceInterface::intersects(pSurf, seg);
}

int TYAcousticSemiCircle::intersects(const OSegment3D& seg, OPoint3D& pt) const
{
    int res = INTERS_NULLE;

    res = _pBoundingRect->intersects(seg, pt);

    if (res != INTERS_NULLE)
    {
        res = intersects(pt);
    }

    return res;
}

int TYAcousticSemiCircle::intersects(const OPoint3D& pt) const
{
    int res = INTERS_NULLE;

    // Pour que le point coupe le cercle, la dist [centre du cercle , pt]
    // doit etre <= au rayon du demi-cercle

    double distPtCentre = OVector3D(pt, getCenter()).norme();

    if (2 * distPtCentre <= getDiameter())
    {
        res = INTERS_OUI;
    }

    return res;
}

TYPoint TYAcousticSemiCircle::getCenter() const
{
    OVector3D vecCote(_pBoundingRect->_pts[0], _pBoundingRect->_pts[3]);
    OVector3D vecCentre = OVector3D(_pBoundingRect->_pts[0]) + (vecCote * 0.5);

    return TYPoint(vecCentre);
}

double TYAcousticSemiCircle::getDiameter() const
{
    return OVector3D(_pBoundingRect->_pts[0], _pBoundingRect->_pts[3]).norme();
}

void TYAcousticSemiCircle::setDiameter(double diameter)
{
    double rayon = diameter / 2.0;
    double norm = 1.41421356 * rayon;

    OPoint3D ptCenter = getCenter();
    OVector3D vecOP0(ptCenter, _pBoundingRect->_pts[0]);
    double normOP0 = vecOP0.norme();
    OVector3D vecOP1(ptCenter, _pBoundingRect->_pts[1]);
    double normOP1 = vecOP1.norme();
    OVector3D vecOP2(ptCenter, _pBoundingRect->_pts[2]);
    double normOP2 = vecOP2.norme();
    OVector3D vecOP3(ptCenter, _pBoundingRect->_pts[3]);
    double normOP3 = vecOP3.norme();

    if ((normOP0 == 0) || (normOP1 == 0) || (normOP2 == 0) || (normOP3 == 0))
    {
        // Cercle invalide
        return;
    }

    OVector3D vecPtCenter = getCenter();

    _pBoundingRect->_pts[0] = vecPtCenter + (vecOP0 * (rayon / normOP0));
    _pBoundingRect->_pts[1] = vecPtCenter + (vecOP1 * (norm / normOP1));
    _pBoundingRect->_pts[2] = vecPtCenter + (vecOP2 * (norm / normOP2));
    _pBoundingRect->_pts[3] = vecPtCenter + (vecOP3 * (rayon / normOP3));

    setIsGeometryModified(true);
}

void TYAcousticSemiCircle::exportMesh(
		std::deque<OPoint3D>& points,
                std::deque<OTriangle>& triangles) const
{
    assert(points.size()==0 &&
           "Output arguments 'points' is expected to be initially empty");
    assert(triangles.size()==0 &&
           "Output arguments 'triangles' is expected to be initially empty");

    int resolution = TYDEFAULTRESOLUTIONIONCIRCLE;
#if TY_USE_IHM
    if (TYPreferenceManager::exists(TYDIRPREFERENCEMANAGER, "ResolutionCircle"))
        resolution = TYPreferenceManager::getInt(TYDIRPREFERENCEMANAGER, "ResolutionCircle");
    else
        TYPreferenceManager::setInt(TYDIRPREFERENCEMANAGER, "ResolutionCircle", resolution);
#endif // TY_USE_IHM

    TYTabPoint3D poly = getOContour(resolution);
    OPoint3D center = getCenter();
    points.push_back(center);
    assert( resolution = poly.size() && "Inconsistency in contour size");
    for(int i=0; i<resolution; ++i)
    {
        points.push_back(poly[i]);
        OTriangle tri(center, poly[i], poly[(i+1)%resolution]);
        tri._p1=0; tri._p2=i+1; tri._p3=(i+1)%resolution+1;
    }
}
