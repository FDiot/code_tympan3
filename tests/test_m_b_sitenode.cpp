/**
 * \file test_ground_contour.cpp
 * \test test ground contour computation for some sites read from XML projects
 *
 *  Created on: july 2014
 *      Author: Laura Médioni <laura.medioni@logilab.fr>
 */

#include "gtest/gtest.h"
#include "tests/TympanTestsConfig.hpp"
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

