/** \test prettyprint.cpp
 * @author Anthony Truchet <anthony.truchet@logilab.org>
 */

#include <strstream>
#include <iostream>

#include "gtest/gtest.h"

#include "test_utils/prettyprint.hpp"

using std::cout;
using std::endl;
using std::stringstream;


TEST(TestTestUtils, pp_base_geom) {
    stringstream out;

    OPoint3D p1(1,2,3);
    out << p1;
    cout << out.str() << endl;
    EXPECT_STREQ("OPoint3D(1, 2, 3)", out.str().c_str());
}
