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


// TODO Factor out the fixture into ProjectLoader
TEST_F(BuildingFromSiteFixture, check_triangles)
{
    load_file("../data/tiny_site.xml");

    SolverModel model;
    SolverDataModelBuilder builder(model);
    builder.walkTroughtSite(project->getSite());
    model.export_triangles_soup("export_check_triangles");
}
