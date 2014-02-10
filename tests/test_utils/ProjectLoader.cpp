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

void load_project_from_file(const char * filename, LPTYProjet& project)
{
	ASSERT_FALSE(project);
    QString xml_filename;
    TYXMLManager xmlManager;
    TYElementCollection elements;

    xml_filename = QString(filename);
    // Load XML file.
    int is_loaded = xmlManager.load(xml_filename, elements);
    ASSERT_EQ(is_loaded, 1);

    // Retrieve project.
    BOOST_FOREACH(LPTYElement& elt, elements)
    {
        if (std::strcmp(elt->getClassName(), "TYProjet") == 0)
        {
        	project = TYProjet::safeDownCast(elt);
            break;
        }
    }
    // Project should no longer be NULL
    ASSERT_TRUE(project);
    // Update site node.
    project->getSite()->update();
}

void assert_loaded_project(const LPTYProjet& project)
{
	ASSERT_TRUE(project);
	LPTYSiteNode site = project->getSite();
	ASSERT_TRUE(site) << "Invalid root site";
	ASSERT_EQ(0, site->getListSiteNode().size()) << "Sub-sites are not handled.";
}
} /* namespace tympan */
