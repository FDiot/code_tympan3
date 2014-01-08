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


TYRoute::TYRoute():
    _offSet(0.05)
{
    _name = TYNameManager::get()->generateName(getClassName());

    float r = 80.0f, g = 80.0f, b = 80.0f;

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

    for(unsigned i=0; i<NB_TRAFFIC_REGIMES; ++i)
    {
        traffic_regimes[i].setParent(this);
        // On rajoute un ieme regime (un premier a ete construit par TYAcousticLine)
        if (i>0) addRegime(buildRegime());
    }

    _largeur = 3.5;

    _typeDistribution = TY_PUISSANCE_CALCULEE;


    // On nomme les regimes
    _tabRegimes[0].setName("Jour"); // TODO i18n
    _tabRegimes[1].setName("Soir"); // TODO i18n
    _tabRegimes[1].setName("Nuit"); // TODO i18n

    setRoadTrafficArrayForRegime(Day);
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
        road_traffic = other.road_traffic;
        for(unsigned i=0; i<NB_TRAFFIC_REGIMES; ++i)
        {
            traffic_regimes[i] = other.traffic_regimes[i];
        }
        setRoadTrafficArrayForRegime(Day);
    }
    return *this;
}

bool TYRoute::operator==(const TYRoute& other) const
{
    if (this != &other)
    {
        if (TYAcousticLine::operator !=(other)) { return false; }
        // road_traffic = other.road_traffic; // XXX
        for(unsigned i=0; i<NB_TRAFFIC_REGIMES; ++i)
        {
            if(traffic_regimes[i] != other.traffic_regimes[i]) { return false; };
        }
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

    const TYRoute* pOtherRoute = dynamic_cast<const TYRoute*>(pOther);
    assert(pOtherRoute && "Invalid cast to TYRoute*");

    road_traffic = pOtherRoute->road_traffic;
    for(unsigned i=0; i<NB_TRAFFIC_REGIMES; ++i)
    {
        traffic_regimes[i].deepCopy(&pOtherRoute->traffic_regimes[i], copyId);
    }
    setRoadTrafficArrayForRegime(Day);

    return true;
}

std::string TYRoute::toString() const
{
    return "TYRoute";
}

DOM_Element TYRoute::toXML(DOM_Element& domElement)
{
    DOM_Element domNewElem = TYAcousticLine::toXML(domElement);

    // TODO update serialisation : cf. https://extranet.logilab.fr/ticket/1512503

    // TYXMLTools::addElementIntValue(domNewElem, "modeCalcul", _modeCalcul);
    // TYXMLTools::addElementDoubleValue(domNewElem, "vitMoy", _vitMoy);

    // _pTraficJour->toXML(domNewElem);
    // _pTraficNuit->toXML(domNewElem);

    return domNewElem;
}

int TYRoute::fromXML(DOM_Element domElement)
{
    TYAcousticLine::fromXML(domElement);

    // TODO update serialisation: cf. https://extranet.logilab.fr/ticket/1512503

    // bool modeCalculOk = false;
    // bool vitMoyOk = false;
    // bool traficJourFound = false;
    // bool traficNuitFound = false;
    // DOM_Element elemCur;
    // QDomNodeList childs = domElement.childNodes();
    // for (unsigned int i = 0; i < childs.length(); i++)
    // {
    //     elemCur = childs.item(i).toElement();
    //     TYXMLTools::getElementBoolValue(elemCur, "modeCalcul", _modeCalcul, modeCalculOk);
    //     TYXMLTools::getElementDoubleValue(elemCur, "vitMoy", _vitMoy, vitMoyOk);

    //     if (!traficJourFound)
    //     {
    //         traficJourFound = _pTraficJour->callFromXMLIfEqual(elemCur);
    //     }
    //     else if (!traficNuitFound)
    //     {
    //         traficNuitFound = _pTraficNuit->callFromXMLIfEqual(elemCur);
    //     }
    // }

    // if (_tabRegimes.size() == 1) // Cas d'un ancien fichier "sans regime"
    // {
    //     addRegime(buildRegime()); // On rajoute un regime
    //     _tabRegimes[0].setName("Jour");
    //     _tabRegimes[1].setName("Nuit");
    // }

    return 1;
}

TYSpectre TYRoute::computeSpectre(enum TrafficRegimes regime)
{
    TYSpectre s;

    double penteMoy = calculPenteMoyenne();
    TYTrafic& trafic = traffic_regimes[regime];

    // TODO Do actually call here the RoadEmissionNMPB08 library

    // Mise en spectre
    double valeur = 0.0;
    for (unsigned int i = 0 ; i < s.getNbValues() ; i++)
    {
        // TODO Convert here the results from the RoadEmissionNMPB08 library
        //      insto the TYSpectre representation of Code_TYMPAN
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
        for(unsigned i=0; i<NB_TRAFFIC_REGIMES; ++i)
        {
            // Calcul des spectres correspondant aux regimes jours et nuit
            TYSpectre spectrum = computeSpectre(static_cast<enum TrafficRegimes>(i));
            spectrum.setType(SPECTRE_TYPE_LW);
            // Affectation des regimes
            _tabRegimes[i].setSpectre(spectrum);
        }
    }

    // NB : The sources are expected to have already been created during
    //      before calling updateAltitudes, called before thsi method
    //      So we do not want to call distriSrcs();

    // Affectation de la puissance aux sources
    setSrcsLw();

    return true;
}

bool TYRoute::updateAltitudes(const TYAltimetrie& alti, LPTYRouteGeoNode pGeoNode)
{
    bool ok = true;

    assert(pGeoNode->getElement() == static_cast<TYElement*>(this) &&
           "Inconsistent arguments : the geoNode passed must point on `this` !");

    // Transform representing this element pose relative to the world
    const OMatrix& matrix = pGeoNode->getMatrix();
    OMatrix matrixinv = matrix.getInvert();

    // Road heigth relative to the ground
    double hauteur = pGeoNode->getHauteur();

    // Positionning of the road defining points at the specified
    // height above the ground (NB this created tunnels and bridges)
    for (size_t i = 0; i < this->getTabPoint().size(); i++)
    {
        // Transform to site frame pose
        OPoint3D pt = matrix * this->getTabPoint()[i];

        // Init the point at ground altitude
        ok &= alti.updateAltitude(pt);
        // NB updateAltitude already report possibel problems

        // Add the heigth relative to the ground
        pt._z += hauteur;

        // Transform back from site frame pose
        this->getTabPoint()[i] = matrixinv * pt;
    }

    // We create sources along the acoustic line...
    // and then project them on the altimetry and add the required offset
    // Note this is distriSrcs(const TYAltimetrie&) NOT base class distriSrcs()
    distriSrcs(alti, pGeoNode);

    this->setIsGeometryModified(false);
    return true;
}


void TYRoute::distriSrcs(const TYAltimetrie& alti, LPTYRouteGeoNode pGeoNode)
{
    assert(pGeoNode->getElement() == static_cast<TYElement*>(this) &&
           "Inconsistent arguments : the geoNode passed must point on `this` !");

    TYAcousticLine::distriSrcs();

    // Transform representing this element pose relative to the world
    const OMatrix& matrix = pGeoNode->getMatrix();
    OMatrix matrixinv = matrix.getInvert();
    // Road heigth relative to the ground
    double hauteur = pGeoNode->getHauteur();

    for (unsigned int i = 0; i < _pSrcLineic->getNbSrcs(); i++)
    {
        LPTYSourcePonctuelle pSrc=_pSrcLineic->getSrc(i);
        // Transform to site frame pose
        OPoint3D pt = matrix * (*pSrc->getPos());
        alti.updateAltitude(pt);
        // NB updateAltitude already report possibel problems
        // Add the heigth relative to the ground
        pt._z += hauteur;

        // Transform back from site frame pose
        *pSrc->getPos() = matrixinv * pt;
    }
}

RoadTrafficComponent& TYRoute::getRoadTrafficComponent(enum TrafficRegimes regime, enum RoadVehicleType vehic_type)
{
    assert(regime < NB_TRAFFIC_REGIMES);
    return traffic_regimes[regime].arr[vehic_type-1];
}

const RoadTrafficComponent& TYRoute::getRoadTrafficComponent(enum TrafficRegimes regime, enum RoadVehicleType vehic_type) const
{
    assert(regime < NB_TRAFFIC_REGIMES);
    return traffic_regimes[regime].arr[vehic_type-1];
}

void TYRoute::setRoadTrafficArrayForRegime(enum TrafficRegimes regime)
{
    road_traffic.nbComponents = TYTrafic::NB_VEHICULE_TYPES; // LV & HGV
    road_traffic.traffic = &traffic_regimes[regime].arr[0];
}
