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
#include "Tympan/gui/widgets/TYRectangleWidget.h"
#include "Tympan/GraphicIHM/DataManagerGraphic/TYRectangleGraphic.h"
#endif

#ifdef TYMPAN_USE_PRECOMPILED_HEADER
#include "Tympan/models/business/TYPHMetier.h"
#endif // TYMPAN_USE_PRECOMPILED_HEADER

#include "Tympan/models/common/3d.h"
#include "Tympan/models/business/TYPreferenceManager.h"

#include "Tympan/core/logging.h"
#include "Tympan/models/common/triangle.h"
#include "Tympan/core/defines.h"

TY_EXTENSION_INST(TYRectangle);
TY_EXT_GRAPHIC_INST(TYRectangle);

const int TYRectangle::_nbPts = 4;

TYRectangle::TYRectangle()
{
    _name = TYNameManager::get()->generateName(getClassName());

    static const float defaultLon = 1.0;
    static const float defaultLarg = 1.0;

    setDimension(defaultLon, defaultLarg);
}

TYRectangle::TYRectangle(const TYRectangle& other)
{
    *this = other;
}

TYRectangle::TYRectangle(const TYPoint& pt0, const TYPoint& pt1, const TYPoint& pt2, const TYPoint& pt3)
{
    _name = TYNameManager::get()->generateName(getClassName());

    _pts[0] = pt0;
    _pts[1] = pt1;
    _pts[2] = pt2;
    _pts[3] = pt3;

    // Calcul du plan sous jascent a la face
    _plan = plan();

    // Calcul du rectangle englobant
    OPoint3D pts[4];

    for (int i = 0; i < 4; i++) { pts[i] = _pts[i]; }
    OPoint3D ptMin, ptMax;

    OGeometrie::boundingBox(pts, _nbPts, ptMin, ptMax);

    _box._max = ptMax;
    _box._min = ptMin;

    // Calcul de la normale
    _normale = normal();

}

TYRectangle::TYRectangle(const TYSegment& seg0, const TYSegment& seg1)
{
    _name = TYNameManager::get()->generateName(getClassName());

    _pts[0] = seg0._ptA;
    _pts[1] = seg0._ptB;
    _pts[2] = seg1._ptB;

    OVector3D vecPt0(_pts[0]);
    OVector3D vecPt1(_pts[1]);
    OVector3D vecPt2(_pts[2]);

    OVector3D vecBC = vecPt2 - vecPt1;
    OVector3D vecPt3 = vecBC + vecPt0;

    _pts[3] = vecPt3;

    // Calcul du plan sous jascent a la face
    _plan = plan();

    // Calcul du rectangle englobant
    OPoint3D pts[4];

    for (int i = 0; i < 4; i++) { pts[i] = _pts[i]; }
    OPoint3D ptMin, ptMax;

    OGeometrie::boundingBox(pts, _nbPts, ptMin, ptMax);

    _box._max = ptMax;
    _box._min = ptMin;

    // Calcul de la normale
    _normale = normal();
}

TYRectangle::~TYRectangle()
{
}

void TYRectangle::set(TYPoint pt0, TYPoint pt1, TYPoint pt2, TYPoint pt3)
{
    _pts[0] = pt0;
    _pts[1] = pt1;
    _pts[2] = pt2;
    _pts[3] = pt3;

    // Calcul du plan sous jascent a la face
    _plan = plan();

    // Calcul du rectangle englobant
    OPoint3D pts[4];

    for (int i = 0; i < 4; i++) { pts[i] = _pts[i]; }
    OPoint3D ptMin, ptMax;

    OGeometrie::boundingBox(pts, _nbPts, ptMin, ptMax);

    _box._max = ptMax;
    _box._min = ptMin;

    // Calcul de la normale
    _normale = normal();

    setIsGeometryModified(true);
}

void TYRectangle::setDimension(float lon, float haut)
{
    float demiLon = lon / 2.0;
    float demiHaut = haut / 2.0;

    _pts[0].set(-demiLon, demiHaut, 0);
    _pts[1].set(demiLon, demiHaut, 0);
    _pts[2].set(demiLon, -demiHaut, 0);
    _pts[3].set(-demiLon, -demiHaut, 0);

    // Calcul du plan sous jascent a la face
    _plan = plan();

    // Calcul du rectangle englobant
    OPoint3D pts[4];

    for (int i = 0; i < 4; i++) { pts[i] = _pts[i]; }
    OPoint3D ptMin, ptMax;

    OGeometrie::boundingBox(pts, _nbPts, ptMin, ptMax);

    _box._max = ptMax;
    _box._min = ptMin;

    // Calcul de la normale
    _normale = normal();

    setIsGeometryModified(true);
}

void TYRectangle::setSize(float sizeX, float sizeY)
{
    setDimension(sizeX, sizeY);
}

void TYRectangle::getSize(float& sizeX, float& sizeY)
{
    sizeX = getSizeX();
    sizeY = getSizeY();
}

float TYRectangle::getSizeX()
{
    return OVector3D(_pts[0], _pts[1]).norme();
}

float TYRectangle::getSizeY()
{
    return OVector3D(_pts[1], _pts[2]).norme();
}

double TYRectangle::getMinX() const
{
    double minX = _pts[0]._x;
    for (unsigned int i = 0; i < 3; ++i)
        if (_pts[i + 1]._x < minX)
        {
            minX = _pts[i + 1]._x;
        }

    return minX;
}

double TYRectangle::getMinY() const
{
    double minY = _pts[0]._y;
    for (unsigned int i = 0; i < 3; ++i)
        if (_pts[i + 1]._y < minY)
        {
            minY = _pts[i + 1]._y;
        }

    return minY;
}

double TYRectangle::getMaxY() const
{
    double maxY = _pts[0]._y;
    for (unsigned int i = 0; i < 3; ++i)
        if (_pts[i + 1]._y > maxY)
        {
            maxY = _pts[i + 1]._y;
        }

    return maxY;
}

void TYRectangle::scale(float factorX, float factorY)
{
    int i;
    for (i = 0; i < 4; i++)
    {
        _pts[i]._x *= factorX;
        _pts[i]._y *= factorY;
    }

    // Calcul du plan sous jascent a la face
    _plan = plan();

    // Calcul du rectangle englobant
    OPoint3D pts[4];

    for (i = 0; i < 4; i++) { pts[i] = _pts[i]; }
    OPoint3D ptMin, ptMax;

    OGeometrie::boundingBox(pts, _nbPts, ptMin, ptMax);

    _box._max = ptMax;
    _box._min = ptMin;

    // Calcul de la normale
    _normale = normal();

    setIsGeometryModified(true);
}

double TYRectangle::getDiagSize()
{
    double longueur = (_pts[2]._x - _pts[0]._x) * (_pts[2]._x - _pts[0]._x) +
                      (_pts[2]._y - _pts[0]._y) * (_pts[2]._y - _pts[0]._y) +
                      (_pts[2]._z - _pts[0]._z) * (_pts[2]._z - _pts[0]._z) ;
    return sqrt(longueur);
}

double TYRectangle::getCircleEqDiameter()
{
    return sqrt(surface() / M_PI) * 2.0;

}

bool TYRectangle::intersectRect(LPTYRectangle pRect, TYPoint originRect/* = TYPoint(0,0,0)*/, TYPoint origin/* = TYPoint(0,0,0)*/)
{
    int i;

    TYTabPoint tabPtMid;
    tabPtMid.push_back(TYSegment(_pts[0], _pts[1]).centreOf());
    tabPtMid.push_back(TYSegment(_pts[1], _pts[2]).centreOf());
    tabPtMid.push_back(TYSegment(_pts[2], _pts[3]).centreOf());
    tabPtMid.push_back(TYSegment(_pts[3], _pts[0]).centreOf());

    for (i = 0; i < 4; i++)
    {
        if (((tabPtMid[i]._x + origin._x) < (pRect->_pts[2]._x + originRect._x)) &&
            ((tabPtMid[i]._x + origin._x) > (pRect->_pts[3]._x + originRect._x)) &&
            ((tabPtMid[i]._y + origin._y) > (pRect->_pts[2]._y + originRect._y)) &&
            ((tabPtMid[i]._y + origin._y) < (pRect->_pts[0]._y + originRect._y)))
        {
            return true;
        }
    }

    for (i = 0; i < 4; i++)
    {
        if (((_pts[i]._x + origin._x) < (pRect->_pts[2]._x + originRect._x)) &&
            ((_pts[i]._x + origin._x) > (pRect->_pts[3]._x + originRect._x)) &&
            ((_pts[i]._y + origin._y) > (pRect->_pts[2]._y + originRect._y)) &&
            ((_pts[i]._y + origin._y) < (pRect->_pts[0]._y + originRect._y)))
        {
            return true;
        }
    }

    tabPtMid.clear();

    tabPtMid.push_back(TYSegment(pRect->_pts[0], pRect->_pts[1]).centreOf());
    tabPtMid.push_back(TYSegment(pRect->_pts[1], pRect->_pts[2]).centreOf());
    tabPtMid.push_back(TYSegment(pRect->_pts[2], pRect->_pts[3]).centreOf());
    tabPtMid.push_back(TYSegment(pRect->_pts[3], pRect->_pts[0]).centreOf());

    for (i = 0; i < 4; i++)
    {
        if (((tabPtMid[i]._x + originRect._x) < (_pts[2]._x + origin._x)) &&
            ((tabPtMid[i]._x + originRect._x) > (_pts[3]._x + origin._x)) &&
            ((tabPtMid[i]._y + originRect._y) > (_pts[2]._y + origin._y)) &&
            ((tabPtMid[i]._y + originRect._y) < (_pts[0]._y + origin._y)))
        {
            return true;
        }
    }

    for (i = 0; i < 4; i++)
    {
        if (((pRect->_pts[i]._x + originRect._x) < (_pts[2]._x + origin._x)) &&
            ((pRect->_pts[i]._x + originRect._x) > (_pts[3]._x + origin._x)) &&
            ((pRect->_pts[i]._y + originRect._y) > (_pts[2]._y + origin._y)) &&
            ((pRect->_pts[i]._y + originRect._y) < (_pts[0]._y + origin._y)))
        {
            return true;
        }
    }

    return false;
}

TYRectangle& TYRectangle::operator=(const TYRectangle& other)
{
    if (this != &other)
    {
        TYElement::operator =(other);
        for (int i = 0; i < 4; i++)
        {
            _pts[i] = other._pts[i];
        }
    }

    // Calcul du plan sous jascent a la face
    _plan = plan();

    // Calcul du rectangle englobant
    OPoint3D pts[4];

    for (int i = 0; i < 4; i++) { pts[i] = _pts[i]; }
    OPoint3D ptMin, ptMax;

    OGeometrie::boundingBox(pts, _nbPts, ptMin, ptMax);

    _box._max = ptMax;
    _box._min = ptMin;

    // Calcul de la normale
    _normale = normal();

    return *this;
}

bool TYRectangle::operator==(const TYRectangle& other) const
{
    if (this != &other)
    {
        if (TYElement::operator !=(other)) { return false; }
        for (int i = 0; i < 4; i++)
            if (_pts[i] != other._pts[i])
            {
                return false;
            }
    }
    return true;
}

bool TYRectangle::operator!=(const TYRectangle& other) const
{
    return !operator==(other);
}

bool TYRectangle::deepCopy(const TYElement* pOther, bool copyId /*=true*/)
{
    if (!TYElement::deepCopy(pOther, copyId)) { return false; }

    int i;

    for (i = 0; i < 4; i++)
    {
        TYPoint pt;
        pt.deepCopy(&((TYRectangle*) pOther)->_pts[i], copyId);
        _pts[i] = pt;
    }

    // Calcul du plan sous jascent a la face
    _plan = plan();

    // Calcul du rectangle englobant
    OPoint3D pts[4];

    for (i = 0; i < 4; i++) { pts[i] = _pts[i]; }
    OPoint3D ptMin, ptMax;

    OGeometrie::boundingBox(pts, _nbPts, ptMin, ptMax);

    _box._max = ptMax;
    _box._min = ptMin;

    // Calcul de la normale
    _normale = normal();

    return true;
}

std::string TYRectangle::toString() const
{
    std::string str = "TYRectangle";
    for (int i = 0; i < 4; i++)
    {
        str += "\n\tPt" + intToStr(i) + "=" + _pts[i].toString();
    }
    return str;
}

DOM_Element TYRectangle::toXML(DOM_Element& domElement)
{
    DOM_Element domNewElem = TYElement::toXML(domElement);

    _pts[0].toXML(domNewElem);
    _pts[1].toXML(domNewElem);
    _pts[2].toXML(domNewElem);
    _pts[3].toXML(domNewElem);

    return domNewElem;
}

int TYRectangle::fromXML(DOM_Element domElement)
{
    TYElement::fromXML(domElement);

    int ptNb = 0;
    unsigned int i;
    DOM_Element elemCur;

    QDomNodeList childs = domElement.childNodes();

    for (i = 0; i < childs.length(); i++)
    {
        elemCur = childs.item(i).toElement();

        if (ptNb < 4)
        {
            // Increment l'indice si le pt est trouve
            _pts[ptNb].callFromXMLIfEqual(elemCur) ? ptNb++ : ptNb;
        }
    }

    // Calcul du plan sous jascent a la face
    _plan = plan();

    // Calcul du rectangle englobant
    OPoint3D pts[4];

    for (i = 0; i < 4; i++) { pts[i] = _pts[i]; }
    OPoint3D ptMin, ptMax;

    OGeometrie::boundingBox(pts, _nbPts, ptMin, ptMax);

    _box._max = ptMax;
    _box._min = ptMin;

    // Calcul de la normale
    _normale = normal();

    return 1;
}

double TYRectangle::surface() const
{
    // Largeur x Longeur
    TYSegment segLargeur(_pts[0], _pts[1]);
    TYSegment segLongueur(_pts[1], _pts[2]);

    return (segLargeur.longueur() * segLongueur.longueur());
}

OVector3D TYRectangle::normal() const
{
    // On calcul la normale avec les 3 1ers points
    OVector3D vecPt0(_pts[0]);
    OVector3D vecPt1(_pts[1]);
    OVector3D vecPt2(_pts[2]);

    // On inverse le signe afin de respecter la convention definie
    // pour le sens de rotation du numero des points (horaire)
    return (vecPt0.normal(vecPt1, vecPt2) * (-1));
    //return (vecPt0.normal(vecPt1, vecPt2));
}

OPlan TYRectangle::plan() const
{
    return OPlan(_pts[0], _pts[1], _pts[2]);
}

TYTabPoint TYRectangle::getContour(int /*n=-1*/) const
{
    TYTabPoint tab;

    for (int i = 0; i < 4; i++)
    {
        tab.push_back(_pts[i]);
    }

    return tab;
}

TYTabPoint3D TYRectangle::getOContour(int /*n=-1*/) const
{
    TYTabPoint3D tab;

    for (int i = 0; i < 4; i++)
    {
        tab.push_back(_pts[i]);
    }

    return tab;
}

int TYRectangle::intersects(const TYSurfaceInterface* pSurf, OSegment3D& seg) const
{
    return TYSurfaceInterface::intersects(pSurf, seg);
}

int TYRectangle::intersects(const OSegment3D& seg, OPoint3D& pt) const
{
    int res = INTERS_NULLE;

    // On cherche le pt d'intersection avec le plan
    res = _plan.intersectsSegment(seg._ptA, seg._ptB, pt);

    if (res != INTERS_NULLE)
    {
        // Il faut ensuite tester si le point d'intersection trouve
        // est inclu dans le rectangle
        res = intersects(pt);
    }

    return res;
}

int TYRectangle::intersects(const OPoint3D& pt) const
{
    int res = INTERS_NULLE;

    // Test
#if TY_USE_IHM
    if (OGeometrie::pointInPolygonRayCasting(pt, _pts, _nbPts))
    {
#else
    if (OGeometrie::pointInPolygonAngleSum(pt, _pts, _nbPts))
    {
#endif
        res = INTERS_OUI;
    }

    return res;
}

bool TYRectangle::isValid() const
{
    // 4 points -> 4 plans possibles
    OPlan plans[4];
    int i;
    plans[0] = OPlan(_pts[0], _pts[1], _pts[2]);
    plans[1] = OPlan(_pts[0], _pts[1], _pts[3]);
    plans[2] = OPlan(_pts[0], _pts[2], _pts[3]);
    plans[3] = OPlan(_pts[1], _pts[2], _pts[3]);

    double seuilConfondus = TYSEUILCONFONDUS;

#if TY_USE_IHM
    if (TYPreferenceManager::exists(TYDIRPREFERENCEMANAGER, "SeuilConfondus"))
    {
        seuilConfondus = TYPreferenceManager::getDouble(TYDIRPREFERENCEMANAGER, "SeuilConfondus");
    }
    else
    {
        TYPreferenceManager::setDouble(TYDIRPREFERENCEMANAGER, "SeuilConfondus", seuilConfondus);
    }
#endif

    // On arrondi avec la prise en compte du seuil de precision
    for (i = 0; i < 4; i++)
    {
        plans[i]._a = ROUND(plans[i]._a / seuilConfondus);
        plans[i]._b = ROUND(plans[i]._b / seuilConfondus);
        plans[i]._c = ROUND(plans[i]._c / seuilConfondus);
        plans[i]._d = ROUND(plans[i]._d / seuilConfondus);
    }

    // Les plans doivent etre les memes
    for (i = 0; i < 3; i++)
    {
        if (plans[i] != plans[i + 1]) { return false; }
    }

    // 4 points -> 4 vecteurs
    OVector3D vecs[4];
    vecs[0] = OVector3D(_pts[0], _pts[1]);
    vecs[1] = OVector3D(_pts[1], _pts[2]);
    vecs[2] = OVector3D(_pts[2], _pts[3]);
    vecs[3] = OVector3D(_pts[3], _pts[0]);

    // Les vecteurs doivent etre orthogonaux => produit scalaire nul
    for (i = 0; i < 3; i++)
    {
        if (ABS(vecs[i].scalar(vecs[i + 1])) > seuilConfondus) { return false; }
    }

    return true;
}

ORepere3D TYRectangle::getORepere3D() const
{
    ORepere3D repere;

    repere._origin = _pts[3];
    repere._vecI = OVector3D(_pts[3], _pts[2]);
    repere._vecI.normalize();
    repere._vecJ = OVector3D(_pts[3], _pts[0]);
    repere._vecJ.normalize();
    repere._vecK = normal();
    repere._vecK.normalize();

    return repere;
}

TYPolygon* TYRectangle::toPolygon() const
{
    TYTabPoint tabpoints;

    for (int i = 0; i < 4; i++)
    {
        tabpoints.push_back(_pts[i]);
    }

    return new TYPolygon(tabpoints);
}

void TYRectangle::inverseNormale()
{
    // inversion de deux points en diagonale
    TYPoint tmp;
    tmp = _pts[0];
    _pts[0] = _pts[2];
    _pts[2] = tmp;

    // Calcul de la normale
    _normale = normal();
}

void TYRectangle::exportMesh(
    std::deque<OPoint3D>& points,
    std::deque<OTriangle>& triangles,
    const TYGeometryNode& geonode) const
{
    assert(points.size() == 0 &&
           "Output arguments 'points' is expected to be initially empty");
    assert(triangles.size() == 0 &&
           "Output arguments 'triangles' is expected to be initially empty");

    // exports the point to the mesh, converting to global r/ frame
    for (int i = 0; i < 4; ++i)
    {
        points.push_back(geonode.localToGlobal() * _pts[i]);
    }
    // exports triangle (0, 1, 2)
    OTriangle tri(0, 1, 2);
    // Use already converted to global r/ frame points
    tri._A = points[0];
    tri._B = points[1];
    tri._C = points[2];
    triangles.push_back(tri);
    // exports triangle (0, 2, 3)
    tri._p1 = 0; tri._A = points[0];
    tri._p2 = 2; tri._B = points[2];
    tri._p3 = 3; tri._C = points[3];
    triangles.push_back(tri);
}
