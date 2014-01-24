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
 *
 *
 *
 */

#ifndef __TY_ROUTE__
#define __TY_ROUTE__

#include "gtest/gtest_prod.h"

#include "Tympan/MetierSolver/DataManagerMetier/ComposantGeoAcoustique/TYAcousticLine.h"
#include "Tympan/MetierSolver/DataManagerMetier/ComposantAcoustique/TYTrafic.h"

#if TY_USE_IHM
#include "Tympan/GraphicIHM/DataManagerIHM/TYRouteWidget.h"
#include "Tympan/GraphicIHM/DataManagerGraphic/TYRouteGraphic.h"
#endif

#include "RoadEmissionNMPB08.h"


///Noeud geometrique de type TYRoute.
typedef TYGeometryNode TYRouteGeoNode;
///Smart Pointer sur TYRouteGeoNode.
typedef SmartPtr<TYRouteGeoNode> LPTYRouteGeoNode;
///Collection de noeuds geometriques de type TYRoute.
typedef std::vector<LPTYRouteGeoNode> TYTabRouteGeoNode;

/**
 * Comprend les proprietes acoustiques et geometriques d'une route.
 *
 * @author Projet_Tympan
 *
 */
class TYRoute: public TYAcousticLine
{

    FRIEND_TEST(TestRoads, xml_roundtrip);

    OPROTOSUPERDECL(TYRoute, TYAcousticLine)
    TY_EXTENSION_DECL(TYRoute)
    TY_EXT_GRAPHIC_DECL(TYRoute)

    // Methodes
public:
    static const double undefined_declivity;

    enum TrafficRegimes { Day, Evening, Night, NB_TRAFFIC_REGIMES} ;

    /**
     * Constructeur.
     */
    TYRoute();
    /**
     * Constructeur par copie.
     */
    TYRoute(const TYRoute& other);
    /**
     * Destructeur.
     */
    virtual ~TYRoute();

    ///Operateur =.
    TYRoute& operator=(const TYRoute& other);
    ///Operateur ==.
    bool operator==(const TYRoute& other) const;
    ///Operateur !=.
    bool operator!=(const TYRoute& other) const;

    virtual bool deepCopy(const TYElement* pOther, bool copyId = true);

    virtual std::string toString() const;

    virtual DOM_Element toXML(DOM_Element& domElement);
    virtual int fromXML(DOM_Element domElement);

    // TODO/WIP The speed is modelised at the RoadTrafficComponent in NMPB08
    /**
     * Set/Get de la vitesse moyenne.
     */
    double getVitMoy() const { return getTraficJour().lv.trafficSpeed; }

    // TODO/WIP The speed is modelised at the RoadTrafficComponent in NMPB08
    /**
     * Set/Get de la vitesse moyenne.
     */
    void setVitMoy(double vit) {
       getTraficJour().lv.trafficSpeed = vit;
       getTraficNuit().hgv.trafficSpeed = vit;
    }

    /**
     * Set/Get du trafic de jour.
     */
    const TYTrafic& getTraficJour() const { return traffic_regimes[Day]; }
    TYTrafic& getTraficJour() { return traffic_regimes[Day]; }

    /**
     * Set/Get du trafic de jour.
     */
    //  void setTraficJour(const LPTYTrafic pTrafic);

    /**
     * Set/Get du trafic de nuit.
     */
    const TYTrafic& getTraficNuit() const { return traffic_regimes[Night]; }
    TYTrafic& getTraficNuit() { return traffic_regimes[Night]; }

    /**
     * Set/Get du trafic de nuit.
     */
    //  void setTraficNuit(const LPTYTrafic pTrafic);


    enum RoadType {Intercity, Motorway};
    enum RoadFunction {Regional, LongDistance};

    /**
     * \brief Apply Note77 from Setra to estimate trafic from AADT and road kind.
     *
     * Annual Average Daily Traffic for both HGV and LV can be used to estimate
     * traffic as long as the kind of the road is identified. This approach is
     * described in Setra Note 77 (french) or appendix C from the guide
     * `Road Noise Prediction`.
     *
     * This method implements this approach and sets the road traffic if successful.
     * If the argument in the scope of application of the Note77 the method succeed
     * and update the traffic. Otherwise it fails and does not modifyt the road.
     *
     * \param aadt_hgv AADT for the HGV
     * \param aadt_lv  AADT for the LV
     * \param road_type The type (motor way or intercity)
     * \param road_function The function of the road (long distance or regional)
     * \return whether the call succeded.
     */
    bool setFromAADT(double aadt_hgv, double aadt_lv,
                     RoadType road_type, RoadFunction road_function);

    // TODO in NMPB08 `mode calcul` is obsoleted.
    //      Is now represented by the RoadFlowType at RoadTrafficComponent level.

    /*
     * Calcul du spectre de la route en prenant en compte sont trafic
     */
    TYSpectre computeSpectre(enum TrafficRegimes regime);

    /**
     * Mise a jour des caracteristiques acoustiques de la route
     */
    virtual bool updateAcoustic(const bool& force = false);

    /**
     * \brief Required the road to update its altitude after altimetry changed
     *
     * \param alti the altimetry the altitude must be updated from
     * \param the GeoNode associated with this Road
     * \return whether the update succeeded.
     */
    virtual bool updateAltitudes(const TYAltimetrie& alti, LPTYRouteGeoNode pGeoNode );

    const RoadTrafficComponent& getRoadTrafficComponent(
        enum TrafficRegimes regime, enum TYTrafic::VehicleTypes vehic_type) const;

private:
    virtual void distriSrcs()
    {assert(false && "You must use distriSrcs(const TYAltimetrie&) for roads");}

    /**
     * \brief internally set the RoadTraffic's array of RoadTrafficComponents
     *        according to the given regime.
     */
    void setRoadTrafficArrayForRegime(enum TrafficRegimes regime);

protected:

    RoadTrafficComponent& getRoadTrafficComponent(
          enum TrafficRegimes regime, enum TYTrafic::VehicleTypes vehic_type);

    /**
     * \brief Calcul de la pente moyenne de la route
     */
    double calculPenteMoyenne();

    /**
     * \brief update the mean declivity from the current source distribution.
     *
     * NB Do ensure that \c distriSrcs(const TYAltimetrie&) has been called
     * since last geometry or altimetry changes.
     */
    void updateComputedDeclivity();

    /**
     * Distribution des sources
     */
    virtual void distriSrcs(const TYAltimetrie& alti, LPTYRouteGeoNode pGeoNode);

    /// Get/Set de l'offset des sources de la route
    double getOffSet() { return _offSet; }
    void setOffSet(const double& offSet) { _offSet = offSet; }

    // Membres
protected:

    RoadTraffic road_traffic;
    TYTrafic traffic_regimes[NB_TRAFFIC_REGIMES];

    // Hauteur des sources par rapport a la route
    double _offSet;

private:

    bool note77_check_validity(double aadt_hgv, double aadt_lv,
                               RoadType road_type, RoadFunction road_function);

    typedef double note77_tables[2][2][3];
    static const note77_tables note77_lower_bounds; // table C1
    static const note77_tables note77_upper_bounds; // table C1
    static const note77_tables note77_hourly_HGV_coeff; // table C2
    static const note77_tables note77_hourly_LV_coeff; // table C2
};


#endif // __TY_ROUTE__
