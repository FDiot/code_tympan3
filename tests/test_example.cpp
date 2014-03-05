/**
 * \file test_examples.cpp
 * \test Various examples of testing and utils to debug testing environment
 *
 *  Created on: 6 nov. 2012
 *      Author: Anthony Truchet <anthony.truchet@logilab.fr>
 */

#include <cstdlib>

#include "gtest/gtest.h"

using std::cout;
using std::cerr;
using std::endl;

TEST(ExampleTest, dumpenv)
{

    char* path = std::getenv("PATH");
    ASSERT_FALSE(path == NULL);
    cerr << "PATH = " << path << endl;
}

