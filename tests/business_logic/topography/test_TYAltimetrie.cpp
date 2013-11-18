/**
 * \file test_OBox.cpp
 * \test Testing of OBox libs used by lot of TYMPAN features
 *
 *  Created on: april 21, 2013
 *      Author: Denis THOMASSON <denis.thomasson@edf.fr>
 */

#include <cstdlib>
#include <deque>

#include "gtest/gtest.h"
#include "Tympan/MetierSolver/ToolsMetier/OGeometrie.h"
#include "Tympan/MetierSolver/ToolsMetier/OCoord3D.h"
#include "Tympan/MetierSolver/ToolsMetier/OPoint3D.h"
#include "Tympan/MetierSolver/ToolsMetier/OVector3D.h"
#include "Tympan/MetierSolver/ToolsMetier/OSegment3D.h"
#include "Tympan/MetierSolver/ToolsMetier/OBox.h"
#include "Tympan/MetierSolver/DataManagerMetier/EltTopographique/TYCourbeNiveau.h"
#include "Tympan/MetierSolver/DataManagerMetier/EltTopographique/TYAltimetrie.h"
#include "Tympan/MetierSolver/DataManagerMetier/Site/TYTopographie.h"
#include "Tympan/MetierSolver/DataManagerMetier/Site/TYSiteNode.h"


//using std::cout;
//using std::cerr;
//using std::endl;

TYAltimetrie* buildSlopeAltimetry(void)
{
	// Création de la topographie
	TYTopographie *pTopo = new TYTopographie();
	TYAltimetrie *pAlti = new TYAltimetrie();
	TYCourbeNiveau* pCrb = NULL;
        // Those dimension match with the default emprise
	const double xMin = -200.0, xMax = 200.0, yMin = -200.0, yMax = +200.0;
        double z;
	for (double x = xMin; x <= xMax; x += 10.)
	{
		// Construction de courbes de niveau
		z = x < -50. ? 0 : 3*x + 150;
		z = x > +50. ? 300. : z;
		pCrb = new TYCourbeNiveau();
		pCrb->addPoint(x, yMin, z);
		pCrb->addPoint(x, yMax, z);
		pCrb->setAltitude(z);
		pCrb->setDistMax(10.0);

		// Ajout de la courbe de niveau à la topographie
		pTopo->addCrbNiv(pCrb);
	}

	// Création de l'altimetrie
	std::deque<OPoint3D> points;
	std::deque<OTriangle> triangles;
	// the false argument for use_emprise_as_level_curve is required
	// because in this test the TYTopographie as no TYSiteNode as parent.
	pTopo->computeAltimetricTriangulation(points, triangles, false);
	pAlti->plugBackTriangulation(points, triangles);

	//compute(pTopo->collectPointsForAltimetrie(false), 1.E-5);

	return pAlti;
}

TEST(TYAltimetryTest, update_point_altitude) {
	// Create altimetry
	TYAltimetrie* pAlti = buildSlopeAltimetry();

	// TEST POSITIF : Les points sont dans l'espace défini par l'altimétrie
	OPoint3D pt(-70., 0., 0.);
	bool bRes = pAlti->updateAltitude(pt);
	EXPECT_DOUBLE_EQ(pt._z, 0.0);

	pt._x = 0.;
	bRes = pAlti->updateAltitude(pt);
	EXPECT_DOUBLE_EQ(150.0, pt._z);

	pt._x = 70.;
	bRes = pAlti->updateAltitude(pt);
	EXPECT_DOUBLE_EQ(300.0, pt._z);

	// TEST NEGATIF : Le point est hors zone
	pt._x = -500;
	bRes = pAlti->updateAltitude(pt);
	EXPECT_FALSE(bRes);
}

static const double level_curve_A_alti = 10.0;

LPTYSiteNode buildSiteSimpleAltimetry(void)
{
    // Creating the site and getting the topography
    LPTYSiteNode pSite = new TYSiteNode();
    pSite->setAltiEmprise(-100);
    pSite->setUseEmpriseAsCrbNiv(false);

    // Those dimension match with the default emprise
    const double xMin = -200.0, xMax = 200.0, yMin = -200.0, yMax = +200.0;

    #define NB_POINTS_LEVEL_CURVE 5
    TYCourbeNiveau* pCrb = new TYCourbeNiveau();
    // Initialise the level curve
    double level_curve_x[NB_POINTS_LEVEL_CURVE] = { -150, -150,  150,  150, -150};
    double level_curve_y[NB_POINTS_LEVEL_CURVE] = { -150,  150,  150, -150, -150};
    for(unsigned i=0; i<NB_POINTS_LEVEL_CURVE; ++i)
        pCrb->addPoint(level_curve_x[i], level_curve_y[i]);
    pCrb->setAltitude(level_curve_A_alti);
    #undef NB_POINTS_LEVEL_CURVE

    // Add the level curve and updates the altimetry
    pSite->getTopographie()->addCrbNiv(pCrb);
    return pSite;
}


TEST(TYAltimetryTest, site_add_terrain) {

    LPTYSiteNode pSite = buildSiteSimpleAltimetry();
    LPTYTopographie pTopo = pSite->getTopographie();
    LPTYAltimetrie pAlti = pTopo->getAltimetrie();

    // Update the altimetry and check the altitude of an inner point
    ASSERT_TRUE(pSite->updateAltimetrie(true));
    OPoint3D pt(10.0, 10.0, 0.0);
    EXPECT_TRUE(pAlti->updateAltitude(pt));
    EXPECT_DOUBLE_EQ(level_curve_A_alti, pt._z);

    // Check altitude of an outer point

}
