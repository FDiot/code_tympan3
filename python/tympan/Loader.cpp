/**
 * @file Loader.cpp
 *
 * @brief Implementation of utility to load a solver
 *
 */


#include "python/include/Loader.hpp"

#include <boost/foreach.hpp>

#include "Tympan/MetierSolver/DataManagerCore/TYPluginManager.h"
#include "Tympan/MetierSolver/DataManagerCore/TYSolverInterface.h"
namespace tympan
{

void load_solver(const char *foldername, TYCalcul *calcul)
{
    LPTYPluginManager plugin_manager = TYPluginManager::get();
    plugin_manager->unloadPlugins();
    plugin_manager->loadPlugins(foldername, false);
    plugin_manager->setCurrent("DefaultSolver");
    calcul->setPlugin(plugin_manager->getPlugin(plugin_manager->getCurrent()));
}


} /* namespace tympan */


