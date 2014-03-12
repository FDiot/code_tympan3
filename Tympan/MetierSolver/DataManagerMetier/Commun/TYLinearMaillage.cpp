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



#ifdef TYMPAN_USE_PRECOMPILED_HEADER
#include "Tympan/MetierSolver/DataManagerMetier/TYPHMetier.h"
#endif // TYMPAN_USE_PRECOMPILED_HEADER

#include <qdir.h>

#include "Tympan/Tools/OMessageManager.h"
#include "Tympan/MetierSolver/DataManagerCore/TYXMLManager.h"


OPROTOINST(TYLinearMaillage);


TYLinearMaillage::TYLinearMaillage()
{
    _name = TYNameManager::get()->generateName(getClassName());

    _pSeg = new TYSegment();
    _densite = 0.1;
}

TYLinearMaillage::TYLinearMaillage(const TYLinearMaillage& other)
{
    *this = other;
}

TYLinearMaillage::~TYLinearMaillage()
{
}

TYLinearMaillage& TYLinearMaillage::operator=(const TYLinearMaillage& other)
{
    if (this != &other)
    {
        TYMaillage::operator =(other);
        _pSeg = other._pSeg;
        _densite = other._densite;
    }
    return *this;
}

bool TYLinearMaillage::operator==(const TYLinearMaillage& other) const
{
    if (this != &other)
    {
        if (TYMaillage::operator !=(other)) { return false; }
        if (!(_pSeg == other._pSeg)) { return false; }
        if (!(_densite == other._densite)) { return false; }

    }
    return true;
}

bool TYLinearMaillage::operator!=(const TYLinearMaillage& other) const
{
    return !operator==(other);
}

bool TYLinearMaillage::deepCopy(const TYElement* pOther, bool copyId /*=true*/)
{
    if (!TYMaillage::deepCopy(pOther, copyId)) { return false; }

    TYLinearMaillage* pOtherMaillage = (TYLinearMaillage*) pOther;

    _densite = pOtherMaillage->_densite;

    _pSeg->deepCopy(pOtherMaillage->_pSeg, copyId);

    return true;
}

std::string TYLinearMaillage::toString() const
{
    return "TYLinearMaillage";
}

DOM_Element TYLinearMaillage::toXML(DOM_Element& domElement)
{
    DOM_Element domNewElem = TYMaillage::toXML(domElement);

    TYXMLTools::addElementDoubleValue(domNewElem, "densite", _densite);

    _pSeg->toXML(domNewElem);

    for (unsigned int i = 0; i < _ptsCalcul.size(); i++)
    {
        _ptsCalcul[i]->getSpectre()->toXML(domNewElem);
    }

    return domNewElem;
}

int TYLinearMaillage::fromXML(DOM_Element domElement)
{
    TYMaillage::fromXML(domElement);

    bool spectreIsOk = false;

    bool densiteOk = false;

    unsigned int i;

    TYSpectre* pSpectre = new TYSpectre();
    TYTabLPSpectre tabSpectre;

    DOM_Element elemCur;

    QDomNodeList childs = domElement.childNodes();
    for (i = 0; i < childs.length(); i++)
    {
        elemCur = childs.item(i).toElement();
        TYXMLTools::getElementDoubleValue(elemCur, "densite", _densite, densiteOk);

        _pSeg->callFromXMLIfEqual(elemCur);

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
        OVector3D step = _pSeg->toVector3D() * (1.0 / (double)tabSpectre.size());

        double x0 = _pSeg->_ptA._x;
        double y0 = _pSeg->_ptA._y;
        TYPoint point(x0, y0, _hauteur);

        for (i = 0; i < tabSpectre.size(); ++i)
        {
            TYPointCalcul* pPtCalcul = new TYPointCalcul(point);
            pPtCalcul->setSpectre(*(tabSpectre[i]));
            addPointCalcul(pPtCalcul);

            point._x += step._x;
            point._y += step._y;
        }
    }

    delete pSpectre;

    return 1;
}

bool TYLinearMaillage::toXML(const std::string& sFilePath)
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

bool TYLinearMaillage::fromXML(const std::string& sFilePath)
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
            if (strcmp(elt->getClassName(), "TYLinearMaillage") == 0)
            {
                TYLinearMaillage* pTmpMaillage = static_cast<TYLinearMaillage*>(elt.getRealPointer());
                TYLinearMaillage::operator=(*pTmpMaillage);
                bRet = true;
            }
        }
    }

    return bRet;
}

std::string TYLinearMaillage::toXMLString()
{
    TYXMLManager xmlManager;

    xmlManager.createDoc(TY_PRODUCT_XMLTAG_, TY_PRODUCT_VERSION_);
    xmlManager.addElement(this);

    QString retString = xmlManager.saveToString();

    return retString.toStdString();
}

bool TYLinearMaillage::fromXMLString(const std::string& sXMLString)
{
    bool bRet = false;

    TYXMLManager xmlManager;
    LPTYElementArray elements;
    if (xmlManager.loadFromString(QString(sXMLString.c_str()), elements))
    {
        if (elements.size() == 1)
        {
            LPTYElement elt = elements[0];
            if (strcmp(elt->getClassName(), "TYLinearMaillage") == 0)
            {
                TYLinearMaillage* pTmpMaillage = static_cast<TYLinearMaillage*>(elt.getRealPointer());
                TYLinearMaillage::operator=(*pTmpMaillage);
                bRet = true;
            }
        }
    }

    return bRet;
}

void TYLinearMaillage::clearResult()
{
    make(_pSeg, _densite);
}


void TYLinearMaillage::make(LPTYSegment pSeg, double densite)
{
    assert(pSeg);

    // Reset
    remAllPointCalcul();

    // Param
    _pSeg = pSeg;
    _densite = densite;

    OVector3D vec(pSeg->_ptA, pSeg->_ptB);

    // On calcul le nb de points
    int nbPts = (int)(densite * vec.norme());

    // Calcul du vecteur pas (ecart entre chaque point)
    vec = vec * (1.0 / (double) nbPts);

    // Calcul du pt de depart pour le placement des points
    OVector3D startPt = OVector3D(pSeg->_ptA);

    // Scan
    for (register int i = 0; i < nbPts; i++)
    {
        // Position du point
        OPoint3D pos = startPt + (vec * i);

        // Ajout du point au maillage
        addPointCalcul(new TYPointCalcul(pos));
    }

    setIsGeometryModified(true);
}
