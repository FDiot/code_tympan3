/**
 * \file test_mathlib.cpp
 * \test Testing of core_mathlib libs used by lot of TYMPAN features
 *
 *  Created on: june, 12 2013
 *      Author: Denis THOMASSON <denis.thomasson@edf.fr>
 */

#include <cstdlib>

#include "gtest/gtest.h"
#include "Tympan/MetierSolver/AcousticRaytracer/Geometry/mathlib.h"

using std::cout;
using std::cerr;
using std::endl;


TEST(buildBitSet, dumpenv)
{
    // Create a bitSet of length 17
    unsigned int bs = core_mathlib::buildBitSet(17);

    cout << "bs = " << bs << '\n';

    // TEST 1 : Check value build (2^17-1) = 131071
    ASSERT_TRUE(bs == 131071);
}

TEST(builComplmentarydBitSet, dumpenv)
{
    // Build complementary bitset for number number 235 (11101011)
    // with length of 17 (11111111100010100 = 130836)
    unsigned int cbs = core_mathlib::buildComplementaryBitSet(17, 235);

    cout << "cbs = " << cbs << '\n';

    // TEST 1 : Check value build (130836)
    ASSERT_TRUE(cbs == 130836);
}
