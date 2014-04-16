/**
 * @file Loader.cpp
 *
 * @brief Implementation of utilities to load a project and a solver.
 *
 * Inspired by \ref Tympan/Appli/TympanConsole/TYConsole.cpp
 */


#include "Loader.hpp"

#include <boost/foreach.hpp>

#include "Tympan/MetierSolver/DataManagerCore/TYXMLManager.h"
#include "Tympan/MetierSolver/DataManagerCore/TYPluginManager.h"
#include "Tympan/Config.h"

namespace tympan
{

void load_project_from_file(const char* filename, LPTYProjet& project)
{
    QString xml_filename;
    TYXMLManager xmlManager;
    LPTYElementArray elements;

    // Load XML file.
    int is_loaded = xmlManager.load(filename, elements);

    if (is_loaded)
    {
        // Retrieve project.
        BOOST_FOREACH(LPTYElement & elt, elements)
        {
            if (std::strcmp(elt->getClassName(), "TYProjet") == 0)
            {
                project = TYProjet::safeDownCast(elt);
                break;
            }
        }
        // Update site node.
        project->getSite()->update();
    }
}

void save_project(const char* filename, const LPTYProjet& project)
{
    TYXMLManager xmlManagerOutput;
    xmlManagerOutput.createDoc(TY_PRODUCT_XMLTAG_, TY_PRODUCT_VERSION_);
    xmlManagerOutput.addElement(project);
    xmlManagerOutput.save(filename);
}

void load_solver(const char *foldername, TYCalcul *calcul)
{
    LPTYPluginManager plugin_manager = TYPluginManager::get();
    plugin_manager->unloadPlugins();
    plugin_manager->loadPlugins(foldername, false);
    plugin_manager->setCurrent("DefaultSolver");
    calcul->setSolverId(plugin_manager->getCurrent());
}


} /* namespace tympan */
