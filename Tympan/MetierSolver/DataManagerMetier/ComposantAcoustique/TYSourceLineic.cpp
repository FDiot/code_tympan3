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
#include "Tympan/MetierSolver/DataManagerCore/TYElementCollection.h"

#include "Tympan/Tools/OMessageManager.h"


OPROTOINST(TYSourceLineic);


TYSourceLineic::TYSourceLineic():
    _densiteSrcs(100.0),
    _srcsCorr(false)
{
    _name = TYNameManager::get()->generateName(getClassName());
}

TYSourceLineic::TYSourceLineic(const TYSourceLineic& other)
{
    *this = other;
}

TYSourceLineic::~TYSourceLineic()
{
    remAllSrc();
}

TYSourceLineic& TYSourceLineic::operator=(const TYSourceLineic& other)
{
    if (this != &other)
    {
        TYSource::operator =(other);
        _densiteSrcs = other._densiteSrcs;
        _srcsCorr = other._srcsCorr;
        _listSrcPonct = other._listSrcPonct;
    }
    return *this;
}

bool TYSourceLineic::operator==(const TYSourceLineic& other) const
{
    if (this != &other)
    {
        if (TYSource::operator !=(other)) { return false; }
        if (_densiteSrcs != other._densiteSrcs) { return false; }
        if (_srcsCorr != other._srcsCorr) { return false; }
        if (!(_listSrcPonct == other._listSrcPonct)) { return false; }
    }
    return true;
}

bool TYSourceLineic::operator!=(const TYSourceLineic& other) const
{
    return !operator==(other);
}

bool TYSourceLineic::deepCopy(const TYElement* pOther, bool copyId /*=true*/)
{
    if (!TYSource::deepCopy(pOther, copyId)) { return false; }

    TYSourceLineic* pOtherSrc = (TYSourceLineic*) pOther;

    _densiteSrcs = pOtherSrc->_densiteSrcs;
    _srcsCorr = pOtherSrc->_srcsCorr;

    purge();
    for (unsigned int i = 0; i < _listSrcPonct.size(); i++)
    {
        LPTYSourcePonctuelle pSrc = (TYSourcePonctuelle*) _listSrcPonct[i]->clone();
        pSrc->deepCopy(_listSrcPonct[i], copyId);
        addSrc(pSrc);
    }

    return true;
}

std::string TYSourceLineic::toString() const
{
    return "TYSourceLineic";
}

DOM_Element TYSourceLineic::toXML(DOM_Element& domElement)
{
    DOM_Element domNewElem = TYSource::toXML(domElement);

    TYXMLTools::addElementIntValue(domNewElem, "nbSrcs", (int)getNbSrcs());
    TYXMLTools::addElementDoubleValue(domNewElem, "densiteSrcs", _densiteSrcs);
    TYXMLTools::addElementIntValue(domNewElem, "srcsCorr", _srcsCorr);

    for (unsigned int i = 0; i < _listSrcPonct.size(); i++)
    {
        assert(_listSrcPonct[i]);
        _listSrcPonct[i]->toXML(domNewElem);
    }

    return domNewElem;
}

int TYSourceLineic::fromXML(DOM_Element domElement)
{
    unsigned int i;
    TYSource::fromXML(domElement);

    remAllSrc();

    bool nbSrcsOk = false;
    bool densiteSrcsOk = false;
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

        TYXMLTools::getElementDoubleValue(elemCur, "densiteSrcs", _densiteSrcs, densiteSrcsOk);
        TYXMLTools::getElementBoolValue(elemCur, "srcsCorr", _srcsCorr, srcsCorrOk);
    }

    // Recupere l'ensemble des source ponctuelles et derivees
    TYElementCollection srcs = TYElement::findTypeCollectionAndCallFromXML(domElement, "TYSourcePonctuelle");

    // Ajoute les srcs trouvees
    for (i = 0; i < srcs.getCount(); i++)
    {
        addSrc((LPTYSourcePonctuelle&) srcs[i]);
    }

    return 1;
}

bool TYSourceLineic::addSrc(LPTYSourcePonctuelle pSrcPonct)
{
    if (pSrcPonct)
    {
        pSrcPonct->setParent(this);
        _listSrcPonct.push_back(pSrcPonct);
        return true;
    }

    return false;
}

bool TYSourceLineic::remSrc(const LPTYSourcePonctuelle pSrcPonct)
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

bool TYSourceLineic::remSrc(QString idSrc)
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

void TYSourceLineic::remAllSrc()
{
    _listSrcPonct.clear();
}
