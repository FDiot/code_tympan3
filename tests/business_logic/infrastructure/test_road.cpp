//! \test test_road.cpp


#include <iostream>
#include <fstream>

#include <boost/math/special_functions/fpclassify.hpp>

#include "gtest/gtest.h"

#include "test_utils/misc.hpp"
#include "TympanTestsConfig.hpp"

#include <QtXml>
#include <QDir>


#include "Tympan/core/smartptr.h"

#include "Tympan/MetierSolver/DataManagerMetier/Site/TYSiteNode.h"
#include "Tympan/MetierSolver/DataManagerMetier/Site/TYInfrastructure.h"
#include "Tympan/MetierSolver/DataManagerMetier/EltInfrastructure/TYRoute.h"
#include "Tympan/MetierSolver/DataManagerMetier/ComposantAcoustique/TYTrafic.h"
#include "Tympan/MetierSolver/DataManagerMetier/ComposantGeoAcoustique/TYAcousticLine.h"


using namespace std;

LPTYSiteNode buildFlatSiteSimpleRoad(void)
{
    // Creating the site and getting the topography
    LPTYSiteNode pSite = new TYSiteNode();
    pSite->setAltiEmprise(10);
    pSite->setUseEmpriseAsCrbNiv(true);

    LPTYRoute pRoad = new TYRoute();
#define NB_POINTS_ROAD 3
    // Initialise the level curve
    double road_x[NB_POINTS_ROAD] = { -150,    0,  100};
    double road_y[NB_POINTS_ROAD] = { -150,    0,    0};
    TYTabPoint pts;
    for (unsigned i = 0; i < NB_POINTS_ROAD; ++i)
    {
        pts.push_back(TYPoint(road_x[i], road_y[i], 0.0));    // NB This is not the right altitude
    }
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
                               DOM_Element* p_xml = NULL)
{
    QDomDocument doc_xml; // Needs to be in scope for the whole function
    DOM_Element parent_xml;
    if (p_xml == NULL)
    {
        parent_xml = doc_xml.createElement("saveAndReload");
        doc_xml.appendChild(parent_xml);
    }
    else
    {
        parent_xml = *p_xml;
    }
    // Actually serialise
    DOM_Element original_xml = original->toXML(parent_xml);
    SmartPtr<ElType> pLoaded = new ElType();
    // Actually deserialise
    int status = pLoaded->fromXML(original_xml);
    if (status == 1)
    {
        return pLoaded;
    }
    else
    {
        return SmartPtr<ElType>();    // NULL smart pointer denotes failure
    }
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

    for (unsigned i = 0; i < TYRoute::NB_TRAFFIC_REGIMES; ++i)
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


TEST(TestRoads, computeSpectreHalvedTraffic)
{
    LPTYRoute pRoad = new TYRoute();

    pRoad->setSurfaceType(RoadSurface_DR1);
    pRoad->setSurfaceAge(10);
    pRoad->setRamp(30);

    pRoad->setRoadTrafficComponent(TYRoute::Day, TYTrafic::LV,
                                   2000, 100);
    pRoad->setRoadTrafficComponent(TYRoute::Day, TYTrafic::HGV,
                                   0, 80);
    const RoadTrafficComponent& lv_rtc =
        pRoad->getNMPB08RoadTrafficComponent(TYRoute::Day, TYTrafic::LV);

    EXPECT_EQ(2000, lv_rtc.trafficFlow);
    EXPECT_EQ(30, pRoad->ramp());

    TYSpectre spectrum = pRoad->computeSpectre(TYRoute::Day);

    // NB This reference value has been computed from the CEREMA website:
    // http://213.215.52.146/RoadEmissionNMPB08
    EXPECT_NEAR(87.64, spectrum.valGlobDBA(), 0.1);

    EXPECT_EQ(2000, lv_rtc.trafficFlow);
    EXPECT_EQ(30, pRoad->ramp());

} // TEST(TestRoads, computeSpectreHalvedTraffic)


LPTYRoute functionnalResults_initRoadFromRow(const deque<double>& row)
{
    LPTYRoute pRoad = new TYRoute();

    pRoad->setSurfaceType(static_cast<RoadSurfaceType>((int)row[0]));
    pRoad->setSurfaceAge(row[1]);
    pRoad->setRamp(row[2]);

    pRoad->setRoadTrafficComponent(
        TYRoute::Day,  TYTrafic::HGV, row[3], 80 /* km/h */);
    pRoad->setRoadTrafficComponent(
        TYRoute::Day,  TYTrafic::LV, row[4], 120 /* km/h */);
    return pRoad;
}

TEST(TestRoads, functionnalResults)
{
    const double precision = 0.15; // in dB

    // Locate CSV data file
    std::string data_file_name = tympan::path_to_test_data("dataRoadEmissionNMPB2008.csv");

    ifstream file(data_file_name.c_str());
    ASSERT_TRUE(file.is_open()) <<
        "Can not open test data file : " << data_file_name.c_str();
    string header;
    getline(file, header); // Read and check the header line
    ASSERT_FALSE(file.fail())<<
        "Failure while reading test data file : " << data_file_name.c_str();
    EXPECT_EQ("Surface type,Age,Declivity,HGV,LV,% HGV,global,dB(A)", header);
    deque<deque<double> > table = readCsvAsTableOf<double>(file);
    ASSERT_EQ(32, table.size()) <<
        "Bad number of rows in test data file " << data_file_name.c_str();

    unsigned row_num = 0;
    BOOST_FOREACH(const deque<double>& row, table)
    {
        ++row_num;
        LPTYRoute pRoad = functionnalResults_initRoadFromRow(row);
        TYSpectre spectrum = pRoad->computeSpectre(TYRoute::Day);
        const double& loaded_ref = row[7];
        double global_dBA = spectrum.valGlobDBA();
        EXPECT_TRUE(boost::math::isfinite(global_dBA)) << spectrum << endl;
        EXPECT_NEAR(loaded_ref, global_dBA, precision) <<
                                                       "Incorrect results for the row #" << row_num << " (header is #0)"
                                                       " of the test data file : " << data_file_name.c_str();
    }
    file.close();
}

TEST(TestRoads, note77_bounds)
{
    TYRoute::RoadType road_type = TYRoute::Intercity;
    TYRoute::RoadFunction road_function = TYRoute::Regional;
    QString out_msg;
    bool ok;
    double aadt_hgv,  aadt_lv;

    aadt_hgv = 1; // Heavy Goods Vehicle
    aadt_lv = 1; // Light Vehicle
    ok = TYRoute::note77_check_validity(aadt_hgv, aadt_lv,
                                        road_type, road_function,
                                        &out_msg);
    ASSERT_FALSE(ok) << "Manifest bad data to check the reported bounds";
    // NB TMJA is the french acronym for AADT
    const char* expected =
        "TMJA total (2 v/j) invalide : doit être entre 2500 et 22000 v/j.\n"
        "TMJA poids-lourds (1 v/j) invalide : doit être entre 250 et 2500 v/j.\n"
        "Proportion de poids-lourds (50%) invalide : doit être entre 5% et 17%.\n";
    EXPECT_STREQ(expected, out_msg.toUtf8().constData());
}
