/**
 * @file cgal_tool.hpp
 *
 * @brief Utilities to ease (and wrap) use of CGAL
 *
 * @author Anthony Truchet <anthony.truchet@logilab.fr>
 *
 */

#ifndef TYMPAN__CGAL_TOOLS_HPP__INCLUDED
#define TYMPAN__CGAL_TOOLS_HPP__INCLUDED

#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/Triangulation_face_base_with_info_2.h>
#include <CGAL/Triangulation_vertex_base_with_info_2.h>
#include <CGAL/Constrained_Delaunay_triangulation_2.h>
#include <CGAL/Polygon_2.h>
#include <CGAL/Polygon_2_algorithms.h>

#include "Tympan/MetierSolver/ToolsMetier/OPoint3D.h"
#include "Tympan/MetierSolver/ToolsMetier/OVector3D.h"
#include "Tympan/MetierSolver/ToolsMetier/OPlan.h"


namespace tympan {


// From CGAL manuel Section 37.8
// http://www.cgal.org/Manual/latest/doc_html/cgal_manual/Triangulation_2/Chapter_main.html#Section_37.8
typedef CGAL::Exact_predicates_inexact_constructions_kernel        CGAL_K;
typedef CGAL_K CGAL_Gt; /* Geometric Traits */
typedef CGAL::Point_2<CGAL_Gt>                                     CGAL_Point2;
typedef CGAL::Point_3<CGAL_Gt>                                     CGAL_Point3;
typedef CGAL::Vector_2<CGAL_Gt>                                    CGAL_Vector2;
typedef CGAL::Vector_3<CGAL_Gt>                                    CGAL_Vector3;
typedef CGAL::Polygon_2<CGAL_Gt>                                   CGAL_Polygon;
typedef CGAL::Plane_3<CGAL_Gt>                                     CGAL_Plane;


inline OPoint3D from_cgal(const CGAL_Point3& cp)
{ return OPoint3D(cp.x(), cp.y(), cp.z()); }

inline CGAL_Point3 to_cgal(const OPoint3D& op)
{ return CGAL_Point3(op._x, op._y, op._z); }

inline OVector3D from_cgal(const CGAL_Vector3& cp)
{ return OVector3D(cp.x(), cp.y(), cp.z()); }

inline CGAL_Vector3 to_cgal(const OVector3D& op)
{ return CGAL_Vector3(op._x, op._y, op._z); }

CGAL_Plane to_cgal(const OPlan& oplan);

} //namespace tympan

#endif // TYMPAN__CGAL_TOOLS_HPP__INCLUDED
