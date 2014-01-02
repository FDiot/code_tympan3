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

#include "TYRoute.h"
#include "Tympan/Tools/OMessageManager.h"


OPROTOINST(TYRoute);

const float TYRoute::_tabR[] = { /*   16*/ -200.0, /*   20  */ -200.0,
                                           /*   25*/ -200.0, /*   31.5*/ -200.0, /*   40*/ -200.0,
                                           /*   50*/ -200.0, /*   63  */ -200.0, /*   80*/ -200.0,
                                           /*  100*/ -014.0, /*  125  */ -014.0, /*  160*/ -014.0,
                                           /*  200*/ -010.0, /*  250  */ -010.0, /*  315*/ -010.0,
                                           /*  400*/ -007.0, /*  500  */ -007.0, /*  630*/ -007.0,
                                           /*  800*/ -004.0, /* 1000  */ -004.0, /* 1250*/ -004.0,
                                           /* 1600*/ -007.0, /* 2000  */ -007.0, /* 2500*/ -007.0,
                                           /* 3150*/ -012.0, /* 4000  */ -012.0, /* 5000*/ -012.0,
                                           /* 6300*/ -200.0, /* 8000  */ -200.0, /*10000*/ -200.0,
                                           /*12500*/ -200.0, /*16000  */ -200.0
                               };



TYRoute::TYRoute(): _vitMoy(80),
    _modeCalcul(true),
    _offSet(0.05)
{
    _name = TYNameManager::get()->generateName(getClassName());

    float r = 138.0f, g = 138.0f, b = 138.0f;

#if TY_USE_IHM
    if (TYPreferenceManager::exists(TYDIRPREFERENCEMANAGER, "TYRouteGraphicColor"))
    {
        TYPreferenceManager::getColor(TYDIRPREFERENCEMANAGER, "TYRouteGraphicColor", r, g, b);
    }
    else
    {
        TYPreferenceManager::setColor(TYDIRPREFERENCEMANAGER, "TYRouteGraphicColor", r, g, b);
    }

    if (TYPreferenceManager::exists(TYDIRPREFERENCEMANAGER, "TYRouteSourceOffSet"))
    {
        _offSet = TYPreferenceManager::getDouble(TYDIRPREFERENCEMANAGER, "TYRouteSourceOffSet");
    }
    else
    {
        TYPreferenceManager::setDouble(TYDIRPREFERENCEMANAGER, "TYRouteSourceOffSet", 0.05);
    }
#endif // TY_USE_IHM
    OColor color;
    color.r = r / 255;
    color.g = g / 255;
    color.b = b / 255;

    setColor(color);

    _regimeChangeAble = false;

    _pTraficJour = new TYTrafic();
    _pTraficJour->setParent(this);

    _pTraficNuit = new TYTrafic();
    _pTraficNuit->setParent(this);

    _largeur = 3.5;

    _typeDistribution = TY_PUISSANCE_CALCULEE;

    // On rajoute un deuxieme regime (un premier a ete construit par TYAcousticLine)
    addRegime(buildRegime());

    // On nomme les regimes
    _tabRegimes[0].setName("Jour");
    _tabRegimes[1].setName("Nuit");
}

TYRoute::TYRoute(const TYRoute& other)
{
    *this = other;
}

TYRoute::~TYRoute()
{
}

TYRoute& TYRoute::operator=(const TYRoute& other)
{
    if (this != &other)
    {
        TYAcousticLine::operator =(other);
        _vitMoy = other._vitMoy;
        _modeCalcul = other._modeCalcul;
        _pTraficJour = other._pTraficJour;
        _pTraficNuit = other._pTraficNuit;
    }
    return *this;
}

bool TYRoute::operator==(const TYRoute& other) const
{
    if (this != &other)
    {
        if (TYAcousticLine::operator !=(other)) { return false; }
        if (_vitMoy != other._vitMoy) { return false; }
        if (_modeCalcul != other._modeCalcul) { return false; }
        if (_pTraficJour != other._pTraficJour) { return false; }
        if (_pTraficNuit != other._pTraficNuit) { return false; }
    }
    return true;
}

bool TYRoute::operator!=(const TYRoute& other) const
{
    return !operator==(other);
}

bool TYRoute::deepCopy(const TYElement* pOther, bool copyId /*=true*/)
{
    if (!TYAcousticLine::deepCopy(pOther, copyId)) { return false; }

    TYRoute* pOtherRoute = (TYRoute*) pOther;

    _vitMoy = pOtherRoute->_vitMoy;
    _modeCalcul = pOtherRoute->_modeCalcul;

    _pTraficJour->deepCopy(pOtherRoute->_pTraficJour, copyId);
    _pTraficNuit->deepCopy(pOtherRoute->_pTraficNuit, copyId);

    return true;
}

std::string TYRoute::toString() const
{
    return "TYRoute";
}

DOM_Element TYRoute::toXML(DOM_Element& domElement)
{
    DOM_Element domNewElem = TYAcousticLine::toXML(domElement);

    TYXMLTools::addElementIntValue(domNewElem, "modeCalcul", _modeCalcul);
    TYXMLTools::addElementDoubleValue(domNewElem, "vitMoy", _vitMoy);

    _pTraficJour->toXML(domNewElem);
    _pTraficNuit->toXML(domNewElem);

    return domNewElem;
}

int TYRoute::fromXML(DOM_Element domElement)
{
    TYAcousticLine::fromXML(domElement);

    bool modeCalculOk = false;
    bool vitMoyOk = false;
    bool traficJourFound = false;
    bool traficNuitFound = false;
    DOM_Element elemCur;
    QDomNodeList childs = domElement.childNodes();
    for (unsigned int i = 0; i < childs.length(); i++)
    {
        elemCur = childs.item(i).toElement();
        TYXMLTools::getElementBoolValue(elemCur, "modeCalcul", _modeCalcul, modeCalculOk);
        TYXMLTools::getElementDoubleValue(elemCur, "vitMoy", _vitMoy, vitMoyOk);

        if (!traficJourFound)
        {
            traficJourFound = _pTraficJour->callFromXMLIfEqual(elemCur);
        }
        else if (!traficNuitFound)
        {
            traficNuitFound = _pTraficNuit->callFromXMLIfEqual(elemCur);
        }
    }

    if (_tabRegimes.size() == 1) // Cas d'un ancien fichier "sans regime"
    {
        addRegime(buildRegime()); // On rajoute un regime
        _tabRegimes[0].setName("Jour");
        _tabRegimes[1].setName("Nuit");
    }

    return 1;
}

double TYRoute::calculEVL(bool plat /*=true*/)
{
    double valEVL;
    if (plat)           // Terrain plat
    {
        if (_modeCalcul)    // ecoulement fluide
        {
            if (_vitMoy < 44.0)     // Balayage des differentes plages de vitesse
            {
                valEVL = 29.4;
            }
            else
            {
                valEVL = 0.139 * _vitMoy + 23.27;
            }
        }
        else                // ecoulement pulse
        {
            if (_vitMoy <= 40.0)    // Balayage des differentes plages de vitesse
            {
                valEVL = -0.14 * _vitMoy + 36.8;
            }
            else if (_vitMoy <= 53.0)
            {
                valEVL = 31.2;
            }
            else
            {
                valEVL = 0.139 * _vitMoy + 23.27;
            }
        }
    }
    else                // Terrain en pente : dans ce cas, pas de calcul en ecoulement pulse
    {
        if (_vitMoy <= 20.0)
        {
            valEVL = -0.047 * 20.0 + 35.8;
        }
        else if (_vitMoy <= 43.0)
        {
            valEVL = -0.047 * _vitMoy + 35.8;
        }
        else if (_vitMoy <= 80.0)
        {
            valEVL = 0.032 * _vitMoy + 32.4;
        }
        else
        {
            valEVL = 0.139 * _vitMoy + 23.27;
        }
    }

    return valEVL;
}

double TYRoute::calculEPL(bool plat /*=true*/)
{
    double valEPL;

    if (plat)           // Terrain plat : Pas d'ecoulement pulse pour les camions
    {
        if (_vitMoy <= 20.0)        // Balayage des differentes plages de vitesse
        {
            valEPL = -0.12 * 20.0 + 49.6;
        }
        else if (_vitMoy <= 51.0)
        {
            valEPL = -0.12 * _vitMoy + 49.6;
        }
        else if (_vitMoy <= 70.0)
        {
            valEPL = 43.0;
        }
        else if (_vitMoy <= 100.0)
        {
            valEPL = 0.1 * _vitMoy + 36.0;
        }
        else
        {
            valEPL = 0.1 * 100.0 + 36.0;

        }
    }
    else                // Terrain en pente : dans ce cas, pas de calcul en ecoulement pulse
    {
        if (_vitMoy <= 20.0)
        {
            valEPL = -0.129 * 20 + 49.6;
        }
        else if (_vitMoy <= 51.0)
        {
            valEPL = -0.129 * _vitMoy + 49.6;
        }
        else if (_vitMoy <= 70.0)
        {
            valEPL = 43.0;
        }
        else if (_vitMoy <= 100.0)
        {
            valEPL = 0.1 * _vitMoy + 36.0;
        }
        else
        {
            valEPL = 0.1 * 100.0 + 36.0;
        }
    }


    return valEPL;
}

TYSpectre TYRoute::computeSpectre(const LPTYTrafic regime)
{
    TYSpectre s;

    double penteMoy = calculPenteMoyenne();

    // Calcul de EVL et EPL


    // On calcule d'abord sur terrain plat
    double evlPlat  = calculEVL(true);
    double eplPlat  = calculEPL(true);
    double evlPente = 0.0;
    double eplPente = 0.0;

    // Puis sur terrain en pente (si necessaire)
    if (penteMoy >= 0.02)
    {
        evlPente = calculEVL(false); // Voitures
        eplPente = calculEPL(false); // Camions
    }
    else
    {
        evlPente = evlPlat;
        eplPente = eplPlat;
    }

    // Calcul de LAWi
    double debitVLdB = 10 * log10(regime->getDebitVL());
    double debitPLdB = 10 * log10(regime->getDebitPL());
    double longdB = 0.0; // Pour la lisibilite = 10 * log10(1.0) : longueur de reference = 1 metre

    double partVLPlat  = pow(10.0, (evlPlat + debitVLdB) / 10.0) ;
    double partPLPlat  = pow(10.0, (eplPlat + debitPLdB) / 10.0) ;
    double lAWiPlat    = partVLPlat + partPLPlat ;

    double partVLPente = pow(10.0, (evlPente + debitVLdB) / 10.0);
    double partPLPente = pow(10.0, (eplPente + debitPLdB) / 10.0);
    double lAWiPente   = partVLPente + partPLPente;

    // LAWi = moyenne (en energie) de LAWi sur le plat et LAWi sur une pente)
    double lAWi = 10 * log10((lAWiPlat + lAWiPente) / 2.0) + 20.0 + longdB;

    // Mise en spectre
    double valeur = 0.0;
    for (unsigned int i = 0 ; i < s.getNbValues() ; i++)
    {
        valeur = lAWi + _tabR[i] - 10 * log10(3.0);
        s.getTabValReel()[i] = valeur;
    }

    return s;
}

double TYRoute::calculPenteMoyenne()
{
    size_t nbPoint = _tabPoint.size();
    double* XTemp = new double [nbPoint]; // sert a ramener les points repartis sur une surface sur une droite
    double X  = 0.0;
    double Z  = 0.0;
    //  double XZ = 0.0;

    double moy_x   = 0.0;
    double moy_z   = _tabPoint[0]._z;
    double sommeX  = 0.0;
    double sommeXZ = 0.0;

    size_t i;
    //1. Calcul de la moyenne des x et des z et calcul des pseudo x
    for (i = 1; i < nbPoint ; i++)
    {
        XTemp[i] = _tabPoint[i].distFrom(_tabPoint[i - 1]);
        moy_x = moy_x + XTemp[i];
        moy_z = moy_z + _tabPoint[i]._z;
    }

    moy_x = moy_x / nbPoint;
    moy_z = moy_z / nbPoint;

    //2. Calcul du carre des ecarts a la moyenne leurs sommes et produits
    for (i = 0 ; i < nbPoint ; i++)
    {
        X  = (XTemp[i] - moy_x) * (XTemp[i] - moy_x);
        Z  = (_tabPoint[i]._z - moy_z) * (_tabPoint[i]._z - moy_z);

        sommeX = sommeX + X;
        sommeXZ = sommeXZ + sqrt(X * Z);
    }

    // destruction des tableaux
    delete [] XTemp;

    return sommeXZ / sommeX;
}
/*
void TYRoute::setTraficJour(const LPTYTrafic pTrafic)
{
    _pTraficJour = pTrafic;
    _pTraficJour->setParent(this);
    TYSpectre aTYSpectre=computeSpectre(_pTraficJour);
    _pSrcLineic->setRegime(aTYSpectre, 0); // calcul du spectre associe a ce regime

    this->distriSrcs(); //Distribution des sources sur la TYSourceLineic
}

void TYRoute::setTraficNuit(const LPTYTrafic pTrafic)
{
    _pTraficNuit = pTrafic;
    _pTraficNuit->setParent(this);
    TYSpectre aTYSpectre=computeSpectre(_pTraficNuit);
    _pSrcLineic->setRegime(aTYSpectre, 1); // calcul du spectre associe a ce regime

    this->distriSrcs(); //Distribution des sources sur la TYSourceLineic
}
*/
bool TYRoute::updateAcoustic(const bool& force) //force = false
{
    if (_typeDistribution == TY_PUISSANCE_CALCULEE)
    {
        // Calcul des spectres correspondant aux regimes jours et nuit
        TYSpectre traficJour = computeSpectre(_pTraficJour);
        TYSpectre traficNuit = computeSpectre(_pTraficNuit);

        traficJour.setType(SPECTRE_TYPE_LW);
        traficNuit.setType(SPECTRE_TYPE_LW);

        // Affectation des regimes
        _tabRegimes[0].setSpectre(traficJour);
        _tabRegimes[1].setSpectre(traficNuit);
    }

    // Distribution des sources et affectation de la puissance acoustique
    TYAcousticLine::updateAcoustic(force);

    return true;
}

void TYRoute::distriSrcs()
{
    TYAcousticLine::distriSrcs();

    TYTabLPSourcePonctuelle tabSrcs = _pSrcLineic->getSrcs();

    LPTYPoint pPos = NULL;

    // Ajout d'un offset a la hauteur des sources
    for (unsigned int i = 0; i < tabSrcs.size(); i++)
    {
        pPos = tabSrcs[i]->getPos();
        pPos->_z += _offSet;
    }

}
