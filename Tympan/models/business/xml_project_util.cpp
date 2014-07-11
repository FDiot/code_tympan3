/**
 * @file xml_project_utility.cpp
 *
 * @brief Utility to load and save a project (implementation)
 *
 */


#include "xml_project_util.h"

#include <boost/foreach.hpp>

#include "Tympan/core/logging.h"
#include "Tympan/models/business/TYXMLManager.h"
#include "Tympan/core/exceptions.h"
#include "Tympan/Config.h"

namespace tympan
{

LPTYProjet load_project(const char* filename)
{
    TYXMLManager xmlManager;
    LPTYElementArray elements;
    LPTYProjet project;

    // Load XML file
    int is_loaded = xmlManager.load(filename, elements);
    if (is_loaded != 1)
    {
        throw tympan::invalid_data("XML Project file could not be loaded ")
            << tympan_source_loc
            << boost::errinfo_file_name(filename);
    }

    // Retrieve project
    BOOST_FOREACH(LPTYElement & elt, elements)
    {
        if (std::strcmp(elt->getClassName(), "TYProjet") == 0)
        {
            project = TYProjet::safeDownCast(elt);
            break;
        }
    }

    if (!project)
    {
        throw tympan::invalid_data("No project was found in the XML file")
            << boost::errinfo_file_name(filename);
    }

    // Update site node
    project->getSite()->update();
    return project;
}

void save_project(const char* filename, const LPTYProjet& project)
{
    if (!project)
    {
        throw tympan::invalid_data("A NULL project can not be saved");
    }

    TYXMLManager xmlManager;
    xmlManager.createDoc(TY_PRODUCT_XMLTAG_, TY_PRODUCT_VERSION_);
    xmlManager.addElement(project);
    if (xmlManager.save(filename) != 0)
    {
        throw tympan::invalid_data("Project could not be saved")
            << boost::errinfo_file_name(filename);
    }
}

} /* namespace tympan */


