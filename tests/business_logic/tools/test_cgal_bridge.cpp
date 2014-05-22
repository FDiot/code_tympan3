/**
 * @file test_cgal_bridge.cpp
 *
 * @brief Test the bridge between TY* and CGAL functionalities
 *
 * @author Anthony Truchet <anthony.truchet@logilab.fr>
 *
 */



#include <vector>
#include <deque>
#include <memory>

using std::vector;
using std::deque;

#include "gtest/gtest.h"

#include "Tympan/MetierSolver/CommonTools/OPoint3D.h"
#include "Tympan/MetierSolver/CommonTools/OVector3D.h"
#include "Tympan/MetierSolver/CommonTools/OPlan.h"
#include "Tympan/MetierSolver/CommonTools/OTriangle.h"
#include "Tympan/MetierSolver/DataManagerMetier/ComposantGeometrique/TYPolygon.h"

#include "Tympan/MetierSolver/CommonTools/prettyprint.hpp"
#include "Tympan/MetierSolver/DataManagerMetier/cgal_bridge.hpp"

using tympan::ITYPolygonTriangulator;

TEST(TYPolygonTriangulatorTest, instanciation)
{
    TYPolygon poly;

    auto p_triangulator = tympan::make_polygon_triangulator(poly);
    ASSERT_TRUE(p_triangulator != NULL);
}

TEST(TYPolygonTriangulatorTest, single_triangle)
{
    // NB New c++'11 initializer-list syntax
    // Cf. http://en.wikipedia.org/wiki/C++11#Initializer_lists
    vector<TYPoint> pts { {0,0,0}, {0,1,0}, {1, 0, 0}};
    TYPolygon poly(pts);

    deque<OPoint3D> points;
    deque<OTriangle> triangles;

    auto p_triangulator = tympan::make_polygon_triangulator(poly);
    p_triangulator->exportMesh(points, triangles);

    ASSERT_EQ(3, points.size());
    ASSERT_EQ(1, triangles.size());
}

TEST(TYPolygonTriangulatorTest, concave_quadrangle)
{
    // NB New c++'11 initializer-list syntax
    // Cf. http://en.wikipedia.org/wiki/C++11#Initializer_lists
    vector<TYPoint> pts { {0,0,0}, {5, 0,0}, {1, 1, 0}, {0.5, 6, 0}};
    TYPolygon poly(pts);

    deque<OPoint3D> points;
    deque<OTriangle> triangles;

    auto p_triangulator = tympan::make_polygon_triangulator(poly);
    p_triangulator->exportMesh(points, triangles);

    ASSERT_EQ(4, points.size());
    ASSERT_EQ(2, triangles.size());
}
