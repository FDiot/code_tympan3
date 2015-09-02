/**
 * \file test_ground_contour.cpp
 * \test test ground contour computation for some sites read from XML projects
 *
 *  Created on: july 2014
 *      Author: Laura Médioni <laura.medioni@logilab.fr>
 */

#include "gtest/gtest.h"
#include "tests/TympanTestsConfig.h"
#include "Tympan/models/business/TYProjet.h"
#include "Tympan/models/business/infrastructure/TYSiteNode.h"
#include "Tympan/models/business/xml_project_util.h"

TEST(GroundContourTest, single_machine_on_ground)
{
    LPTYProjet project = tympan::load_project(tympan::path_to_test_data(
            "projects-panel/site_with_single_machine.xml").c_str());

    // Get a pointer to the TYSiteNode.
    LPTYSiteNode site_ptr = project->getSite();

    // Retrieve ground contour
    map<TYUUID, deque<TYTabPoint3D>> groundbased_faces;
    site_ptr->getFacesOnGround(groundbased_faces);

    // Check values:
    // We must get a single contour (TYTabPoint3D) containing the coordinates
    // of the basis of the machine.
    EXPECT_EQ(groundbased_faces.size(), 1);
    // Volume has only one contour on ground.
    EXPECT_EQ((groundbased_faces.begin())->second.size(), 1);  // drop parentheses
    // Rectangle
    TYTabPoint3D contour = (groundbased_faces.begin())->second[0];
    EXPECT_EQ(contour.size(), 4);
    // Coordinates of the rectangle vertices: (0,0), (20,0), (20,10), (0,10)
    EXPECT_EQ(contour[0]._x, 0);
    EXPECT_EQ(contour[0]._y, 0);
    EXPECT_EQ(contour[1]._x, 20);
    EXPECT_EQ(contour[1]._y, 0);
    EXPECT_EQ(contour[2]._x, 20);
    EXPECT_EQ(contour[2]._y, 10);
    EXPECT_EQ(contour[3]._x, 0);
    EXPECT_EQ(contour[3]._y, 10);
}

TEST(GroundContourTest, single_flying_building)
{
    LPTYProjet project = tympan::load_project(tympan::path_to_test_data(
            "projects-panel/site_with_single_flying_building.xml").c_str());

    // Get a pointer to the TYSiteNode.
    LPTYSiteNode site_ptr = project->getSite();

    // Retrieve ground contour
    map<TYUUID, deque<TYTabPoint3D>> groundbased_faces;
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
    map<TYUUID, deque<TYTabPoint3D>> groundbased_faces;
    map<TYUUID, deque<TYTabPoint3D>>::iterator faces_it;
    deque<TYTabPoint3D> contours;
    site_ptr->getFacesOnGround(groundbased_faces);

    EXPECT_EQ(groundbased_faces.size(), 2);
    faces_it = groundbased_faces.begin();
    contours = faces_it->second;
    EXPECT_EQ(contours.size(), 1);
    // Coordinates of the first rectangle vertices: (20,10), (30,10), (30,0), (20,0)
    EXPECT_EQ(contours[0][0]._x, 20);
    EXPECT_EQ(contours[0][0]._y, 10);
    EXPECT_EQ(contours[0][1]._x, 30);
    EXPECT_EQ(contours[0][1]._y, 10);
    EXPECT_EQ(contours[0][2]._x, 30);
    EXPECT_EQ(contours[0][2]._y, 0);
    EXPECT_EQ(contours[0][3]._x, 20);
    EXPECT_EQ(contours[0][3]._y, 0);
    faces_it ++;
    contours = faces_it->second;
    EXPECT_EQ(contours.size(), 1);
    // Coordinates of the second rectangle vertices: (0,10), (20,10), (20,0), (0,0)
    EXPECT_EQ(contours[0][0]._x, 0);
    EXPECT_EQ(contours[0][0]._y, 10);
    EXPECT_EQ(contours[0][1]._x, 20);
    EXPECT_EQ(contours[0][1]._y, 10);
    EXPECT_EQ(contours[0][2]._x, 20);
    EXPECT_EQ(contours[0][2]._y, 0);
    EXPECT_EQ(contours[0][3]._x, 0);
    EXPECT_EQ(contours[0][3]._y, 0);
}

TEST(GroundContourTest, two_floors_building)
{
     LPTYProjet project = tympan::load_project(tympan::path_to_test_data(
            "projects-panel/site_with_two_floors_building.xml").c_str());

    // Get a pointer to the TYSiteNode.
    LPTYSiteNode site_ptr = project->getSite();

    // Retrieve ground contour
    map<TYUUID, deque<TYTabPoint3D>> groundbased_faces;
    site_ptr->getFacesOnGround(groundbased_faces);
    deque<TYTabPoint3D> contours;
    map<TYUUID, deque<TYTabPoint3D>>::iterator faces_it = groundbased_faces.begin();

    // We are expecting to get only the contour of the ground floor
    EXPECT_EQ(groundbased_faces.size(), 1);
    contours = faces_it->second;
    EXPECT_EQ(contours.size(), 1);
    // Coordinates of the ground floor vertices: (0,10), (20,10), (20,0), (0,0)
    EXPECT_EQ(contours[0][0]._x, 0);
    EXPECT_EQ(contours[0][0]._y, 10);
    EXPECT_EQ(contours[0][1]._x, 20);
    EXPECT_EQ(contours[0][1]._y, 10);
    EXPECT_EQ(contours[0][2]._x, 20);
    EXPECT_EQ(contours[0][2]._y, 0);
    EXPECT_EQ(contours[0][3]._x, 0);
    EXPECT_EQ(contours[0][3]._y, 0);
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
    map<TYUUID, deque<TYTabPoint3D>> groundbased_faces;
    site_ptr->getFacesOnGround(groundbased_faces);
    map<TYUUID, deque<TYTabPoint3D>>::iterator faces_it = groundbased_faces.begin();

    // Only one volume (the screen),
    EXPECT_EQ(groundbased_faces.size(), 1);
    // with four ground contours.
    deque<TYTabPoint3D> contours = faces_it->second;
    TYTabPoint3D points;
    EXPECT_EQ(contours.size(), 4);
    points = contours[0];
    EXPECT_EQ(points.size(), 4);
    EXPECT_NEAR(points[0]._x, 51.8781, 1.e-2);
    EXPECT_NEAR(points[0]._y, 36.1227, 1.e-2);
    EXPECT_NEAR(points[1]._x, 71.8553, 1.e-2);
    EXPECT_NEAR(points[1]._y, 109.04, 1.e-2);
    EXPECT_NEAR(points[2]._x, 72.1447, 1.e-2);
    EXPECT_NEAR(points[2]._y, 108.96, 1.e-2);
    EXPECT_NEAR(points[3]._x, 52.1219, 1.e-2);
    EXPECT_NEAR(points[3]._y, 35.8773, 1.e-2);
    points = contours[1];
    EXPECT_EQ(points.size(), 4);
    EXPECT_NEAR(points[0]._x, -38.15, 1.e-2);
    EXPECT_NEAR(points[0]._y, 12.1152, 1.e-2);
    EXPECT_NEAR(points[1]._x, 51.8781, 1.e-2);
    EXPECT_NEAR(points[1]._y, 36.1227, 1.e-2);
    EXPECT_NEAR(points[2]._x, 52.1219, 1.e-2);
    EXPECT_NEAR(points[2]._y, 35.8773, 1.e-2);
    EXPECT_NEAR(points[3]._x, -37.85, 1.e-2);
    EXPECT_NEAR(points[3]._y, 11.8848, 1.e-2);
    points = contours[2];
    EXPECT_EQ(points.size(), 4);
    EXPECT_NEAR(points[0]._x, -38.15, 1.e-2);
    EXPECT_NEAR(points[0]._y, -66.85, 1.e-2);
    EXPECT_NEAR(points[1]._x, -38.15, 1.e-2);
    EXPECT_NEAR(points[1]._y, 12.1152, 1.e-2);
    EXPECT_NEAR(points[2]._x, -37.85, 1.e-2);
    EXPECT_NEAR(points[2]._y, 11.8848, 1.e-2);
    EXPECT_NEAR(points[3]._x, -37.85, 1.e-2);
    EXPECT_NEAR(points[3]._y, -67.15, 1.e-2);
    points = contours[3];
    EXPECT_EQ(points.size(), 4);
    EXPECT_NEAR(points[0]._x, -136, 1.e-2);
    EXPECT_NEAR(points[0]._y, -66.85, 1.e-2);
    EXPECT_NEAR(points[1]._x, -38.15, 1.e-2);
    EXPECT_NEAR(points[1]._y, -66.85, 1.e-2);
    EXPECT_NEAR(points[2]._x, -37.85, 1.e-2);
    EXPECT_NEAR(points[2]._y, -67.15, 1.e-2);
    EXPECT_NEAR(points[3]._x, -136, 1.e-2);
    EXPECT_NEAR(points[3]._y, -67.15, 1.e-2);
}

TEST(GroundContourTest, single_traversing_cylinder)
// This functional test has a cylinder with a low face at a negative altitude.
{
    LPTYProjet project = tympan::load_project(tympan::path_to_test_data(
            "projects-panel/cylindre.xml").c_str());

    // Get a pointer to the TYSiteNode.
    LPTYSiteNode site_ptr = project->getSite();

    // Retrieve ground contour
    map<TYUUID, TYTabPoint3D> groundbased_faces;
    site_ptr->getFacesOnGround(groundbased_faces);
    map<TYUUID, TYTabPoint3D>::iterator faces_it = groundbased_faces.begin();

    // Get one face, with a negatize altitude.
    EXPECT_EQ(groundbased_faces.size(), 1);
    EXPECT_EQ((*faces_it).second[0]._z, -9);
}
