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

using std::cout;
using std::cerr;
using std::endl;

TYAltimetrie* buildAltimetry(void)
{
	// Création de la topographie
	TYTopographie *pTopo = new TYTopographie();
	TYAltimetrie *pAlti = new TYAltimetrie();
	TYCourbeNiveau* pCrb = NULL;
	double x = -200.0, yMin = -200.0, yMax = +200.0, z = 0;
	for (unsigned int i = 0; i <= 40; i++)
	{
		// Construction de courbes de niveau
		x += 10;
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

TEST(AltitudePtTest, dumpenv) {
	// Create altimetry
	TYAltimetrie* pAlti = buildAltimetry();

	// TEST POSITIF : Les points sont dans l'espace défini par l'altimétrie
	OPoint3D pt(-70., 0., 0.);
	bool bRes = pAlti->updateAltitude(pt);
	ASSERT_TRUE(pt._z == 0.0);
	
	pt._x = 0.;
	bRes = pAlti->updateAltitude(pt);
	ASSERT_TRUE(pt._z == 150.0);
	
	pt._x = 70.;
	bRes = pAlti->updateAltitude(pt);
	ASSERT_TRUE(pt._z == 300.0);

	// TEST NEGATIF : Le point est hors zone
	pt._x = -500;
	bRes = pAlti->updateAltitude(pt);
	ASSERT_FALSE(bRes);
}
