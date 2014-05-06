//! \test test_computation_launcher.cpp

#include <iostream>

#include "gtest/gtest.h"

#include "test_utils/ComputationLauncher.hpp"

#include "Tympan/MetierSolver/DataManagerCore/TYPluginManager.h"

#include "TympanTestsConfig.hpp"

#define DEFAULT_SOLVER_DIR "../../../Tympan/MetierSolver/Solvers/DefaultSolver/"

static const std::string XML_TEST_FILE = tympan::path_to_test_data(
        "projects-panel/20_PROJET_Site_emprise_seule_avec_un_ecran_plusieurs_segments.xml");

// Test fixture which loads a project and create an instance of
// 'ComputationLauncher'.
class LauncherTestFixture: public ::testing::Test
{
public:
    LauncherTestFixture() :
        launcher(XML_TEST_FILE.c_str()) {}
    virtual void SetUp()
    {
    }
    tympan::ComputationLauncher launcher;
};


// Try to launch the default solver.
TEST_F(LauncherTestFixture, default_solver)
{

    ASSERT_GT(launcher.getComputationNumber(), (size_t)0);
    cout << "###  Number of computations: "
         << launcher.getComputationNumber() << endl;

    LPTYCalcul sim_ptr = launcher.getCurrentComputation();
    ASSERT_TRUE(sim_ptr != NULL);

    launcher.launchDefault(DEFAULT_SOLVER_DIR);
    LPTYCalcul current_computation = launcher.getCurrentComputation();
    // XXX Try to get/export some results from the current computation.
}
