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

#include <vector>

#if TY_USE_IHM
#include "Tympan/gui/widgets/TYEcranWidget.h"
#include "Tympan/gui/gl/TYEcranGraphic.h"
#endif


#include "Tympan/core/logging.h"
#include "Tympan/models/common/3d.h"
#include "Tympan/models/business/infrastructure/TYEcran.h"
#include "Tympan/models/business/geoacoustic/TYAcousticPolygon.h"
#include "Tympan/models/business/TYPreferenceManager.h"
#include "TYEcran.h"

TY_EXTENSION_INST(TYEcran);
TY_EXT_GRAPHIC_INST(TYEcran);

TYEcran::TYEcran()
{
    _name = TYNameManager::get()->generateName(getClassName());

    _bPourCalculTrajet = false;
    float r = 206.0f, g = 206.0f, b = 206.0f;

    _hauteur = 5.0f;
    _epaisseur = 0.3f;

#if TY_USE_IHM
    if (TYPreferenceManager::exists(TYDIRPREFERENCEMANAGER, "UseNewAlgoGeom"))
    {
        _bPourCalculTrajet = TYPreferenceManager::getBool(TYDIRPREFERENCEMANAGER, "UseNewAlgoGeom");
    }
    else
    {
        TYPreferenceManager::setBool(TYDIRPREFERENCEMANAGER, "UseNewAlgoGeom", _bPourCalculTrajet);
    }


    if (TYPreferenceManager::exists(TYDIRPREFERENCEMANAGER, "TYEcranGraphicColorR"))
    {
        TYPreferenceManager::getColor(TYDIRPREFERENCEMANAGER, "TYEcranGraphicColor", r, g, b);
    }
    else
    {
        TYPreferenceManager::setColor(TYDIRPREFERENCEMANAGER, "TYEcranGraphicColor", r, g, b);
    }

    if (TYPreferenceManager::exists(TYDIRPREFERENCEMANAGER, "DefaultHMur"))
    {
        _hauteur = TYPreferenceManager::getFloat(TYDIRPREFERENCEMANAGER, "DefaultHMur");
    }
    else
    {
        TYPreferenceManager::setFloat(TYDIRPREFERENCEMANAGER, "DefaultHMur", _hauteur);
    }


    if (TYPreferenceManager::exists(TYDIRPREFERENCEMANAGER, "DefaultWidthEcran"))
    {
        _epaisseur = TYPreferenceManager::getFloat(TYDIRPREFERENCEMANAGER, "DefaultWidthEcran");
    }
    else
    {
        TYPreferenceManager::setFloat(TYDIRPREFERENCEMANAGER, "DefaultWidthEcran", _epaisseur);
    }
#endif

    OColor color;
    color.r = r / 255;
    color.g = g / 255;
    color.b = b / 255;
    setColor(color);

    _bEpaisseurModified = false;
    _bHauteurModified = false;
}

TYEcran::~TYEcran()
{
}

TYEcran::TYEcran(const TYEcran& other)
{
    *this = other;
}

TYEcran& TYEcran::operator=(const TYEcran& other)
{
    TYAcousticFaceSet::operator =(other);
    _bPourCalculTrajet = other._bPourCalculTrajet;

    _hauteur = other._hauteur;
    _epaisseur = other._epaisseur;
    _squelette = other._squelette;

    updateGeometry();

    _volEnglob = volEnglob();
    calculRayonSphere(_volEnglob);
    calculCentreGravite();

    return *this;
}

bool TYEcran::operator==(const TYEcran& other) const
{
    if (this != &other)
    {
        if (TYAcousticFaceSet::operator !=(other)) { return false; }
        if (_bPourCalculTrajet != (other._bPourCalculTrajet)) { return false; }
        if (_hauteur != other._hauteur) { return false; }
        if (_epaisseur != other._epaisseur) { return false; }
        if (_squelette != other._squelette) { return false; }
    }
    return true;
}

bool TYEcran::operator!=(const TYEcran& other) const
{
    return !operator==(other);
}

bool TYEcran::deepCopy(const TYElement* pOther, bool copyId)  // copyId = true
{
    if (!TYAcousticFaceSet::deepCopy(pOther, copyId)) { return false; }

    TYEcran* pOtherEcran = (TYEcran*) pOther;

    _hauteur = pOtherEcran->_hauteur;
    _epaisseur = pOtherEcran->_epaisseur;
    _bPourCalculTrajet = pOtherEcran->_bPourCalculTrajet;
    _squelette = pOtherEcran->_squelette;

    updateGeometry();

    _volEnglob = volEnglob();
    calculRayonSphere(_volEnglob);
    calculCentreGravite();

    return true;
}

std::string TYEcran::toString() const
{
    return "TYEcran";
}

DOM_Element TYEcran::toXML(DOM_Element& domElement)
{
    DOM_Element domNewElem = TYAcousticFaceSet::toXML(domElement, false); // do not save faces

    TYXMLTools::addElementFloatValue(domNewElem, "hauteur", _hauteur);
    TYXMLTools::addElementFloatValue(domNewElem, "epaisseur", _epaisseur);

    DOM_Document domDoc = domElement.ownerDocument();
    DOM_Element domSquelElem = domDoc.createElement("squelette");

    size_t nbPts = _squelette.size();
    TYPoint pt;
    for (size_t i = 0; i < nbPts; i++)
    {
        pt = _squelette[i];
        pt.toXML(domSquelElem);
    }

    domNewElem.appendChild(domSquelElem);

    return domNewElem;
}

int TYEcran::fromXML(DOM_Element domElement)
{
    TYAcousticFaceSet::fromXML(domElement, false); // do not load faces

    // Reset
    _squelette.clear();

    float height, width;
    bool hauteurOk = false;
    bool epaisseurOk = false;

    DOM_Element elemCur;

    QDomNodeList childs = domElement.childNodes();
    for (unsigned int i = 0; i < childs.length(); i++)
    {
        elemCur = childs.item(i).toElement();

        TYXMLTools::getElementFloatValue(elemCur, "hauteur", height, hauteurOk);
        TYXMLTools::getElementFloatValue(elemCur, "epaisseur", width, epaisseurOk);

        if (elemCur.nodeName() == "squelette") // on charge le squelette...
        {
            TYPoint pt;
            DOM_Element elemCur2;
            QDomNodeList childs2 = elemCur.childNodes();
            for (unsigned int j = 0; j < childs2.length(); j++)
            {
                elemCur2 = childs2.item(j).toElement();
                if (pt.callFromXMLIfEqual(elemCur2))
                {
                    _squelette.push_back(pt);
                }
            }
        }
    }

    if (hauteurOk)
    {
        _hauteur = height;
    }

    if (epaisseurOk)
    {
        _epaisseur = width;
    }

    // On reconstruit les faces a partir du squelette
    setElements(_squelette, _hauteur, _epaisseur);
    _bEpaisseurModified = false;
    _bHauteurModified = false;

    _volEnglob = volEnglob();
    calculRayonSphere(_volEnglob);
    calculCentreGravite();

    return 1;
}

void TYEcran::getChilds(LPTYElementArray& childs, bool recursif /*=true*/)
{
    TYAcousticFaceSet::getChilds(childs, recursif);
}


TYTabPoint TYEcran::getContour() const
{
    TYPoint centre = centreGravite();
    TYTabPoint edges = sommets();
    TYTabPoint res;

    size_t nbPts = edges.size();

    if (nbPts < 1) { return res; } // return nothing, if no points

    TYPoint pt;

    for (size_t i = 0; i < nbPts; i++)
    {
        pt = edges[i];

        if (pt._z < centre._z) // on selectionne les sommets en dessous du centre de gravite
        {
            // Ajout du point
            res.push_back(pt);
        }
    }

    return res;
}

TYTabLPPolygon TYEcran::getContours() const
{
    // On récupère la liste des faces
    TYTabLPPolygon tabFaces = this->faces();

    // On recherche le plus bas
    double minZ = 1.E12;
    for (unsigned int i = 0; i < tabFaces.size(); i++)
    {
        TYTabPoint tabPts = tabFaces[i]->getContour();
        for (unsigned j = 0; j < tabPts.size(); j++)
        {
            minZ = tabPts[j]._z < minZ ? tabPts[j]._z : minZ;
        }
    }

    // On recherche la face dont tous les points sont égals à minZ
    std::vector<unsigned int> listIndices;
    bool faceOk = false;
    for (unsigned int i = 0; i < tabFaces.size(); i++)
    {
        TYTabPoint tabPts = tabFaces[i]->getContour();
        faceOk = false;
        for (unsigned j = 0; j < tabPts.size(); j++)
        {
            if (tabPts[j]._z != minZ) { faceOk = false; break; }
            faceOk = true;
        }

        if (faceOk) { listIndices.push_back(i); }
    }

    // Construction du tableau de faces retourné
    TYTabLPPolygon ret;
    for (unsigned int i = 0; i < listIndices.size(); i++)
    {
        ret.push_back(tabFaces[listIndices[i]]);
    }

    return ret;
}

void TYEcran::setHauteur(double hauteur)
{
    if (hauteur > 0.0f)
    {
        _hauteur = hauteur;
        _bHauteurModified = true;
    }
}

void TYEcran::setEpaisseur(double epaisseur)
{
    if (epaisseur > 0.0f)
    {
        _epaisseur = epaisseur;
        _bEpaisseurModified = true;
    }
}

double TYEcran::getHauteur() const
{
    return _hauteur;
}

double TYEcran::getEpaisseur() const
{
    return _epaisseur;
}

void TYEcran::updateGeometry()
{
    if (_bEpaisseurModified || _bHauteurModified)
    {
        setElements(_squelette, _hauteur, _epaisseur);
        _bEpaisseurModified = false;
        _bHauteurModified = false;
    }
}

TYPolygon* TYEcran::newFace(OVector3D s0, OVector3D s1, OVector3D s2, OVector3D s3)
{
    TYPolygon* pFace = NULL;
    TYTabPoint vertices;

    // create a new face
    pFace = new TYPolygon();
    if (pFace != NULL)
    {
        vertices.clear();
        vertices.push_back(s0);
        vertices.push_back(s1);
        vertices.push_back(s2);
        vertices.push_back(s3);
        pFace->setPoints(vertices);
    }

    return pFace;
}

// XBH: 24/11/04, version finale
bool TYEcran::setElements(TYTabPoint tabPts, double hauteur, double epaisseur)
{
    OVector3D pt0, pt1, pt2;
    OVector3D s0, s1, s2, s3; // sommets pour les faces (current)
    OVector3D ls0, ls1, ls2, ls3; // last computed values
    size_t count = tabPts.size();
    TYRepere repMur;

    if ((count < 2) || (hauteur <= 0.0) || (epaisseur <= 0.0))
    {
        return false;
    }

    _squelette.clear();
    _squelette = tabPts;
    _hauteur = hauteur;
    _epaisseur = epaisseur;

    TYTabLPPolygon faces; // l'ensembles des faces de l'ecran
    TYPolygon* pFace = NULL;
    OVector3D zvec(0, 0, 1);
    OVector3D vecN; //vecteur normal au segment courant

    // Cree la premiere face de cote
    pt0 = tabPts[0];
    pt1 = tabPts[1];

    // Vecteur pour la "longueur" de la face
    OVector3D vec01(pt0, pt1);
    vec01.normalize();
    // compute normal to the vector vec01
    vecN = vec01.cross(zvec);
    vecN.normalize();
    s0 = pt0 + (epaisseur / 2.0f) * vecN;
    s1 = pt0 - (epaisseur / 2.0f) * vecN;
    s2 = s0 + hauteur * zvec;
    s3 = s1 + hauteur * zvec;

    // Define 1st extremity
    pFace = newFace(s0, s2, s3, s1); // order of point define normal of face
    if (pFace != NULL) { faces.push_back(pFace); }

    ls0 = s0;
    ls1 = s1;
    ls2 = s2;
    ls3 = s3;

    // On construit les faces de la tubulure
    OPoint3D a1, b1, c1, d1; // (a1,b1) et (c1,d1) segment // au 1er segment
    OPoint3D a2, b2, c2, d2; // (a2,b2) et (c2,d2) segment // au segment suivant
    OPoint3D ptI1, ptI2;

    // if number of segments > 1 loop on each segment from the 1st to the last
    for (size_t i = 1; i < count - 1; i++)
    {
        pt0 = tabPts[i - 1];
        pt1 = tabPts[i];
        pt2 = tabPts[i + 1];

        // Vecteur pour la "longueur" de la face
        OVector3D vec01(pt0, pt1); // 1er segment
        vec01.normalize();
        OVector3D vec02(pt1, pt2); // segment suivant
        vec02.normalize();

        vecN = vec01.cross(zvec); // compute normal to the vector vec01
        vecN.normalize();

        a1 = pt0 + (epaisseur / 2.0f) * vecN;
        b1 = pt1 + (epaisseur / 2.0f) * vecN;
        c1 = pt0 - (epaisseur / 2.0f) * vecN;
        d1 = pt1 - (epaisseur / 2.0f) * vecN;

        vecN = vec02.cross(zvec); // compute normal to the vector vec01
        vecN.normalize();

        a2 = pt1 + (epaisseur / 2.0f) * vecN;
        b2 = pt2 + (epaisseur / 2.0f) * vecN;
        c2 = pt1 - (epaisseur / 2.0f) * vecN;
        d2 = pt2 - (epaisseur / 2.0f) * vecN;

        OGeometrie::intersDroitesPoints(a1, b1, a2, b2, ptI1);
        OGeometrie::intersDroitesPoints(c1, d1, c2, d2, ptI2);

        s0 = ptI1;
        s1 = ptI2;
        s2 = s0 + hauteur * zvec;
        s3 = s1 + hauteur * zvec;

        // bottom
        pFace = newFace(s0, ls0, ls1, s1);
        if (pFace != NULL) { faces.push_back(pFace); }

        // front
        pFace = newFace(s1, ls1, ls3, s3);
        if (pFace != NULL) { faces.push_back(pFace); }

        // top
        pFace = newFace(s3, ls3, ls2, s2);
        if (pFace != NULL) { faces.push_back(pFace); }

        // back
        pFace = newFace(ls0, s0, s2, ls2);
        if (pFace != NULL) { faces.push_back(pFace); }

        // keep previous vertice
        ls0 = s0;
        ls1 = s1;
        ls2 = s2;
        ls3 = s3;
    }

    // derniere portion de tubulure
    pt0 = tabPts[count - 2];
    pt1 = tabPts[count - 1];

    // Vecteur pour la "longueur" de la face
    OVector3D vec02(pt0, pt1);
    vec02.normalize();
    // compute normal to the vector vec02
    vecN = vec02.cross(zvec);
    vecN.normalize();
    s0 = pt1 + (epaisseur / 2.0f) * vecN;
    s1 = pt1 - (epaisseur / 2.0f) * vecN;
    s2 = s0 + hauteur * zvec;
    s3 = s1 + hauteur * zvec;

    // bottom
    pFace = newFace(s0, ls0, ls1, s1);
    if (pFace != NULL) { faces.push_back(pFace); }

    // front
    pFace = newFace(s1, ls1, ls3, s3);
    if (pFace != NULL) { faces.push_back(pFace); }

    // top
    pFace = newFace(s3, ls3, ls2, s2);
    if (pFace != NULL) { faces.push_back(pFace); }

    // back
    pFace = newFace(ls0, s0, s2, ls2);
    if (pFace != NULL) { faces.push_back(pFace); }

    // on bouche le cote de fin
    pFace = newFace(s0, s1, s3, s2);
    if (pFace != NULL) { faces.push_back(pFace); }

    // on sauve le tout
    _faces.setFaces(faces);

    _volEnglob = volEnglob();
    calculRayonSphere(_volEnglob);
    calculCentreGravite();

    return true;
}

int TYEcran::intersects(const OSegment3D& seg, TYTabPoint& ptList) const
{
    return TYAcousticFaceSet::intersects(seg, ptList);
}

void TYEcran::setacousticFacesPourCalcul(bool bPourCalculTrajet)
{
    _bPourCalculTrajet = bPourCalculTrajet;
}

TYTabAcousticSurfaceGeoNode TYEcran::acousticFaces()
{
    TYTabAcousticSurfaceGeoNode tab;

    size_t nbFaces = _faces.getNbFaces();
    TYTabLPPolygon faces = _faces.faces();

    LPTYAcousticSurfaceGeoNode pAccSurfGeoNode;
    TYAcousticPolygon* pAccPolygon;

    for (size_t i = 0; i < nbFaces; i++)
    {
        pAccPolygon = new TYAcousticPolygon();
        pAccPolygon->setParent(this); // XBH: pourquoi on prend ce parent la??...

        // Geomtrie
        *pAccPolygon->getPolygon() = *faces.at(i);
        tab.push_back(new TYAcousticSurfaceGeoNode(pAccPolygon));
    }

    return tab;
}

int TYEcran::isInside(const TYPoint& pt) const
{
    int res = INTERS_NULLE;
    OPoint3D ptCopy(pt);

    // Le test avec le volume englobant permet de tester si le pt
    // se trouve entre les plans du sol et du plafond,
    // et d'evite aussi des calculs inutiles...
    if (_volEnglob.isInside(pt) == INTERS_NULLE) { return res; }

    TYTabLPPolygon tabFaces = getContours();

    double minZ = 0.0, maxZ = 0.0;

    // Pour toutes les faces
    for (unsigned int i = 0; i < tabFaces.size(); i++)
    {
        TYTabPoint tabPts = tabFaces[i]->getContour();
        size_t nbPts = tabPts.size();
        minZ = tabPts[0]._z;
        maxZ = getHauteur() + minZ;

        // On s'assure enfin que le point est compris dans la hauteur de l'écran
        if ((pt._z < minZ) || (pt._z > maxZ)) { return INTERS_NULLE; }

        if (nbPts >= 3)
        {
            // On pose tout au sol !
            ptCopy._z = 0.0;

            // On se ramene a un plan (z=0), on teste si le pt est a l'interieur
            // du polygone forme par le contour de l'etage
            OPoint3D* pts = new OPoint3D[nbPts];

            for (size_t i = 0; i < nbPts; i++)
            {
                // Creation du polygone
                pts[i] = tabPts[i];

                // ... tout au sol !
                pts[i]._z = 0.0;
            }

            LPTYPolygon pPoly = new TYPolygon(tabPts);

            OPoint3D ptMin, ptMax;

            OGeometrie::boundingBox(pts, static_cast<int>(nbPts), ptMin, ptMax);

            OBox box(ptMin, ptMax);

#if TY_USE_IHM
            res = OGeometrie::pointInPolygonRayCasting(ptCopy, pts, static_cast<int>(nbPts));
#else
            res = OGeometrie::pointInPolygonAngleSum(ptCopy, pts, nbPts);
#endif

            delete [] pts;

            if (res) { break; } // Si on a trouvé au moins une face qui "matche" on sort de la boucle
        }
    }

    return res;
}
