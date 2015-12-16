/**
 * \file test_m_b_sitenode.cpp
 * \test various tests regarding TYSiteNode business objects
 *
 *  Created on: july 2014
 *      Author: Laura Médioni <laura.medioni@logilab.fr>
 */

#include "gtest/gtest.h"
#include "tests/TympanTestsConfig.h"
#include "Tympan/models/business/TYProjet.h"
#include "Tympan/models/business/infrastructure/TYSiteNode.h"
#include "Tympan/models/business/infrastructure/TYInfrastructure.h"
#include "Tympan/models/business/xml_project_util.h"


TEST(ExternalEntityTest, external_elements)
{
    LPTYProjet project = tympan::load_project(tympan::path_to_test_data(
                "test_external_elements.xml").c_str());
    LPTYInfrastructure infra_ptr = project->getSite()->getInfrastructure();
    TYTabBatimentGeoNode buildings = infra_ptr->getListBatiment();
    EXPECT_EQ(buildings.size(), 2);
    TYTabMachineGeoNode machines = infra_ptr->getListMachine();
    EXPECT_EQ(machines.size(), 2);
    TYTabRouteGeoNode roads = infra_ptr->getListRoute();
    EXPECT_EQ(roads.size(), 0);

}


TEST(GroundContourTest, single_machine_on_ground)
{
    LPTYProjet project = tympan::load_project(tympan::path_to_test_data(
            "projects-panel/site_with_single_machine.xml").c_str());

    // Get a pointer to the TYSiteNode.
    LPTYSiteNode site_ptr = project->getSite();

    // Retrieve ground contour
    map<TYUUID, TYTabPoint3D> groundbased_faces;
    site_ptr->getFacesOnGround(groundbased_faces);

    // Check values:
    // We must get a single contour (TYTabPoint3D) containing the coordinates
    // of the basis of the machine.
    EXPECT_EQ(groundbased_faces.size(), 1);
    // Rectangle
    EXPECT_EQ((*groundbased_faces.begin()).second.size(), 4);
    // Coordinates of the rectangle vertices: (0,0), (20,0), (20,10), (0,10)
    EXPECT_EQ((*groundbased_faces.begin()).second[0]._x, 0);
    EXPECT_EQ((*groundbased_faces.begin()).second[0]._y, 0);
    EXPECT_EQ((*groundbased_faces.begin()).second[1]._x, 20);
    EXPECT_EQ((*groundbased_faces.begin()).second[1]._y, 0);
    EXPECT_EQ((*groundbased_faces.begin()).second[2]._x, 20);
    EXPECT_EQ((*groundbased_faces.begin()).second[2]._y, 10);
    EXPECT_EQ((*groundbased_faces.begin()).second[3]._x, 0);
    EXPECT_EQ((*groundbased_faces.begin()).second[3]._y, 10);
}

TEST(GroundContourTest, single_flying_building)
{
    LPTYProjet project = tympan::load_project(tympan::path_to_test_data(
            "projects-panel/site_with_single_flying_building.xml").c_str());

    // Get a pointer to the TYSiteNode.
    LPTYSiteNode site_ptr = project->getSite();

    // Retrieve ground contour
    map<TYUUID, TYTabPoint3D> groundbased_faces;
    site_ptr->getFacesOnGround(groundbased_faces);

    // As the building is placed at a 100 meters height, the ground contour
    // is expected to be nonexistent.
    EXPECT_TRUE(groundbased_faces.empty());
}

TEST(GroundContourTest, joined_buildings)
{
     LPTYProjet project = tympan::load_project(tympan::path_to_test_data(
            "projects-panel/site_with_two_joined_buildings.xml").c_str());

    // Get a pointer to the TYSiteNode.
    LPTYSiteNode site_ptr = project->getSite();

    // Retrieve ground contour
    map<TYUUID, TYTabPoint3D> groundbased_faces;
    map<TYUUID, TYTabPoint3D>::iterator faces_it;
    site_ptr->getFacesOnGround(groundbased_faces);

    EXPECT_EQ(groundbased_faces.size(), 2);
    faces_it = groundbased_faces.begin();
    // Coordinates of the first rectangle vertices: (20,10), (30,10), (30,0), (20,0)
    EXPECT_EQ((*faces_it).second[0]._x, 20);
    EXPECT_EQ((*faces_it).second[0]._y, 10);
    EXPECT_EQ((*faces_it).second[1]._x, 30);
    EXPECT_EQ((*faces_it).second[1]._y, 10);
    EXPECT_EQ((*faces_it).second[2]._x, 30);
    EXPECT_EQ((*faces_it).second[2]._y, 0);
    EXPECT_EQ((*faces_it).second[3]._x, 20);
    EXPECT_EQ((*faces_it).second[3]._y, 0);
    faces_it ++;
    // Coordinates of the second rectangle vertices: (0,10), (20,10), (20,0), (0,0)
    EXPECT_EQ((*faces_it).second[0]._x, 0);
    EXPECT_EQ((*faces_it).second[0]._y, 10);
    EXPECT_EQ((*faces_it).second[1]._x, 20);
    EXPECT_EQ((*faces_it).second[1]._y, 10);
    EXPECT_EQ((*faces_it).second[2]._x, 20);
    EXPECT_EQ((*faces_it).second[2]._y, 0);
    EXPECT_EQ((*faces_it).second[3]._x, 0);
    EXPECT_EQ((*faces_it).second[3]._y, 0);
}

TEST(GroundContourTest, two_floors_building)
{
     LPTYProjet project = tympan::load_project(tympan::path_to_test_data(
            "projects-panel/site_with_two_floors_building.xml").c_str());

    // Get a pointer to the TYSiteNode.
    LPTYSiteNode site_ptr = project->getSite();

    // Retrieve ground contour
    map<TYUUID, TYTabPoint3D> groundbased_faces;
    site_ptr->getFacesOnGround(groundbased_faces);
    map<TYUUID, TYTabPoint3D>::iterator faces_it = groundbased_faces.begin();

    // We are expecting to get only the contour of the ground floor
    EXPECT_EQ(groundbased_faces.size(), 1);
    // Coordinates of the ground floor vertices: (0,10), (20,10), (20,0), (0,0)
    EXPECT_EQ((*faces_it).second[0]._x, 0);
    EXPECT_EQ((*faces_it).second[0]._y, 10);
    EXPECT_EQ((*faces_it).second[1]._x, 20);
    EXPECT_EQ((*faces_it).second[1]._y, 10);
    EXPECT_EQ((*faces_it).second[2]._x, 20);
    EXPECT_EQ((*faces_it).second[2]._y, 0);
    EXPECT_EQ((*faces_it).second[3]._x, 0);
    EXPECT_EQ((*faces_it).second[3]._y, 0);
}

TEST(GroundContourTest, two_screens_multiple_segment)
// Functional test for the join of polygons when an infrastructure volume has
// more that one face on ground (a screen with several segments, here).
{
    LPTYProjet project = tympan::load_project(tympan::path_to_test_data(
                "projects-panel/20_PROJET_Site_emprise_seule_avec_un_ecran_plusieurs_segments.xml").c_str());

    // Get a pointer to the TYSiteNode.
    LPTYSiteNode site_ptr = project->getSite();

    // Retrieve ground contour
    map<TYUUID, TYTabPoint3D> groundbased_faces;
    site_ptr->getFacesOnGround(groundbased_faces);
    map<TYUUID, TYTabPoint3D>::iterator faces_it = groundbased_faces.begin();

    // We are expecting to get only the contour of the screen as *one* face,
    EXPECT_EQ(groundbased_faces.size(), 1);
    // with ten points, as there are four segments in the screen.
    TYTabPoint3D points = faces_it->second;
    EXPECT_EQ(points.size(), 10);
    EXPECT_NEAR(points[0]._x, -136, 1.e-2);
    EXPECT_NEAR(points[0]._y, -67.15, 1.e-2);
    EXPECT_NEAR(points[1]._x, -37.85, 1.e-2);
    EXPECT_NEAR(points[1]._y, -67.15, 1.e-2);
    EXPECT_NEAR(points[2]._x, -37.85, 1.e-2);
    EXPECT_NEAR(points[2]._y, 11.8848, 1.e-2);
    EXPECT_NEAR(points[3]._x, 52.1219, 1.e-2);
    EXPECT_NEAR(points[3]._y, 35.8773, 1.e-2);
    EXPECT_NEAR(points[4]._x, 72.1447, 1.e-2);
    EXPECT_NEAR(points[4]._y, 108.96, 1.e-2);
    EXPECT_NEAR(points[5]._x, 71.8553, 1.e-2);
    EXPECT_NEAR(points[5]._y, 109.04, 1.e-2);
    EXPECT_NEAR(points[6]._x, 51.8781, 1.e-2);
    EXPECT_NEAR(points[6]._y, 36.1227, 1.e-2);
    EXPECT_NEAR(points[7]._x, -38.15, 1.e-2);
    EXPECT_NEAR(points[7]._y, 12.1152, 1.e-2);
    EXPECT_NEAR(points[8]._x, -38.15, 1.e-2);
    EXPECT_NEAR(points[8]._y, -66.85, 1.e-2);
    EXPECT_NEAR(points[9]._x, -136, 1.e-2);
    EXPECT_NEAR(points[9]._y, -66.85, 1.e-2);
}
