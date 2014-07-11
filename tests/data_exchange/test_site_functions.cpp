/**
 * \file test_site_functions.cpp
 * \test XXX actually test some numbers from a XML loaded site
 *
 *  Created on: 8 nov. 2012
 *      Author: Damien Garaud <damien.garaud@logilab.fr>
 */

#include <iostream>
#include <cstring>

#include "gtest/gtest.h"

#include <QString>

#include "Tympan/MetierSolver/DataManagerCore/TYXMLManager.h"

#include "Tympan/models/business/TYProjet.h"
#include "Tympan/models/business/infrastructure/TYSiteNode.h"

#include "test_utils/ProjectLoader.hpp"

using namespace tympan;

using std::cout;
using std::cerr;
using std::endl;

// Check the size of a few elements such as the number of points, surfaces,
// etc. from site described by a XML file.
TEST_F(BuildingFromSiteFixture, check_size)
{

    load_file("../data/tiny_site.xml");

    // Get a pointer to the TYSiteNode.
    LPTYSiteNode site_ptr = project->getSite();

    // Get the number of points.
    unsigned int points_number = site_ptr->getTopographie()->number_of_vertices();
    EXPECT_EQ(14, points_number);

    // Get the number of surfaces ('ecran' is obsoleted).
    TYTabAcousticSurfaceGeoNode tab_faces;
    unsigned int nb_faces_infra = 0;
    std::vector<bool> is_face_ecran_index;
    site_ptr->getListFaces(tab_faces, nb_faces_infra, is_face_ecran_index);
    unsigned int surfaces_number = tab_faces.size();
    EXPECT_EQ(28, surfaces_number);
}
