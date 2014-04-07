/**
 * @file xml_project_utility.cpp
 *
 * @brief Utility to load and save a project (implementation)
 *
 */


#include "xml_project_util.hpp"

#include <boost/foreach.hpp>

#include "Tympan/Tools/OMessageManager.h"
#include "Tympan/MetierSolver/DataManagerCore/TYXMLManager.h"
#include "Tympan/Config.h"

namespace tympan
{

bool load_project(const char* filename, LPTYProjet& project)
{
    TYXMLManager xmlManager;
    LPTYElementArray elements;

    // we need an empty object
    if (!project)
    {
        // Load XML file.
        int is_loaded = xmlManager.load(filename, elements);

        if (is_loaded == 1)
        {
            // Retrieve project
            BOOST_FOREACH(LPTYElement & elt, elements)
            {
                if (std::strcmp(elt->getClassName(), "TYProjet") == 0)
                {
                    project = TYProjet::safeDownCast(elt);
                    break;
                }
            }
            if (project)
            {
                // Update site node
                project->getSite()->update();
                return true;
            }
            OMessageManager::get()->info(
                "[XML project util.] No project was found in the XML file\n");
        }
        OMessageManager::get()->info(
            "[XML project util.] Project file could not be loaded\n");
    }
    else
    {
        OMessageManager::get()->info(
            "[XML project util.] Project must be NULL to be constructed from a XML file\n");
    }
    return false;
}

bool save_project(const char* filename, const LPTYProjet& project)
{
    if (!project)
    {
        OMessageManager::get()->info("[XML project util.] Project is NULL\n");
        return false;
    }

    TYXMLManager xmlManagerOutput;

    int doc_created = xmlManagerOutput.createDoc(TY_PRODUCT_XMLTAG_,
            TY_PRODUCT_VERSION_);
    if (doc_created)
    {
        xmlManagerOutput.addElement(project);
        if (xmlManagerOutput.save(filename) == 0)
        {
            return true;
        }
        OMessageManager::get()->info(
            "[XML project util.] Project could not be saved\n");
    }
    return false;
}

} /* namespace tympan */


