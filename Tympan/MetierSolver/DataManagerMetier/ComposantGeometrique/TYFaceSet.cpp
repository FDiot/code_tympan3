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
#include "Tympan/GraphicIHM/DataManagerIHM/TYFaceSetWidget.h"
#include "Tympan/GraphicIHM/DataManagerGraphic/TYFaceSetGraphic.h"
#endif

#ifdef TYMPAN_USE_PRECOMPILED_HEADER
#include "Tympan/MetierSolver/DataManagerMetier/TYPHMetier.h"
#endif // TYMPAN_USE_PRECOMPILED_HEADER
#include "Tympan/MetierSolver/ToolsMetier/OSegment3D.h"

#include "Tympan/MetierSolver/DataManagerCore/TYPreferenceManager.h"

#include "Tympan/Tools/OMessageManager.h"
#include "Tympan/Tools/OChrono.h"


OPROTOINST(TYFaceSet);
TY_EXTENSION_INST(TYFaceSet);
TY_EXT_GRAPHIC_INST(TYFaceSet);

TYFaceSet::TYFaceSet()
{
    _name = TYNameManager::get()->generateName(getClassName());
}

TYFaceSet::TYFaceSet(const TYFaceSet& other)
{
    *this = other;
}

TYFaceSet::~TYFaceSet()
{
    //  purge();
}

TYFaceSet& TYFaceSet::operator=(const TYFaceSet& other)
{
    if (this != &other)
    {
        TYElement::operator =(other);
        _volEnglob = other._volEnglob;
        _rayonSphere = other._rayonSphere;
        _centreGravite = other._centreGravite;

        _faces = other._faces;
        _sommets = other._sommets;
        _normals = other._normals;
    }
    return *this;
}

bool TYFaceSet::operator==(const TYFaceSet& other) const
{
    if (this != &other)
    {
        if (TYElement::operator !=(other)) { return false; }
        if (_faces != other._faces) { return false; }
        if (_sommets != other._sommets) { return false; }
        if (_normals != other._normals) { return false; }
    }
    return true;
}

bool TYFaceSet::operator!=(const TYFaceSet& other) const
{
    return !operator==(other);
}

bool TYFaceSet::deepCopy(const TYElement* pOther, bool copyId /*=true*/)
{
    if (!TYElement::deepCopy(pOther, copyId)) { return false; }

    TYFaceSet* pOtherFaceSet = (TYFaceSet*) pOther;

    purge();

    TYTabLPPolygon otherFaces = pOtherFaceSet->faces();
    TYPolygon* pFace = NULL;
    for (int i = 0; i < pOtherFaceSet->getNbFaces(); i++)
    {
        pFace = new TYPolygon();
        pFace->deepCopy(otherFaces[i], copyId);
        _faces.push_back(pFace);
    }

    _sommets = pOtherFaceSet->_sommets;
    _normals = pOtherFaceSet->_normals;

    _volEnglob = pOtherFaceSet->_volEnglob;
    _rayonSphere = pOtherFaceSet->_rayonSphere;
    _centreGravite = pOtherFaceSet->_centreGravite;

    return true;
}

std::string TYFaceSet::toString() const
{
    std::string str = "TYFaceSet";
    for (unsigned int i = 0; i < _faces.size(); i++)
    {
        str += "\n\tFace" + intToStr(i) + "=" + _faces[i]->toString();
    }

    return str;
}

DOM_Element TYFaceSet::toXML(DOM_Element& domElement)
{
    DOM_Element domNewElem = TYElement::toXML(domElement);

    TYPolygon* pFace = NULL;
    for (unsigned int i = 0; i < _faces.size(); i++)
    {
        pFace = _faces[i];
        if (pFace != NULL)
        {
            pFace->toXML(domNewElem);
        }
    }
    return domNewElem;
}

int TYFaceSet::fromXML(DOM_Element domElement)
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
            //          _pts.push_back(pt);
        }
    }

    UpdateInternals();

    return 1;
}

double TYFaceSet::volume() const
{
    double res = 0.0;

    TYBox bbox = getVolEnglob();
    // Calcul du volume du bounding rect
    res = bbox._sizeX * bbox._sizeY * bbox._sizeZ;

    return res;
}

double TYFaceSet::surface() const
{
    double res = 0.0;

    TYBox bbox = getVolEnglob();
    // Calcul de la surface du bounding rect
    res = 2 * (bbox._sizeX * bbox._sizeY + bbox._sizeX * bbox._sizeZ + bbox._sizeY * bbox._sizeZ);

    return res;
}

double TYFaceSet::activeSurface() const
{
    double res = 0.0;
    return res;
}

double TYFaceSet::activeSurface()
{
    double res = 0.0;
    return res;
}

TYTabVector TYFaceSet::normals() const
{
    return _normals;
}

TYTabPoint TYFaceSet::sommets() const
{
    return _sommets;
}

TYTabLPPolygon TYFaceSet::faces() const
{
    return _faces;
}

TYPoint TYFaceSet::centreGravite() const
{
    TYPoint centre;
    size_t nbPts = _sommets.size();
    float x = 0.0f, y = 0.0f, z = 0.0f;

    if (nbPts > 0)
    {
        x = 0.0f;
        y = 0.0f;
        z = 0.0f;

        TYPoint pt;
        for (size_t i = 0; i < nbPts; i++)
        {
            pt = _sommets[i];
            x += pt._x;
            y += pt._y;
            z += pt._z;
        }

        x /= (float) nbPts;
        y /= (float) nbPts;
        z /= (float) nbPts;
    }

    centre._x = x;
    centre._y = y;
    centre._z = z;

    return centre;
}

int TYFaceSet::intersects(const OSegment3D& seg, TYTabPoint& ptList) const
{
    return TYVolumeInterface::intersects(seg, ptList);
}

int TYFaceSet::isInside(const TYPoint& pt) const
{
    int inside = INTERS_NULLE;
    // find a second point outside the volume using the bounding box of the object.
    TYBox bbox = getVolEnglob();
    TYPoint scdPt = bbox.getPosition();
    scdPt._x += bbox._sizeX * 2.0f;

    OSegment3D seg(pt, scdPt);

    // Pour eviter des calculs inutiles...
    TYTabPoint pointTest;
    if (volEnglob().intersects(seg, pointTest) == INTERS_NULLE)
    {
        return inside;
    }

    // Recherche de l'intersection du segment avec chacune des faces
    // composant ce volume
    TYTabLPPolygon tabFaces = faces();
    TYPoint tmpPt;
    int nbIntersects = 0;

    for (unsigned int i = 0; i < tabFaces.size(); i++)
    {
        LPTYPolygon pFace = tabFaces[i];
        assert(pFace);

        if (pFace->intersects(seg, tmpPt) == INTERS_OUI)
        {
            // Increment du nb d'intersections trouvees
            nbIntersects++;
        }
    }

    // Si le nb d'intersection est impaire, le point est en dedans, sinon dehors.
    if ((nbIntersects % 2) != 0)
    {
        inside = INTERS_OUI;
    }
    else
    {
        inside = INTERS_NULLE;
    }

    return inside;
}

void TYFaceSet::purge()
{
    // Free all the polygon pointers in the list first
    TYPolygon* pFace = NULL;
    for (unsigned int i = 0; i < _faces.size(); i++)
    {
        pFace = _faces[i];
        delete pFace;
    }
    // then clear list content...
    _faces.clear();

    UpdateInternals();

    setIsGeometryModified(true);
}

void TYFaceSet::transform(const OMatrix& matrix)
{
    TYPolygon* pFace = NULL;
    for (unsigned int i = 0; i < _faces.size(); i++)
    {
        pFace = _faces[i];
        pFace->transform(matrix);
    }

    UpdateInternals();
}

void TYFaceSet::UpdateInternals()
{
    _normals.clear();
    _sommets.clear();
    unsigned int i;

    if (_faces.size() > 0)
    {
        // update normals list
        TYPolygon* pFace = NULL;
        for (i = 0; i < _faces.size(); i++)
        {
            pFace = _faces[i];
            _normals.push_back(pFace->normal());
        }

        // update vertice list
        pFace = NULL;
        OPoint3D pt;
        bool notFound = false;
        for (i = 0; i < _faces.size(); i++)
        {
            pFace = _faces[i];

            for (int j = 0; j < 4; j++) // Four vertice per face
            {
                pt = pFace->getPoint(j);
                notFound = true;
                for (unsigned int k = 0; k < _sommets.size(); k++)
                {
                    if (pt == _sommets[k]) { notFound = false; }
                }
                if (notFound) { _sommets.push_back(pt); }
            }
        }
    }

    // update other internal data
    calculCentreGravite();
    _volEnglob = volEnglob();
    calculRayonSphere(_volEnglob);
}

void TYFaceSet::inverseNormales()
{
    for (unsigned int i = 0; i < _faces.size(); i++)
    {
        _faces[i]->inverseNormale();
    }
}


