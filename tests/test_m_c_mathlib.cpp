/**
 * \file test_mathlib.cpp
 * \test Testing of core_mathlib libs used by lot of TYMPAN features
 *
 *  Created on: june, 12 2013
 *      Author: Denis THOMASSON <denis.thomasson@edf.fr>
 */

#include <cstdlib>

#include "gtest/gtest.h"
#include "Tympan/models/common/mathlib.h"

using std::cout;
using std::cerr;
using std::endl;


// Test the buildBitSet() function
TEST(test_mathlib, build_bit_set)
{
    // Create a bitSet of length 17
    unsigned int bs = core_mathlib::buildBitSet(17);

    cout << "bs = " << bs << '\n';

    // TEST 1 : Check value build (2^17-1) = 131071
    ASSERT_TRUE(bs == 131071);
}

// Test the buildComplementaryBitSet() function
TEST(test_mathlib, buil_complmentaryd_bit_set)
{
    // Build complementary bitset for number number 235 (11101011)
    // with length of 17 (11111111100010100 = 130836)
    unsigned int cbs = core_mathlib::buildComplementaryBitSet(17, 235);

    cout << "cbs = " << cbs << '\n';

    // TEST 1 : Check value build (130836)
    ASSERT_TRUE(cbs == 130836);
}


// Test the SIGN() function
TEST(test_mathlib, sign)
{

	double A=84.52;
	double B=123.58;
	double C=-19.8;
	double D=-1.4;

	EXPECT_DOUBLE_EQ(1,SIGN(A)); 
	EXPECT_DOUBLE_EQ(1,SIGN(B));
	EXPECT_DOUBLE_EQ(-1,SIGN(C)); 
	EXPECT_DOUBLE_EQ(-1,SIGN(D)); 
	
}