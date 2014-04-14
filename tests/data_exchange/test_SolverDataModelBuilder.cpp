/**
 * \file test_SolverDataModelBuilder.cpp
 * \test Try to build some solver data model entities from a TYSite.
 */

#include <iostream>

#include "gtest/gtest.h"

#include <QString>

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

    AcousticProblemModel model;
    SolverDataModelBuilder builder(model);
    builder.walkTroughtSite(project->getSite());
    model.export_triangles_soup("export_check_triangles");
}

// This test is temporaryly disable due to hard to specifiy 'good' results
TEST_F(BuildingFromSiteFixture, DISABLED_check_base)
{
    load_file("../data/solver_export/base.xml");

    AcousticProblemModel model;
    SolverDataModelBuilder builder(model);
    builder.walkTroughtSite(project->getSite());

    EXPECT_EQ(1, model.num_materials());
    // FIXME the default material is replicated once per triangle.

    EXPECT_EQ(6, model.num_points());

    EXPECT_EQ(4, model.num_triangles());
    // FIXME a triangle is exported outside the land take. (Ticket #1471928)

    // TODO : how to test the altitude of a point ? or access a
    // triangle at some place ?
    // TODO to be completed: cf. ticket #1468184

    // Note spatial accelerating structure in solver data model ? :
    // cf. http://www.cs.ubc.ca/research/flann/ or check CGAL meshes...
}

// This test uses expected bad values profided by the current implementation
TEST_F(BuildingFromSiteFixture, check_base_known_bad_results)
{
    load_file("../data/solver_export/base.xml");

    AcousticProblemModel model;
    SolverDataModelBuilder builder(model);
    builder.walkTroughtSite(project->getSite());

    EXPECT_EQ(5, model.num_materials());
    EXPECT_EQ(6, model.num_points());
    EXPECT_EQ(5, model.num_triangles());
}

TEST_F(BuildingFromSiteFixture, DISABLED_check_ground)
{
    load_file("../data/solver_export/ground_materials.xml");

    AcousticProblemModel model;
    SolverDataModelBuilder builder(model);
    builder.walkTroughtSite(project->getSite());

    EXPECT_EQ(3, model.num_materials());
    // FIXME the default material is replicated once per triangle.

    // TODO to be completed: cf. ticket #1468184
}

// TODO cf. ticket #1474395
// TEST_F(BuildingFromSiteFixture, check_building)
