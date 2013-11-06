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

#include <stdlib.h>
#include <cassert>

#include "Tympan/MetierSolver/ToolsMetier/OSegment3D.h"
#include "Tympan/MetierSolver/ToolsMetier/OBox.h"
#include "Tympan/MetierSolver/ToolsMetier/OBox2.h"

#include "TYAltimetrie.h"
#include "Tympan/MetierSolver/DataManagerMetier/ComposantGeometrique/TYSegment.h"

#include "Tympan/MetierSolver/DataManagerCore/TYPreferenceManager.h"
#include "Tympan/Tools/TYProgressManager.h"
#include "Tympan/Tools/OMessageManager.h"

#include "Tympan/MetierSolver/ToolsMetier/ODelaunayMaker.h"

#if defined(WIN32)
#include <crtdbg.h>
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new new(_NORMAL_BLOCK, THIS_FILE, __LINE__)
#endif
#endif


#define GRID_STEP(x)    sqrt(x) / 2.0f

struct triangle
{
    double pts[3][3];
};
static int compareTriangle(const void* elem1, const void* elem2);

OPROTOINST(TYAltimetrie);

const double TYAltimetrie::invalid_altitude = -1E5;

TYAltimetrie::TYAltimetrie()
{
    _name = TYNameManager::get()->generateName(getClassName());

    _pSortedFaces = NULL;
    _gridSX = 0;
    _gridSY = 0;
    _gridDX = 0.0f;
    _gridDY = 0.0f;
}

TYAltimetrie::TYAltimetrie(const TYAltimetrie& other)
{
    _gridSX = 0;
    _gridSY = 0;
    _gridDX = 0.0f;
    _gridDY = 0.0f;
    _pSortedFaces = NULL;
    *this = other;
}

TYAltimetrie::~TYAltimetrie()
{
    _listFaces.clear();

    if ((_gridSX != 0) && (_gridSY != 0))
    {
        // clear existing grid
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
}

TYAltimetrie& TYAltimetrie::operator=(const TYAltimetrie& other)
{
    if (this != &other)
    {
        TYElement::operator =(other);
        _listFaces = other._listFaces;
        _bbox = other._bbox;
        _gridSX = other._gridSX;
        _gridSY = other._gridSY;
        _gridDX = other._gridDX;
        _gridDY = other._gridDY;

        if ((_gridSX != 0) && (_gridSY != 0))
        {
            int k;
            // clear existing grid
            for (k = 0; k < _gridSX; k++)
            {
                for (int l = 0; l < _gridSY; l++)
                {
                    _pSortedFaces[k][l].clear();
                }
                delete[] _pSortedFaces[k];
            }
            delete[] _pSortedFaces;
            _pSortedFaces = new TYTabLPPolygon*[_gridSX];
            for (k = 0; k < _gridSX; k++)
            {
                _pSortedFaces[k] = new TYTabLPPolygon[_gridSY];
                for (int l = 0; l < _gridSY; l++)
                {
                    _pSortedFaces[k][l] = other._pSortedFaces[k][l];
                }
            }
        }
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

    TYAltimetrie* pOtherAlt = (TYAltimetrie*) pOther;

    _listFaces.clear();

    for (unsigned int i = 0; i < _listFaces.size(); i++)
    {
        TYPolygon* pNewChild = (TYPolygon*) pOtherAlt->getFace(i)->clone();
        pNewChild->deepCopy(pOtherAlt->getFace(i), copyId);
        addFace(pNewChild);
    }

    _bbox = pOtherAlt->_bbox;
    _gridSX = pOtherAlt->_gridSX;
    _gridSY = pOtherAlt->_gridSY;
    _gridDX = pOtherAlt->_gridDX;
    _gridDY = pOtherAlt->_gridDY;

    TYPolygon* pPolygon = NULL;
    if ((_gridSX != 0) && (_gridSY != 0))
    {
        // clear existing grid (if exist)
        if (_pSortedFaces != NULL)
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

        _pSortedFaces = new TYTabLPPolygon*[_gridSX];
        for (int k = 0; k < _gridSX; k++)
        {
            _pSortedFaces[k] = new TYTabLPPolygon[_gridSY];
            for (int l = 0; l < _gridSY; l++)
            {
                for (unsigned int m = 0; m < pOtherAlt->_pSortedFaces[k][l].size(); m++)
                {
                    pPolygon = new TYPolygon(*(pOtherAlt->_pSortedFaces[k][l]).at(m));
                    _pSortedFaces[k][l].push_back(pPolygon);
                }
            }
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

    // OMessageManager::get()->info("Mise a jour altimetrie (Tri des faces)...");
    // qsort(mesh, nbTriangles, sizeof(triangle), compareTriangle);

    // Definition du tableau de faces.
    TYTabPoint tabPt;
    tabPt.reserve(3); // Ce sont des triangles

    TYPoint point;

    if ((_gridSX != 0) && (_gridSY != 0))
    {
        // clear existing grid
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

    // compute density
    float fsx = GRID_STEP((double)nbTriangles);
    _gridSX = _gridSY = (int) fsx;
    _gridDX = (_bbox._max._x - _bbox._min._x) / _gridSX;
    _gridDY = (_bbox._max._y - _bbox._min._y) / _gridSY;

    // On initialise la grille de tri des triangles
    _pSortedFaces = new TYTabLPPolygon*[_gridSX];
    for (int k = 0; k < _gridSX; k++)
    {
        _pSortedFaces[k] = new TYTabLPPolygon[_gridSY];
        for (int l = 0; l < _gridSY; l++)
        {
            _pSortedFaces[k][l].clear();
            _pSortedFaces[k][l].reserve(10);
        }
    }

    OBox bb;

    LPTYPolygon pPolygon = NULL;
    double p, q;
    int ipmin, ipmax, iqmin, iqmax;
    for (i = 0; i < nbTriangles; i++)
    {
        // On recupere le triangle
        OTriangle& oTriangle = triangles[i];

        tabPt.clear();
        tabPt.resize(3);
        for (j = 0; j < 3; j++)
        {
            tabPt[j] = oTriangle.vertex(j);
        }

        // create a new face and add it the face lists
        pPolygon = new TYPolygon(tabPt);

        if (pPolygon != NULL)
        {
            pPolygon->setConvex(true);
            pPolygon->setParent(this);
            _listFaces.push_back(pPolygon);

            // On rempli la grille de tri des triangles
            // On trouve les min, max des indices des carres intersectes par le
            // triangle sur la grille.
            ipmin = ipmax = iqmin = iqmax = -1;
            for (j = 0; j < 3; j++)
            {
                p = (oTriangle.vertex(j)._x - _bbox._min._x) / _gridDX;
                q = (oTriangle.vertex(j)._y - _bbox._min._y) / _gridDY;
                if ((int)p > ipmax) { ipmax = (int) p; }
                if ((int)q > iqmax) { iqmax = (int) q; }
                if (((int)p < ipmin) || (ipmin == -1)) { ipmin = (int) p; }
                if (((int)q < iqmin) || (iqmin == -1)) { iqmin = (int) q; }
            }
            // Pour chacun des carres, on affecte le triangle
            // Todo: Optim: faire le test d'intersection carre/trianle avant d'ajouter.
            for (int k = ipmin; k <= ipmax; k++)
                for (int l = iqmin; l <= iqmax; l++)
                {
                    if ((k >= 0) && (l >= 0) && (k < _gridSX) && (l < _gridSY))
                    {
                        _pSortedFaces[k][l].push_back(pPolygon);
                    }
                }
        }
    }

    setIsGeometryModified(false);
}

// The method compute is obsolete and thus commented out
// Asignificant part of it has been moved into the plugBackTriangulation method
// void TYAltimetrie::compute(const TYTabPoint& points, const double& delaunay)//virtual

int compareTriangle(const void* elem1, const void* elem2)
{
    triangle* tr1 = (triangle*) elem1;
    triangle* tr2 = (triangle*) elem2;

    double x11, x12, x13, x1min; //absisse des 3 sommets du triangle 1
    double x21, x22, x23, x2min; //absisse des 3 sommets du triangle 2

    x11 = tr1->pts[0][0];
    x12 = tr1->pts[1][0];
    x13 = tr1->pts[2][0];
    x21 = tr2->pts[0][0];
    x22 = tr2->pts[1][0];
    x23 = tr2->pts[2][0];

    x1min = x11 > x12 ? x12 : x11;
    x1min = x1min > x13 ? x13 : x1min;
    x2min = x21 > x22 ? x22 : x21;
    x2min = x2min > x23 ? x23 : x2min;

    return (int)(x1min - x2min);
}

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
	unsigned int iMinMax[2];
	bool test = getGridIndices(pt, iMinMax);
	if (!test) return 0;
	unsigned int pi = iMinMax[0], qi = iMinMax[1];

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
	if (!test) return 0;

	// Récupération des faces correspondantes
	TYTabLPPolygon faces;
	getFacesinIndices(iMinMax[0], iMinMax[1], iMinMax[2], iMinMax[3], faces);

	// Test des points des faces par rapport à la box
	// Si au moins un point de la face est dans la box, la face est mise dans la liste
	unsigned int faceCount = 0; // Compteur de faces
	for (size_t i = 0 ; i < faces.size() ; i++)
	{
		TYTabPoint& pts = faces[i]->getPoints();
		for (size_t y = 0 ; y < pts.size(); y++)
		{
			if ( box.isInside(pts[y]) )
			{
				tabPolygon.push_back(faces[i]);
				faceCount++;
				continue;
			}
		}
	}

	if (faceCount == 0) // Aucune face trouvée
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
	if (!test) return 0;

	// Récupération des faces correspondantes
	TYTabLPPolygon faces;
	getFacesinIndices(iMinMax[0], iMinMax[1], iMinMax[2], iMinMax[3], faces);

	// Test des points des faces par rapport à la box
	// Si le point est dans le périmètre, il est ajouté à la liste
	for (size_t i = 0 ; i < faces.size() ; i++)
	{
		TYTabPoint& ptsFaces = faces[i]->getPoints();
		for (size_t y = 0 ; y < ptsFaces.size(); y++)
		{
			if ( OGeometrie::pointInPolygonRayCasting(ptsFaces[y], pts, 4) )
			{
				tabPoints.push_back(ptsFaces[y]);
				pointCount++;
			}
		}
	}

	return pointCount;
}

bool TYAltimetrie::getGridIndices(const OPoint3D& pt, unsigned int* indXY)
{
    if (_gridSX == 0) { return false; } // aucune face dans l'objet altimetrie...
    if (_pSortedFaces == NULL) { return false; } // Sanity Check...
    if (!_bbox.isInside2D(pt)) { return false; } //si le point n'est pas dans les bornes de l'altimetrie

    double p, q;
    int pi, qi;

    if ((_gridDX == 0) || (_gridDY == 0)) { return false; }  // sanity check

    p = (pt._x - _bbox._min._x) / _gridDX;
    q = (pt._y - _bbox._min._y) / _gridDY;
    pi = (int) p;
    qi = (int) q;

    if ((pi < 0) || (qi < 0) || (pi >= _gridSX) || (qi >= _gridSY)) { return false; }  // sanity check

	indXY[0] = pi;
	indXY[1] = qi;

	return true;
}

bool TYAltimetrie::getGridIndices(const OPoint3D* pts, unsigned int* iMinMax)
{
	unsigned int minX = 65535;
	unsigned int maxX = 0;
	unsigned int minY = 65535;
	unsigned int maxY = 0;

	// Test des quatre points et récupération des indices
	unsigned int iXY[2];
	bool res = true;
	for (size_t i=0 ; i<4 ; i++)
	{
		res &= getGridIndices(pts[i], iXY);
		iXY[0] < minX ? iXY[0] : minX;
		iXY[0] < maxX ? iXY[0] : maxX;
		iXY[1] < minY ? iXY[1] : minY;
		iXY[1] < maxY ? iXY[1] : maxY;
	}

	iMinMax[0] = minX;
	iMinMax[1] = maxX;
	iMinMax[2] = minY;
	iMinMax[3] = maxY;

	return res;
}

bool TYAltimetrie::getGridIndices(const OBox2 &box, unsigned int* iMinMax)
{
	OPoint3D pts[4];

	for (size_t i=1, j=0 ; i<5 ; i++, j++)
	{
		pts[j] = box.BoxCoord(i);
	}

	return getGridIndices(pts, iMinMax);
}


void TYAltimetrie::getFacesinIndices(unsigned int& minX, unsigned int& maxX, unsigned int& minY, unsigned int& maxY, TYTabLPPolygon& faces)
{
	map <LPTYPolygon, int> mapFaces;
	TYTabLPPolygon* pDivRef = NULL;

	//Utilisation d'un map pour filtrer les faces
	for (unsigned int i = minX ; i <= maxX ; i++)
	{
		for (unsigned int j = minY; j <= maxY ; j++)
		{
			pDivRef = &(_pSortedFaces[i][j]);
			for (size_t k = 0; k < pDivRef->size(); k++)
			{
				mapFaces[pDivRef->at(k)] = i*j*k;
			}
		}
	}

	//Remplissage du tableau
	map <LPTYPolygon, int>::iterator it;
	for (it = mapFaces.begin(); it != mapFaces.end(); it++)
	{
		faces.push_back(it->first);
	}
}

OPoint3D TYAltimetrie::projection(const OPoint3D& pt) const
{
    OPoint3D ptTest(pt);
    ptTest._z = -1.E5;

    if (_gridSX == 0) { return ptTest; } // aucune face dans l'objet altimetrie...
    if (_pSortedFaces == NULL) { return ptTest; } // Sanity Check...
    if (!_bbox.isInside2D(pt)) { return ptTest; } //si le point n'est pas dans les bornes de l'altimetrie

    double M_DOUBLE_INFINITE = 100000.0f;
    double p, q;
    int pi, qi;

    if ((_gridDX == 0) || (_gridDY == 0)) { return ptTest; }  // sanity check

    p = (pt._x - _bbox._min._x) / _gridDX;
    q = (pt._y - _bbox._min._y) / _gridDY;
    pi = (int) p;
    qi = (int) q;

    if ((pi < 0) || (qi < 0) || (pi >= _gridSX) || (qi >= _gridSY)) { return ptTest; }  // sanity check

    int i = 0;
    int size = 0;
    OSegment3D segTest;
    TYPolygon* pFace = NULL;
    TYTabLPPolygon* pDivRef = &(_pSortedFaces[pi][qi]);

    if (pDivRef != NULL) // sanity check
    {
        size = static_cast<int>(pDivRef->size());

        while (i < size)
        {
            pFace = pDivRef->at(i);

            if (pFace != NULL)
            {
                if (IsInsideFace(pFace->getPoints(), ptTest))
                {
                    segTest._ptA = ptTest;
                    segTest._ptB = ptTest;
                    segTest._ptA._z = +M_DOUBLE_INFINITE;
                    segTest._ptB._z = -M_DOUBLE_INFINITE;

                    if (pFace->intersects(segTest, ptTest, false) == INTERS_OUI)
                    {
						break;
                    }
                }
            }
            i++;
        }
    }

	return ptTest;
}

bool TYAltimetrie::updateAltitude(OPoint3D& pt) const
{
	pt = projection(pt);
	// XXX What is this hard-coded -1.E5 ? Is this meant to represent an unspecified/out of scope altitude ?
	if ( pt._z == -1.E5 ) { return false; }

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

TYTabPoint TYAltimetrie::altSup(const TYPoint& pt) const
{
    TYTabPoint tabPoint;
    size_t numFaces = _listFaces.size();
    LPTYPolygon pFace = NULL;

    OMsg::msgToDo("TYAltimetrie::altSup, Eliminer les pts en doublons...");

    for (int i = 0; i < numFaces; i++)
    {
        pFace = _listFaces.at(i);

        for (int j = 0; j < pFace->getNbPts(); i++)
        {
            OPoint3D point = pFace->getPoint(j);

            if (point._z > pt._z)
            {
                tabPoint.push_back(point);
            }
        }
    }

    return tabPoint;
}
