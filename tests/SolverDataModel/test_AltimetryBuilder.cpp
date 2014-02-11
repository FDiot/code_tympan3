/**
 * @file test_AltimetryBuilder.cpp
 *
 * @brief The (unit) tests for \c AltimetryBuilder
 * @author Anthony Truchet <anthony.truchet@logilab.fr>
 *
 * You want want to have a look to :
 *  - http://code.google.com/p/googletest/wiki/AdvancedGuide#Testing_Private_Code
 *  - http://code.google.com/p/googletest/wiki/FAQ#How_do_I_test_private_class_members_without_writing_FRIEND_TEST%28
 */

#include <deque>

#include "gtest/gtest.h"

#include "Tympan/MetierSolver/SolverDataModel/AltimetryBuilder.hpp"

using namespace tympan;

namespace tympan
{

// To use this fixture, use 'TEST_F' (instead of 'TEST').
class AltimetryFixture:
    public ::testing::Test
{
public:
    // static void SetUpTestCase()
    virtual void SetUp()
    {
    }

    //std::deque<LevelCurve*> level_curves;
    //std::deque<MaterialPolygon*> material_polygons;

    static TYTabPoint contour; ///< The "embase" as a level curve
    static TYTabPoint curveB;
    static TYTabPoint curveC;
    static TYTabPoint curveC1; ///< Degenerated curve with 1 point
    static TYTabPoint curveC2; ///< Degenerated curve with 2 points
    static TYTabPoint grass1;


private:
    static TYPoint ini_points[];
};

TYPoint AltimetryFixture::ini_points[] =
{
    TYPoint(0,  0, 0.0),
    TYPoint(0, 10, 0.0),
    TYPoint(10, 10, 0.0),
    TYPoint(10,  0, 0.0),
    TYPoint(1,  2, 1.5),
    TYPoint(3,  8, 1.5),
    TYPoint(8,  1, 1.5),
    TYPoint(5,  3, 2.5),
    TYPoint(3,  4, 2.5),
    TYPoint(5,  5, 2.5),
    TYPoint(2,  1, unspecified_altitude),
    TYPoint(7,  6, unspecified_altitude),
    TYPoint(6,  1, unspecified_altitude)
};

TYTabPoint AltimetryFixture::contour(ini_points + 0, ini_points + 4);
TYTabPoint AltimetryFixture::curveB(ini_points + 4, ini_points + 7);
TYTabPoint AltimetryFixture::curveC(ini_points + 7, ini_points + 10);
TYTabPoint AltimetryFixture::curveC1(ini_points + 7, ini_points + 8);
TYTabPoint AltimetryFixture::curveC2(ini_points + 8, ini_points + 10);
TYTabPoint AltimetryFixture::grass1(ini_points + 10, ini_points + 13);

TEST_F(AltimetryFixture, dummysquare)
{
    AltimetryBuilder builder;
    builder.addAsConstraint(contour | transformed(to_cgal), true);
    ASSERT_TRUE(builder.cdt.is_valid());
    EXPECT_EQ(builder.cdt.number_of_faces(), 2);
    EXPECT_EQ(builder.cdt.number_of_vertices(), 4);
    std::pair<unsigned, unsigned> edges_counts = builder.count_edges();
    EXPECT_EQ(edges_counts.first, 5);
    EXPECT_EQ(edges_counts.second, 4);
}

TEST_F(AltimetryFixture, insertrange)
{
    AltimetryBuilder builder;

    std::deque<CDT::Vertex_handle>
    vhandles = builder.insert_range(contour | transformed(to_cgal));
    ASSERT_TRUE(builder.cdt.is_valid());
    EXPECT_EQ(builder.cdt.number_of_faces(), 2);
    EXPECT_EQ(builder.cdt.number_of_vertices(), 4);
    EXPECT_EQ(vhandles.size(), 4);
    for (int i = 0; i < 4; i++)
    {
        EXPECT_EQ(vhandles[i]->point(), to_cgal(contour[i]));
    }
}



} // namespace tympan
