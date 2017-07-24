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


#include <boost/foreach.hpp>
#include <boost/tuple/tuple.hpp>

#include "Tympan/core/color.h"
#include "Tympan/core/logging.h"
#include "Tympan/models/business/TYPreferenceManager.h"
#include "Tympan/models/business/TYCalcul.h"
#include "Tympan/models/business/TYProjet.h"
#include "Tympan/models/business/TYLinearMaillage.h"
#include "Tympan/models/business/TYRectangularMaillage.h"
#include "TYMaillage.h"

#if TY_USE_IHM
  #include "Tympan/gui/widgets/TYMaillageWidget.h"
#endif

TY_EXTENSION_INST(TYMaillage);

TYMaillage::TYMaillage()
{
    _name = TYNameManager::get()->generateName(getClassName());

    _hauteur = 2.0;
    _computeAlti = true;

    _dataType = ValGlobalDBA;
    _dataFreq = 100.0f;

    _pPalette = new TYPalette();

    _pPanel = new TYPanel();

#if TY_USE_IHM
    QString pref = "MaillageDefault";
    if (TYPreferenceManager::exists(TYDIRPREFERENCEMANAGER, pref + "Hauteur"))
    {
        _hauteur = TYPreferenceManager::getFloat(TYDIRPREFERENCEMANAGER, pref + "Hauteur");
        _dataType = TYPreferenceManager::getInt(TYDIRPREFERENCEMANAGER, pref + "DataType");
        _dataFreq = TYPreferenceManager::getFloat(TYDIRPREFERENCEMANAGER, pref + "DataFreq");
    }
    else
    {
        TYPreferenceManager::setFloat(TYDIRPREFERENCEMANAGER, pref + "Hauteur", _hauteur);
        TYPreferenceManager::setInt(TYDIRPREFERENCEMANAGER, pref + "DataType", _dataType);
        TYPreferenceManager::setFloat(TYDIRPREFERENCEMANAGER, pref + "DataFreq", _dataFreq);
    }
#endif
}

TYMaillage::TYMaillage(const TYMaillage& other)
{
    *this = other;
}

TYMaillage::~TYMaillage()
{
    remAllPointCalcul();
}

TYMaillage& TYMaillage::operator=(const TYMaillage& other)
{
    if (this != &other)
    {
        TYElement::operator =(other);
        _ptsCalcul = other._ptsCalcul;
        _hauteur = other._hauteur;
        _computeAlti = other._computeAlti;
        _dataType = other._dataType;
        _dataFreq = other._dataFreq;
        _pPalette = other._pPalette;
    }
    return *this;
}

bool TYMaillage::operator==(const TYMaillage& other) const
{
    if (this != &other)
    {
        if (TYElement::operator !=(other)) { return false; }
        if (!(_ptsCalcul == other._ptsCalcul)) { return false; }
        if (_hauteur != other._hauteur) { return false; }
        if (_computeAlti != other._computeAlti) { return false; }
        if (_dataType != other._dataType) { return false; }
        if (_dataFreq != other._dataFreq) { return false; }
        if (_pPalette != other._pPalette) { return false; }
    }
    return true;
}

bool TYMaillage::operator!=(const TYMaillage& other) const
{
    return !operator==(other);
}

bool TYMaillage::deepCopy(const TYElement* pOther, bool copyId /*=true*/)
{
    if (!TYElement::deepCopy(pOther, copyId)) { return false; }

    TYMaillage* pOtherMaillage = (TYMaillage*) pOther;

    _hauteur = pOtherMaillage->_hauteur;
    _computeAlti = pOtherMaillage->_computeAlti;
    _dataType = pOtherMaillage->_dataType;
    _dataFreq = pOtherMaillage->_dataFreq;

    _pPalette->deepCopy(pOtherMaillage->_pPalette, copyId);

    _ptsCalcul.clear();
    for (unsigned int i = 0; i < pOtherMaillage->_ptsCalcul.size(); i++)
    {
        LPTYPointCalcul pPointCalcul = new TYPointCalcul();
        pPointCalcul->deepCopy(pOtherMaillage->_ptsCalcul[i], copyId);
        addPointCalcul(pPointCalcul);
    }

    return true;
}

std::string TYMaillage::toString() const
{
    return "TYMaillage";
}

DOM_Element TYMaillage::toXML(DOM_Element& domElement)
{
    DOM_Element domNewElem = TYElement::toXML(domElement);

    TYXMLTools::addElementDoubleValue(domNewElem, "hauteur", _hauteur);
    TYXMLTools::addElementIntValue(domNewElem, "computeAlti", _computeAlti);
    TYXMLTools::addElementIntValue(domNewElem, "dataType", _dataType);
    TYXMLTools::addElementFloatValue(domNewElem, "dataFreq", _dataFreq);

    DOM_Document domDoc = domElement.ownerDocument();
    TYMapIdBool::iterator it_b;
    for (it_b = _tabEtats.begin(); it_b != _tabEtats.end(); ++it_b)
    {
        DOM_Element tmpNode = domDoc.createElement("etatCalcul");
        domNewElem.appendChild(tmpNode);

        tmpNode.setAttribute("idCalcul", it_b->first.toString());
        tmpNode.setAttribute("Etat", QString(intToStr(_tabEtats[it_b->first]).c_str()));
    }

    // Sauvegarde de la palette
    _pPalette->toXML(domNewElem);

    return domNewElem;
}

int TYMaillage::fromXML(DOM_Element domElement)
{
    TYElement::fromXML(domElement);

    remAllPointCalcul();

    unsigned int i;
    bool getOk[5];
    for (i = 0; i < 5; i++) { getOk[i] = false; }
    TYPointCalcul* pPtCalcul = new TYPointCalcul();

    bool oldPalette = false;
    float dataRange[2];
    int nbOfColors;
    float hueRange[2];
    float saturationRange[2];
    float valueRange[2];
    TYUUID idCalcul;

    DOM_Element elemCur;

    QDomNodeList childs = domElement.childNodes();
    for (i = 0; i < childs.length(); i++)
    {
        elemCur = childs.item(i).toElement();
        TYXMLTools::getElementDoubleValue(elemCur, "hauteur", _hauteur, getOk[0]);
        TYXMLTools::getElementBoolValue(elemCur, "computeAlti", _computeAlti, getOk[1]);
        TYXMLTools::getElementIntValue(elemCur, "dataType", _dataType, getOk[2]);
        TYXMLTools::getElementFloatValue(elemCur, "dataFreq", _dataFreq, getOk[3]);

        if (elemCur.nodeName() == "etatCalcul")
        {
            QString strIdCalcul = TYXMLTools::getElementAttributeToString(elemCur, "idCalcul");
            idCalcul.FromString(strIdCalcul);
            bool bEtat = TYXMLTools::getElementAttributeToInt(elemCur, "Etat");
            _tabEtats[idCalcul] = bEtat;
        }

        // Ancienne version, on recupere les points de calcul.
        if (pPtCalcul->callFromXMLIfEqual(elemCur))
        {
            addPointCalcul(pPtCalcul);
            pPtCalcul = new TYPointCalcul();
        }

        // Ancienne version, on recupere les attributes pour la palette
        if (TYXMLTools::getElementFloatValue(elemCur, "dataRangeMin", dataRange[0])) { oldPalette = true; }
        TYXMLTools::getElementFloatValue(elemCur, "dataRangeMax", dataRange[1]);
        TYXMLTools::getElementIntValue(elemCur, "nbOfColors", nbOfColors);
        TYXMLTools::getElementFloatValue(elemCur, "colorRangeMinH", hueRange[0]);
        TYXMLTools::getElementFloatValue(elemCur, "colorRangeMinS", saturationRange[0]);
        TYXMLTools::getElementFloatValue(elemCur, "colorRangeMinV", valueRange[0]);
        TYXMLTools::getElementFloatValue(elemCur, "colorRangeMaxH", hueRange[1]);
        TYXMLTools::getElementFloatValue(elemCur, "colorRangeMaxS", saturationRange[1]);
        TYXMLTools::getElementFloatValue(elemCur, "colorRangeMaxV", valueRange[1]);

        // Chargement de la palette
        _pPalette->callFromXMLIfEqual(elemCur);
    }

    // Ancienne version : construction d'un palette
    if (oldPalette)
    {
        OLookupTable lookuptable(nbOfColors);
        TYColorManager::getLinearColorTable(nbOfColors, hueRange, saturationRange, valueRange, lookuptable);
        _pPalette->resetcolorMapFromColors(dataRange[0], dataRange[1], lookuptable);
    }

    delete pPtCalcul;

    return 1;
}

void TYMaillage::updateFromCalcul(LPTYCalcul pCalcul)
{
    std::vector<LPTYSpectre> *tabSpectre = pCalcul->getSpectrumDatas( getID() );

    if (tabSpectre != nullptr)
    {
        TYTabLPPointCalcul& ptsCalcul = getPtsCalcul();
        for (unsigned int i = 0; i < ptsCalcul.size() ; ++i)
        {
            ptsCalcul[i]->setSpectre(tabSpectre->at(i));
        }
    }
}

void TYMaillage::clearResult()
{
    for(unsigned int i=0; i<_ptsCalcul.size(); i++)
    {
        _ptsCalcul[i].getRealPointer()->setSpectre( new TYSpectre() );
    }
}

bool TYMaillage::addPointCalcul(LPTYPointCalcul pPtCalcul)
{
    assert(pPtCalcul);

    pPtCalcul->setParent(this);
    pPtCalcul->setEtat(true);  // Active le point pour ce calcul
    _ptsCalcul.push_back(pPtCalcul);

    setIsGeometryModified(true);

    return true;
}

bool TYMaillage::remPointCalcul(const LPTYPointCalcul pPtCalcul)
{
    assert(pPtCalcul);
    bool ret = false;
    TYTabLPPointCalcul::iterator ite;

    for (ite = _ptsCalcul.begin(); ite != _ptsCalcul.end(); ite++)
    {
        if ((*ite) == pPtCalcul)
        {
            _ptsCalcul.erase(ite);
            ret = true;
            break;
        }
    }

    setIsGeometryModified(true);

    return ret;
}

bool TYMaillage::remPointCalcul(QString idPtCalcul)
{
    bool ret = false;
    TYTabLPPointCalcul::iterator ite;

    for (ite = _ptsCalcul.begin(); ite != _ptsCalcul.end(); ite++)
    {
        if ((*ite)->getID() == idPtCalcul)
        {
            _ptsCalcul.erase(ite);
            ret = true;
            break;
        }
    }

    setIsGeometryModified(true);

    return ret;
}

void TYMaillage::remAllPointCalcul()
{
    _ptsCalcul.clear();
    setIsGeometryModified(true);
}

void TYMaillage::make(const TYTabPoint& points)
{
    for (unsigned int i = 0; i < points.size(); i++)
    {
        LPTYPointCalcul pPoint = new TYPointCalcul(points[i]);
        pPoint->setSpectre(new TYSpectre());
        addPointCalcul(pPoint);
    }
}


bool TYMaillage::checkSimilar(const TYMaillage* const pMaillage1, const TYMaillage* const pMaillage2)
{
    if (pMaillage1->getClassName() != pMaillage2->getClassName())
    {
        return false;
    }
    if (pMaillage1->isA("TYLinearMaillage"))
    {
        TYLinearMaillage* pLinearMaillage1 = (TYLinearMaillage*) pMaillage1;
        TYLinearMaillage* pLinearMaillage2 = (TYLinearMaillage*) pMaillage2;
        if (pLinearMaillage1->getSegment()->longueur() != pLinearMaillage2->getSegment()->longueur())
        {
            return false;
        }
        if (pLinearMaillage1->getDensite() != pLinearMaillage2->getDensite())
        {
            return false;
        }
    }
    else if (pMaillage1->isA("TYRectangularMaillage"))
    {
        TYRectangularMaillage* pRectangularMaillage1 = (TYRectangularMaillage*) pMaillage1;
        TYRectangularMaillage* pRectangularMaillage2 = (TYRectangularMaillage*) pMaillage2;
        if (pRectangularMaillage1->getRectangle()->getSizeX() != pRectangularMaillage2->getRectangle()->getSizeX())
        {
            return false;
        }
        if (pRectangularMaillage1->getRectangle()->getSizeY() != pRectangularMaillage2->getRectangle()->getSizeY())
        {
            return false;
        }
        if (pRectangularMaillage1->getDensiteX() != pRectangularMaillage2->getDensiteX())
        {
            return false;
        }
        if (pRectangularMaillage1->getDensiteY() != pRectangularMaillage2->getDensiteY())
        {
            return false;
        }
    }
    return true;
}

void TYMaillage::computeMesh(std::vector<MTriangle> &mesh) const
{
    mesh.clear();
    //TYCalcul* pCalcul = static_cast<TYCalcul*>(getParent());
    const TYTabLPPointCalcul& ptsCalcul = getPtsCalcul();
    int nbPointsX = 0;
    int nbPointsY = 0;
    getDimensions(nbPointsX, nbPointsY);

    int id1 = 0, id2 = 0, id3 = 0, id4 = 0;

    // Orientation 1-2-3 (and 2-4-3)
    //    3----4
    //    |\   |
    //    | \  |
    //    |  \ |
    //    |   \|
    //    1----2
    for (int i = 0; i < nbPointsX - 1 ; ++i)
    {
        for (int j = 0; j < nbPointsY - 1 ; ++j)
        {
            id1 = getIndexPtCalcul(i, j);
            id2 = getIndexPtCalcul(i + 1, j);
            id3 = getIndexPtCalcul(i, j + 1);
            id4 = getIndexPtCalcul(i + 1, j + 1);

            if (id1 < 0 || !ptsCalcul[id1]->etat())
            {
                // Higher right triangle
                if (id2 >= 0 && id4 >= 0 && id3 >= 0)
                    if (ptsCalcul[id2]->etat() && ptsCalcul[id4]->etat() && ptsCalcul[id3]->etat())
                    {
                        mesh.push_back(computeTriangle(*ptsCalcul[id2], *ptsCalcul[id4], *ptsCalcul[id3]));
                    }
            }
            else if (id2 < 0 || !ptsCalcul[id2]->etat())
            {
                // Higher left triangle
                if (id1 >= 0 && id4 >= 0 && id3 >= 0)
                    if (ptsCalcul[id1]->etat() && ptsCalcul[id4]->etat() && ptsCalcul[id3]->etat())
                    {
                        mesh.push_back(computeTriangle(*ptsCalcul[id1], *ptsCalcul[id4], *ptsCalcul[id3]));
                    }
            }
            else if (id3 < 0 || !ptsCalcul[id3]->etat())
            {
                // Lower right triangle
                if (id1 >= 0 && id2 >= 0 && id4 >= 0)
                    if (ptsCalcul[id1]->etat() && ptsCalcul[id2]->etat() && ptsCalcul[id4]->etat())
                    {
                        mesh.push_back(computeTriangle(*ptsCalcul[id1], *ptsCalcul[id2], *ptsCalcul[id4]));
                    }
            }
            else if (id4 < 0 || !ptsCalcul[id4]->etat())
            {
                // Lower left triangle
                if (id1 >= 0 && id2 >= 0 && id3 >= 0)
                    if (ptsCalcul[id1]->etat() && ptsCalcul[id2]->etat() && ptsCalcul[id3]->etat())
                    {
                        mesh.push_back(computeTriangle(*ptsCalcul[id1], *ptsCalcul[id2], *ptsCalcul[id3]));
                    }
            }
            else // We divide the square by two arbitrary triangles
            {
                // Lower left triangle
                mesh.push_back(computeTriangle(*ptsCalcul[id1], *ptsCalcul[id2], *ptsCalcul[id3]));
                // Higher right triangle
                mesh.push_back(computeTriangle(*ptsCalcul[id3], *ptsCalcul[id2], *ptsCalcul[id4]));
            }
        }
    }
}

MTriangle TYMaillage::computeTriangle(TYPointCalcul& pt1, TYPointCalcul& pt2, TYPointCalcul& pt3) const
{
    MTriangle tri;
    tri.pts[0].pt = pt1;
    tri.pts[0].scalar = getSpectrumValue(pt1);

    tri.pts[1].pt = pt2;
    tri.pts[1].scalar = getSpectrumValue(pt2);

    tri.pts[2].pt = pt3;
    tri.pts[2].scalar = getSpectrumValue(pt3);

    return tri;
}

double TYMaillage::getSpectrumValue(TYPointCalcul& pt) const
{
    if (pt.getSpectre() != nullptr)
    {
        switch (_dataType)
        {
            case ValGlobalDBA:
            default:
                return pt.getSpectre().getRealPointer()->valGlobDBA();
            case ValGlobalDBLin:
                return pt.getSpectre().getRealPointer()->valGlobDBLin();
            case DataFreq:
                return pt.getSpectre()->getValueReal(_dataFreq);
        }
    }

    return 0.;
}

void TYMaillage::computeIsoCurve(std::vector<MTriangle> &mesh, std::vector<MPoint> &isoCurve) const
{
    isoCurve.clear();
    size_t nbTriangles = mesh.size();

    OVector3D up(0, 0, 1);
    OPoint3D origin;
    MPoint mp1, mp2, mp3;
    OHPlane3D plane;
    int imask; // intersections mask
    TYPalette::values_type value;
    OColor color;
    BOOST_FOREACH(boost::tie(value, color), _pPalette->getColorMap())
    {
        origin._z = value;
        plane.set(up, origin);

        for (size_t i = 0; i < nbTriangles; i++)
        {
            imask = 0;
            const MTriangle& tri = mesh[i];
            const MPoint& pt1 = tri.pts[0];
            const MPoint& pt2 = tri.pts[1];
            const MPoint& pt3 = tri.pts[2];

            if (computeIsoPoint(plane, pt1, pt2, mp1))
            {
                imask += 1;
            }
            if (computeIsoPoint(plane, pt1, pt3, mp2))
            {
                imask += 2;
            }
            if (computeIsoPoint(plane, pt2, pt3, mp3))
            {
                imask += 4;
            }

            switch (imask)
            {
                case 0: break;
                case 3: // the plane is intersected by the segment pt1-pt2 and pt1-pt3
                    isoCurve.push_back(mp1);
                    isoCurve.push_back(mp2);
                    break;
                case 5: // the plane is intersected by the segment pt2-pt1 and pt2-pt3
                    isoCurve.push_back(mp1);
                    isoCurve.push_back(mp3);
                    break;
                case 6: // the plane is intersected by the segment pt3-pt1 and pt3-pt2
                    isoCurve.push_back(mp2);
                    isoCurve.push_back(mp3);
            }
        }
    }
}

bool TYMaillage::computeIsoPoint(const OHPlane3D& plane, const MPoint& pt1, const MPoint& pt2, MPoint& mp) const
{
    OVector3D a(pt1.pt._x, pt1.pt._y, pt1.scalar);
    OVector3D b(pt2.pt._x, pt2.pt._y, pt2.scalar);
    OPoint3D ptInter;
    double t;
    if (plane.intersects(a, b, ptInter, t) == 1)
    {
        a._z = pt1.pt._z;
        b._z = pt2.pt._z;
        // The position of the final point on the segment (a,b) depends from the
        // intersection : it'll be next to a or b (visually) depending from the value
        // of the acoustic value. We then obtain a correct isovalue.
        mp.pt = a + t * (b - a);
        mp.scalar = ptInter._z;
        return true;
    }
    return false;
}
void TYMaillage::setEtat(const TYUUID& id_calc, bool etat)
{
    _tabEtats[id_calc] = etat;
}

bool TYMaillage::etat()
{
    TYUUID id_calc = dynamic_cast<TYProjet*>( getParent() )->getCurrentCalcul()->getID();

    return etat(id_calc);
}

bool TYMaillage::etat(const TYUUID& id_calc)
{
    // Constrol point knows the calcul
    TYMapIdBool::iterator it = _tabEtats.find(id_calc);
    if ( it != _tabEtats.end() )
    {
        return (*it).second;
    }
    else
    {
        _tabEtats[id_calc] = false;
    }

    return false;
}

bool TYMaillage::etat(const TYCalcul* pCalc)
{
    assert(pCalc != nullptr);
    return etat( pCalc->getID() );
}

void TYMaillage::copyEtats(TYMaillage* pOther)
{
    _tabEtats.clear(); //On vide la map actuelle

    TYMapIdBool::iterator it_b;
    for (it_b = pOther->_tabEtats.begin(); it_b != pOther->_tabEtats.end(); ++it_b)
    {
        _tabEtats[it_b->first] = it_b->second;
    }
}

void TYMaillage::duplicateEtat(const TYUUID& idCalculRef, const TYUUID& idCalculNew)
{
    _tabEtats[idCalculNew] = _tabEtats[idCalculRef];
}

bool TYMaillage::remEtat(TYCalcul* pCalcul)
{
    assert(pCalcul);
    TYUUID id = pCalcul->getID();

    TYMapIdBool::iterator it = _tabEtats.find(id);
    if ( it != _tabEtats.end() )
    {
        _tabEtats.erase(it);
        return true;
    }

    return false;
}
