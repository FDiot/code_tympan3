/**
 * \file test_build_from_site.cpp
 * \test Try to build some solver data model entities from a TYSite.
 *
 *  Created on: 8 nov. 2012
 *      Author: Damien Garayd <damien.garaud@logilab.fr>
 */

#include <iostream>
#include <cstring>

#include "gtest/gtest.h"

#include <QString>

#include "Tympan/MetierSolver/DataManagerCore/TYXMLManager.h"

#include "Tympan/MetierSolver/DataManagerMetier/Commun/TYProjet.h"
#include "Tympan/MetierSolver/DataManagerMetier/Site/TYSiteNode.h"

#include "Tympan/MetierSolver/DataManagerMetier/site_functions.hpp"

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
    unsigned int points_number = total_point_number(site_ptr);
    EXPECT_EQ(14, points_number);

    // Get the number of surfaces ('ecran' is obsoleted).
    unsigned int surfaces_number = get_acoustic_surface_number(site_ptr);
    EXPECT_EQ(28, surfaces_number);
}
