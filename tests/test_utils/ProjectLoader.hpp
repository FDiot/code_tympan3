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

#include "Tympan/MetierSolver/DataManagerCore/TYElementCollection.h"
#include "Tympan/MetierSolver/DataManagerCore/TYXMLManager.h"

#include "Tympan/MetierSolver/DataManagerMetier/Commun/TYProjet.h"
#include "Tympan/MetierSolver/DataManagerMetier/Site/TYSiteNode.h"


namespace tympan
{

/**
 * @brief load an XML project file
 * @param filename the filename of the project to be open.
 * @param project Output argument : this pointer (expected to be NULL) will be
 *                make to refer to the loaded \c TYProjet.
 */
void load_project_from_file(const char* filename, LPTYProjet& project);

/**
 * @brief Assert some assumptions on a loaded project (like no subsites...)
 * @param project the project to be asserted.
 */
void assert_loaded_project(const LPTYProjet& project);

/**
 * @brief Assert some assumptions on the current computation in a loaded project
 * @param project the project to be asserted.
 */
void assert_current_computation_project(const LPTYProjet& project);

/**
 * @brief Extract some caracteristic counts for the altimetry
 * @param project the TYMPAN project
 * @param nb_triangles Output arg for the number of triangles
 * @param nb_vertices Output arg for the number of vertices
 * @param nb_edges Output arg for the total number of edges
 * @param nb_constrained_edges utput arg for the number of constrained edges
 */
void get_altimetry_numbers(LPTYProjet project,
                           unsigned& nb_triangles, unsigned& b_vertices,
                           unsigned& nb_edges, unsigned& nb_constrained_edges);


} /* namespace tympan */

/* This Fixture loads a project from an XML project file */
class BuildingFromSiteFixture: public ::testing::Test
{

public:
    // static void SetUpTestCase()

    // Load the project file `filename` into the `project` member
    virtual void load_file(const char filename[])
    {
        using namespace tympan;
        project = LPTYProjet(); // a way to explicitely release the smart pointer
        load_project_from_file(filename, project); // This output to `project`
        assert_loaded_project(project);
    }

    // virtual void SetUp() {}

    LPTYProjet project;
};

#endif /* TYMPAN__PROJECTLOADER_HPP__INCLUDED */
