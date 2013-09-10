/**
 * @file cgal_tool.cpp
 *
 * @brief Utilities to ease (and wrap) use of CGAL
 *
 * @author Anthony Truchet <anthony.truchet@logilab.fr>
 *
 */

#include "cgal_tools.hpp"

namespace tympan {

CGAL_Plane to_cgal(const OPlan& oplan)
{
    CGAL_Point3 o(to_cgal(oplan.rframe._origin));
    CGAL_Vector3 n(to_cgal(oplan.rframe._vecK));
    return CGAL_Plane(o, n);
}

} //namespace tympan
