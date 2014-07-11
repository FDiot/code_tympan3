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
#include "Tympan/gui/widgets/TYRectangularMaillageWidget.h"
#include "Tympan/GraphicIHM/DataManagerGraphic/TYRectangularMaillageGraphic.h"
#endif

#ifdef TYMPAN_USE_PRECOMPILED_HEADER
#include "Tympan/models/business/TYPHMetier.h"
#endif // TYMPAN_USE_PRECOMPILED_HEADER

#include <qdir.h>

#include "Tympan/core/logging.h"
#include "Tympan/models/business/TYXMLManager.h"


TY_EXTENSION_INST(TYRectangularMaillage);
TY_EXT_GRAPHIC_INST(TYRectangularMaillage);

TYRectangularMaillage::TYRectangularMaillage()
{
    _name = TYNameManager::get()->generateName(getClassName());

    _pRect = new TYRectangle();
    _densiteX = 0.1;
    _densiteY = 0.1;
    _nbPointsX = 0;
    _posLabel = TYPoint(0, 0, 0);
}

TYRectangularMaillage::TYRectangularMaillage(const TYRectangularMaillage& other)
{
    *this = other;
}

TYRectangularMaillage::~TYRectangularMaillage()
{
}

TYRectangularMaillage& TYRectangularMaillage::operator=(const TYRectangularMaillage& other)
{
    if (this != &other)
    {
        TYMaillage::operator =(other);
        _pRect = other._pRect;
        _densiteX = other._densiteX;
        _densiteY = other._densiteY;
        _nbPointsX = other._nbPointsX;
    }
    return *this;
}

bool TYRectangularMaillage::operator==(const TYRectangularMaillage& other) const
{
    if (this != &other)
    {
        if (TYMaillage::operator !=(other)) { return false; }
        if (!(*_pRect == *(other._pRect))) { return false; }
        if (!(_densiteX == other._densiteX)) { return false; }
        if (!(_densiteY == other._densiteY)) { return false; }
    }
    return true;
}

bool TYRectangularMaillage::operator!=(const TYRectangularMaillage& other) const
{
    return !operator==(other);
}

bool TYRectangularMaillage::deepCopy(const TYElement* pOther, bool copyId /*=true*/)
{
    if (!TYMaillage::deepCopy(pOther, copyId)) { return false; }

    TYRectangularMaillage* pOtherMaillage = (TYRectangularMaillage*) pOther;

    _densiteX = pOtherMaillage->_densiteX;
    _densiteY = pOtherMaillage->_densiteY;

    _pRect->deepCopy(pOtherMaillage->_pRect, copyId);

    return true;
}

std::string TYRectangularMaillage::toString() const
{
    return "TYRectangularMaillage";
}

DOM_Element TYRectangularMaillage::toXML(DOM_Element& domElement)
{
    DOM_Element domNewElem = TYMaillage::toXML(domElement);

    TYXMLTools::addElementDoubleValue(domNewElem, "densiteX", _densiteX);
    TYXMLTools::addElementDoubleValue(domNewElem, "densiteY", _densiteY);

    _pRect->toXML(domNewElem);

    if (TYProjet::gSaveValues)
    {
        for (unsigned int i = 0; i < _ptsCalcul.size(); i++)
        {
            _ptsCalcul[i]->getSpectre()->toXML(domNewElem);
        }
    }

    return domNewElem;
}

int TYRectangularMaillage::fromXML(DOM_Element domElement)
{
    TYMaillage::fromXML(domElement);

    bool spectreIsOk = false;

    bool densiteXOk = false;
    bool densiteYOk = false;

    unsigned int i, j;

    TYSpectre* pSpectre = new TYSpectre();
    TYTabLPSpectre tabSpectre;

    DOM_Element elemCur;

    QDomNodeList childs = domElement.childNodes();
    for (i = 0; i < childs.length(); i++)
    {
        elemCur = childs.item(i).toElement();

        TYXMLTools::getElementDoubleValue(elemCur, "densiteX", _densiteX, densiteXOk);
        TYXMLTools::getElementDoubleValue(elemCur, "densiteY", _densiteY, densiteYOk);

        _pRect->callFromXMLIfEqual(elemCur);

        // Nouvelle version : si on trouve des spectres
        if (pSpectre->callFromXMLIfEqual(elemCur))
        {
            tabSpectre.push_back(pSpectre);
            pSpectre = new TYSpectre();
            if (!spectreIsOk)
            {
                spectreIsOk = true;
            }
        }
    }

    // Nouvelle version
    if (spectreIsOk)
    {
        unsigned long x, y;
        OVector3D stepX, stepY;
        getDimensionsAndSteps(x, y, stepX, stepY);
        _nbPointsX = x;

        double x0 = _pRect->getMinX();
        double y0 = _pRect->getMaxY();
        TYPoint point(x0, y0, _hauteur);

        // On calcul chaque points
        for (j = 0; j < y; ++j)
        {
            for (i = 0; i < x; ++i)
            {
                TYPointCalcul* pPtCalcul = new TYPointCalcul(point);
                pPtCalcul->setSpectre(*(tabSpectre[i + j * x]));
                addPointCalcul(pPtCalcul);

                point._x += stepX._x;
            }

            point._x = x0;
            point._y += stepY._y;
        }
    }
    else
    {
        // CLM-NT33 : Chargement de fichier avec maillage inexistant
        make(_pRect, _densiteX, _densiteY);
    }

    delete pSpectre;

    return 1;
}

bool TYRectangularMaillage::toXML(const std::string& sFilePath)
{
    bool bRet = false;
    QString fileName = QString(sFilePath.c_str());
    int i = fileName.lastIndexOf('/');
    QDir fileDirectory = QDir(fileName.mid(0, i));
    if (!fileDirectory.exists())
    {
        fileDirectory.mkdir(fileName.mid(0, i));
    }

    if (!fileName.isEmpty())
    {
        if (!fileName.endsWith(".xml"))
        {
            fileName += ".xml";
        }
    }
    else
    {
        return false;
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

bool TYRectangularMaillage::fromXML(const std::string& sFilePath)
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
            if (strcmp(elt->getClassName(), "TYRectangularMaillage") == 0)
            {
                TYRectangularMaillage* pTmpMaillage = static_cast<TYRectangularMaillage*>(elt.getRealPointer());
                TYRectangularMaillage::operator=(*pTmpMaillage);
                bRet = true;
            }
        }
    }

    return bRet;
}

std::string TYRectangularMaillage::toXMLString()
{
    TYXMLManager xmlManager;

    xmlManager.createDoc(TY_PRODUCT_XMLTAG_, TY_PRODUCT_VERSION_);
    xmlManager.addElement(this);

    QString retString = xmlManager.saveToString();

    return retString.toStdString();
}

bool TYRectangularMaillage::fromXMLString(const std::string& sXMLString)
{
    bool bRet = false;

    TYXMLManager xmlManager;
    LPTYElementArray elements;
    if (xmlManager.loadFromString(QString(sXMLString.c_str()), elements))
    {
        if (elements.size() == 1)
        {
            LPTYElement elt = elements[0];
            if (strcmp(elt->getClassName(), "TYRectangularMaillage") == 0)
            {
                TYRectangularMaillage* pTmpMaillage = static_cast<TYRectangularMaillage*>(elt.getRealPointer());
                TYRectangularMaillage::operator=(*pTmpMaillage);
                bRet = true;
            }
        }
    }

    return bRet;
}

void TYRectangularMaillage::clearResult()
{
    make(_pRect, _densiteX, _densiteY);
}

void TYRectangularMaillage::make(LPTYRectangle pRect, double densiteX, double densiteY)
{
    OVector3D vecX, vecY;
    unsigned long nbPtsX, nbPtsY;

    assert(pRect);

    // Reset
    remAllPointCalcul();

    // Param
    _pRect = pRect;
    _densiteX = densiteX;
    _densiteY = densiteY;

    getDimensionsAndSteps(nbPtsX, nbPtsY, vecX, vecY);
    _nbPointsX = nbPtsX;

    // Calcul du pt de depart pour le placement des points
    OVector3D startPt = OVector3D(pRect->_pts[0]);

    _posLabel = startPt;

    // Scan Y
    for (register unsigned int iY = 0; iY < nbPtsY; iY++)
    {
        // Scan X
        for (register unsigned int iX = 0; iX < nbPtsX; iX++)
        {
            // Position du point
            OPoint3D pos = startPt + (vecX * iX) + (vecY * iY);

            // Ajout du point au maillage
            addPointCalcul(new TYPointCalcul(pos));
        }
    }

    setIsGeometryModified(true);
}

void TYRectangularMaillage::getDimensionsAndSteps(unsigned long& x, unsigned long& y, OVector3D& stepx, OVector3D& stepy) const
{
    assert(_pRect);

    // On cherche les vecteurs des cotes X et Y du rectangle
    OVector3D vecX(_pRect->_pts[0], _pRect->_pts[1]);
    OVector3D vecY(_pRect->_pts[0], _pRect->_pts[3]);

    // On calcul le nb de points en X et Y
    unsigned long nbPtsX = (unsigned long)(_densiteX * vecX.norme());
    unsigned long nbPtsY = (unsigned long)(_densiteY * vecY.norme());

    // On essaye de se rapprocher au mieux de la densite totale (H*V)
    // (On veut au moins 1 point a l'interieur)
    if ((nbPtsX * nbPtsY) != (_pRect->surface() * _densiteX * _densiteY))
    {
        nbPtsX ? nbPtsX : nbPtsX++;
        nbPtsY ? nbPtsY : nbPtsY++;
    }

    x = nbPtsX + 1;
    y = nbPtsY + 1;

    // Calcul des vecteurs pas en X et Y (ecart entre chaque point)
    stepx = vecX * (1.0 / (double) nbPtsX);
    stepy = vecY * (1.0 / (double) nbPtsY);
}

int TYRectangularMaillage::getIndexPtCalcul(int x, int y) const
{
    return x + y * _nbPointsX;
}

void TYRectangularMaillage::getDimensions(int& x, int& y) const
{
    unsigned long longx, longy;
    OVector3D stepx, stepy;
    getDimensionsAndSteps(longx, longy, stepx, stepy);
    x = longx;
    y = longy;
}
