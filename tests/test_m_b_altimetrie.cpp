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

#include "Tympan/models/common/3d.h"

#include "Tympan/models/business/topography/TYCourbeNiveau.h"
#include "Tympan/models/business/topography/TYAltimetrie.h"
#include "Tympan/models/business/infrastructure/TYTopographie.h"
#include "Tympan/models/business/infrastructure/TYSiteNode.h"
#include "Tympan/models/business/material/TYSol.h"
#include "Tympan/models/business/altimetry_file_reader_impl.h"
#include "Tympan/models/business/subprocess_util.h"
#include "Tympan/models/business/xml_project_util.h"
#include "TympanTestsConfig.hpp"

static  const double precision = 1e-6;

void update_altimetry(LPTYSiteNode pSite)
{
    QTemporaryFile xml_project;
    QTemporaryFile mesh;
    init_tmp_file(xml_project, false);
    init_tmp_file(mesh, false);
    tympan::save_project(xml_project.fileName().toUtf8().data(), pSite->getProjet());
    QStringList args;
    args << QString(tympan::path_to_python_scripts("process_site_altimetry.py").c_str()) << xml_project.fileName()
        << mesh.fileName();
    string error_msg;
    python(args, error_msg);
    std::deque<OPoint3D> points;
    std::deque<OTriangle> triangles;
    std::deque<LPTYSol> materials;
    pSite->readMesh(points, triangles, materials, mesh.fileName());
    pSite->getTopographie()->getAltimetrie()->plugBackTriangulation(points, triangles, materials);
}


LPTYSiteNode buildSiteSlopeAltimetry(void)
{
    LPTYSiteNode pSite = new TYSiteNode();
    LPTYTopographie pTopo = pSite->getTopographie();
    TYCourbeNiveau* pCrb = NULL;
    // Those dimension match with the default emprise
    const double xMin = -180.0, xMax = 180.0, yMin = -180.0, yMax = +180.0;
    double z;
    for (double x = xMin; x <= xMax; x += 10.)
    {
        // Building the level curves
        z = x < -50. ? 0 : 3 * x + 150;
        z = x > +50. ? 300. : z;
        pCrb = new TYCourbeNiveau();
        pCrb->addPoint(x, yMin, z);
        pCrb->addPoint(x, yMax, z);
        pCrb->setAltitude(z);
        pCrb->setDistMax(10.0);
        // Adding the level curve to the topography
        pTopo->addCrbNiv(pCrb);
    }
    return pSite;
}

TEST(TYAltimetryTest, update_point_altitude)
{
    LPTYProjet pProjet = new TYProjet();
    LPTYSiteNode pSite = buildSiteSlopeAltimetry();
    pProjet->setSite(pSite);
    LPTYTopographie pTopo = pSite->getTopographie();
    update_altimetry(pSite);
    LPTYAltimetrie pAlti = pTopo->getAltimetrie();

    // Positive tests : Points lie in the convex hull of the altimetry
    OPoint3D pt(-70., 0., 0.);
    bool bRes = pAlti->updateAltitude(pt);
    EXPECT_NEAR(0.0, pt._z, precision);
    pt._x = 0.;
    bRes = pAlti->updateAltitude(pt);
    EXPECT_NEAR(150.0, pt._z, precision);
    pt._x = 70.;
    bRes = pAlti->updateAltitude(pt);
    EXPECT_NEAR(300.0, pt._z, precision);

    // Negative test : points lie outside
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
    // No landtake specified. default landtake has dimension 400X400m

#define NB_POINTS_LEVEL_CURVE 5
    TYCourbeNiveau* pCrb = new TYCourbeNiveau();
    // Initialise the level curve
    double level_curve_x[NB_POINTS_LEVEL_CURVE] = { -150, -150,  150,  150, -150};
    double level_curve_y[NB_POINTS_LEVEL_CURVE] = { -150,  150,  150, -150, -150};
    for (unsigned i = 0; i < NB_POINTS_LEVEL_CURVE; ++i)
    {
        pCrb->addPoint(level_curve_x[i], level_curve_y[i]);
    }
    pCrb->setAltitude(level_curve_A_alti);
#undef NB_POINTS_LEVEL_CURVE

    // Add the level curve and updates the altimetry
    pSite->getTopographie()->addCrbNiv(pCrb);
    return pSite;
}

LPTYTerrain addTerrainToSimpleSite(LPTYSiteNode pSite)
{
#define NB_POINTS_TERRAIN 4
    double terrain_x[NB_POINTS_TERRAIN] = {    0,    0,  100,  100};
    double terrain_y[NB_POINTS_TERRAIN] = { -150,   50,   50, -150};

    LPTYTerrain pTerrain = new TYTerrain();
    for (unsigned i = 0; i < NB_POINTS_TERRAIN; ++i)
    {
        pTerrain->getListPoints().push_back(TYPoint(terrain_x[i], terrain_y[i], -100));
    }
    LPTYSol pSol = new TYSol();
    pSol->setName("grass");
    pSol->setResistivite(3.14); // NB This is a dummy value
    pTerrain->setSol(pSol);
#undef NB_POINTS_TERRAIN

    pSite->getTopographie()->addTerrain(pTerrain);

    return pTerrain;
}

static const double level_curve_B_alti = 20.0;

LPTYCourbeNiveau addHillToSimpleSite(LPTYSiteNode pSite)
{
#define NB_POINTS_LEVEL_CURVE 5
    LPTYCourbeNiveau pCrb = new TYCourbeNiveau();
    // Initialise the level curve
    double level_curve_x[NB_POINTS_LEVEL_CURVE] = {    0,    0,  100,  100,    0};
    double level_curve_y[NB_POINTS_LEVEL_CURVE] = { -140,   50,   50, -140, -140};
    for (unsigned i = 0; i < NB_POINTS_LEVEL_CURVE; ++i)
    {
        pCrb->addPoint(level_curve_x[i], level_curve_y[i]);
    }
    pCrb->setAltitude(level_curve_B_alti);
#undef NB_POINTS_LEVEL_CURVE

    // Add the level curve and updates the altimetry
    pSite->getTopographie()->addCrbNiv(pCrb);
    return pCrb;
}

TEST(TYAltimetryTest, dummy_grid)
{
    LPTYProjet pProjet = new TYProjet();
    LPTYSiteNode pSite = buildSiteSimpleAltimetry();
    pProjet->setSite(pSite);
    LPTYTopographie pTopo = pSite->getTopographie();
    LPTYAltimetrie pAlti = pTopo->getAltimetrie();
    update_altimetry(pSite);

    // 2 triangles should give a 1x1 accelerating grid
    EXPECT_EQ(1, pAlti->_gridSX);
    EXPECT_EQ(1, pAlti->_gridSY);

    TYAltimetrie::grid_index idx;
    ASSERT_TRUE(pAlti->getGridIndices(OPoint3D(-10, 10, 0), idx));
    EXPECT_EQ(0, idx.pi);
    EXPECT_EQ(0, idx.qi);

    // Check the altitude of an inner point
    OPoint3D pt(10.0, 10.0, 0.0);
    EXPECT_TRUE(pAlti->updateAltitude(pt));
    EXPECT_NEAR(level_curve_A_alti, pt._z, precision);

    // Check altitude of an outer point
    pt.setCoords(180.0, 180.0, 0.0);
    EXPECT_FALSE(pAlti->updateAltitude(pt));
    EXPECT_DOUBLE_EQ(TYAltimetrie::invalid_altitude, pt._z);
}

TEST(TYAltimetryTest, simple_grid)
{
    LPTYProjet pProjet = new TYProjet();
    LPTYSiteNode pSite = buildSiteSimpleAltimetry();
    pProjet->setSite(pSite);
    LPTYTopographie pTopo = pSite->getTopographie();
    LPTYAltimetrie pAlti = pTopo->getAltimetrie();
    LPTYCourbeNiveau pHill = addHillToSimpleSite(pSite);
    update_altimetry(pSite);

    // 64 triangles should give a 4x4 accelerating grid
    // (_gridSX = _gridSY = sqrt(nb triangles)/2)
    EXPECT_EQ(64, pAlti->getListFaces().size());
    EXPECT_EQ(4, pAlti->_gridSX);
    EXPECT_EQ(4, pAlti->_gridSY);

    // Check getGridIndices in the four quadrants (landtake: [-200;200] on X and Y axes)
    TYAltimetrie::grid_index idx;
    ASSERT_TRUE(pAlti->getGridIndices(OPoint3D(-150, -150, 0), idx));
    EXPECT_EQ(0, idx.pi);
    EXPECT_EQ(0, idx.qi);

    ASSERT_TRUE(pAlti->getGridIndices(OPoint3D(-150, 150, 0), idx));
    EXPECT_EQ(0, idx.pi);
    EXPECT_EQ(3, idx.qi);

    ASSERT_TRUE(pAlti->getGridIndices(OPoint3D(150, 150, 0), idx));
    EXPECT_EQ(3, idx.pi);
    EXPECT_EQ(3, idx.qi);

    ASSERT_TRUE(pAlti->getGridIndices(OPoint3D(150, -150, 0), idx));
    EXPECT_EQ(3, idx.pi);
    EXPECT_EQ(0, idx.qi);

    // Check altitude in the middle of the hill
    OPoint3D pt(10.0, 10.0, 0.0);
    EXPECT_TRUE(pAlti->updateAltitude(pt));
    EXPECT_NEAR(level_curve_B_alti, pt._z, precision);

    // Check altitude on the flank of the hill
    pt.setCoords(50.0, 100.0, 0.0);
    EXPECT_TRUE(pAlti->updateAltitude(pt));
    EXPECT_NEAR((level_curve_A_alti + level_curve_B_alti) / 2, pt._z, precision);

    // Check altitude of an outer point
    pt.setCoords(180.0, 180.0, 0.0);
    EXPECT_FALSE(pAlti->updateAltitude(pt));
    EXPECT_DOUBLE_EQ(TYAltimetrie::invalid_altitude, pt._z);
}

TEST(TYAltimetryTest, simple_terrain)
{
    LPTYProjet pProjet = new TYProjet();
    LPTYSiteNode pSite = buildSiteSimpleAltimetry();
    pProjet->setSite(pSite);
    LPTYTopographie pTopo = pSite->getTopographie();
    LPTYAltimetrie pAlti = pTopo->getAltimetrie();
    update_altimetry(pSite);

    // Check altitude in the middle of the future terrain
    OPoint3D pt(10.0, 10.0, 0.0);
    EXPECT_TRUE(pAlti->updateAltitude(pt));
    EXPECT_NEAR(level_curve_A_alti, pt._z, precision);

    LPTYTerrain pTerrain = addTerrainToSimpleSite(pSite);

    // Check again
    pt.setCoords(10.0, 10.0, 0.0);
    EXPECT_TRUE(pAlti->updateAltitude(pt));
    EXPECT_NEAR(level_curve_A_alti, pt._z, precision);
}
