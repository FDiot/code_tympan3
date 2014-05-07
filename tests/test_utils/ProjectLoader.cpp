/**
 * @file ProjectLoader.cpp
 *
 * @brief Implementation of utilities to load and access a project for testing.
 *
 * Inspired by \ref Tympan/Appli/TympanConsole/TYConsole.cpp
 */


#include "ProjectLoader.hpp"

#include <boost/foreach.hpp>

namespace tympan
{

void assert_loaded_project(const LPTYProjet& project)
{
    ASSERT_TRUE(project);
    LPTYSiteNode site = project->getSite();
    ASSERT_TRUE(site) << "Invalid root site";
    ASSERT_EQ(0, site->getListSiteNode().size()) << "Sub-sites are not handled.";
}

void assert_current_computation_project(const LPTYProjet& project)
{
    EXPECT_TRUE(project->getListCalcul().size() >= 1)
            << "Should have at least one computation";
    LPTYCalcul current_calcul = project->getCurrentCalcul();
    EXPECT_TRUE(current_calcul);
}


void get_altimetry_numbers(LPTYProjet project,
                           unsigned& nb_triangles, unsigned& nb_vertices,
                           unsigned& nb_edges, unsigned& nb_constrained_edges)
{

    LPTYTopographie topo = project->getSite()->getTopographie();

    nb_triangles = topo->number_of_faces();
    nb_vertices = topo->number_of_vertices();
    std::pair<unsigned, unsigned> counts = topo->getAltimetryBuilder().count_edges();
    nb_edges = counts.first;
    nb_constrained_edges = counts.second;
}



} /* namespace tympan */
