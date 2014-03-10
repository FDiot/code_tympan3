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
#include "Tympan/GraphicIHM/DataManagerIHM/TYSourceSurfacicWidget.h"
#endif

#ifdef TYMPAN_USE_PRECOMPILED_HEADER
#include "Tympan/MetierSolver/DataManagerMetier/TYPHMetier.h"
#endif // TYMPAN_USE_PRECOMPILED_HEADER
#include "Tympan/MetierSolver/DataManagerCore/TYElementCollection.h"

#include "Tympan/Tools/OMessageManager.h"


OPROTOINST(TYSourceSurfacic);
TY_EXTENSION_INST(TYSourceSurfacic);

TYSourceSurfacic::TYSourceSurfacic():
    _densiteSrcsH(0.0),
    _densiteSrcsV(0.0),
    _srcsCorr(false)
{
    _name = TYNameManager::get()->generateName(getClassName());
}

TYSourceSurfacic::TYSourceSurfacic(const TYSourceSurfacic& other)
{
    *this = other;
}

TYSourceSurfacic::~TYSourceSurfacic()
{
    remAllSrc();
}

TYSourceSurfacic& TYSourceSurfacic::operator=(const TYSourceSurfacic& other)
{
    if (this != &other)
    {
        TYSource::operator =(other);
        _densiteSrcsH = other._densiteSrcsH;
        _densiteSrcsV = other._densiteSrcsV;
        _srcsCorr = other._srcsCorr;
        _listSrcPonct = other._listSrcPonct;
    }
    return *this;
}

bool TYSourceSurfacic::operator==(const TYSourceSurfacic& other) const
{
    if (this != &other)
    {
        if (TYSource::operator !=(other)) { return false; }
        if (_densiteSrcsH != other._densiteSrcsH) { return false; }
        if (_densiteSrcsV != other._densiteSrcsV) { return false; }
        if (_srcsCorr != other._srcsCorr) { return false; }
        if (!(_listSrcPonct == other._listSrcPonct)) { return false; }
    }
    return true;
}

bool TYSourceSurfacic::operator!=(const TYSourceSurfacic& other) const
{
    return !operator==(other);
}

bool TYSourceSurfacic::deepCopy(const TYElement* pOther, bool copyId /*=true*/)
{
    if (!TYSource::deepCopy(pOther, copyId)) { return false; }

    TYSourceSurfacic* pOtherSrc = (TYSourceSurfacic*) pOther;

    _densiteSrcsH = pOtherSrc->_densiteSrcsH;
    _densiteSrcsV = pOtherSrc->_densiteSrcsV;
    _srcsCorr = pOtherSrc->_srcsCorr;

    purge();
    for (unsigned int i = 0; i < pOtherSrc->_listSrcPonct.size(); i++)
    {
        LPTYSourcePonctuelle pSrc = (TYSourcePonctuelle*) pOtherSrc->_listSrcPonct[i]->clone();
        pSrc->deepCopy(pOtherSrc->_listSrcPonct[i], copyId);
        addSrc(pSrc);
    }

    return true;
}

std::string TYSourceSurfacic::toString() const
{
    return "TYSourceSurfacic";
}

DOM_Element TYSourceSurfacic::toXML(DOM_Element& domElement)
{
    DOM_Element domNewElem = TYSource::toXML(domElement);

    TYXMLTools::addElementIntValue(domNewElem, "nbSrcs", (int)getNbSrcs());
    TYXMLTools::addElementDoubleValue(domNewElem, "densiteSrcsH", _densiteSrcsH);
    TYXMLTools::addElementDoubleValue(domNewElem, "densiteSrcsV", _densiteSrcsV);
    TYXMLTools::addElementIntValue(domNewElem, "srcsCorr", _srcsCorr);

    // On ne devrait pas sauvegarder les sources des sources surfaciques, mais on n'est force parceque c'est le bordel avec les sources bafflees !
    for (unsigned int i = 0; i < _listSrcPonct.size(); i++)
    {
        assert(_listSrcPonct[i]);
        _listSrcPonct[i]->toXML(domNewElem);
    }

    return domNewElem;
}

int TYSourceSurfacic::fromXML(DOM_Element domElement)
{
    unsigned int i;

    TYSource::fromXML(domElement);

    remAllSrc();

    bool nbSrcsOk = false;
    bool densiteSrcsHOk = false;
    bool densiteSrcsVOk = false;
    bool srcsCorrOk = false;
    int nbSrcs;
    DOM_Element elemCur;

    QDomNodeList childs = domElement.childNodes();

    for (i = 0; i < childs.length(); i++)
    {
        elemCur = childs.item(i).toElement();

        TYXMLTools::getElementIntValue(elemCur, "nbSrcs", nbSrcs, nbSrcsOk);
        if (nbSrcsOk)
        {
            _listSrcPonct.reserve(nbSrcs);
        }

        TYXMLTools::getElementDoubleValue(elemCur, "densiteSrcsH", _densiteSrcsH, densiteSrcsHOk);
        TYXMLTools::getElementDoubleValue(elemCur, "densiteSrcsV", _densiteSrcsV, densiteSrcsVOk);
        TYXMLTools::getElementBoolValue(elemCur, "srcsCorr", _srcsCorr, srcsCorrOk);
    }

    // Les sources ponctuelles associees aux sources surfaciques ne sont plus sauvegardees
    // Recupere l'ensemble des source ponctuelles et derivees
    LPTYElementArray srcs = TYElement::findTypeCollectionAndCallFromXML(domElement, "TYSourcePonctuelle");

    // Ajoute les srcs trouvees
    for (i = 0; i < srcs.size(); i++)
    {
        addSrc((LPTYSourcePonctuelle&) srcs[i]);
    }
    //*/
    return 1;
}

double TYSourceSurfacic::getDensiteSrcs() const
{
    return (_densiteSrcsH * _densiteSrcsV);
}

void TYSourceSurfacic::setDensiteSrcs(double densite)
{
    _densiteSrcsH = _densiteSrcsV = sqrt(densite);
}

bool TYSourceSurfacic::addSrc(LPTYSourcePonctuelle pSrcPonct)
{
    if (pSrcPonct)
    {
        pSrcPonct->setParent(this);
        _listSrcPonct.push_back(pSrcPonct);

        return true;
    }

    return false;
}

bool TYSourceSurfacic::remSrc(const LPTYSourcePonctuelle pSrcPonct)
{
    bool ret = false;
    if (pSrcPonct)
    {
        TYTabLPSourcePonctuelle::iterator ite;

        for (ite = _listSrcPonct.begin(); ite != _listSrcPonct.end(); ite++)
        {
            if ((*ite) == pSrcPonct)
            {
                _listSrcPonct.erase(ite);
                ret = true;
                break;
            }
        }
    }

    return ret;
}

bool TYSourceSurfacic::remSrc(QString idSrc)
{
    bool ret = false;
    TYTabLPSourcePonctuelle::iterator ite;

    for (ite = _listSrcPonct.begin(); ite != _listSrcPonct.end(); ite++)
    {
        if ((*ite)->getID().toString() == idSrc)
        {
            _listSrcPonct.erase(ite);
            ret = true;
            break;
        }
    }

    return ret;
}

void TYSourceSurfacic::remAllSrc()
{
    _listSrcPonct.clear();
}
