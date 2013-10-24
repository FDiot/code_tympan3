/**
 * \file test_examples.cpp
 * \test Various examples of testing
 *
 *  Created on: 6 nov. 2012
 *      Author: Anthony Truchet <anthony.truchet@logilab.fr>
 */


#include "gtest/gtest.h"

// In a real test the function would be included and linked against
// instead of being merely defined here.
double my_add_feature(double a, double b)
{
    return a+b;
}

TEST(ExampleTest, addonint) {
    // This macro handles the check and the reporting
    EXPECT_EQUAL(5, my_add_feature(2, 3));
}

TEST(ExampleTest, purposefulfail) {
    // This macro handles the check and the reporting
    EXPECT_EQUAL(0, my_add_feature(2, 3));
}

// TODO provide richer examples by need
