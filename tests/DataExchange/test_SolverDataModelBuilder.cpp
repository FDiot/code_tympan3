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



/* This Fixture loads a project from an XML project file */
class BuildingFromSiteFixture: public ::testing::Test
{
public:
    // static void SetUpTestCase()
    virtual void SetUp()
    {
    	const char filename[] = "../data/tiny_site.xml";
    	load_project_from_file(filename, project);
    	assert_loaded_project(project);
    }

    LPTYProjet project;
};

// TODO FActor out the fixture into ProjectLoader
TEST_F(BuildingFromSiteFixture, check_size)
{
	SolverModel model;
	SolverDataModelBuilder builder(model);
	builder.walkTroughtSite(project->getSite());
}
