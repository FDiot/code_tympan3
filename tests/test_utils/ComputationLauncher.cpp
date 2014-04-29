//! \file ComputationLauncher.cpp


#include "ComputationLauncher.hpp"
#include "Tympan/MetierSolver/DataManagerMetier/xml_project_util.hpp"

namespace tympan
{


ComputationLauncher::ComputationLauncher(const char* filename) :
    _filename(filename)
{
    _project = load_project(filename);
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

void ComputationLauncher::setCurrentComputation(size_t index)
{
    ASSERT_LT(index, getComputationList().size());
    _current_computation = _project->getListCalcul()[0];
}


} /* namespace tympan */
