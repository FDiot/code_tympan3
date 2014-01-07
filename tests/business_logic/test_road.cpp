//! \test test_road.cpp

#include <iostream>

#include "gtest/gtest.h"

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
