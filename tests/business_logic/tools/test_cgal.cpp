/**
 * @file cgal_tool.hpp
 *
 * @brief Test the utilities to ease (and wrap) use of CGAL
 *
 * @author Anthony Truchet <anthony.truchet@logilab.fr>
 *
 */


#include <cstdlib>

#include "gtest/gtest.h"
#include "test_utils/prettyprint.hpp"

#include "Tympan/MetierSolver/ToolsMetier/cgal_tools.hpp"

/*
#include "Tympan/MetierSolver/ToolsMetier/OGeometrie.h"
#include "Tympan/MetierSolver/ToolsMetier/OBox.h"
*/
#include "Tympan/MetierSolver/ToolsMetier/OPoint3D.h"
#include "Tympan/MetierSolver/ToolsMetier/OVector3D.h"
#include "Tympan/MetierSolver/ToolsMetier/OPlan.h"

using std::cout;
using std::cerr;
using std::endl;

TEST(cgal_tools, basic_types) {

using namespace tympan;

OPoint3D op(1, 2, 3);
EXPECT_EQ(op, from_cgal( to_cgal(op) ));
CGAL_Point3 cp(4, 5, 6);
EXPECT_EQ(cp, to_cgal( from_cgal(cp) ));
OVector3D ov(1, 2, 3);
EXPECT_EQ(ov, from_cgal( to_cgal(ov) ));
CGAL_Vector3 cv(4, 5, 6);
EXPECT_EQ(cv, to_cgal( from_cgal(cv) ));

}

// To factorize with the two others geometrical tests
TEST(geometry_tools, plane)
{
using namespace tympan;

OVector3D v1(1, 1.5 , 2);
OPoint3D A(2, 3, 4);
OPlan P(A, v1);

// Test for the explicit representation
EXPECT_EQ( A, P.rframe._origin);
v1.normalize();
EXPECT_DOUBLE_EQ(0.0, (v1-P.rframe._vecK).norme());

// Test for conversion to CGAL
CGAL_Plane CP( to_cgal(P));
CGAL_Point3 cgal_origin(2, 3, 4);
CGAL_Point3 proj( CP.projection(cgal_origin));
EXPECT_NEAR(0, (proj-cgal_origin).squared_length(), 1e-20 );
}
