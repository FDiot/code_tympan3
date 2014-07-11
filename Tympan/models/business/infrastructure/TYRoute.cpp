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
#include "Tympan/models/business/TYPHMetier.h"
#endif // TYMPAN_USE_PRECOMPILED_HEADER

#include "TYRoute.h"

#include <limits>
#include <cmath>

#include "Tympan/core/logging.h"

#if TY_USE_IHM
#include "Tympan/gui/widgets/TYRouteWidget.h"
#include "Tympan/GraphicIHM/DataManagerGraphic/TYRouteGraphic.h"
#endif

#include <boost/math/special_functions/fpclassify.hpp>


TY_EXTENSION_INST(TYRoute);
TY_EXT_GRAPHIC_INST(TYRoute);


bool TYRoute::is_valid_declivity(double decli)
{ return !boost::math::isnan(decli); } // Could and should use std::isnan in C++ '11

const double TYRoute::undefined_declivity = std::numeric_limits<double>::quiet_NaN();
TYRoute::TYRoute():
    computed_declivity(false),
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

    road_traffic.surfaceType = RoadSurface_Default;
    road_traffic.surfaceAge = 0.0;
    road_traffic.ramp = 0.0;
    for (unsigned i = 0; i < NB_TRAFFIC_REGIMES; ++i)
    {
        traffic_regimes[i].setParent(this);
        // On rajoute un ieme regime (un premier a ete construit par TYAcousticLine)
        if (i > 0) { addRegime(buildRegime()); }
    }

    _largeur = 3.5;

    _typeDistribution = TY_PUISSANCE_CALCULEE;


    // On nomme les regimes
    _tabRegimes[0].setName("Jour"); // TODO i18n
    _tabRegimes[1].setName("Soir"); // TODO i18n
    _tabRegimes[2].setName("Nuit"); // TODO i18n

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
        for (unsigned i = 0; i < NB_TRAFFIC_REGIMES; ++i)
        {
            traffic_regimes[i] = other.traffic_regimes[i];
        }
        setRoadTrafficArrayForRegime(Day);
        computed_declivity = other.computed_declivity;
    }
    return *this;
}

bool TYRoute::operator==(const TYRoute& other) const
{
    if (this != &other)
    {
        if (TYAcousticLine::operator !=(other)) { return false; }
        if (road_traffic.surfaceType != other.road_traffic.surfaceType) { return false; }
        if (road_traffic.surfaceAge != other.road_traffic.surfaceAge) { return false; }
        if (road_traffic.ramp != other.road_traffic.ramp) { return false; }
        if (road_traffic.nbComponents != other.road_traffic.nbComponents) { return false; }
        for (unsigned i = 0; i < NB_TRAFFIC_REGIMES; ++i)
        {
            if (traffic_regimes[i] != other.traffic_regimes[i]) { return false; };
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
    for (unsigned i = 0; i < NB_TRAFFIC_REGIMES; ++i)
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
    // NB DOM_Element is actually a QDomElement


    // Serialise the RoadTraffic attribute
    domNewElem.setAttribute("surfaceType", road_traffic.surfaceType);
    domNewElem.setAttribute("ramp", road_traffic.ramp);
    domNewElem.setAttribute("surfaceAge", road_traffic.surfaceAge);

    // Serialise each of the regimes
    for (unsigned i = 0; i < NB_TRAFFIC_REGIMES; ++i)
    {
        traffic_regimes[i].toXML(domNewElem);
    }

    return domNewElem;
}

int TYRoute::fromXML(DOM_Element domElement)
{
    // NB DOM_Element is actually a QDomElement
    TYAcousticLine::fromXML(domElement);

    QString s;
    bool ok;

    // Deserialise the RoadTraffic attribute surfaceType
    s = domElement.attribute("surfaceType", QString());
    if (s.isEmpty()) // Attribute not found
    {
        OMessageManager::get()->error(
            "Can not read the road `surfaceType` attribute for element %s.",
            str_qt2c(getStringID()));
        return 0;
    }
    unsigned surfType = s.toUInt(&ok);
    if (!ok)
    {
        OMessageManager::get()->error(
            "Integer expected for attribute `surfaceType` on element %s, not %s",
            str_qt2c(getStringID()), str_qt2c(s));
        return 0;
    }
    road_traffic.surfaceType = static_cast<RoadSurfaceType>(surfType);

    // Deserialise the RoadTraffic attribute ramp
    s = domElement.attribute("ramp", QString());
    if (s.isEmpty()) // Attribute not found
    {
        OMessageManager::get()->error(
            "Can not read the road `ramp` attribute for element %s.",
            str_qt2c(getStringID()));
        return 0;
    }
    double tmp_d = s.toDouble(&ok);
    if (!ok)
    {
        OMessageManager::get()->error(
            "Floating point number expected for attribute `ramp` on element %s, not %s",
            str_qt2c(getStringID()), str_qt2c(s));
        return 0;
    }
    road_traffic.ramp = tmp_d;

    // Deserialise the RoadTraffic attribute surfaceAge
    s = domElement.attribute("surfaceAge", QString());
    if (s.isEmpty()) // Attribute not found
    {
        OMessageManager::get()->error(
            "Can not read the road `surfaceAge` attribute for element %s.",
            str_qt2c(getStringID()));
        return 0;
    }
    tmp_d = s.toUInt(&ok);
    if (!ok)
    {
        OMessageManager::get()->error(
            "Integer expected for attribute `surfaceAge` on element %s, not %s",
            str_qt2c(getStringID()), str_qt2c(s));
        return 0;
    }
    road_traffic.surfaceAge = tmp_d;

    // Deserialise the traffic for the regimes
    QDomNodeList children = domElement.elementsByTagName("Trafic");
    if (children.size() != NB_TRAFFIC_REGIMES)
    {
        OMessageManager::get()->error(
            "Loading TYRoute element %s, "
            "%u TYTrafic child elements were found but %u were expected",
            str_qt2c(getStringID()), children.size(), NB_TRAFFIC_REGIMES);
        return 0;
    }

    for (unsigned i = 0; i < NB_TRAFFIC_REGIMES; ++i)
    {
        QDomElement elem = children.item(i).toElement();
        if (elem.isNull())
        {
            debugXml(children.item(i));
            return 0;
        }
        traffic_regimes[i].fromXML(elem);
    }

    // TODO : handle ascendant compatibility (out-of-scope for now)
    // Cf. https://extranet.logilab.fr/ticket/1521703
    return 1;
}

TYSpectre TYRoute::computeSpectre(enum TrafficRegimes regime)
{
    // This updates the road_traffic to point to the array of RoadTrafficComponents
    // corresponding to the selected regime.
    setRoadTrafficArrayForRegime(regime);

    updateComputedDeclivity();

    // Handle a simplified model of traffic wrt to the declivity
    // Cf. https://extranet.logilab.fr/ticket/1513437

    // Half the global traffic to get the flow for only one roadway
    TrafficHalfer halves_the_traffic(*this);

    double* tab;
    tab = NMPB08_Lwm(&road_traffic, Spectrum_3oct_lin);
    OSpectre s_one_way(tab, 18, 8);
    s_one_way.setType(SPECTRE_TYPE_LW);
    // NB: This initialise `s_one_way` with the 18 values provided in `tab`
    // Because the 1st frequency provided by NMPB08 is 100Hz whereas 100Hz
    // is the 9th frequency in Code_TYMPAN representation the offset is 8
    // Both spectrum uses 3rd octave as frequency steps and the common
    // higest frequency is 5000Hz.

    // Coinsider the opposite road_way which differ by opposite declivity
    road_traffic.ramp *= -1.0;
    tab = NMPB08_Lwm(&road_traffic, Spectrum_3oct_lin);
    OSpectre s_other_way(tab, 18, 8);
    s_other_way.setType(SPECTRE_TYPE_LW);

    // NB : The original traffic is restored on destroyuing the halves_the_traffic helper
    return s_one_way.sumdB(s_other_way);
}

double TYRoute::calculPenteMoyenne()
{
    // TODO cf https://extranet.logilab.fr/ticket/1513440
    // Iter on the _listSrcPonct (sources) instead of _tabPoint (2d geometry)

    size_t nbPoint = _tabPoint.size();
    if (nbPoint < 2) // Declivity is undefined
    {
        return undefined_declivity;
    }
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
    TYSpectre aTYSpectre=computeSpectre(_pTraficardNuit);
    _pSrcLineic->setRegime(aTYSpectre, 1); // calcul du spectre associe a ce regime

    this->distriSrcs(); //Distribution des sources sur la TYSourceLineic
}
*/

bool TYRoute::updateAcoustic(const bool& force) //force = false
{
    if (_typeDistribution == TY_PUISSANCE_CALCULEE)
    {
        for (unsigned i = 0; i < NB_TRAFFIC_REGIMES; ++i)
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
        alti.updateAltitude(pt);
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

    updateComputedDeclivity();

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
        LPTYSourcePonctuelle pSrc = _pSrcLineic->getSrc(i);
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

RoadTrafficComponent& TYRoute::accessRoadTrafficComponent(enum TrafficRegimes regime, enum TYTrafic::VehicleTypes vehic_type)
{
    return traffic_regimes[regime].arr[vehic_type];
}

const RoadTrafficComponent& TYRoute::getNMPB08RoadTrafficComponent(enum TrafficRegimes regime, enum TYTrafic::VehicleTypes vehic_type) const
{
    return traffic_regimes[regime].arr[vehic_type];
}

void TYRoute::setRoadTrafficArrayForRegime(enum TrafficRegimes regime)
{
    road_traffic.nbComponents = TYTrafic::NB_VEHICLE_TYPES; // LV & HGV
    road_traffic.traffic = &traffic_regimes[regime].arr[0];
}

void TYRoute::updateComputedDeclivity()
{
    if (!computed_declivity)
    {
        return;
    }

    double penteMoy = calculPenteMoyenne();

    if (is_valid_declivity(penteMoy))
    {
        road_traffic.ramp = penteMoy; // TODO assert units
    }
    else
    {
        OMessageManager::get()->error(
            "Can not compute declivity for road %s.",
            str_qt2c(getStringID()));
    }
}

// table C1
const TYRoute::note77_tables TYRoute::note77_lower_bounds =
{
    // Link motorways
    {
        // Long distance function
        {7000, 1300, 16},
        // Regional roads
        {7000,  500,  6}
    },
    // Intercity roads
    {
        // Long distance function
        {2500,  300,  8},
        // Regional roads
        {2500,  250,  5}
    }
};
// table C1
const TYRoute::note77_tables TYRoute::note77_upper_bounds =
{
    // Link motorways
    {
        // Long distance function
        {70000, 13500, 30},
        // Regional roads
        {93000, 14000, 34}
    },
    // Intercity roads
    {
        // Long distance function
        {22500, 5000, 34},
        // Regional roads
        {22000, 2500, 17}
    }
};
// table C2
const TYRoute::note77_tables TYRoute::note77_hourly_HGV_coeff =
{
    // Link motorways
    {
        // Long distance function
        {20, 20, 39},
        // Regional roads
        {17, 28,  50}
    },
    // Intercity roads
    {
        // Long distance function
        {17, 27, 51},
        // Regional roads
        {16, 34, 73}
    }
};
// table C2
const TYRoute::note77_tables TYRoute::note77_hourly_LV_coeff =
{
    // Link motorways
    {
        // Long distance function
        {17, 19,  82},
        // Regional roads
        {17, 18, 100}
    },
    // Intercity roads
    {
        // Long distance function
        {17, 19, 110},
        // Regional roads
        {17, 19, 120}
    }
};


static bool inline is_in(double val, double min, double max)
{return min <= val && val <= max; }

bool TYRoute::note77_check_validity(
    double aadt_hgv, double aadt_lv,
    TYRoute::RoadType road_type, TYRoute::RoadFunction road_function,
    QString* out_msg)
{
    // TODO i18n when i18n will be properly handled by Qt

    double min, max;
    if (out_msg)
    {
        out_msg->clear();
    }

    const double aadt_total = aadt_lv + aadt_hgv;
    // TODO Use propoer loggin to report validity violations
    const double hgv_percent = aadt_hgv / aadt_total * 100;

    min = note77_lower_bounds[road_type][road_function][0];
    max = note77_upper_bounds[road_type][road_function][0];
    bool ok_total = is_in(aadt_total, min, max);
    if (out_msg && !ok_total)
        out_msg->append(QString::fromUtf8("TMJA total (%1 v/j) invalide : "
                                          "doit être entre %2 et %3 v/j.\n")
                        .arg(aadt_total).arg(min).arg(max));

    min = note77_lower_bounds[road_type][road_function][1];
    max = note77_upper_bounds[road_type][road_function][1];
    bool ok_hgv = is_in(aadt_hgv, min, max);
    if (out_msg && !ok_hgv)
        out_msg->append(QString::fromUtf8("TMJA poids-lourds (%1 v/j) invalide : "
                                          "doit être entre %2 et %3 v/j.\n")
                        .arg(aadt_hgv).arg(min).arg(max));

    min = note77_lower_bounds[road_type][road_function][2];
    max = note77_upper_bounds[road_type][road_function][2];
    bool ok_percent = is_in(hgv_percent, min, max);
    if (out_msg && !ok_percent)
        out_msg->append(QString::fromUtf8("Proportion de poids-lourds (%1%) invalide : "
                                          "doit être entre %2% et %3%.\n")
                        .arg(hgv_percent).arg(min).arg(max));

    return ok_total && ok_hgv && ok_percent;
}


bool  TYRoute::setFromAADT(double aadt_hgv, double aadt_lv,
                           TYRoute::RoadType road_type,
                           TYRoute::RoadFunction road_function,
                           QString* out_msg)
{
    if (!note77_check_validity(aadt_hgv, aadt_lv, road_type, road_function, out_msg))
    {
        return false;
    }

    for (unsigned i = 0; i < NB_TRAFFIC_REGIMES; ++i)
    {
        enum TrafficRegimes regime = static_cast<TrafficRegimes>(i);
        RoadTrafficComponent& traffic_lv  = accessRoadTrafficComponent(regime, TYTrafic::LV);
        traffic_lv.trafficFlow = // Compute hourly traffic
            aadt_lv  / note77_hourly_LV_coeff[road_type][road_function][regime];
        RoadTrafficComponent& traffic_hgv = accessRoadTrafficComponent(regime, TYTrafic::HGV);
        traffic_hgv.trafficFlow =  // Compute hourly traffic
            aadt_hgv / note77_hourly_HGV_coeff[road_type][road_function][regime];
    }
    return true;
}


TYRoute::TrafficHalfer::TrafficHalfer(TYRoute& road_) :
    road(road_)
{
    for (unsigned j = 0; j < NB_TRAFFIC_REGIMES; ++j)
    {
        enum TrafficRegimes regime = static_cast<TrafficRegimes>(j);
        for (unsigned i = 0; i < TYTrafic::NB_VEHICLE_TYPES; ++i)
        {
            enum TYTrafic::VehicleTypes vehicle_type = static_cast<TYTrafic::VehicleTypes>(i);
            road.accessRoadTrafficComponent(regime, vehicle_type).trafficFlow /= 2.0;
        }
    }
}

TYRoute::TrafficHalfer::~TrafficHalfer()
{
    // Restore the traffic
    for (unsigned j = 0; j < NB_TRAFFIC_REGIMES; ++j)
    {
        enum TrafficRegimes regime = static_cast<TrafficRegimes>(j);
        for (unsigned i = 0; i < TYTrafic::NB_VEHICLE_TYPES; ++i)
        {
            enum TYTrafic::VehicleTypes vehicle_type = static_cast<TYTrafic::VehicleTypes>(i);
            road.accessRoadTrafficComponent(regime, vehicle_type).trafficFlow *= 2.0;
        }
    }
    road.road_traffic.ramp = std::fabs(road.road_traffic.ramp);
}

const RoadTraffic& TYRoute::getNMBP08RoadTraffic(enum TrafficRegimes regime)
{
    setRoadTrafficArrayForRegime(regime);
    return road_traffic;
}


void TYRoute::setSurfaceType(RoadSurfaceType type)
{
    road_traffic.surfaceType = type;
}

void TYRoute::setSurfaceAge(double age)
{
    road_traffic.surfaceAge = age;
}

void TYRoute::setRamp(double ramp)
{
    road_traffic.ramp = ramp;
}

void TYRoute::setRoadTrafficComponent(
    enum TrafficRegimes regime, enum TYTrafic::VehicleTypes vehic_type,
    double flow, double speed, RoadFlowType type)
{
    RoadTrafficComponent& rtc =
        accessRoadTrafficComponent(regime, vehic_type);
    rtc.trafficFlow = flow; // vehicle / hour
    rtc.trafficSpeed = speed; // km / hour
    rtc.flowType = type;
}
