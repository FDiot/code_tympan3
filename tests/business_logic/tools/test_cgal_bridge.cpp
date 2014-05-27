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

/**
 * \brief Fixture which provides a Polygon to be triangulated
*/
class TriangulatePolygonTest :
    public testing::Test
{
private:
    TYPolygon poly;

protected:
    deque<OPoint3D> points;
    deque<OTriangle> triangles;

    std::unique_ptr<ITYPolygonTriangulator> p_triangulator;

    void triangulate(const vector<TYPoint>& pts)
    {
        poly.setPoints(pts);
        p_triangulator = tympan::make_polygon_triangulator(poly);
        p_triangulator->exportMesh(points, triangles);
    }
}; // class TriangulatePolygonTest


TEST_F(TriangulatePolygonTest, single_triangle)
{
    // NB New c++'11 initializer-list syntax
    // Cf. http://en.wikipedia.org/wiki/C++11#Initializer_lists
    vector<TYPoint> pts;
    pts.push_back(TYPoint(0,0,0));
    pts.push_back(TYPoint(0,1,0));
    pts.push_back(TYPoint(1,0,0));

    triangulate(pts);


    ASSERT_EQ(3, points.size());
    ASSERT_EQ(1, triangles.size());
}

TEST_F(TriangulatePolygonTest, concave_quadrangle)
{
    vector<TYPoint> pts { {0,0,0}, {5, 0,0}, {1, 1, 0}, {0.5, 6, 0}};

    triangulate(pts);

    ASSERT_EQ(4, points.size());
    ASSERT_EQ(2, triangles.size());
}
