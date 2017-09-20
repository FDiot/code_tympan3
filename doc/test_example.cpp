/**
 * \file test_examples.cpp
 * \test Various examples of testing
 *
 *  Created on: 6 nov. 2012
 *      Author: Anthony Truchet <anthony.truchet@logilab.fr>

CMakeLists.txt extract:

add_executable(test_example test_example.cpp)
configure_gtest_target(TARGET test_example
  RUNTIME_PATH "${TYMPAN_3RDPARTY_DLL_NATIVE_DIRS}"
  FOLDER ${CURRENT_FOLDER})

*/


#include "gtest/gtest.h"

// In a real test the function would be included and linked against
// instead of being merely defined here.
double my_add_feature(double a, double b)
{
    return a + b;
}

TEST(ExampleTest, addonint)
{
    // This macro handles the check and the reporting
    EXPECT_EQ(5, my_add_feature(2, 3));
}

// the test name starting with DISABLED tells GTest it has to disable this test.
TEST(ExampleTest, DISABLED_purposefulfail)
{
    // This macro handles the check and the reporting
    EXPECT_EQ(0, my_add_feature(2, 3));
}

// TODO provide richer examples by need
