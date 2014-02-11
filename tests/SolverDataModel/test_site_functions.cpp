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

#include "Tympan/MetierSolver/DataManagerCore/TYElementCollection.h"
#include "Tympan/MetierSolver/DataManagerCore/TYXMLManager.h"

#include "Tympan/MetierSolver/DataManagerMetier/Commun/TYProjet.h"
#include "Tympan/MetierSolver/DataManagerMetier/Site/TYSiteNode.h"

#include "Tympan/MetierSolver/SolverDataModel/site_functions.hpp"


using namespace tympan;

using std::cout;
using std::cerr;
using std::endl;

// Fixture which loads a 'TYSite' from a XML file.
// XXX \todo Do not load the site for every test.
// To use this fixture, use 'TEST_F' (instead of 'TEST').
class BuildingFromSiteFixture: public ::testing::Test
{
public:
    // static void SetUpTestCase()
    virtual void SetUp()
    {
        xml_filename = QString("tiny_site.xml");
        // Load XML file.
        int is_loaded = xmlManager.load(xml_filename, elements);
        EXPECT_EQ(is_loaded, 1);

        // Retrieve project.
        unsigned int i;
        for (i = 0; i < elements.size(); ++i)
        {
            LPTYElement elt = elements[i];
            if (std::strcmp(elt->getClassName(), "TYProjet") == 0)
            {
                project = static_cast<TYProjet*>(elt.getRealPointer());
                break;
            }
        }
        // Project should exist.
        EXPECT_TRUE(project);
        // Update site node.
        project->getSite()->update();
    }

    QString xml_filename;
    TYXMLManager xmlManager;
    TYElementCollection elements;
    LPTYProjet project;
};

// Check the size of a few elements such as the number of points, surfaces,
// etc. from site described by a XML file.
TEST_F(BuildingFromSiteFixture, check_size)
{
    // Get a pointer to the TYSiteNode.
    LPTYSiteNode site_ptr = project->getSite();

    // Get the number of points.
    unsigned int points_number = total_point_number(site_ptr);
    EXPECT_EQ(points_number, 17);

    // Get the number of surfaces (without 'ecran' by default).
    unsigned int surfaces_number = get_acoustic_surface_number(site_ptr);
    EXPECT_EQ(surfaces_number, 22);
}
