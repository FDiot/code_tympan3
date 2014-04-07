//! \file ComputationLauncher.cpp


#include "ComputationLauncher.hpp"
#include "Tympan/MetierSolver/DataManagerMetier/xml_project_util.hpp"

namespace tympan
{


ComputationLauncher::ComputationLauncher(const char* filename) :
    _filename(filename)
{
    load_project(filename, _project);
    setCurrentComputation(0);
}


void ComputationLauncher::launchDefault(const char* directory) const
{
    // Singleton.
    LPTYPluginManager plugin_manager = TYPluginManager::get();
    plugin_manager->unloadPlugins();
    const QString name("DefaultSolver");
    bool with_gui = false;
    bool loaded =
        plugin_manager->loadPlugins(QString(directory), with_gui);
    ASSERT_TRUE(loaded) << "Can load plugins with DefaultSolver.";
    plugin_manager->setCurrent(name);
    getCurrentComputation()->go();
}


void ComputationLauncher::launchSimpleRay(const char* directory) const
{
    // Singleton.
    LPTYPluginManager plugin_manager = TYPluginManager::get();
    plugin_manager->unloadPlugins();
    const QString name("SimpleRaySolver");
    bool with_gui = false;
    bool loaded =
        plugin_manager->loadPlugins(QString(directory), with_gui);
    ASSERT_TRUE(plugin_manager->exist(name))
            << "SimpleRaySolver not present in the directory.";
    _current_computation->setSolverId(QString(SIMPLERAY_SOLVER_UUID));
    plugin_manager->setCurrent(name);

    ASSERT_TRUE(loaded) << "Can load plugins with SimpleRaySolver.";
    getCurrentComputation()->go();
}


void ComputationLauncher::setCurrentComputation(size_t index)
{
    ASSERT_LT(index, getComputationList().size());
    _current_computation = _project->getListCalcul()[0];
}


} /* namespace tympan */
