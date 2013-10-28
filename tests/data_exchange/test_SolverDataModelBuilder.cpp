/**
 * \file test_SolverDataModelBuilder.cpp
 * \test Try to build some solver data model entities from a TYSite.
 */

#include <iostream>

#include "gtest/gtest.h"

#include <QString>

#include "Tympan/MetierSolver/DataManagerCore/TYElementCollection.h"
#include "Tympan/MetierSolver/DataManagerCore/TYXMLManager.h"

#include "Tympan/MetierSolver/DataManagerMetier/Commun/TYProjet.h"
#include "Tympan/MetierSolver/DataManagerMetier/Site/TYSiteNode.h"

#include "Tympan/MetierSolver/DataExchange/SolverDataModelBuilder.hpp"

#include "test_utils/ProjectLoader.hpp"

using namespace tympan;

using std::clog;
using std::endl;


TEST_F(BuildingFromSiteFixture, export_triangles_soup)
{
    load_file("../data/tiny_site.xml");

    SolverModel model;
    SolverDataModelBuilder builder(model);
    builder.walkTroughtSite(project->getSite());
    model.export_triangles_soup("export_check_triangles");
}

TEST_F(BuildingFromSiteFixture, check_base)
{
    load_file("../data/solver_export/base.xml");

    SolverModel model;
    SolverDataModelBuilder builder(model);
    builder.walkTroughtSite(project->getSite());

    EXPECT_EQ(1, model.num_materials());
    // FIXME the default material is replicated once per triangle.

    EXPECT_EQ(6, model.num_points());

    EXPECT_EQ(4, model.num_triangles());
    // FIXME a triangle is exported outside the land take. (Ticket #1471928)

    // TODO : how to test the altitude of a point ? or access a
    // triangle at some place ?

    // TODO Work In Progress

    // Note spatial accelerating structure in solver data model ? :
    // cf. http://www.cs.ubc.ca/research/flann/ or check CGAL meshes...
}


// TODO TEST_F(BuildingFromSiteFixture, check_building)

// TODO TEST_F(BuildingFromSiteFixture, check_ground)
