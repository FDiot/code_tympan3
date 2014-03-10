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
#include "Tympan/GraphicIHM/DataManagerIHM/TYAcousticCircleWidget.h"
#endif

#ifdef TYMPAN_USE_PRECOMPILED_HEADER
#include "Tympan/MetierSolver/DataManagerMetier/TYPHMetier.h"
#endif // TYMPAN_USE_PRECOMPILED_HEADER
#include "Tympan/MetierSolver/ToolsMetier/OPoint3D.h"
#include "Tympan/MetierSolver/ToolsMetier/OSegment3D.h"

#include "Tympan/Tools/TYProgressManager.h"

#include "Tympan/Tools/OMessageManager.h"

#include "Tympan/MetierSolver/DataManagerMetier/ComposantGeometrique/TYGeometryNode.h"

OPROTOINST(TYAcousticCircle);
TY_EXTENSION_INST(TYAcousticCircle);

TYAcousticCircle::TYAcousticCircle()
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

TYAcousticCircle::TYAcousticCircle(const TYAcousticCircle& other)
{
    *this = other;
}

TYAcousticCircle::~TYAcousticCircle()
{
}

TYAcousticCircle& TYAcousticCircle::operator=(const TYAcousticCircle& other)
{
    if (this != &other)
    {
        TYAcousticSurface::operator =(other);
    }
    return *this;
}

bool TYAcousticCircle::operator==(const TYAcousticCircle& other) const
{
    if (this != &other)
    {
        if (TYAcousticSurface::operator !=(other)) { return false; }
    }
    return true;
}

bool TYAcousticCircle::operator!=(const TYAcousticCircle& other) const
{
    return !operator==(other);
}

bool TYAcousticCircle::deepCopy(const TYElement* pOther, bool copyId /*=true*/)
{
    return TYAcousticSurface::deepCopy(pOther, copyId);
}

std::string TYAcousticCircle::toString() const
{
    return "TYAcousticCircle";
}

DOM_Element TYAcousticCircle::toXML(DOM_Element& domElement)
{
    DOM_Element domNewElem = TYAcousticSurface::toXML(domElement);
    return domNewElem;
}

int TYAcousticCircle::fromXML(DOM_Element domElement)
{
    return TYAcousticSurface::fromXML(domElement);
}

TYTabSourcePonctuelleGeoNode TYAcousticCircle::getSrcs() const
{
    return TYAcousticSurface::getSrcs();
}

TYSourcePonctuelle TYAcousticCircle::srcPonctEquiv() const
{
    return TYSourcePonctuelle();
}

void TYAcousticCircle::distriSrcs()
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
    int nbSrcsAngulaire = (int)(2 * M_PI * rayon / _pSrcSurf->getDensiteSrcsH());
    nbSrcsAngulaire = nbSrcsAngulaire < 2 ? 2 : nbSrcsAngulaire;  // Au moins deux sources diametralement opposees
    const double distAngulaire = 2 * M_PI / nbSrcsAngulaire;
    // Distance radiale
    const int nbSrcsRadiale = (int)(rayon / _pSrcSurf->getDensiteSrcsV());
    double distRayon = 0;
    if (nbSrcsRadiale > 0)
    {
        distRayon = rayon / (nbSrcsRadiale + 1);
    }

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

    // On ajoute une source au centre dans tous les cas, on s'assure ainsi
    // d'avoir toujours au moins une source sur la surface si les densites
    // ne sont pas nulles
    pt._x = 0.000, pt._y = 0.000; pt._z = _offsetSources;
    // On revient dans le repere d'origine
    pt = matrix * pt;
    // Creation de la source
    TYSourcePonctuelle* pSrc = new TYSourcePonctuelle();
    *pSrc->getPos() = pt;
    pSrc->setTypeRaynt(CALCULATED);
    _pSrcSurf->addSrc(pSrc);


    for (int i = 0 ; i < nbSrcsRadiale ; i++)
    {
        for (int j = 0 ; j < nbSrcsAngulaire ; j++)
        {
            angle = j * distAngulaire;;
            pt._x = cos(angle) * posCercle;
            pt._y = sin(angle) * posCercle;
            pt._z = _offsetSources;

            // On revient dans le repere d'origine
            pt = matrix * pt;

            // Creation d'une source ponctuelle
            TYSourcePonctuelle* pSrc = new TYSourcePonctuelle();

            // Definition de sa position
            *pSrc->getPos() = pt;

            // Definition des sources comme sources a directivite calculee
            pSrc->setTypeRaynt(CALCULATED);

            // Ajout de la src ponct a la src surf
            _pSrcSurf->addSrc(pSrc);
        }

        // Increment du rayon
        posCercle += distRayon;
    }
}

bool TYAcousticCircle::setSrcsLw()
{
    return TYAcousticSurface::setSrcsLw(); // Appel de la methode generale
}


double TYAcousticCircle::surface() const
{
    // Pi * Ri¿½
    double rayon = getDiameter() / 2.0;
    return (M_PI * rayon * rayon);
}

OVector3D TYAcousticCircle::normal() const
{
    return _pBoundingRect->normal();
}

OPlan TYAcousticCircle::plan() const
{
    return OPlan(getCenter(), normal());
}

TYTabPoint TYAcousticCircle::getContour(int n /*=-1*/) const
{
    TYTabPoint tab;

    if (n == -1)
    {
        // Prise en compte de la precision
        n = ROUND((getDiameter() / 2.0) / TYPRECISIONCIRCLE);

        // On veut au moins tous les Pi/2 (90i¿½)
        n = ROUND(n / 4 * 4);
    }
    else if ((getDiameter() == 0) || (n < 3))
    {
        return tab;
    }

    OMatrix matrix;
    TYSegment seg;
    double angle = 0.0;
    double rayon = getDiameter() / 2.0;

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
    for (int i = 0; i < n; i++)
    {
        // Increment de la portion (sens horaire)
        angle = (M_2PI * (n - i)) / n;

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

TYTabPoint3D TYAcousticCircle::getOContour(int n) const
{
    // TODO This is a mere duplicate of the getContour method and
    // needs to be factorized
    TYTabPoint3D tab;

    if (n == -1)
    {
        // Prise en compte de la precision
        n = ROUND((getDiameter() / 2.0) / TYPRECISIONCIRCLE);

        // On veut au moins tous les Pi/2 (90i¿½)
        n = ROUND(n / 4 * 4);
    }
    else if ((getDiameter() == 0) || (n < 3))
    {
        return tab;
    }

    OMatrix matrix;
    double angle = 0.0;
    double rayon = getDiameter() / 2.0;

    // On recupere le repere local au plan du cercle
    ORepere3D repere = _pBoundingRect->getRepere();

    // On place l'origine au centre du cercle pour simplifier les calculs
    repere._origin = getCenter();

    // Matrice de changement de repere
    repere.getMatChangeRep(matrix);

    // On connait la taille du tableau de point a retourner
    //tab.reserve(n);
    OPoint3D pt;

    // On calcul la position de chaque portion
    // (on travaille dans le repere local au plan du cercle)
    for (int i = 0; i < n; i++)
    {
        // Increment de la portion (sens horaire)
        angle = (M_2PI * (n - i)) / n;

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

int TYAcousticCircle::intersects(const TYSurfaceInterface* pSurf, OSegment3D& seg) const
{
    return TYSurfaceInterface::intersects(pSurf, seg);
}

int TYAcousticCircle::intersects(const OSegment3D& seg, OPoint3D& pt) const
{
    int res = INTERS_NULLE;

    res = _pBoundingRect->intersects(seg, pt);

    if (res != INTERS_NULLE)
    {
        res = intersects(pt);
    }

    return res;
}

int TYAcousticCircle::intersects(const OPoint3D& pt) const
{
    int res = INTERS_NULLE;

    // Pour que le point coupe le cercle, la dist [centre du cercle , pt]
    // doit etre <= au rayon du cercle

    double distPtCentre = OVector3D(pt, getCenter()).norme();

    if (2.0 * distPtCentre <= getDiameter())
    {
        res = INTERS_OUI;
    }

    return res;
}

TYPoint TYAcousticCircle::getCenter() const
{
    OVector3D vecDiagonale(_pBoundingRect->_pts[0], _pBoundingRect->_pts[2]);
    OVector3D vecCentre = OVector3D(_pBoundingRect->_pts[0]) + (vecDiagonale * 0.5);

    return TYPoint(vecCentre);
}

double TYAcousticCircle::getDiameter() const
{
    return OVector3D(_pBoundingRect->_pts[0], _pBoundingRect->_pts[1]).norme();
}

void TYAcousticCircle::setDiameter(double diameter)
{
    double rayon = diameter / 2.0;
    double norm = sqrt(2.0) * rayon;

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

    _pBoundingRect->_pts[0] = vecPtCenter + (vecOP0 * (norm / normOP0));
    _pBoundingRect->_pts[1] = vecPtCenter + (vecOP1 * (norm / normOP1));
    _pBoundingRect->_pts[2] = vecPtCenter + (vecOP2 * (norm / normOP2));
    _pBoundingRect->_pts[3] = vecPtCenter + (vecOP3 * (norm / normOP3));

    setIsGeometryModified(true);
}

void TYAcousticCircle::exportMesh(
    std::deque<OPoint3D>& points,
    std::deque<OTriangle>& triangles,
    const TYGeometryNode& geonode) const
{
    assert(points.size() == 0 &&
           "Output arguments 'points' is expected to be initially empty");
    assert(triangles.size() == 0 &&
           "Output arguments 'triangles' is expected to be initially empty");

    int resolution = TYDEFAULTRESOLUTIONIONCIRCLE;
#if TY_USE_IHM
    if (TYPreferenceManager::exists(TYDIRPREFERENCEMANAGER, "ResolutionCircle"))
    {
        resolution = TYPreferenceManager::getInt(TYDIRPREFERENCEMANAGER, "ResolutionCircle");
    }
    else
    {
        TYPreferenceManager::setInt(TYDIRPREFERENCEMANAGER, "ResolutionCircle", resolution);
    }
#endif // TY_USE_IHM

    TYTabPoint3D poly = getOContour(resolution);
    OPoint3D center = geonode.localToGlobal(getCenter());
    points.push_back(center);
    points.push_back(geonode.localToGlobal(poly[0]));
    for (int i = 1; i < resolution; ++i)
    {
        // poly[i] (local) become points[i+1] (global)
        points.push_back(geonode.localToGlobal(poly[i]));
        OTriangle tri(center, points[i], points[i + 1]);
        tri._p1 = 0; tri._p2 = i; tri._p3 = i + 1;
    }
    OTriangle tri(center, points[resolution], points[1]);
    tri._p1 = 0; tri._p2 = resolution; tri._p3 = 1;
}
