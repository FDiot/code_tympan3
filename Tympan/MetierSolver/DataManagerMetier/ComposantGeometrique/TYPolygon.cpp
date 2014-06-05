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
#include "Tympan/GraphicIHM/DataManagerIHM/TYPolygonWidget.h"
#include "Tympan/GraphicIHM/DataManagerGraphic/TYPolygonGraphic.h"
#endif

#include <memory>

#ifdef TYMPAN_USE_PRECOMPILED_HEADER
#include "Tympan/MetierSolver/DataManagerMetier/TYPHMetier.h"
#endif // TYMPAN_USE_PRECOMPILED_HEADER

#include "Tympan/MetierSolver/CommonTools/OSegment3D.h"
#include "Tympan/MetierSolver/CommonTools/Defines.h"
#include "TYRectangle.h"
#include "TYPolygon.h"
#include "Tympan/MetierSolver/DataManagerCore/TYPreferenceManager.h"
#include "Tympan/MetierSolver/DataManagerMetier/ComposantGeometrique/TYGeometryNode.h"
#include "Tympan/MetierSolver/DataManagerMetier/cgal_bridge.hpp"

#include "TYGeometryNode.h"

#include "Tympan/Tools/OMessageManager.h"
#include "Tympan/Tools/OChrono.h"


#if TY_USE_IHM
#include "Tympan/MetierSolver/CommonTools/ODelaunayMaker.h"
#endif // TY_USE_IHM


TY_EXTENSION_INST(TYPolygon);
TY_EXT_GRAPHIC_INST(TYPolygon);

TYPolygon::TYPolygon()
{
    _name = TYNameManager::get()->generateName(getClassName());

    _bConvex = false;
}

TYPolygon::TYPolygon(const TYPolygon& other)
{
    *this = other;
}

TYPolygon::TYPolygon(const TYTabPoint& pts)
{
    _name = TYNameManager::get()->generateName(getClassName());

    _bConvex = false;
    setPoints(pts);
}

TYPolygon::~TYPolygon()
{
    _pts.clear();
}

TYPolygon& TYPolygon::operator=(const TYPolygon& other)
{
    if (this != &other)
    {
        TYElement::operator =(other);
        _pts = other._pts;
        _bConvex = other._bConvex;
        _plan = other._plan;
        _box = other._box;

    }
    return *this;
}

bool TYPolygon::operator==(const TYPolygon& other) const
{
    if (this != &other)
    {
        if (TYElement::operator !=(other)) { return false; }
        if (_pts != other._pts) { return false; }
        if (_bConvex != other._bConvex) { return false; }
        if (_plan != other._plan) { return false; }
        if (_box != other._box) { return false; }
    }
    return true;
}

bool TYPolygon::operator!=(const TYPolygon& other) const
{
    return !operator==(other);
}

bool TYPolygon::deepCopy(const TYElement* pOther, bool copyId /*=true*/)
{
    if (!TYElement::deepCopy(pOther, copyId)) { return false; }

    TYPolygon* pOtherPoly = (TYPolygon*) pOther;

    _pts.clear();

    for (int i = 0; i < pOtherPoly->getNbPts(); i++)
    {
        TYPoint pt;
        pt.deepCopy(&pOtherPoly->getPoints()[i], copyId);
        _pts.push_back(pt);
    }

    _bConvex = pOtherPoly->_bConvex;
    _plan = pOtherPoly->_plan;
    _box = pOtherPoly->_box;

    return true;
}

std::string TYPolygon::toString() const
{
    std::string str = "TYPolygon";
    for (int i = 0; i < getNbPts(); i++)
    {
        str += "\n\tPt" + intToStr(i) + "=" + _pts[i].toString();
    }
    return str;
}

DOM_Element TYPolygon::toXML(DOM_Element& domElement)
{
    DOM_Element domNewElem = TYElement::toXML(domElement);

    for (int i = 0; i < getNbPts(); i++)
    {
        _pts[i].toXML(domNewElem);
    }

    return domNewElem;
}

int TYPolygon::fromXML(DOM_Element domElement)
{
    TYElement::fromXML(domElement);

    purge();

    TYPoint pt;
    DOM_Element elemCur;

    QDomNodeList childs = domElement.childNodes();

    for (unsigned int i = 0; i < childs.length(); i++)
    {
        elemCur = childs.item(i).toElement();

        if (pt.callFromXMLIfEqual(elemCur))
        {
            _pts.push_back(pt);
        }
    }

    updateNormal();
    updateBox();

    return 1;
}

double TYPolygon::surface() const
{
    double res = 0.0;

#if TY_USE_IHM
    TYTabPoint tabPt = getContour();
    size_t nbPts = tabPt.size();

    if (nbPts >= 3)
    {
        ODelaunayMaker oDelaunayMaker(0.001);
        for (int i = 0; i < nbPts; i++)
        {
            OPoint3D oVertex(tabPt[i]._x, tabPt[i]._y, 0);
            oDelaunayMaker.addVertex(oVertex);
        }

        oDelaunayMaker.compute();
        QList<OPoint3D> vertexes = oDelaunayMaker.getVertex();
        QList<OTriangle> faces = oDelaunayMaker.getFaces();

        QListIterator<OTriangle> it(faces);
        while (it.hasNext())
        {
            OTriangle face = it.next();
            double P = 0;
            double a = 0; //cote AB
            double b = 0; // cote BC
            double c = 0 ; //cote CA
            double Xa = vertexes[face._p1]._x;
            double Xb = vertexes[face._p2]._x;
            double Xc = vertexes[face._p3]._x;
            double Ya = vertexes[face._p1]._y;
            double Yb = vertexes[face._p2]._y;
            double Yc = vertexes[face._p3]._y;

            // calcule des longueurs des cotes du triangle
            a = sqrt((Xa - Xb) * (Xa - Xb) + (Ya - Yb) * (Ya - Yb));
            b = sqrt((Xb - Xc) * (Xb - Xc) + (Yb - Yc) * (Yb - Yc));
            c = sqrt((Xc - Xa) * (Xc - Xa) + (Yc - Ya) * (Yc - Ya));

            // formule de Heron
            P = a + b + c ;
            P = (P / 2) ;
            res += sqrt(P * (P - a) * (P - b) * (P - c));
        }
    }
    else
#endif // TY_USE_IHM
    {
        // Calcul du volume du bounding rect
        res = getBoundingRect().surface();
    }

    return res;
}

OVector3D TYPolygon::normal() const
{
    return OVector3D(_plan._a, _plan._b, _plan._c);
}

OPlan TYPolygon::plan() const
{
    return _plan;
}

TYTabPoint TYPolygon::getContour(int /*n=-1*/) const
{
    return getPoints();
}

TYTabPoint3D TYPolygon::getOContour(int /*n=-1*/) const
{
    TYTabPoint3D pts;
    const TYTabPoint& tabPts = getPoints();

    for (unsigned int i = 0; i < tabPts.size(); i++)
    {
        pts.push_back(tabPts[i]);
    }

    return pts;
}


int TYPolygon::intersects(const TYSurfaceInterface* pSurf, OSegment3D& seg) const
{
    return TYSurfaceInterface::intersects(pSurf, seg);
}

int TYPolygon::intersects(const OSegment3D& seg, OPoint3D& pt) const
{
    return intersects(seg, pt, true);
}

int TYPolygon::intersects(const OSegment3D& seg, OPoint3D& pt, bool insideTest) const
{
    int res = INTERS_NULLE;

    // Minimum 3 points
    if (getNbPts() < 3)
    {
        return res;
    }

    // On cherche le pt d'intersection avec le plan
    res = _plan.intersectsSegment(seg._ptA, seg._ptB, pt);

    if ((insideTest) && (res == INTERS_OUI))
    {
        // Il faut ensuite tester si le point d'intersection trouve
        // est inclu dans le polygone
        res = intersects(pt);
    }

    return res;
}

int TYPolygon::intersects(const OPoint3D& pt) const
{
    int res = INTERS_NULLE;
    // Nb de points constituant le polygon
    int nbPts = static_cast<int>(getNbPts());

    // Minimum 3 points !
    if (nbPts < 3)
    {
        return res;
    }

    if (_box.isInside(pt) == false)
    {
        return res;
    }

    if (isConvex() == true)
    {
        OVector3D normale = normal();

        int max_axe = 0;
        double max_normal = normale[0];

        if (max_normal < normale[1])
        {
            max_axe = 1;
            max_normal = normale[1];
        }

        if (max_normal < normale[2])
        {
            max_axe = 2;
        }
        switch (max_axe)
        {
            case 0:
                if (isInpolyYZ(_pts, nbPts, pt[1], pt[2]))
                {
                    res = INTERS_OUI;
                }
                break;
            case 1:
                if (isInpolyXZ(_pts, nbPts, pt[0], pt[2]))
                {
                    res = INTERS_OUI;
                }
                break;
            case 2:
                if (isInpolyXY(_pts, nbPts, pt[0], pt[1]))
                {
                    res = INTERS_OUI;
                }
                break;
        }
    }
    else
    {
        // Tableau de OPoint3D (taille memoire differente de TYPoint)
        OPoint3D* pts = new OPoint3D[nbPts];

        for (int i = 0; i < nbPts; i++)
        {
            pts[i] = _pts[i];
        }

        bool result;

#if TY_USE_IHM
        result = OGeometrie::pointInPolygonRayCasting(pt, pts, nbPts);
#else
        result = OGeometrie::pointInPolygonAngleSum(pt, pts, nbPts);
#endif

        if (result)
        {
            res = INTERS_OUI;
        }
        // Cleanning !
        delete [] pts;
    }


    return res;
}

void TYPolygon::setPoints(const TYTabPoint& pts)
{
    _pts = pts;
    //assert(checkCoplanar());
    _bConvex = false;
    updateNormal();
    updateBox();
    setIsGeometryModified(true);
}

bool TYPolygon::checkCoplanar() const
{
    size_t nbPts = _pts.size();
    if (nbPts < 3)
    {
        return true;
    }
    OPlan plan(_pts[0], _pts[1], _pts[2]);
    for (size_t i = 3; i < nbPts; ++i)
    {
        if (!plan.isInPlan(_pts[i]))
        {
            return false;
        }
    }
    return true;
}

void TYPolygon::transform(const OMatrix& matrix)
{
    for (register int j = 0; j < this->getNbPts(); j++)
    {
        this->_pts[j] = matrix * this->_pts[j];
    }
    updateNormal();
    updateBox();
}

bool TYPolygon::isValid() const
{
    OMsg::msgToDo("TYPolygon::isValid");
    bool res = false;

#if TY_USE_IHM
    double seuilConfondus = TYSEUILCONFONDUS;

    if (TYPreferenceManager::exists(TYDIRPREFERENCEMANAGER, "SeuilConfondus"))
    {
        seuilConfondus = TYPreferenceManager::getDouble(TYDIRPREFERENCEMANAGER, "SeuilConfondus");
    }
    else
    {
        TYPreferenceManager::setDouble(TYDIRPREFERENCEMANAGER, "SeuilConfondus", seuilConfondus);
    }

#endif

    return res;
}

ORepere3D TYPolygon::getORepere3D() const
{
    ORepere3D res;

    // Minimum 3 points
    if (getNbPts() < 3)
    {
        return res;
    }

    res._origin = _pts[0];

    res._vecI = OVector3D(_pts[0], _pts[1]);
    res._vecI.normalize();

    res._vecK = normal();
    res._vecK.normalize();

    res._vecJ = res._vecI.cross(res._vecK);
    res._vecJ.normalize();

    return res;
}

TYRectangle TYPolygon::getBoundingRect() const
{
    TYRectangle BoundingRect;
    int i ;

    size_t nbPts = getNbPts();

    if (nbPts <= 0)
    {
        return BoundingRect;
    }

    // Changement de repere
    OMatrix matrix;

    matrix = getORepere3D().asMatrix();
    matrix.invert();

    // Init
    TYPoint pt;
    TYPoint ptMax, ptMin;
    ptMax = ptMin = matrix * _pts[0];

    for (i = 0; i < nbPts; i++)
    {
        // Changement de repere
        pt = matrix * _pts[i];

        // On test uniquement x et y car on s'est ramene au plan
        // defini par le polygon

        // Minimuns
        if (pt._x <= ptMin._x)
        {
            ptMin._x = pt._x;
        }
        if (pt._y <= ptMin._y)
        {
            ptMin._y = pt._y;
        }
        // Maximums
        if (pt._x >= ptMax._x)
        {
            ptMax._x = pt._x;
        }
        if (pt._y >= ptMax._y)
        {
            ptMax._y = pt._y;
        }
    }

    // Construction du rectangle
    BoundingRect = TYRectangle(TYPoint(ptMin._x, ptMax._y, 0), TYPoint(ptMax._x, ptMax._y, 0),
                               TYPoint(ptMax._x, ptMin._y, 0), TYPoint(ptMin._x, ptMin._y, 0));

    // Changement de repere inverse
    matrix.invert();

    for (i = 0; i < 4; i++)
    {
        BoundingRect._pts[i] = matrix * BoundingRect._pts[i];
    }

    return BoundingRect;
}


void TYPolygon::updateNormal()
{
    int nbpoints = static_cast<int>(getNbPts());

    // Minimum 3 points
    if (nbpoints >= 3)
    {
        OVector3D normale;
        OPoint3D* pts = new OPoint3D[nbpoints];

        for (int i = 0; i < nbpoints; i++) { pts[i] = _pts[i]; }

        OGeometrie::computeNormal(pts, nbpoints, normale);

        _plan.set(_pts[0], normale);

        delete [] pts;
    }
    else
    {
        _plan = OPlan(0, 0, 0, 0);
    }
}


void TYPolygon::updateBox()
{
    int nbPts = static_cast<int>(getNbPts());

    if (nbPts > 0)
    {
        OPoint3D* pts = new OPoint3D[nbPts];

        for (int i = 0; i < nbPts; i++) { pts[i] = _pts[i]; }

        OPoint3D ptMin, ptMax;

        OGeometrie::boundingBox(pts, nbPts, ptMin, ptMax);

        _box = OBox(ptMin, ptMax);

        delete [] pts;
    }
    else
    {
        _box = OBox(OPoint3D(0, 0, 0), OPoint3D(0, 0, 0));
    }
}


int TYPolygon::isInpolyXY(const TYTabPoint& poly, int npoints, double xt, double yt) const
{
    double xnew, ynew;
    double xold, yold;
    double x1, y1;
    double x2, y2;
    int i;
    int inside = 0;

    if (npoints < 3)
    {
        return(0);
    }

    xold = poly[npoints - 1][0];
    yold = poly[npoints - 1][1];

    for (i = 0 ; i < npoints ; i++)
    {
        xnew = poly[i][0];
        ynew = poly[i][1];

        if (xnew > xold)
        {
            x1 = xold;
            x2 = xnew;
            y1 = yold;
            y2 = ynew;
        }
        else
        {
            x1 = xnew;
            x2 = xold;
            y1 = ynew;
            y2 = yold;
        }

        if ((xnew < xt) == (xt <= xold) && (yt - y1) * (x2 - x1) < (y2 - y1) * (xt - x1))
        {

            inside = !inside;

        }

        xold = xnew;
        yold = ynew;
    }

    return(inside);
}


int TYPolygon::isInpolyXZ(const TYTabPoint& poly, int npoints, double xt, double yt) const
{
    double xnew, ynew;
    double xold, yold;
    double x1, y1;
    double x2, y2;
    int i;
    int inside = 0;

    if (npoints < 3)
    {
        return(0);
    }

    xold = poly[npoints - 1][0];
    yold = poly[npoints - 1][2];

    for (i = 0 ; i < npoints ; i++)
    {
        xnew = poly[i][0];
        ynew = poly[i][2];

        if (xnew > xold)
        {
            x1 = xold;
            x2 = xnew;
            y1 = yold;
            y2 = ynew;
        }
        else
        {
            x1 = xnew;
            x2 = xold;
            y1 = ynew;
            y2 = yold;
        }

        if ((xnew < xt) == (xt <= xold) && (yt - y1) * (x2 - x1) < (y2 - y1) * (xt - x1))
        {

            inside = !inside;

        }

        xold = xnew;
        yold = ynew;
    }

    return(inside);
}


int TYPolygon::isInpolyYZ(const TYTabPoint& poly, int npoints, double xt, double yt) const
{
    double xnew, ynew;
    double xold, yold;
    double x1, y1;
    double x2, y2;
    int i;
    int inside = 0;

    if (npoints < 3)
    {
        return(0);
    }

    xold = poly[npoints - 1][1];
    yold = poly[npoints - 1][2];

    for (i = 0 ; i < npoints ; i++)
    {
        xnew = poly[i][1];
        ynew = poly[i][2];

        if (xnew > xold)
        {
            x1 = xold;
            x2 = xnew;
            y1 = yold;
            y2 = ynew;
        }
        else
        {
            x1 = xnew;
            x2 = xold;
            y1 = ynew;
            y2 = yold;
        }

        if ((xnew < xt) == (xt <= xold) && (yt - y1) * (x2 - x1) < (y2 - y1) * (xt - x1))
        {

            inside = !inside;

        }

        xold = xnew;
        yold = ynew;
    }

    return(inside);
}

void TYPolygon::inverseNormale()
{
    TYTabPoint tabPtsTemp = _pts;
    _pts.clear();
    vector<TYPoint>::reverse_iterator it;
    for (it = tabPtsTemp.rbegin(); it != tabPtsTemp.rend(); it++)
    {
        _pts.push_back((*it));
    }

    // Actualisation de la normale
    updateNormal();
}

void TYPolygon::exportMesh(
    std::deque<OPoint3D>& points,
    std::deque<OTriangle>& triangles,
    const TYGeometryNode& geonode) const
{
    using namespace tympan;

    // Convert Polygon into global frame
    TYPolygon gpoly(*this);
    BOOST_FOREACH(auto& pt, gpoly._pts)
        pt = geonode.localToGlobal() * pt;

    auto triangulator = make_polygon_triangulator(gpoly);
    triangulator->exportMesh(points, triangles);
}
