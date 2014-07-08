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

#include <qdir.h>

#ifdef TYMPAN_USE_PRECOMPILED_HEADER
  #include "Tympan/models/business/TYPHMetier.h"
#endif // TYMPAN_USE_PRECOMPILED_HEADER
#if TY_USE_IHM
  #include "Tympan/gui/gl/TYBoundaryNoiseMapGraphic.h"
  #include "Tympan/gui/widgets/TYBoundaryNoiseMapWidget.h"
#endif
#include "Tympan/core/logging.h"
#include "Tympan/models/business/TYXMLManager.h"

#undef min
#undef max


//! Compute the squared distance between a point and a segment.
/*!
 \param point_x
 \param point_y
 \param a_x
 \param a_y
 \param b_x
 \param b_y
  \return Squared distance.
 */
double compute_segment_point_square_distance(double point_x, double point_y,
                                             double a_x, double a_y,
                                             double b_x, double b_y)
{
    double l2 = (a_x - b_x) * (a_x - b_x) + (a_y - b_y) * (a_y - b_y) ;
    if (l2 != 0)
    {
        double t = ((point_x - a_x) * (b_x - a_x) + (point_y - a_y) * (b_y - a_y)) / l2;
        if (t < 0)
        {
            return (point_x - a_x) * (point_x - a_x) + (point_y - a_y) * (point_y - a_y);
        }
        else if (t > 1)
        {
            return (point_x - b_x) * (point_x - b_x) + (point_y - b_y) * (point_y - b_y);
        }
        else
        {
            return (point_x - (a_x + t * (b_x - a_x)))
                   * (point_x - (a_x + t * (b_x - a_x)))
                   + (point_y - (a_y + t * (b_y - a_y)))
                   * (point_y - (a_y + t * (b_y - a_y)));
        }
    }
    return (point_x - a_x) * (point_x - a_x) + (point_y - a_y) * (point_y - a_y);
}


TY_EXTENSION_INST(TYBoundaryNoiseMap);
TY_EXT_GRAPHIC_INST(TYBoundaryNoiseMap);

TYBoundaryNoiseMap::TYBoundaryNoiseMap()
{
    _name = TYNameManager::get()->generateName(getClassName());

    _thickness = 0.1;
    _closed = false;
    _canBeClosed = true;
    _density = 0.1;
    _posLabel = TYPoint(0, 0, 0);
}

TYBoundaryNoiseMap::TYBoundaryNoiseMap(const TYBoundaryNoiseMap& rhs)
{
    *this = rhs;
}

TYBoundaryNoiseMap::~TYBoundaryNoiseMap()
{
}

TYBoundaryNoiseMap& TYBoundaryNoiseMap::operator=(const TYBoundaryNoiseMap& rhs)
{
    if (this != &rhs)
    {
        TYMaillage::operator =(rhs);
        _thickness = rhs._thickness;
        _closed = rhs._closed;
        _density = rhs._density;
        _nbPointsY = rhs._nbPointsY;
        _tabPoint = rhs._tabPoint;
        _ptsIndices = rhs._ptsIndices;
    }
    return *this;
}

bool TYBoundaryNoiseMap::operator==(const TYBoundaryNoiseMap& rhs) const
{
    if (this != &rhs)
    {
        if (TYMaillage::operator !=(rhs)) { return false; }
        if (!(_thickness == rhs._thickness)) { return false; }
        if (!(_closed == rhs._closed)) { return false; }
        if (!(_density == rhs._density)) { return false; }
    }
    return true;
}

bool TYBoundaryNoiseMap::operator!=(const TYBoundaryNoiseMap& rhs) const
{
    return !operator==(rhs);
}

bool TYBoundaryNoiseMap::deepCopy(const TYElement* pOther, bool copyId /*=true*/)
{
    if (!TYMaillage::deepCopy(pOther, copyId)) { return false; }

    const TYBoundaryNoiseMap* pOtherMaillage =
        static_cast<const TYBoundaryNoiseMap*>(pOther);
    if (!pOtherMaillage)
    {
        return false;
    }

    _thickness = pOtherMaillage->_thickness;
    _closed = pOtherMaillage->_closed;
    _density = pOtherMaillage->_density;

    return true;
}

std::string TYBoundaryNoiseMap::toString() const
{
    return "TYBoundaryNoiseMap";
}

DOM_Element TYBoundaryNoiseMap::toXML(DOM_Element& domElement)
{
    DOM_Element domNewElem = TYMaillage::toXML(domElement);

    TYXMLTools::addElementDoubleValue(domNewElem, "thickness", _thickness);
    TYXMLTools::addElementBoolValue(domNewElem, "closed", _closed);
    TYXMLTools::addElementDoubleValue(domNewElem, "density", _density);

    TYXMLTools::addElementUIntValue(domNewElem, "nbPoints", _tabPoint.size());

    size_t nbPoints = _tabPoint.size();
    for (size_t i = 0; i < nbPoints; ++i)
    {
        _tabPoint[i].toXML(domNewElem);
    }

    if (TYProjet::gSaveValues)
    {
        nbPoints = _ptsCalcul.size();
        for (size_t i = 0; i < nbPoints; ++i)
        {
            _ptsCalcul[i]->getSpectre()->toXML(domNewElem);
        }
    }

    return domNewElem;
}

int TYBoundaryNoiseMap::fromXML(DOM_Element domElement)
{
    TYMaillage::fromXML(domElement);

    bool nbPointsIsOk = false;

    unsigned int i;
    int nbPoints = 0;
    TYPoint pt;

    TYSpectre* pSpectre = new TYSpectre();
    TYTabLPSpectre tabSpectre;

    DOM_Element elemCur;

    QDomNodeList childs = domElement.childNodes();
    for (i = 0; i < childs.length(); i++)
    {
        elemCur = childs.item(i).toElement();

        TYXMLTools::getElementDoubleValue(elemCur, "thickness", _thickness);
        TYXMLTools::getElementBoolValue(elemCur, "closed", _closed);
        TYXMLTools::getElementDoubleValue(elemCur, "density", _density);

        TYXMLTools::getElementIntValue(elemCur, "nbPoints", nbPoints, nbPointsIsOk);
        if (nbPointsIsOk)
        {
            _tabPoint.reserve(nbPoints);
        }

        if (pt.callFromXMLIfEqual(elemCur))
        {
            _tabPoint.push_back(pt);
        }

        // New version : if we encounter spectra
        if (pSpectre->callFromXMLIfEqual(elemCur))
        {
            tabSpectre.push_back(pSpectre);
            pSpectre = new TYSpectre();
        }
    }

    clearResult();

    if (nbPointsIsOk)
    {
        // For each TYPointCalcul we set its spectrum value,
        // the array of spectra and TYPointCalcul should be matched together
        // because the position of TYPointCalcul are not stored
        TYTabLPPointCalcul& ptsCalcul = getPtsCalcul();
        size_t nbSpectre = std::min(tabSpectre.size(), ptsCalcul.size());
        for (i = 0; i < nbSpectre; ++i)
        {
            ptsCalcul[i]->setSpectre(*(tabSpectre[i]));
        }
    }
    delete pSpectre;

    return 1;
}

bool TYBoundaryNoiseMap::toXML(const std::string& sFilePath)
{
    bool bRet = false;
    QString fileName = QString(sFilePath.c_str());
    int i = fileName.lastIndexOf('/');
    QDir fileDirectory = QDir(fileName.mid(0, i));
    if (!fileDirectory.exists())
    {
        fileDirectory.mkdir(fileName.mid(0, i));
    }

    if (fileName.isEmpty())
    {
        return false;
    }
    if (!fileName.endsWith(".xml"))
    {
        fileName += ".xml";
    }

    TYXMLManager xmlManager;

    xmlManager.createDoc(TY_PRODUCT_XMLTAG_, TY_PRODUCT_VERSION_);
    xmlManager.addElement(this);

    if (xmlManager.save(fileName) == 0)
    {
        bRet = true;
    }

    return bRet;
}

bool TYBoundaryNoiseMap::fromXML(const std::string& sFilePath)
{
    bool bRet = false;
    QString fileName = QString(sFilePath.c_str());

    if (fileName.isEmpty())
    {
        return false;
    }

    TYXMLManager xmlManager;
    LPTYElementArray elements;
    if (xmlManager.load(fileName, elements))
    {
        if (elements.size() == 1)
        {
            LPTYElement elt = elements[0];
            if (strcmp(elt->getClassName(), "TYBoundaryNoiseMap") == 0)
            {
                TYBoundaryNoiseMap* pTmpMaillage = static_cast<TYBoundaryNoiseMap*>(elt.getRealPointer());
                TYBoundaryNoiseMap::operator=(*pTmpMaillage);
                bRet = true;
            }
        }
    }

    return bRet;
}

std::string TYBoundaryNoiseMap::toXMLString()
{
    TYXMLManager xmlManager;

    xmlManager.createDoc(TY_PRODUCT_XMLTAG_, TY_PRODUCT_VERSION_);
    xmlManager.addElement(this);

    QString retString = xmlManager.saveToString();

    return retString.toStdString();
}

bool TYBoundaryNoiseMap::fromXMLString(const std::string& sXMLString)
{
    bool bRet = false;

    TYXMLManager xmlManager;
    LPTYElementArray elements;
    if (xmlManager.loadFromString(QString(sXMLString.c_str()), elements))
    {
        if (elements.size() == 1)
        {
            LPTYElement elt = elements[0];
            if (strcmp(elt->getClassName(), "TYBoundaryNoiseMap") == 0)
            {
                TYBoundaryNoiseMap* pTmpMaillage = static_cast<TYBoundaryNoiseMap*>(elt.getRealPointer());
                TYBoundaryNoiseMap::operator=(*pTmpMaillage);
                bRet = true;
            }
        }
    }

    return bRet;
}

void TYBoundaryNoiseMap::clearResult()
{
    make(_tabPoint, _thickness, _closed, _density);
}

// XXX Add some comments.
void TYBoundaryNoiseMap::make(const TYTabPoint& tabPoint, double thickness, bool closed, double density)
{
    // Reset
    remAllPointCalcul();
    _ptsIndices.clear();

    // Param
    _thickness = thickness;
    _closed = closed;
    _density = density;
    _tabPoint = tabPoint;

    const size_t nbPoints = _tabPoint.size();
    for (size_t i = 0; i < nbPoints; ++i)
    {
        _tabPoint[i]._z = _hauteur;
    }

    double box_x_min = 0., box_x_max = 0., box_y_min = 0., box_y_max = 0.;
    computeBoundingBox(box_x_min, box_x_max, box_y_min, box_y_max);

    computePoints(box_x_min, box_x_max, box_y_min, box_y_max);

    setIsGeometryModified(true);
}

void TYBoundaryNoiseMap::computeBoundingBox(double& box_x_min, double& box_x_max, double& box_y_min, double& box_y_max) const
{
    box_x_min = _tabPoint[0]._x;
    box_x_max = _tabPoint[0]._x;
    box_y_min = _tabPoint[0]._y;
    box_y_max = _tabPoint[0]._y;

    // XXX Should use OBox with OBox::EnLarge? BUT it's in 3D.
    const size_t length = _tabPoint.size();
    double current_x = 0.;
    double current_y = 0.;
    for (size_t i = 1; i < length; ++i)
    {
        current_x = _tabPoint[i]._x;
        current_y = _tabPoint[i]._y;

        box_x_min = std::min(box_x_min, current_x);
        box_x_max = std::max(box_x_max, current_x);

        box_y_min = std::min(box_y_min, current_y);
        box_y_max = std::max(box_y_max, current_y);
    }

    // Create the bounding box (greater than tickness / 2.).
    box_x_min -= _thickness * 0.5;
    box_x_max += _thickness * 0.5;
    box_y_min -= _thickness * 0.5;
    box_y_max += _thickness * 0.5;
}

void TYBoundaryNoiseMap::computePoints(double box_x_min, double box_x_max, double box_y_min, double box_y_max)
{
    int nb_points_x = std::ceil((box_x_max - box_x_min) * _density);
    int nb_points_y = std::ceil((box_y_max - box_y_min) * _density);
    const double step_x = (box_x_max - box_x_min) / (double) nb_points_x;
    const double step_y = (box_y_max - box_y_min) / (double) nb_points_y;

    // Initialize 2D array
    ++nb_points_x;
    ++nb_points_y;
    _nbPointsY = nb_points_y;
    int nbPoints = nb_points_x * nb_points_y;
    _ptsIndices = std::vector<int>(nbPoints, -1);

    size_t l = 1;
    double current_x = 0.;
    double current_y = 0.;
    const double squared_thick = _thickness * _thickness / 4.;
    const size_t length = _tabPoint.size();
    const size_t nb_segment = _closed ? length : length - 1;

    for (int i = 0; i < nb_points_x; ++i)
    {
        current_x = box_x_min + i * step_x;
        for (int j = 0; j < nb_points_y; ++j)
        {
            int index2D = i * nb_points_y + j;
            current_y = box_y_min + j * step_y;
            for (size_t k = 0; k < nb_segment; ++k)
            {
                l = (k < length - 1) ? k + 1 : 0;
                double squared_distance =
                    compute_segment_point_square_distance(current_x, current_y,
                                                          _tabPoint[k]._x, _tabPoint[k]._y,
                                                          _tabPoint[l]._x, _tabPoint[l]._y);
                if (squared_distance <= squared_thick)
                {
                    addPointCalcul(new TYPointCalcul(TYPoint(current_x, current_y, _hauteur)));
                    _ptsIndices[index2D] = _ptsCalcul.size() - 1;
                    break; //no need to test with other segments
                }
            }
        }
    }
}

double TYBoundaryNoiseMap::computeMinimumDensity(double thickness)
{
    return 2.0 / thickness;
}

int TYBoundaryNoiseMap::getIndexPtCalcul(int x, int y) const
{
    return _ptsIndices[x * _nbPointsY + y];
}

void TYBoundaryNoiseMap::getDimensions(int& x, int& y) const
{
    y = _nbPointsY;
    x = _ptsIndices.size() / y;
}
