//! \test test_road.cpp


#include "gtest/gtest.h"

#include <QtXml>

#include "Tympan/Tools/OSmartPtr.h"

#include "Tympan/MetierSolver/DataManagerMetier/Site/TYSiteNode.h"
#include "Tympan/MetierSolver/DataManagerMetier/Site/TYInfrastructure.h"
#include "Tympan/MetierSolver/DataManagerMetier/EltInfrastructure/TYRoute.h"
#include "Tympan/MetierSolver/DataManagerMetier/ComposantAcoustique/TYTrafic.h"

#include "Tympan/MetierSolver/DataManagerMetier/ComposantGeoAcoustique/TYAcousticLine.h"


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


TEST(TestRoads, basic_flat_road_creation)
{
LPTYSiteNode pSite = buildFlatSiteSimpleRoad();
LPTYInfrastructure pInfra = pSite->getInfrastructure();

ASSERT_EQ(1, pInfra->getListRoute().size());
LPTYRoute pRoad = TYRoute::safeDownCast(pInfra->getListRoute()[0]->getElement());
} // TEST(TestRoads, basic_flat_road_creation)


// TODO This utility template could be factored-out for future use
template<class ElType>
SmartPtr<ElType> saveAndReload(SmartPtr<ElType> original,
                               DOM_Element* p_xml=NULL)
{
    QDomDocument doc_xml; // Needs to be in scope for the whole function
    DOM_Element parent_xml;
    if(p_xml==NULL)
    {
        parent_xml = doc_xml.createElement("saveAndReload");
        doc_xml.appendChild(parent_xml);
    }
    else
        parent_xml = *p_xml;
    // Actually serialise
    DOM_Element original_xml = original->toXML(parent_xml);
    SmartPtr<ElType> pLoaded = new ElType();
    // Actually deserialise
    int status = pLoaded->fromXML(original_xml);
    if(status==1)
        return pLoaded;
    else
        return SmartPtr<ElType>(); // NULL smart pointer denotes failure
}


TEST(TestRoads, xml_roundtrip)
{
LPTYRoute pRoad = new TYRoute();

pRoad->road_traffic.surfaceType = RoadSurface_DR1;
pRoad->road_traffic.surfaceAge = 10;
pRoad->road_traffic.ramp = 0.3;

LPTYRoute pLoadedRoad = saveAndReload(pRoad);
ASSERT_NE(LPTYRoute(), pLoadedRoad); // Success of saveAndReload

EXPECT_EQ(pRoad->road_traffic.surfaceType, pLoadedRoad->road_traffic.surfaceType);
EXPECT_EQ(pRoad->road_traffic.ramp,        pLoadedRoad->road_traffic.ramp);
EXPECT_EQ(pRoad->road_traffic.surfaceAge,  pLoadedRoad->road_traffic.surfaceAge);

for(unsigned i=0; i<TYRoute::NB_TRAFFIC_REGIMES; ++i)
{
    // For some obscure reason GTest does not find the operator==
    // so we can not use EXPECT_EQ as usual.
    // EXPECT_EQ(pLoadedRoad->traffic_regimes[i], pRoad->traffic_regimes[i]);
    EXPECT_TRUE(pLoadedRoad->traffic_regimes[i] == pRoad->traffic_regimes[i]);
}

// NB: The pre-existing operator== can not be trusted to be consistent
// with XML round trip or test for equality (and not identity)
// See ticket https://extranet.logilab.fr/ticket/1522889

} // TEST(TestRoads, xml_roundtrip)

// This test is disabled because it is know to fail
// See https://extranet.logilab.fr/ticket/1522889
TEST(TestAcousticLine, DISABLED_equality)
{
    LPTYAcousticLine pAcLine = new TYAcousticLine();
    LPTYAcousticLine pReloadedAcLine  = saveAndReload(pAcLine);
    ASSERT_NE(LPTYAcousticLine(), pReloadedAcLine);

    ASSERT_TRUE(*pReloadedAcLine == *pAcLine) <<
        "Invalid pre-existing operator== on TYAcousticLine";
} // TEST(TestAcousticLine, DISABLED_equality)

TEST(TestTraffic, equality)
{
    LPTYTrafic pTraffic = new TYTrafic();
    LPTYTrafic pReloadedTraffic  = saveAndReload(pTraffic);
    ASSERT_NE(LPTYTrafic(), pReloadedTraffic);

    ASSERT_TRUE(*pReloadedTraffic == *pTraffic) <<
        "Invalid pre-existing operator== on TYTraffic";
} // TEST(TestTraffic, equality)
