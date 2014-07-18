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

#include <cstdlib>
#include <cassert>
#include <cmath>
#include <limits>
#include <algorithm>

#include <boost/current_function.hpp>
#include <boost/foreach.hpp>

#include "Tympan/core/logging.h"
#include "Tympan/models/common/3d.h"
#include "Tympan/models/business/geometry/TYSegment.h"
#include "Tympan/models/business/TYPreferenceManager.h"
#include "Tympan/models/common/delaunay_maker.h"
#if TY_USE_IHM
  #include "Tympan/gui/widgets/TYAltimetrieWidget.h"
  #include "Tympan/gui/gl/TYAltimetrieGraphic.h"
#endif
#include "TYAltimetrie.h"

#if defined(WIN32)
  #include <crtdbg.h>
  #ifdef _DEBUG
    #undef THIS_FILE
    static char THIS_FILE[] = __FILE__;
    #define new new(_NORMAL_BLOCK, THIS_FILE, __LINE__)
  #endif
#endif

#undef min
#undef max


static inline double grid_step(double nb_triangles) {return sqrt(nb_triangles) / 2;}

struct triangle
{
    double pts[3][3];
};

TY_EXTENSION_INST(TYAltimetrie);
TY_EXT_GRAPHIC_INST(TYAltimetrie);

const double TYAltimetrie::invalid_altitude = -1E5;

TYAltimetrie::TYAltimetrie()
{
    _name = TYNameManager::get()->generateName(getClassName());
    initNullGrid();
}

TYAltimetrie::TYAltimetrie(const TYAltimetrie& other)
{
    initNullGrid();
    *this = other;
}

TYAltimetrie::~TYAltimetrie()
{
    _listFaces.clear();
    clearAcceleratingGrid();
}

TYAltimetrie& TYAltimetrie::operator=(const TYAltimetrie& other)
{
    if (this != &other)
    {
        TYElement::operator =(other);
        _listFaces = other._listFaces;
        _bbox = other._bbox;
        copyAcceleratingGrid(other);
    }
    return *this;
}

bool TYAltimetrie::operator==(const TYAltimetrie& other) const
{
    if (this != &other)
    {
        if (TYElement::operator !=(other)) { return false; }
        if (!(_listFaces == other._listFaces)) { return false; }
    }
    return true;
}

bool TYAltimetrie::operator!=(const TYAltimetrie& other) const
{
    return !operator==(other);
}

bool TYAltimetrie::deepCopy(const TYElement* pOther, bool copyId /*=true*/)
{
    if (!TYElement::deepCopy(pOther, copyId)) { return false; }

    const TYAltimetrie* pOtherAlt = dynamic_cast<const TYAltimetrie*>(pOther);
    assert(pOtherAlt && "The TYElement given was no TYAltimetry.");

    _listFaces.clear();

    for (unsigned int i = 0; i < _listFaces.size(); i++)
    {
        TYPolygon* pNewChild = (TYPolygon*) pOtherAlt->getFace(i)->clone();
        pNewChild->deepCopy(pOtherAlt->getFace(i), copyId);
        addFace(pNewChild);
    }

    _bbox = pOtherAlt->_bbox;

    copyAcceleratingGrid(*pOtherAlt);
    for (int k = 0; k < _gridSX; k++)
        for (int l = 0; l < _gridSY; l++)
        {
            unsigned nbFaces = _pSortedFaces[k][l].size();
            _pSortedFaces[k][l].reserve(nbFaces);
            for (unsigned int m = 0; m < nbFaces; m++)
            {
                TYPolygon* pPolygon = _pSortedFaces[k][l].at(m).getRealPointer();
                // We break the sharing of the TYPolygon with other
                LPTYPolygon pNewPoly = new TYPolygon(*pPolygon);
                _pSortedFaces[k][l].push_back(pNewPoly);
            }
        }
    return true;
}

std::string TYAltimetrie::toString() const
{
    return "TYAltimetrie";
}

DOM_Element TYAltimetrie::toXML(DOM_Element& domElement)
{
    DOM_Element domNewElem = TYElement::toXML(domElement);

    return domNewElem;
}

int TYAltimetrie::fromXML(DOM_Element domElement)
{
    TYElement::fromXML(domElement);

    return 1;
}


void TYAltimetrie::plugBackTriangulation(
    const std::deque<OPoint3D>& vertices,
    std::deque<OTriangle>& triangles)
{
    const unsigned nbTriangles = triangles.size();
    unsigned i; // Index over triangles
    unsigned j; // Index over {0, 1, 2} for vertices of triangles

    // Purge de la liste des faces
    _listFaces.clear();
    // Reserve le nombre d'emplacements pour les triangles
    _listFaces.reserve(nbTriangles);

    /*
     * This section seems to have two functions :
     *  - getting the actual coordinates of the vertices (from their indices)
     *  - computing their bounding box
     *  both of which can be better fullfiled by a preprocessing from the AltimetryBuilder
     *
     *  TODO suppress the mesh and access directly to triangles holding BOTH
     *  their indices and vertices (by reference instead of by value ?)
     *
     *  TODO What is the sorting for ?
     */
    for (i = 0; i < nbTriangles; i++)
    {
        // We fetch the ith triangle
        OTriangle& oTriangle = triangles[i];

        // compute global bounding box by adding each points
        oTriangle._A = vertices[oTriangle._p1];
        _bbox.Enlarge(oTriangle._A);
        oTriangle._B = vertices[oTriangle._p2];
        _bbox.Enlarge(oTriangle._B);
        oTriangle._C = vertices[oTriangle._p3];
        _bbox.Enlarge(oTriangle._C);
    }

    // Reset the grid to its new size
    clearAcceleratingGrid();

    if (nbTriangles == 0)
    {
        // TODO Is this a logic_error, invalid_data or a do-nothing degenerate case ?
        // For now we throw after having cleaned the data structure.
        throw tympan::invalid_data("Empty triangulation") << tympan_source_loc;
    }

    // compute density
    float fsx = grid_step(nbTriangles);
    _gridSX = _gridSY = ceil(fsx);
    assert(_gridSX > 0);
    assert(_gridSY > 0);
    _gridDX = (_bbox._max._x - _bbox._min._x) / _gridSX;
    _gridDY = (_bbox._max._y - _bbox._min._y) / _gridSY;
    initAcceleratingGrid(10);

    for (i = 0; i < nbTriangles; i++)
    {
        // Making a TYPolygon from the OTriangle
        OTriangle& oTriangle = triangles[i];
        // Definition du tableau de faces.
        TYTabPoint tabPt;
        tabPt.resize(3);
        for (j = 0; j < 3; j++)
        {
            tabPt[j] = oTriangle.vertex(j);
        }
        LPTYPolygon pPolygon = new TYPolygon(tabPt);
        assert(pPolygon != NULL);

        pPolygon->setConvex(true);
        pPolygon->setParent(this);
        _listFaces.push_back(pPolygon);

        // On rempli la grille de tri des triangles
        // On trouve les min, max des indices des carres intersectes par le
        // triangle sur la grille.
        unsigned int ipmin, ipmax, iqmin, iqmax;
        ipmin = iqmin = std::numeric_limits<unsigned int>::max();
        ipmax = iqmax = std::numeric_limits<unsigned int>::min(); // Yes : zero for unsigned
        for (j = 0; j < 3; j++)
        {
            grid_index idx;
            if (!getGridIndices(oTriangle.vertex(j), idx))
            {
                throw tympan::logic_error("Point out of the altimetry's bounding box")
                        << tympan_source_loc << tympan::position_errinfo(oTriangle.vertex(j));
            };

            ipmax = std::max(ipmax, idx.pi);
            iqmax = std::max(iqmax, idx.qi);
            ipmin = std::min(ipmin, idx.pi);
            iqmin = std::min(iqmin, idx.qi);
        }
        // Pour chacun des carres, on affecte le triangle
        // Todo: Optim: faire le test d'intersection carre/trianle avant d'ajouter.
        for (int k = ipmin; k <= ipmax; k++)
            for (int l = iqmin; l <= iqmax; l++)
            {
                assert((k >= 0) && (l >= 0) && (k < _gridSX) && (l < _gridSY));
                _pSortedFaces[k][l].push_back(pPolygon);
            }
    }

    setIsGeometryModified(false);
}

// The method compute is obsolete and thus commented out
// Asignificant part of it has been moved into the plugBackTriangulation method
// void TYAltimetrie::compute(const TYTabPoint& points, const double& delaunay)//virtual

bool TYAltimetrie::addFace(LPTYPolygon pFace)
{
    assert(pFace);

    pFace->setParent(this);
    _listFaces.push_back(pFace);

    setIsGeometryModified(true);

    return true;
}

bool TYAltimetrie::remFace(const LPTYPolygon pFace)
{
    assert(pFace);
    bool ret = false;
    TYTabLPPolygon::iterator ite;

    for (ite = _listFaces.begin(); ite != _listFaces.end(); ite++)
    {
        if ((*ite) == pFace)
        {
            _listFaces.erase(ite);
            ret = true;
            break;
        }
    }

    setIsGeometryModified(true);

    return ret;
}

bool TYAltimetrie::remFace(QString idFace)
{
    bool ret = false;
    TYTabLPPolygon::iterator ite;

    for (ite = _listFaces.begin(); ite != _listFaces.end(); ite++)
    {
        if ((*ite)->getID() == idFace)
        {
            _listFaces.erase(ite);
            ret = true;
            break;
        }
    }

    setIsGeometryModified(true);

    return ret;
}

inline bool TYAltimetrie::IsInsideFace(const TYTabPoint& pts, OPoint3D& pt) const
{
    if (pts.size() < 3) { return false; } // le triangle n'est pas complet!!!!

    double x, y; // coord 2D du point
    double xa, ya, xb, yb, xc, yc; // coord 2D des sommets du triangle
    double bx, by, cx, cy;
    double hx, hy;
    double a2, a3;

    x = pt._x;
    y = pt._y;
    xa = pts[0]._x;
    ya = pts[0]._y;
    xb = pts[1]._x;
    yb = pts[1]._y;
    xc = pts[2]._x;
    yc = pts[2]._y;

    bx = xc - xa;
    by = yc - ya;
    cx = xb - xa;
    cy = yb - ya;
    hx = x - xa;
    hy = y - ya;
    a2 = (bx * hy - by * hx) / (bx * cy - by * cx);
    a3 = (hx * cy - hy * cx) / (bx * cy - by * cx);

    if (a2 < 0) { return false; }
    if (a3 < 0) { return false; }
    if (a2 + a3 > 1) { return false; }

    return true;
}


LPTYPolygon TYAltimetrie::getFaceUnder(OPoint3D pt)
{
    // Recherche des indices de la grilles qui incluent les sommets de la boite
    grid_index idx;
    bool test = getGridIndices(pt, idx);
    if (!test) { return 0; }
    unsigned int pi = idx.pi, qi = idx.qi;

    TYTabLPPolygon* pDivRef = &(_pSortedFaces[pi][qi]);
    TYPolygon* pFace = NULL;

    if (pDivRef != NULL) // sanity check
    {
        for (size_t i = 0; i < pDivRef->size(); i++)
        {
            pFace = pDivRef->at(i);

            if (pFace != NULL)
            {
                if (IsInsideFace(pFace->getPoints(), pt))
                {
                    return pFace;
                }
            }
        }
    }

    return pFace;
}


unsigned int TYAltimetrie::getFacesInBox(const OBox2& box, TYTabLPPolygon& tabPolygon)
{
    // Recherche des indices de la grilles qui incluent les sommets de la boite
    unsigned int iMinMax[4];
    bool test = getGridIndices(box, iMinMax);
    if (!test) { return 0; }

    // Rcupration des faces correspondantes
    TYTabLPPolygon faces;
    getFacesinIndices(iMinMax[0], iMinMax[1], iMinMax[2], iMinMax[3], faces);

    // Test des points des faces par rapport  la box
    // Si au moins un point de la face est dans la box, la face est mise dans la liste
    unsigned int faceCount = 0; // Compteur de faces
    for (size_t i = 0 ; i < faces.size() ; i++)
    {
        TYTabPoint& pts = faces[i]->getPoints();
        for (size_t y = 0 ; y < pts.size(); y++)
        {
            if (box.isInside(pts[y]))
            {
                tabPolygon.push_back(faces[i]);
                faceCount++;
                continue;
            }
        }
    }

    if (faceCount == 0) // Aucune face trouve
    {
        for (size_t i = 0 ; i < faces.size() ; i++)
        {
            tabPolygon.push_back(faces[i]);
        }
    }

    return  faceCount;
}

unsigned int TYAltimetrie::getPointsInBox(const OPoint3D& pt0, const OPoint3D& pt1, const OPoint3D& pt2, const OPoint3D& pt3, TYTabPoint& tabPoints)
{
    unsigned int pointCount = 0;
    OPoint3D pts[4];
    pts[0] = pt0;
    pts[1] = pt1;
    pts[2] = pt2;
    pts[3] = pt3;

    unsigned int iMinMax[4];

    bool test = getGridIndices(pts, iMinMax);
    if (!test) { return 0; }

    // Rcupration des faces correspondantes
    TYTabLPPolygon faces;
    getFacesinIndices(iMinMax[0], iMinMax[1], iMinMax[2], iMinMax[3], faces);

    // Test des points des faces par rapport  la box
    // Si le point est dans le primtre, il est ajout  la liste
    for (size_t i = 0 ; i < faces.size() ; i++)
    {
        TYTabPoint& ptsFaces = faces[i]->getPoints();
        for (size_t y = 0 ; y < ptsFaces.size(); y++)
        {
            if (OGeometrie::pointInPolygonRayCasting(ptsFaces[y], pts, 4))
            {
                tabPoints.push_back(ptsFaces[y]);
                pointCount++;
            }
        }
    }

    return pointCount;
}

bool TYAltimetrie::getGridIndices(const OPoint3D& pt, grid_index& indXY) const
{

    if ((_gridDX == 0) || (_gridDY == 0))
    {
        // This is supposed to be an error case isn't it ? Then we should NOT return SILENTLY.
        // but raising this exception seem to causes regression in some cases...
        throw tympan::logic_error("No face in accelerating structure of the altimetry") << tympan_source_loc;
    }
    // A sanity check has to be represented by an assert, not an if silencing a failure.
    assert(_pSortedFaces != NULL && "Sanity Check...");

    if (!_bbox.isInside2D(pt)) //si le point n'est pas dans la bounding box de l'altimetrie
    {
        return false;
        // The return above preserves legacy behaviour but the exception below should be prefered
        /*
        throw tympan::logic_error("Point out of the altimetry's bounding box")
            << tympan_source_loc << tympan::position_errinfo(pt);
        */
    }

    double p, q;
    unsigned pi, qi;
    assert(_gridDX != 0);
    assert(_gridDY != 0);
    p = (pt._x - _bbox._min._x) / _gridDX;
    q = (pt._y - _bbox._min._y) / _gridDY;
    pi = floor(p);
    qi = floor(q);

    // In case pt lies on the top-most (resp. right-most) border of the bounding box
    // p could be exactly _gridSX (resp. q exactly _gridSY) and this needs clipping.
    pi = std::min(pi, _gridSX - 1);
    qi = std::min(qi, _gridSY - 1);

    assert((pi >= 0) && (qi >= 0) && (pi < _gridSX) && (qi < _gridSY));

    indXY.pi = pi;
    indXY.qi = qi;

    return true;
}

bool TYAltimetrie::getGridIndices(const OPoint3D* pts, unsigned int* iMinMax) const
{
    unsigned minX = std::numeric_limits<unsigned>::max();
    unsigned maxX = std::numeric_limits<unsigned>::min();
    unsigned minY = std::numeric_limits<unsigned>::max();
    unsigned maxY = std::numeric_limits<unsigned>::min();

    // Test des quatre points et rcupration des indices
    grid_index iXY;

    bool res = true;

    for (size_t i = 0 ; i < 4 ; i++)
    {
        res &= getGridIndices(pts[i], iXY);
        minX = std::min(minX, iXY.pi);
        minY = std::min(minY, iXY.qi);
        maxX = std::max(maxX, iXY.pi);
        maxY = std::max(maxY, iXY.qi);
    }

    iMinMax[0] = minX;
    iMinMax[1] = maxX;
    iMinMax[2] = minY;
    iMinMax[3] = maxY;

    return res;
}

bool TYAltimetrie::getGridIndices(const OBox2& box, unsigned int* iMinMax) const
{
    OPoint3D pts[4];

    for (size_t i = 1, j = 0 ; i < 5 ; i++, j++)
    {
        pts[j] = box.BoxCoord(i);
    }

    return getGridIndices(pts, iMinMax);
}


void TYAltimetrie::getFacesinIndices(unsigned int& minX, unsigned int& maxX, unsigned int& minY, unsigned int& maxY, TYTabLPPolygon& faces)
{
    std::map <LPTYPolygon, int> mapFaces;
    TYTabLPPolygon* pDivRef = NULL;

    //Utilisation d'un map pour filtrer les faces
    for (unsigned int i = minX ; i <= maxX ; i++)
    {
        for (unsigned int j = minY; j <= maxY ; j++)
        {
            pDivRef = &(_pSortedFaces[i][j]);
            for (size_t k = 0; k < pDivRef->size(); k++)
            {
                mapFaces[pDivRef->at(k)] = i * j * k;
            }
        }
    }

    //Remplissage du tableau
    std::map <LPTYPolygon, int>::iterator it;
    for (it = mapFaces.begin(); it != mapFaces.end(); it++)
    {
        faces.push_back(it->first);
    }
}

OPoint3D TYAltimetrie::projection(const OPoint3D& pt) const
{
    OPoint3D ptTest(pt);
    ptTest._z = invalid_altitude;

    const double M_DOUBLE_INFINITE = 1e6; // CAUTION ! Numerical instability:
    // One could prefer using `std::numeric_limits<double>::infinity()` or
    // `std::numeric_limits<double>::max()` but both cause the call to
    // pFace->intersects(...) below to fail silently
    // Using this big but no too big ad hoc 1E6 seems OK...

    grid_index idx;
    if (!getGridIndices(pt, idx))
    {
        // Early termination with invalid altitude generaly due to
        // pt being out the geometrical scope of the altimetry.
        return ptTest;
    }

    OSegment3D segTest;
    TYTabLPPolygon& divRef = _pSortedFaces[idx.pi][idx.qi];

    BOOST_FOREACH(const LPTYPolygon & pFace, divRef)
    {
        if (IsInsideFace(pFace->getPoints(), ptTest))
        {
            segTest._ptA = ptTest;
            segTest._ptB = ptTest;
            segTest._ptA._z = +M_DOUBLE_INFINITE;
            segTest._ptB._z = -M_DOUBLE_INFINITE;

            if (pFace->intersects(segTest, ptTest, false) == INTERS_OUI)
            {
                assert(ptTest._z != invalid_altitude && "Successful intersection expected");
                return ptTest;
            }
        }
    }
    assert(ptTest._z == invalid_altitude && "invalid_altitude expected to denote failure");
    return ptTest;
}

bool TYAltimetrie::updateAltitude(OPoint3D& pt) const
{
    pt = projection(pt);
    if (pt._z == invalid_altitude)
    {
        OMessageManager::get()->warning(
            "%s Could not compute valid altitude for point at (%f, %f)",
            BOOST_CURRENT_FUNCTION, pt._x, pt._y);
        return false;
    }
    return true;
}

double TYAltimetrie::altitude(const OPoint3D& pt)
{
    return projection(pt)._z;
}

double TYAltimetrie::HauteurMoyenne(TYTabPoint& pts)
{
    double dHauteurMoyenne = 0;
    OPoint3D pt;
    bool  bOK;
    int nNbHauteurs = 0;
    for (unsigned int i = 0; i < pts.size(); i++)
    {
        pt = pts[i];
        bOK = updateAltitude(pt);
        if (bOK)
        {
            dHauteurMoyenne += pts.at(i)._z - pt._z;
            nNbHauteurs++;
        }
    }
    if (nNbHauteurs == 0 || dHauteurMoyenne < 0) //si l'altitude de la polyligne (info z de TYTabPoint) n'est pas passee, on renvoie une hauteur nulle
    {
        return 0;
    }
    dHauteurMoyenne /= nNbHauteurs;
    return dHauteurMoyenne;
}

double TYAltimetrie::PremiereHauteurPositiveOuNulle(TYTabPoint& ptsIn)
{
    //On calcule la moyenne de hauteur de tous les points de la polyligne
    double dHauteur = 0;
    int i = 0;
    do
    {
        OPoint3D courant(0.0, 0.0, 0.0);
        courant._x = ptsIn.at(i)._x;
        courant._y = ptsIn.at(i)._y;
        bool bOK = updateAltitude(courant);
        if (!bOK)
        {
            return 0;
        }
        dHauteur = ptsIn.at(i)._z - courant._z;
        i ++;
    }
    while (dHauteur < 0 && i < (int) ptsIn.size());
    if (dHauteur < 0)
    {
        return 0;
    }
    return dHauteur;
}

void TYAltimetrie::initNullGrid()
{
    // Initialisation of members
    _pSortedFaces = NULL;
    _gridSX = 0;
    _gridSY = 0;
    _gridDX = 0.0f;
    _gridDY = 0.0f;
}

void TYAltimetrie::clearAcceleratingGrid()
{
    // clear existing grid
    if (_pSortedFaces)
    {
        for (int k = 0; k < _gridSX; k++)
        {
            for (int l = 0; l < _gridSY; l++)
            {
                _pSortedFaces[k][l].clear();
            }
            delete[] _pSortedFaces[k];
        }
        delete[] _pSortedFaces;
    }
    initNullGrid();
}

void TYAltimetrie::initAcceleratingGrid(unsigned to_be_reserved)
{
    assert(_pSortedFaces == NULL && "The accelerating grid must have been cleared.");
    assert(_gridSX && _gridSY && "The size of the grid must be positive.");

    _pSortedFaces = new TYTabLPPolygon*[_gridSX];
    for (int k = 0; k < _gridSX; k++)
    {
        _pSortedFaces[k] = new TYTabLPPolygon[_gridSY];
        for (int l = 0; l < _gridSY; l++)
        {
            _pSortedFaces[k][l].reserve(to_be_reserved);
        }
    }

}

void TYAltimetrie::copyAcceleratingGrid(const TYAltimetrie& other)
{
    clearAcceleratingGrid();

    _gridSX = other._gridSX;
    _gridSY = other._gridSY;
    _gridDX = other._gridDX;
    _gridDY = other._gridDY;
    initAcceleratingGrid();

    for (int k = 0; k < _gridSX; k++)
        for (int l = 0; l < _gridSY; l++)
        {
            // This is a copy of a STL vector/deque of SmartPtr
            _pSortedFaces[k][l] = other._pSortedFaces[k][l];
        }

}
