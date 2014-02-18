/**
 * \file test_environment.cpp
 * \test Some debuging of the testing environment
 *
 *  Created on: 6 nov. 2012
 *      Author: Anthony Truchet <anthony.truchet@logilab.fr>
 */

#include <cstdlib>

#include "gtest/gtest.h"

using std::cout;
using std::cerr;
using std::endl;

TEST(PathTest, dumpenv)
{

    char* path = std::getenv("PATH");
    ASSERT_FALSE(path == NULL);
    cerr << "PATH = " << path << endl;
}
