//! \test test_computation_launcher.cpp

#include <iostream>

#include "gtest/gtest.h"

#include "test_utils/ComputationLauncher.hpp"

#include "Tympan/MetierSolver/DataManagerCore/TYPluginManager.h"

#define DEFAULT_SOLVER_DIR "../../../Tympan/MetierSolver/Solvers/DefaultSolver/"
#define SIMPLERAY_SOLVER_DIR "../../../Tympan/MetierSolver/Solvers/SimpleRaySolver/"
#define XML_TEST_FILE "../../data/projects-panel/20_PROJET_Site_emprise_seule_avec_un_ecran_plusieurs_segments.xml"


using std::cout;
using std::endl;

// Test fixture which loads a project and create an instance of
// 'ComputationLauncher'.
class LauncherTestFixture: public ::testing::Test {
public:
    LauncherTestFixture() :
        xml_filename(XML_TEST_FILE),
        launcher(xml_filename) {}
    virtual void SetUp() {
    }

    char* xml_filename;
    tympan::ComputationLauncher launcher;
};


// Try to launch the default solver.
TEST_F(LauncherTestFixture, default_solver) {

    ASSERT_GT(launcher.getComputationNumber(), (size_t)0);
    cout << "###  Number of computations: "
         << launcher.getComputationNumber() << endl;

    LPTYCalcul sim_ptr = launcher.getCurrentComputation();
    ASSERT_TRUE(sim_ptr != NULL);

    launcher.launchDefault(DEFAULT_SOLVER_DIR);
    LPTYCalcul current_computation = launcher.getCurrentComputation();
    // XXX Try to get/export some results from the current computation.
}


// Try to launch the simple ray solver.
TEST_F(LauncherTestFixture, simple_ray_solver) {
    ASSERT_GT(launcher.getComputationNumber(), (size_t)0);
    cout << "###  Number of computations: "
         << launcher.getComputationNumber() << endl;

    LPTYCalcul sim_ptr = launcher.getCurrentComputation();
    ASSERT_TRUE(sim_ptr != NULL);

    launcher.launchSimpleRay(SIMPLERAY_SOLVER_DIR);
}
