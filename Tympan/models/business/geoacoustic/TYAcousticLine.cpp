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

#if TY_USE_IHM
  #include "Tympan/gui/widgets/TYAcousticLineWidget.h"
  #include "Tympan/gui/gl/TYAcousticLineGraphic.h"
#endif


TY_EXTENSION_INST(TYAcousticLine);
TY_EXT_GRAPHIC_INST(TYAcousticLine);

TYAcousticLine::TYAcousticLine() : _typeDistribution(TY_PUISSANCE_IMPOSEE)
{
    _name = TYNameManager::get()->generateName(getClassName());

    _curRegime = 0;
    _nextRegime = 0;
    _useAtt = false;
    _isRayonnant = true;
    _regimeChangeAble = true;

    _pAtt = NULL;

    _largeur = 1;

    _pSrcLineic = new TYSourceLineic();
    _pSrcLineic->setParent(this);

    addRegime(buildRegime());
}

TYAcousticLine::TYAcousticLine(const TYAcousticLine& other)
{
    *this = other;
}

TYAcousticLine::~TYAcousticLine()
{
    _tabPoint.clear();
}

TYAcousticLine& TYAcousticLine::operator=(const TYAcousticLine& other)
{
    if (this != &other)
    {
        TYElement::operator =(other);
        _curRegime = other._curRegime;
        _nextRegime = other._nextRegime;
        _isRayonnant = other._isRayonnant;
        _regimeChangeAble = other._regimeChangeAble;
        _largeur = other._largeur;

        _useAtt = other._useAtt;
        if (other._useAtt && (other._pAtt != NULL))
        {
            _pAtt = new TYAttenuateur(*other._pAtt);
        }
        else
        {
            _pAtt = NULL;
        }

        if (other._pSrcLineic != NULL) { _pSrcLineic = new TYSourceLineic(*other._pSrcLineic); }

        _tabPoint = other._tabPoint;
        _tabRegimes = other._tabRegimes;
    }
    return *this;
}

bool TYAcousticLine::operator==(const TYAcousticLine& other) const
{
    if (this != &other)
    {
        // TODO See ticket https://extranet.logilab.fr/ticket/1522889
        if (_largeur != other._largeur) { return false; }
        if (_pSrcLineic != other._pSrcLineic) { return false; }
        if (!(_tabPoint == other._tabPoint)) { return false; }
        if (!(_tabRegimes == other._tabRegimes)) { return false; }
        if (_curRegime != other._curRegime) { return false; }
        if (_nextRegime != other._nextRegime) { return false; }
        if (_isRayonnant != other._isRayonnant) { return false; }
        if (_regimeChangeAble != other._regimeChangeAble) { return false; }
        if (_useAtt != other._useAtt) { return false; }
        if (_pAtt != other._pAtt) { return false; }
    }

    return true;
}

bool TYAcousticLine::operator!=(const TYAcousticLine& other) const
{
    return !operator==(other);
}

bool TYAcousticLine::deepCopy(const TYElement* pOther, bool copyId /*=true*/)
{

    unsigned int i;
    if (!TYSource::deepCopy(pOther, copyId)) { return false; }

    TYAcousticLine* pOtherLine = (TYAcousticLine*) pOther;

    _largeur = pOtherLine->_largeur;

    _pSrcLineic->deepCopy(pOtherLine->_pSrcLineic, copyId);

    _tabPoint.clear();
    for (i = 0; i < pOtherLine->getTabPoint().size(); i++)
    {
        TYPoint pt;
        pt.deepCopy(&pOtherLine->getTabPoint()[i], copyId);
        _tabPoint.push_back(pt);
    }

    _curRegime = pOtherLine->_curRegime;
    _nextRegime = pOtherLine->_nextRegime;
    _isRayonnant = pOtherLine->_isRayonnant;
    _regimeChangeAble = pOtherLine->_regimeChangeAble;
    _largeur = pOtherLine->_largeur;

    _useAtt = pOtherLine->_useAtt;
    if (pOtherLine->_useAtt && (pOtherLine->_pAtt != NULL))
    {
        _pAtt = new TYAttenuateur();
        _pAtt->deepCopy(pOtherLine->_pAtt);
    }
    else
    {
        _pAtt = NULL;
    }

    _tabRegimes.clear();
    for (i = 0 ; i < _tabRegimes.size(); i++)
    {
        TYRegime regime;
        regime.deepCopy(&pOtherLine->_tabRegimes[i], copyId);
        _tabRegimes.push_back(regime);
    }


    return true;
}

std::string TYAcousticLine::toString() const
{
    return "TYAcousticLine";
}

DOM_Element TYAcousticLine::toXML(DOM_Element& domElement)
{
    unsigned int i = 0;

    DOM_Element domNewElem = TYSource::toXML(domElement);

    TYXMLTools::addElementDoubleValue(domNewElem, "largeur", _largeur);

    _pSrcLineic->toXML(domNewElem);

    TYXMLTools::addElementUIntValue(domNewElem, "nbPoints", _tabPoint.size());

    for (i = 0; i < _tabPoint.size(); i++)
    {
        _tabPoint.at(i).toXML(domNewElem);
    }

    TYXMLTools::addElementIntValue(domNewElem, "isRayonnant", _isRayonnant);
    TYXMLTools::addElementIntValue(domNewElem, "useAtt", _useAtt);

    if ((_useAtt) && (_pAtt != NULL))
    {
        _pAtt->toXML(domNewElem);
    }

    TYXMLTools::addElementIntValue(domNewElem, "curRegime", _curRegime);
    TYXMLTools::addElementUIntValue(domNewElem, "nbRegimes", getNbRegimes());

    // Sauvegarde du numero d'ordre du prochain regime
    TYXMLTools::addElementIntValue(domNewElem, "nextRegime", _nextRegime);

    // Ensuite on sauve tous les regimes
    for (i = 0 ; i < _tabRegimes.size() ; i++)
    {
        _tabRegimes[i].toXML(domNewElem);
    }

    return domNewElem;
}

int TYAcousticLine::fromXML(DOM_Element domElement)
{
    TYSource::fromXML(domElement);

    // Reset
    _tabPoint.clear();
    _tabRegimes.clear();

    bool largeurOk = false;
    bool nbPointsOk = false;
    bool useAttOk = false;
    bool isRayonnantOk = false;
    bool curRegimeOk = false;
    bool regimeFound = false;
    bool nextRegimeFound = false;

    TYPoint point;
    int nbPoints = 0;

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

        TYXMLTools::getElementDoubleValue(elemCur, "largeur", _largeur, largeurOk);
        TYXMLTools::getElementBoolValue(elemCur, "isRayonnant", _isRayonnant, isRayonnantOk);
        TYXMLTools::getElementBoolValue(elemCur, "useAtt", _useAtt, useAttOk);
        TYXMLTools::getElementIntValue(elemCur, "curRegime", _curRegime, curRegimeOk);
        TYXMLTools::getElementIntValue(elemCur, "nextRegime", _nextRegime, nextRegimeFound);

        _pSrcLineic->callFromXMLIfEqual(elemCur);

        TYXMLTools::getElementIntValue(elemCur, "nbPoints", nbPoints, nbPointsOk);
        if (nbPointsOk)
        {
            _tabPoint.reserve(nbPoints);
        }

        if (point.callFromXMLIfEqual(elemCur))
        {
            _tabPoint.push_back(point);
        }

        // Relecture attenuateur
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
    }

    // Si on ne connait pas le numero du dernier regime, on le "numerote" en fonction de la taille du tableau
    if (!nextRegimeFound) { _nextRegime = static_cast<int>(_tabRegimes.size()) + 2; }

    return 1;
}

/*virtual*/void TYAcousticLine::computeSpectre()
{
}

void TYAcousticLine::remAllSrcs()
{
    _pSrcLineic->purge();
}

void TYAcousticLine::distriSrcs()
{
    double densite = 1.0 / _pSrcLineic->getDensiteSrcs();
    double distance = 0.0;
    double x = 0.0, y = 0.0, z = 0.0;                   // Coordonnees du point courant
    int nbSources = 1;
    int i, is;
    LPTYPoint pos;
    LPTYSourcePonctuelle pSource = NULL ;               // Source ponctuelle courante
    TYSpectre Spectre;                                  // Spectre courant

    // On commence par supprimer les anciennes sources
    remAllSrcs();

    // nombre d'iterations (_tabPoint.size()-2 * _pSrcLineic->getNbRegimes());

    //Pour tous les points definissant la TYAcousticLine
    for (i = 1 ; i < (int)_tabPoint.size() ; i++)
    {
        distance = _tabPoint[i].distFrom(_tabPoint[i - 1]);
        nbSources = (int)(distance * densite);
        nbSources = (int)(nbSources > 0 ? nbSources : 1.0) ; //Au minimum une source par segment

        double distanceX = _tabPoint[i]._x - _tabPoint[i - 1]._x;
        double distanceY = _tabPoint[i]._y - _tabPoint[i - 1]._y;
        double distanceZ = _tabPoint[i]._z - _tabPoint[i - 1]._z;

        double deltaSourcesX = distanceX / nbSources ;
        double deltaSourcesY = distanceY / nbSources ;
        double deltaSourcesZ = distanceZ / nbSources ;

        // Position de la premiere source
        x = _tabPoint[i - 1]._x + deltaSourcesX / 2;
        y = _tabPoint[i - 1]._y + deltaSourcesY / 2;
        z = _tabPoint[i - 1]._z + deltaSourcesZ / 2;

        pos = new TYPoint(x, y, z);

        for (is = 0 ; is < nbSources; is++)
        {
            pSource = new TYSourcePonctuelle() ;
            pSource->setParent(this);
            pSource->setType(SPHERIC);

            // Positionnement de la source
            pSource->getPos()->set(pos->_x, pos->_y, pos->_z);

            pSource->setRegime(Spectre);
            _pSrcLineic->addSrc(pSource); // Ajout de la source ponctuelle a la source lineique

            pos->_x += deltaSourcesX;
            pos->_y += deltaSourcesY;
            pos->_z += deltaSourcesZ;
        }
    }
}

bool TYAcousticLine::updateAcoustic(const bool& force) // force = false
{
    distriSrcs();
    // Affectation de la puissance aux sources
    setSrcsLw();

    return true;
}

TYTabSourcePonctuelleGeoNode TYAcousticLine::getSrcs() const
{
    TYTabSourcePonctuelleGeoNode tabRet;
    TYTabLPSourcePonctuelle tab = _pSrcLineic->getSrcs();

    for (unsigned int i = 0; i < tab.size(); i++)
    {
        tabRet.push_back(new TYSourcePonctuelleGeoNode((LPTYElement)tab[i]));
    }

    return tabRet;
}

bool TYAcousticLine::setSrcsLw()
{
    double densite = 1.0 / _pSrcLineic->getDensiteSrcs(); // nb sources/metre
    double deltaSources = 0.0 ;                         //Ecart entre chaque sources

    TYTabLPSourcePonctuelle tabSrcs = _pSrcLineic->getSrcs();

    TYSpectre s = getSpectre()->toGPhy().div(tabSrcs.size());

    unsigned int indexTab = 0; // Permet de parcourir le tableau des sources
    int i = 0, nbSources = 0;
    double distance = 0.0;

    //Pour tous les points definissant la TYAcousticLine
    for (i = 1 ; i < (int)_tabPoint.size() ; i++)
    {
        // Longueur du segment
        distance = _tabPoint[i].distFrom(_tabPoint[i - 1]);

        // Nombre de sources sur le segment
        nbSources = (int)(distance * densite);
        nbSources = (int)(nbSources > 0 ? nbSources : 1.0) ; //Au minimum une source par segment

        // Longueur associee a la source
        deltaSources = distance / nbSources ;

        // LW(source) = LW/metre * longueur associee a la source
        TYSpectre s = getSpectre()->toGPhy().mult(deltaSources);

        // Saisie du spectre pour chacune des sources du segment
        for (unsigned int is = 0 ; is < nbSources ; is++, indexTab++)
        {
            if (indexTab >= tabSrcs.size()) { break; } // Securite

            tabSrcs[indexTab]->setRegime(s);
        }
    }

    return true;
}

void TYAcousticLine::setRegime(TYSpectre& Spectre, int regime /*=-1*/, bool recursif /*=false*/)
{
    // Type Puissance
    Spectre.setType(SPECTRE_TYPE_LW);

    _pSpectre = new TYSpectre(Spectre);
    updateCurrentRegime();
}

int TYAcousticLine::addRegime(TYRegime regime)
{
    _tabRegimes.push_back(regime);

    return static_cast<int>(_tabRegimes.size());
}

bool TYAcousticLine::remRegime(int regimeNumber)
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

void TYAcousticLine::setCurRegime(int regimeNumber)
{
    if (regimeNumber > (int)_tabRegimes.size() - 1) { return; }
    if (regimeNumber < 0) { regimeNumber = (int)_tabRegimes.size() - 1; }

    TYRegime& regime = _tabRegimes[regimeNumber];

    _isRayonnant = regime._isRayonnant;
    _useAtt = regime._useAtt;
    _pSpectre = new TYSpectre(regime._spectre);
    _pAtt = regime._pAtt;
    _typeDistribution = regime._typeDistribution;

    _curRegime = regimeNumber;

    setSrcsLw();

    setIsAcousticModified(true);
}

TYRegime TYAcousticLine::buildRegime()
{
    char buf[3];
    sprintf(buf, "%d", _nextRegime);

    QString temp = buf;

    TYRegime regime;
    regime.setName("regime" + temp);
    regime._isRayonnant = _isRayonnant;
    regime._useAtt = _useAtt;
    regime._typeDistribution = TY_PUISSANCE_IMPOSEE;
    regime._spectre = *_pSpectre;
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

void TYAcousticLine::updateCurrentRegime()
{
    TYRegime& regime = _tabRegimes[_curRegime];

    regime._isRayonnant = _isRayonnant;
    regime._useAtt = _useAtt;
    regime._typeDistribution = _typeDistribution;
    regime._spectre = *_pSpectre;

    if (_useAtt)
    {
        regime._pAtt = new TYAttenuateur();
        regime._pAtt->deepCopy(_pAtt, false);
    }
}
void TYAcousticLine::loadRegime(int regimeNb)
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
    *_pSpectre = regime._spectre;
    _typeDistribution = regime._typeDistribution;

    if (_useAtt && (regime._pAtt != NULL))
    {
        _pAtt = new TYAttenuateur();
        _pAtt->deepCopy(regime._pAtt, false);
    }
}

void TYAcousticLine::setRegimeName(const QString& name)
{
    TYRegime& reg = getRegimeNb(_curRegime);
    reg.setRegimeName(name);
}


void TYAcousticLine::setTypeDistribution(int typeDistri)
{
    _typeDistribution = typeDistri;
    _tabRegimes[_curRegime]._typeDistribution = typeDistri;
}


void TYAcousticLine::setAtt(LPTYAttenuateur pAtt)
{
    _pAtt = pAtt;

    _tabRegimes[_curRegime]._pAtt = pAtt;


    if (_pAtt) { setUseAtt(true); }// En associant un attenuateur on dit qu'on va utiliser celui-ci
}


LPTYSpectre TYAcousticLine::getRealPowerSpectrum()
{
    // on prend en compte l'eventuel attenuateur
    if (_useAtt && (_pAtt != NULL))
    {
        OSpectre temp = _pSpectre->subst(_pAtt->getSpectreAtt());
        return new TYSpectre(temp);
    }

    return _pSpectre;
}

void TYAcousticLine::exportCSV(std::ofstream& ofs)
{
    // Mise a jour acoustique
    updateAcoustic();

    // Export du nom de l'objet
    ofs << getName().toAscii().data() << '\n';

    // Export du type de l'objet
    ofs << toString() << '\n';
    // Largeur de la ligne
    ofs << "Largeur : " << _largeur << " m\n";

    // Export des proprietes acoustiques
    getRegimeNb(_curRegime).getName();
    ofs << getRegimeName().toAscii().data() << '\n';

    // Densite de sources
    ofs << "Densite de sources : " << _pSrcLineic->getDensiteSrcs() << " source(s)/metre\n";

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

    _pSpectre->exportCSV(ofs);

    ofs << '\n';
}
