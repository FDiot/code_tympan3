/**
 * @file ProjectLoader.hpp
 *
 * @brief Utilities to load and access a project for testing.
 *
 */

#ifndef TYMPAN__PROJECTLOADER_HPP__INCLUDED
#define TYMPAN__PROJECTLOADER_HPP__INCLUDED

#include <cstring>
#include <QString>

#include "gtest/gtest.h"

#include "Tympan/models/business/xml_project_util.h"
#include "Tympan/models/business/TYProjet.h"
#include "Tympan/models/business/infrastructure/TYSiteNode.h"


/* This Fixture loads a project from an XML project file */
class BuildingFromSiteFixture: public ::testing::Test
{

public:
    // static void SetUpTestCase()

    // Load the project file `filename` into the `project` member
    virtual void load_file(const char filename[])
    {
        using namespace tympan;
        project = load_project(filename); // This output to `project`
        LPTYSiteNode site = project->getSite();
        ASSERT_TRUE(site) << "Invalid root site";
    }

    // virtual void SetUp() {}

    LPTYProjet project;
};

#endif /* TYMPAN__PROJECTLOADER_HPP__INCLUDED */
