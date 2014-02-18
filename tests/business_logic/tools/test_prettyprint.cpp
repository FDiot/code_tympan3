/** \test prettyprint.cpp
 * @author Anthony Truchet <anthony.truchet@logilab.org>
 */

#include <sstream>
#include <iostream>

#include "gtest/gtest.h"

#include "Tympan/MetierSolver/ToolsMetier/prettyprint.hpp"

using std::cout;
using std::endl;
using std::stringstream;


TEST(TestTestUtils, pp_base_geom)
{
    stringstream out;

    OPoint3D p1(1, 2, 3);
    out << p1;
    EXPECT_STREQ("OPoint3D(1, 2, 3)", out.str().c_str());
}

TEST(TestTestUtils, pp_ospectre)
{
    stringstream out;

    OSpectre s = OSpectre::getEmptyLinSpectre(1);
    out << s;
    const char* exp_cstr = "Spectrum[type=0, state=1]\n"
                           "        (1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, )\n";
    EXPECT_STREQ(exp_cstr, out.str().c_str());
}
