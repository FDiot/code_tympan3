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

#include "Tympan/models/business/OLocalizator.h"
#include "Tympan/models/common/3d.h"
#if TY_USE_IHM
  #include "Tympan/gui/widgets/TYUserSourcePonctuelleWidget.h"
#endif
#include "TYUserSourcePonctuelle.h"

TY_EXTENSION_INST(TYUserSourcePonctuelle);

TYUserSourcePonctuelle::TYUserSourcePonctuelle():
    _curRegime(0)
{
    _name = TYNameManager::get()->generateName(getClassName());

    _type = TYSourcePonctuelle::TypeUser;
    _useAtt = false;
    _isRayonnant = true;
    _pAttenuateur = NULL;
    _nextRegime = 0;
    _curRegime = addRegime(buildRegime()) - 1;
}

TYUserSourcePonctuelle::TYUserSourcePonctuelle(const TYUserSourcePonctuelle& other)
{
    *this = other;
}

TYUserSourcePonctuelle::TYUserSourcePonctuelle(const TYSourcePonctuelle* other)
{
    // On recopie les elements de l'objet source
    TYSourcePonctuelle::operator =(*other);

    _name = TYNameManager::get()->generateName(getClassName());

    // Enfin on construit la "TYUserSourcePonctuelle
    _useAtt = false;
    _isRayonnant = true;
    _pAttenuateur = NULL;

    // Construction d'un premier regime qui devient le regime courant
    _nextRegime = 0;
    _curRegime = addRegime(buildRegime()) - 1;

}
TYUserSourcePonctuelle::~TYUserSourcePonctuelle()
{
}

TYUserSourcePonctuelle& TYUserSourcePonctuelle::operator=(const TYUserSourcePonctuelle& other)
{
    if (this != &other)
    {
        TYSourcePonctuelle::operator =(other);
        _hauteur = other._hauteur;
        _curRegime = other._curRegime;
        _nextRegime = other._nextRegime;

        _useAtt = other._useAtt;

        if ((other._useAtt) && (other._pAttenuateur != NULL))
        {
            if (!_pAttenuateur) { _pAttenuateur = new TYAttenuateur(); }
            _pAttenuateur = new TYAttenuateur(*other._pAttenuateur);
        }
        else
        {
            _pAttenuateur = NULL;
            _useAtt = false;
        }

        _tabRegimes = other._tabRegimes;
    }
    return *this;
}

bool TYUserSourcePonctuelle::operator==(const TYUserSourcePonctuelle& other) const
{
    if (this != &other)
    {
        if (TYSourcePonctuelle::operator !=(other)) { return false; }
        if (_hauteur != other._hauteur) { return false; }
        if (_pAttenuateur != other._pAttenuateur) { return false; }
        if (_curRegime != other._curRegime) { return false; }
        if (_nextRegime != other._nextRegime) { return false; }
        if (_tabRegimes != other._tabRegimes) { return false; }
    }
    return true;
}

bool TYUserSourcePonctuelle::operator!=(const TYUserSourcePonctuelle& other) const
{
    return !operator==(other);
}

bool TYUserSourcePonctuelle::deepCopy(const TYElement* pOther, bool copyId /*=true*/)
{
    if (!TYSourcePonctuelle::deepCopy(pOther, copyId)) { return false; }

    TYUserSourcePonctuelle* pOtherSrc = (TYUserSourcePonctuelle*) pOther;

    _hauteur = pOtherSrc->_hauteur;
    _curRegime = pOtherSrc->_curRegime;
    _nextRegime = pOtherSrc->_nextRegime;

    if ((pOtherSrc->getUseAtt()) && (pOtherSrc->_pAttenuateur != NULL))
    {
        if (!_pAttenuateur) { _pAttenuateur = new TYAttenuateur(); }
        _pAttenuateur->deepCopy(pOtherSrc->_pAttenuateur, copyId);
    }

    // Copie du tableau des regimes
    _tabRegimes.clear();
    for (unsigned int i = 0 ; i < pOtherSrc->_tabRegimes.size() ; i++)
    {
        TYUserSrcRegime regime;
        regime.deepCopy(&pOtherSrc->_tabRegimes[i], copyId);
        _tabRegimes.push_back(regime);
    }

    return true;
}

std::string TYUserSourcePonctuelle::toString() const
{
    return "TYUserSourcePonctuelle";
}

DOM_Element TYUserSourcePonctuelle::toXML(DOM_Element& domElement)
{
    // On sauve l'etat courant
    DOM_Element domNewElem = TYSourcePonctuelle::toXML(domElement);

    TYXMLTools::addElementIntValue(domNewElem, "curRegime", _curRegime);
    TYXMLTools::addElementIntValue(domNewElem, "nextRegime", _nextRegime);

    // Ensuite on sauve tous les regimes
    for (unsigned int i = 0 ; i < _tabRegimes.size() ; i++)
    {
        _tabRegimes[i].toXML(domNewElem);
    }


    return domNewElem;
}

int TYUserSourcePonctuelle::fromXML(DOM_Element domElement)
{
    TYSourcePonctuelle::fromXML(domElement);

    // On vide le tableau des regimes
    _tabRegimes.clear();
    _nextRegime = 0;
    _curRegime = 0;

    //  bool hauteurOk = false;
    bool curRegimeOk = false;
    bool regimeFound = false;
    bool useAttOk = false;
    bool isRayonnantOk = false;
    bool nextRegimeFound = false;
    //  int regime = 0;

    LPTYAttenuateur pAtt = new TYAttenuateur();

    DOM_Element elemCur;

    QDomNodeList childs = domElement.childNodes();

    for (unsigned int i = 0; i < childs.length(); i++)
    {
        elemCur = childs.item(i).toElement();

        TYXMLTools::getElementIntValue(elemCur, "curRegime", _curRegime, curRegimeOk);
        TYXMLTools::getElementBoolValue(elemCur, "isRayonnant", _isRayonnant, isRayonnantOk);
        TYXMLTools::getElementBoolValue(elemCur, "useAtt", _useAtt, useAttOk);
        TYXMLTools::getElementIntValue(elemCur, "nextRegime", _nextRegime, nextRegimeFound);

        if (pAtt->callFromXMLIfEqual(elemCur))
        {
            _pAttenuateur = pAtt;
        }

        TYUserSrcRegime regime;
        if (regime.callFromXMLIfEqual(elemCur))
        {
            regimeFound = true;
            _tabRegimes.push_back(regime);
        }
    }

    if (regimeFound) // Si au moins un regime a ete trouve, on recharge le regime courant
    {
        setCurrentRegime(_curRegime);
    }
    else // Sinon on construit un "regime 0"
    {
        _curRegime = addRegime(buildRegime()) - 1;
        nextRegimeFound = true;
    }

    if (!nextRegimeFound) { _nextRegime = static_cast<int>(_tabRegimes.size()) + 2; }

    return 1;
}

int TYUserSourcePonctuelle::addRegime(TYUserSrcRegime regime)
{
    _tabRegimes.push_back(regime);

    return static_cast<int>(_tabRegimes.size());
}

bool TYUserSourcePonctuelle::remRegime(int regimeNumber)
{
    // Il doit exister au moins un regime
    if (_tabRegimes.size() == 1) { return false; }

    if (regimeNumber > (int)_tabRegimes.size() - 1) { return false; }

    TYTabUserSrcRegimes::iterator it; // Iterateur sur le tableau
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

TYUserSrcRegime TYUserSourcePonctuelle::buildRegime()
{
    char buf[3];
    sprintf(buf, "%d", _nextRegime);

    QString temp = buf;

    TYUserSrcRegime regime;
    regime.setName(OLocalizator::getString("DisplayName", "TYUserSrcRegime") + " " + temp); // Incrementation du numero de regime
    regime._isRayonnant = true;
    regime._useAtt = _useAtt;
    regime._typeDistribution = 1; // TY_PUISSANCE_IMPOSEE;
    regime._spectre = *_pSpectre;
    regime._pAtt = _pAttenuateur;
    regime._pDirectivite = Directivity;

    // Pour eviter de depasser 999 regimes dans ce cas on revient a 0
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

void TYUserSourcePonctuelle::updateCurrentRegime()
{
    TYUserSrcRegime& regime = _tabRegimes[_curRegime];

    regime._isRayonnant = _isRayonnant;
    regime._useAtt = _useAtt;
    regime._typeDistribution = 1; // TY_PUISSANCE_IMPOSEE;
    regime._spectre = *_pSpectre;

    if (_useAtt)
    {
        regime._pAtt = new TYAttenuateur();
        regime._pAtt->deepCopy(_pAttenuateur, false);
    }

    regime._pDirectivite = Directivity;
}

void TYUserSourcePonctuelle::setRegimeName(const QString& name)
{
    TYRegime& reg = getRegimeNb(_curRegime);
    reg.setRegimeName(name);
}

TYUserSrcRegime& TYUserSourcePonctuelle::getRegimeNb(const int& nb)
{
    if (nb >= _tabRegimes.size())
    {
        return _tabRegimes[0];
    }

    return _tabRegimes[nb];
}

const TYUserSrcRegime& TYUserSourcePonctuelle::getRegimeNb(const int& nb) const
{
    if (nb >= _tabRegimes.size())
    {
        return _tabRegimes[0];
    }

    return _tabRegimes[nb];
}

void TYUserSourcePonctuelle::setCurrentRegime(const int& regimeNumber)
{
    int currentRegime = regimeNumber;
    if (regimeNumber > (int)_tabRegimes.size() - 1) { return; }
    if (regimeNumber < 0) { currentRegime = (int)_tabRegimes.size() - 1; } // on bascule sur le dernier regime

    TYUserSrcRegime& regime = _tabRegimes[currentRegime];

    _isRayonnant = regime._isRayonnant;
    _useAtt = regime._useAtt;
    //  _typeDistribution = regime._typeDistribution;
    *_pSpectre = regime._spectre;
    _pAttenuateur = regime._pAtt;
    Directivity = regime._pDirectivite;

    _curRegime = currentRegime;
}

void TYUserSourcePonctuelle::loadRegime(int regimeNb)
{
    if (regimeNb == -1)
    {
        regimeNb = _curRegime;
    }

    if ((regimeNb < 0) || (regimeNb > (int)_tabRegimes.size() - 1))
    {
        regimeNb = 0;
    }

    TYUserSrcRegime& regime = _tabRegimes[regimeNb];

    _isRayonnant = regime._isRayonnant;
    _useAtt = regime._useAtt;
    *_pSpectre = regime._spectre;

    if (_useAtt && (regime._pAtt != NULL))
    {
        _pAttenuateur = new TYAttenuateur();
        _pAttenuateur->deepCopy(regime._pAtt, false);
    }

    if (regime._pDirectivite != NULL)
    {
        Directivity = regime._pDirectivite;
    }

}

TYSpectre* TYUserSourcePonctuelle::getCurrentSpectre() const
{
    return _pSpectre;
}

TYSpectre* TYUserSourcePonctuelle::getSpectre(int regime /*= -1*/) const
{
    // on prend en compte l'eventuel attenuateur
    if (_useAtt && (_pAttenuateur != NULL))
    {
        OSpectre temp = _pSpectre->toGPhy().div(_pAttenuateur->getSpectreAtt().toGPhy());
        return new TYSpectre(temp);
    }

    return new TYSpectre(_pSpectre->toGPhy());
}

LPTYSpectre TYUserSourcePonctuelle::getRealPowerSpectrum()
{
    return new TYSpectre(getSpectre()->toDB());
}


void TYUserSourcePonctuelle::exportCSV(std::ofstream& ofs)
{
    // Export du nom de l'objet
    ofs << getName().toAscii().data() << '\n';

    // Export du type de l'objet
    ofs << toString() << '\n';
    // Export des proprietes acoustiques
    getRegimeNb(_curRegime).getName();
    ofs << getRegimeName().toAscii().data() << '\n';

    if (_useAtt)
    {
        ofs << "Attenuateur : OUI\n";
        if (_pAttenuateur)
        {
            _pAttenuateur->getSpectreAtt().exportCSV(ofs);
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

    _pSpectre->exportCSV(ofs);

    ofs << '\n';
}
