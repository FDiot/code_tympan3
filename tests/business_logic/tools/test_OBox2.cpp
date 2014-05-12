/**
 * \file test_OBox.cpp
 * \test Testing of OBox libs used by lot of TYMPAN features
 *
 *  Created on: november 6, 2013
 *      Author: Denis THOMASSON <denis.thomasson@edf.fr>
 */

#include <cstdlib>

#include "gtest/gtest.h"
#include "Tympan/MetierSolver/CommonTools/OGeometrie.h"
#include "Tympan/MetierSolver/CommonTools/OCoord3D.h"
#include "Tympan/MetierSolver/CommonTools/OPoint3D.h"
#include "Tympan/MetierSolver/CommonTools/OVector3D.h"
#include "Tympan/MetierSolver/CommonTools/ORepere3D.h"
#include "Tympan/MetierSolver/CommonTools/OBox.h"
#include "Tympan/MetierSolver/CommonTools/OBox2.h"
#include "Tympan/MetierSolver/CommonTools/prettyprint.hpp"

using std::cout;
using std::cerr;
using std::endl;

TEST(OBox2Test, IsInsideTest)
{
    // Cration des points min et max
    OCoord3D pt1(-2.0, -4.0, -3.0);
    OCoord3D pt2(5.0, 2.0, 1.0);

    // Cration de la box
    ORepere3D rep;
    OBox box(pt1, pt2);
    OBox2 box2(box);

    // Cration du point  tester
    OPoint3D pt(-1.0, -3.0, -2.0);

    // Test positif
    bool resu = box2.isInside(pt);
    EXPECT_TRUE(resu);

    // Test ngatif
    pt._z = 3.0;
    resu = box2.isInside(pt);
    EXPECT_FALSE(resu);
}

TEST(OBox2Test, IsInside2DTest)
{
    // Cration des points min et max
    OCoord3D pt1(-2.0, -4.0, -3.0);
    OCoord3D pt2(5.0, 2.0, 1.0);

    // Cration de la box
    ORepere3D rep;
    OBox box(pt1, pt2);
    OBox2 box2(box);

    // Cration du point  tester
    OPoint3D pt(-1.0, -3.0, -2.0);

    // Test positif (1) : Point is inside 2D and 3D box
    bool resu = box2.isInside2D(pt);
    EXPECT_TRUE(resu);

    // Test positif (2) : Point is inside 2D but not 3D box
    pt._z = 3.0;
    resu = box2.isInside2D(pt);
    EXPECT_TRUE(resu);

    // Test ngatif : Point is outside 2D box
    pt._x = -5;
    resu = box2.isInside2D(pt);
    EXPECT_FALSE(resu);
}

TEST(OBox2Test, DISABLED_translateTest)
{
    // Cration des points min et max
    OCoord3D pt1(-2.0, -4.0, -3.0);
    OCoord3D pt2(5.0, 2.0, 1.0);

    // Cration de la box
    ORepere3D rep;
    OBox box(pt1, pt2);
    OBox2 box2(box);

    // Cration du point servant  translater
    OVector3D vt(-1.0, -3.0, -2.0);

    // Test positif (1) : Point is inside 2D and 3D box
    box2.Translate(vt);

    EXPECT_EQ( box2._A, OPoint3D(-3.0, -5.0, -4.0) );
    EXPECT_EQ( box2._center, OPoint3D( 0.5, -2.0, -2.0 ) );
    EXPECT_EQ( box2._H, OPoint3D( 4.0, 1.0, 0.0 ) );
}

TEST(OBox2Test, DISABLED_rotationTest)
{
    // Cration des points min et max
    OCoord3D pt1(-2.0, -4.0, -3.0);
    OCoord3D pt2(5.0, 2.0, 1.0);

    // Cration de la box
    ORepere3D rep;
    OBox box(pt1, pt2);
    OBox2 box2(box);

    // Cration du point servant  translater
    OVector3D vt(-1.0, -3.0, -2.0);

    // Test positif (1) : Point is inside 2D and 3D box
    box2.Translate(vt);

    EXPECT_EQ( box2._A, OPoint3D(-3.0, -5.0, -4.0) );
    EXPECT_EQ( box2._center, OPoint3D( 0.5, -2.0, -2.0 ) );
    EXPECT_EQ( box2._H, OPoint3D( 4.0, 1.0, 0.0 ) );
}
