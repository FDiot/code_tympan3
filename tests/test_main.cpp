#include "gtest/gtest.h"
#include "Tympan/MetierSolver/DataManagerMetier/init_registry.h"

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    tympan::init_registry(); // register tympan business classes
    return RUN_ALL_TESTS();
}
