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

#include <stdio.h>

#include "Tympan/models/business/OLocalizator.h"
#include "TYAcousticInterface.h"



TYAcousticInterface::TYAcousticInterface():
    _densiteSrcsH(25.0),
    _densiteSrcsV(25.0),
    _offsetSources(0.01),
    _isRayonnant(true),
    _regimeChangeAble(false),
    _typeDistribution(TY_PUISSANCE_CALCULEE),
    _useAtt(false)
{
    _pAtt = NULL;

    _curRegime = 0;

    _nextRegime = 0;

    _pCurrentSpectre.setType(SPECTRE_TYPE_LW);

    addRegime(buildRegime()); // Construction d'un regime a partir du regime par defaut
}

TYAcousticInterface::TYAcousticInterface(const TYAcousticInterface& other)
{
    *this = other;
}

TYAcousticInterface::~TYAcousticInterface()
{
}

TYAcousticInterface& TYAcousticInterface::operator=(const TYAcousticInterface& other)
{
    if (this != &other)
    {
        setDensiteSrcsH(other._densiteSrcsH);
        setDensiteSrcsV(other._densiteSrcsV);
        _regimeChangeAble = other._regimeChangeAble;
        _typeDistribution = other._typeDistribution;
        _curRegime = other._curRegime;
        _nextRegime = other._nextRegime;
        _pCurrentSpectre = other._pCurrentSpectre;
        _useAtt = other._useAtt;
        _pAtt = other._pAtt;
        _isRayonnant = other._isRayonnant;
        _tabRegimes = other._tabRegimes;
    }
    return *this;
}

bool TYAcousticInterface::operator==(const TYAcousticInterface& other) const
{
    if (this != &other)
    {
        if (_densiteSrcsH != other._densiteSrcsH) { return false; }
        if (_densiteSrcsV != other._densiteSrcsV) { return false; }
        if (_regimeChangeAble != other._regimeChangeAble) { return false; }
        if (_typeDistribution != other._typeDistribution) { return false; }
        if (_curRegime != other._curRegime) { return false; }
        if (_nextRegime != other._nextRegime) { return false; }
        if (_pCurrentSpectre != other._pCurrentSpectre) { return false; }
        if (_useAtt != other._useAtt) { return false; }
        if (_pAtt != other._pAtt) { return false; }
        if (_isRayonnant != other._isRayonnant) { return false; }
        if (_tabRegimes != other._tabRegimes) { return false; }
    }
    return true;
}

bool TYAcousticInterface::operator!=(const TYAcousticInterface& other) const
{
    return !operator==(other);
}

void TYAcousticInterface::deepCopy(const TYAcousticInterface* pOther, bool copyId /*=true*/)
{
    *this = *pOther;

    _tabRegimes.clear();
    for (unsigned short i = 0 ; i < pOther->_tabRegimes.size(); i++)
    {
        TYRegime regime;
        regime.deepCopy(&pOther->_tabRegimes[i], copyId);
        _tabRegimes.push_back(regime);
    }

    if ((pOther->getUseAtt()) && (pOther->_pAtt != NULL))
    {
        if (!_pAtt) { _pAtt = new TYAttenuateur(); }
        _pAtt->deepCopy(pOther->_pAtt, copyId);
    }
}

DOM_Element TYAcousticInterface::toXML(DOM_Element& domElement)
{
    TYXMLTools::addElementDoubleValue(domElement, "densiteSrcsH", _densiteSrcsH);
    TYXMLTools::addElementDoubleValue(domElement, "densiteSrcsV", _densiteSrcsV);
    TYXMLTools::addElementIntValue(domElement, "curRegime", _curRegime);
    TYXMLTools::addElementUIntValue(domElement, "nbRegimes", getNbRegimes());
    // Sauvegarde du numero d'ordre du prochain regime
    TYXMLTools::addElementIntValue(domElement, "nextRegime", _nextRegime);

    // Ensuite on sauve tous les regimes
    for (unsigned int i = 0 ; i < _tabRegimes.size() ; i++)
    {
        _tabRegimes[i].toXML(domElement);
    }

    return domElement;
}

int TYAcousticInterface::fromXML(DOM_Element domElement)
{
    bool densiteHOk = false;
    bool densiteVOk = false;
    bool curRegimeOk = false;
    bool typeDistributionOk = false;
    bool useAttOk = false;
    bool isRayonnantOk = false;
    bool regimeFound = false;
    bool nextRegimeFound = false;

    double densite = 0.0;
    LPTYSpectre spectre = new TYSpectre();
    LPTYAttenuateur pTempAtt = new TYAttenuateur();

    // On efface le tableau des regimes car au moins un a ete cree a la construction de l'objet
    _tabRegimes.clear();
    _curRegime = 0;
    _nextRegime = 0;
    DOM_Element elemCur;

    QDomNodeList childs = domElement.childNodes();
    for (unsigned int i = 0; i < childs.length(); i++)
    {
        elemCur = childs.item(i).toElement();

        if (!densiteHOk)
        {
            TYXMLTools::getElementDoubleValue(elemCur, "densiteSrcsH", densite, densiteHOk);
            setDensiteSrcsH(densite, false); // La densite relue ne se propage pas aux sous-elements
        }
        if (!densiteVOk)
        {
            TYXMLTools::getElementDoubleValue(elemCur, "densiteSrcsV", densite, densiteVOk);
            setDensiteSrcsV(densite, false); // La densite relue ne se propage pas aux sous-elements
        }

        TYXMLTools::getElementBoolValue(elemCur, "isRayonnant", _isRayonnant, isRayonnantOk);
        TYXMLTools::getElementBoolValue(elemCur, "useAtt", _useAtt, useAttOk);
        TYXMLTools::getElementIntValue(elemCur, "typeDistribution", _typeDistribution, typeDistributionOk);
        TYXMLTools::getElementIntValue(elemCur, "curRegime", _curRegime, curRegimeOk);
        TYXMLTools::getElementIntValue(elemCur, "nextRegime", _nextRegime, nextRegimeFound);

        if (spectre->callFromXMLIfEqual(elemCur))
        {
            _pCurrentSpectre = *spectre;
        }

        if (pTempAtt->callFromXMLIfEqual(elemCur))
        {
            _pAtt = pTempAtt;
        }

        // Ensuite il devrait y avoir la liste des regimes de la machine
        TYRegime regime;
        if (regime.callFromXMLIfEqual(elemCur))
        {
            regimeFound = true; // On a trouve au moins un regime
            _tabRegimes.push_back(regime);
        }
    }

    if (regimeFound) // Si au moins un regime a ete trouve, on recharge le regime courant
    {
        setCurRegime(_curRegime);
    }
    else // Sinon on construit un "regime 0"
    {
        addRegime(buildRegime());
        nextRegimeFound = true;
    }

    if (!nextRegimeFound) { _nextRegime = static_cast<int>(_tabRegimes.size()) + 2; }

    return 1;
}

double TYAcousticInterface::getDensiteSrcs() const
{
    return (_densiteSrcsH * _densiteSrcsV);
}

void TYAcousticInterface::setDensiteSrcs(double densite, bool recursif /*= true */)
{
    densite = sqrt(densite);
    setDensiteSrcsH(densite, recursif);
    setDensiteSrcsV(densite, recursif);
}

void TYAcousticInterface::setDensiteSrcsH(double densite, bool recursif /*= true */)
{
    _densiteSrcsH = densite;
}

void TYAcousticInterface::setDensiteSrcsV(double densite, bool recursif /*= true */)
{
    _densiteSrcsV = densite;
}

void TYAcousticInterface::setIsRayonnant(bool rayonnant, bool recursif)
{
    _isRayonnant = rayonnant;
    _tabRegimes[_curRegime]._isRayonnant = rayonnant;
}

void TYAcousticInterface::setTypeDistribution(int typeDistri)
{
    _typeDistribution = typeDistri;
    _tabRegimes[_curRegime]._typeDistribution = typeDistri;
}

void TYAcousticInterface::setUseAtt(bool state)
{
    _useAtt = state;
    if (!_useAtt)
    {
        _pAtt = NULL;
    }
    else
    {
        if (!_pAtt) { _pAtt = new TYAttenuateur(); }
    }

    _tabRegimes[_curRegime]._useAtt = _useAtt;
    _tabRegimes[_curRegime]._pAtt = _pAtt;
}

void TYAcousticInterface::setAtt(LPTYAttenuateur pAtt)
{
    _pAtt = pAtt;

    _tabRegimes[_curRegime]._pAtt = pAtt;


    if (_pAtt) { setUseAtt(true); }// En associant un attenuateur on dit qu'on va utiliser celui-ci
}


/*virtual*/ void TYAcousticInterface::propagateAtt(LPTYAttenuateur pAtt)
{
    _useAtt = true;

    if (!pAtt)
    {
        _pAtt = NULL;
        _useAtt = false;
    }
    else if (!_pAtt)
    {
        _pAtt = new TYAttenuateur();
    }


    if (_pAtt)
    {
        _pAtt->deepCopy(pAtt, false);
    }

    updateCurrentRegime(); // On met a jour le regime courant
}

/*virtual*/ TYSpectre TYAcousticInterface::setGlobalLW(const TYSpectre& spectre, const double& surfGlobale, const int& regime/*=-1*/)
{
    return TYSpectre();
}

void TYAcousticInterface::setRegime(TYSpectre& Spectre, int regime /*=-1*/, bool recursif /*=false*/)
{
    // Type Puissance
    Spectre.setType(SPECTRE_TYPE_LW);

    _pCurrentSpectre = Spectre;
    //  updateCurrentRegime();
}

int TYAcousticInterface::addRegime()
{
    return addRegime(buildRegime());
}

int TYAcousticInterface::addRegime(const TYRegime& regime)
{
    _tabRegimes.push_back(regime);

    return static_cast<int>(_tabRegimes.size());
}

bool TYAcousticInterface::remRegime(int regimeNumber)
{
    // Il doit exister au moins un regime
    if (_tabRegimes.size() == 1) { return false; }

    if (regimeNumber > (int)_tabRegimes.size() - 1) { return false; }

    TYTabRegimes::iterator it; // Iterateur sur le tableau
    it = _tabRegimes.begin() + regimeNumber; // On le fait pointer sur l'element a supprimer

    _tabRegimes.erase(it); // On efface l'element pointe

    // On charge le premier regime
    if (_curRegime == regimeNumber)
    {
        _curRegime = 0;
        loadRegime();
    }

    return true; // Tout s'est bien passe du moins on l'espere
}

void TYAcousticInterface::propagateRegime()
{
    addRegime(buildRegime());
}

TYRegime TYAcousticInterface::buildRegime()
{
    char buf[4];
    sprintf(buf, "%d", _nextRegime);

    QString temp = buf;

    TYRegime regime;
    regime.setName(OLocalizator::getString("DisplayName", "TYRegime") + " " + temp);
    regime._isRayonnant = _isRayonnant;
    regime._useAtt = _useAtt;
    regime._typeDistribution = _typeDistribution;
    regime._spectre = _pCurrentSpectre;
    regime._pAtt = _pAtt;

    // Pour eviter de depasser 999 regimes dans ce cas on revient a 0 et l'utilisateur se demerde !
    if (_nextRegime == 999)
    {
        _nextRegime = 0;
    }
    else
    {
        _nextRegime++; // Incremente le numero d'ordre pour le prochain regime
    }

    return regime;
}

void TYAcousticInterface::setCurRegime(int regimeNumber)
{
    if (regimeNumber > (int)_tabRegimes.size() - 1) { return; }
    if (regimeNumber < 0) { regimeNumber = (int)_tabRegimes.size() - 1; }

    TYRegime& regime = _tabRegimes[regimeNumber];

    _isRayonnant = regime._isRayonnant;
    _useAtt = regime._useAtt;
    _typeDistribution = regime._typeDistribution;
    _pCurrentSpectre = regime._spectre;
    _pAtt = regime._pAtt;

    _curRegime = regimeNumber;

    //  ((TYElement*)this)->setIsAcousticModified(true);
}

void TYAcousticInterface::updateCurrentRegime()
{
    TYRegime& regime = _tabRegimes[_curRegime];

    regime._isRayonnant = _isRayonnant;
    regime._useAtt = _useAtt;
    regime._typeDistribution = _typeDistribution;
    regime._spectre = _pCurrentSpectre;

    if (_useAtt && _pAtt)
    {
        regime._pAtt = new TYAttenuateur();
        regime._pAtt->deepCopy(_pAtt, false);
    }
    else if (!_pAtt)
    {
        _useAtt = false;
    }
}

void TYAcousticInterface::loadRegime(int regimeNb)
{
    if (regimeNb == -1)
    {
        regimeNb = _curRegime;
    }
    else if ((regimeNb < 0) || (regimeNb > (int)_tabRegimes.size() - 1))
    {
        regimeNb = 0;
    }

    TYRegime& regime = _tabRegimes[regimeNb];

    _isRayonnant = regime._isRayonnant;
    _useAtt = regime._useAtt;
    _typeDistribution = regime._typeDistribution;
    _pCurrentSpectre = regime._spectre;

    if (_useAtt && (regime._pAtt != NULL))
    {
        _pAtt = new TYAttenuateur();
        _pAtt->deepCopy(regime._pAtt, false);
    }
}

TYRegime& TYAcousticInterface::getRegimeNb(const int& nb, bool& status)
{
    status = true;
    if (nb >= _tabRegimes.size())
    {
        status = false;
        return _tabRegimes[0];
    }

    return _tabRegimes[nb];
}

const TYRegime& TYAcousticInterface::getRegimeNb(const int& nb, bool& status) const
{
    status = true;
    if (nb >= _tabRegimes.size())
    {
        status = false;
        return _tabRegimes[0];
    }

    return _tabRegimes[nb];
}

QString TYAcousticInterface::getRegimeName()
{
    bool status = true;
    QString name = getRegimeNb(_curRegime, status).getRegimeName();
    if (!status)
    {
        name = "";
    }
    return name;
}

const QString TYAcousticInterface::getRegimeName() const
{
    bool status = true;
    QString name = getRegimeNb(_curRegime, status).getRegimeName();
    if (!status)
    {
        name = "";
    }
    return name;
}

void TYAcousticInterface::setRegimeName(const QString& name)
{
    bool status = true;
    TYRegime& reg = getRegimeNb(_curRegime, status);
    if (status) { reg.setRegimeName(name); }
}

/// Export au format csv sur un flux transmis
void TYAcousticInterface::exportCSV(std::ofstream& ofs)
{
    std::string textTypeDistrib;

    if (_typeDistribution == TY_PUISSANCE_CALCULEE)
    {
        textTypeDistrib = "Puissance calculee";
    }
    else
    {
        textTypeDistrib = "Puissance imposee";
    }

    ofs << getRegimeName().toAscii().data() << '\n';
    if (_isRayonnant)
    {
        ofs << "Rayonnant : OUI\n";
        ofs << "Type distribution : " << textTypeDistrib.c_str() << '\n';

        // Densite de sources
        ofs << "Densite de sources horizontale: " << _densiteSrcsH << " source(s)/metre\n";
        ofs << "Densite de sources verticale: " << _densiteSrcsV << " source(s)/metre\n";

        if (_useAtt)
        {
            ofs << "Attenuateur : OUI\n";
            if (_pAtt)
            {
                _pAtt->getSpectreAtt().exportCSV(ofs);
            }
            else
            {
                ofs << "ATTENUATEUR NON DEFINI !!!\n";
            }
        }
        else
        {
            ofs << "Attenuateur : NON\n";
        }

        _pCurrentSpectre.exportCSV(ofs);
    }
    else
    {
        ofs << "Rayonnant : NON\n";
    }

    ofs << '\n';
}
