/**
 * @file test_uuid_adapter.cpp
 * @brief Test the \c tympan::UuidAdapter. Can be build from \c OGenID (aka \c
 * TYUUID). Can be converted to a \c OGenID.
 *
 * @date Jan. 17 2013
 * @author Damien Garaud <damien.garaud@logilab.fr>
 */

#include <iostream>

#include "gtest/gtest.h"

#include "Tympan/MetierSolver/DataExchange/UuidAdapter.hpp"


using std::cout;
using std::cerr;
using std::endl;

TEST(TestUuidAdapter, default)
{
    TYUUID element_uid;
    element_uid.GenUniqueID();
    tympan::UuidAdapter wuid(element_uid); // Construct from a \c TYUUID.

    TYUUID element_uid_bis = static_cast<TYUUID>(wuid); // Conversion operator.
    ASSERT_EQ(element_uid, element_uid_bis);
}
