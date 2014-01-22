//! \test test_road.cpp

#include <iostream>

#include "gtest/gtest.h"

#include <QtXml>

#include "Tympan/MetierSolver/DataManagerMetier/Site/TYSiteNode.h"
#include "Tympan/MetierSolver/DataManagerMetier/Site/TYInfrastructure.h"
#include "Tympan/MetierSolver/DataManagerMetier/EltInfrastructure/TYRoute.h"
#include "Tympan/MetierSolver/DataManagerMetier/ComposantAcoustique/TYTrafic.h"

using std::cout;
using std::endl;

LPTYSiteNode buildFlatSiteSimpleRoad(void)
{
    // Creating the site and getting the topography
    LPTYSiteNode pSite = new TYSiteNode();
    pSite->setAltiEmprise(10);
    pSite->setUseEmpriseAsCrbNiv(true);

    // Those dimension match with the default emprise
    const double xMin = -200.0, xMax = 200.0, yMin = -200.0, yMax = +200.0;

    LPTYRoute pRoad = new TYRoute();
    #define NB_POINTS_ROAD 3
    // Initialise the level curve
    double road_x[NB_POINTS_ROAD] = { -150,    0,  100};
    double road_y[NB_POINTS_ROAD] = { -150,    0,    0};
    TYTabPoint pts;
    for(unsigned i=0; i<NB_POINTS_ROAD; ++i)
        pts.push_back(TYPoint(road_x[i], road_y[i], 0.0)); // NB This is not the right altitude
    #undef NB_POINTS_ROAD

    pRoad->setTabPoint(pts);

    // Add the level curve and updates the altimetry
    pSite->getInfrastructure()->addRoute(pRoad);
    return pSite;
}


TEST(TestRoads, basic_flat_road_creation) {

LPTYSiteNode pSite = buildFlatSiteSimpleRoad();
LPTYInfrastructure pInfra = pSite->getInfrastructure();

ASSERT_EQ(1, pInfra->getListRoute().size());
LPTYRoute pRoad = TYRoute::safeDownCast(pInfra->getListRoute()[0]->getElement());
}


TEST(TestRoads, xml_roundtrip)
{
LPTYRoute pRoad = new TYRoute();

pRoad->road_traffic.surfaceType = RoadSurface_DR1;
pRoad->road_traffic.surfaceAge = 10;
pRoad->road_traffic.ramp = 0.3;

QDomDocument doc_xml;
DOM_Element parent_xml = doc_xml.createElement("whatever");
DOM_Element road_xml = pRoad->toXML(parent_xml);

LPTYRoute pLoadedRoad = new TYRoute();
ASSERT_NE(RoadSurface_DR1, pLoadedRoad->road_traffic.surfaceType);
DOM_Element loaded_xml;
int status = pLoadedRoad->fromXML(road_xml);

ASSERT_EQ(1, status);
EXPECT_EQ(pRoad->road_traffic.surfaceType, pLoadedRoad->road_traffic.surfaceType);
EXPECT_EQ(pRoad->road_traffic.ramp,        pLoadedRoad->road_traffic.ramp);
EXPECT_EQ(pRoad->road_traffic.surfaceAge,  pLoadedRoad->road_traffic.surfaceAge);
}
