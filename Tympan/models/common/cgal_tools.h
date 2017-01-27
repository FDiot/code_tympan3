/**
 * @file cgal_tools.h
 *
 * @brief Utilities to ease (and wrap) use of CGAL
 *
 * @author Anthony Truchet <anthony.truchet@logilab.fr>
 *
 */

#ifndef TYMPAN__CGAL_TOOLS_HPP__INCLUDED
#define TYMPAN__CGAL_TOOLS_HPP__INCLUDED

#include <boost/array.hpp>
#include <boost/foreach.hpp>
#include <boost/range.hpp>
#include <boost/range/iterator_range.hpp>
// #include <boost/range/functions.hpp>
// #include <boost/range/adaptors.hpp>
// #include <boost/range/adaptor/transformed.hpp>
// using boost::adaptors::transformed;

// TODO This header complexity will have to be masked as an implementation detail
#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/Triangulation_face_base_with_info_2.h>
#include <CGAL/Triangulation_vertex_base_with_info_2.h>
#include <CGAL/Constrained_Delaunay_triangulation_2.h>
#include <CGAL/Constrained_triangulation_plus_2.h>
#include <CGAL/Polygon_2.h>
#include <CGAL/Polygon_2_algorithms.h>
#include <CGAL/centroid.h>
#include <CGAL/box_intersection_d.h>
#include <CGAL/intersections.h>

#include "Tympan/models/common/3d.h"
#include "Tympan/models/common/plan.h"

namespace tympan
{


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
typedef CGAL::Triangle_3<CGAL_Gt>                                  CGAL_Triangle;
typedef std::deque<CGAL_Triangle>                                  CGAL_Triangles;
typedef CGAL::Box_intersection_d::Box_with_handle_d<double,3,CGAL_Triangles::iterator> CGAL_TBox;
typedef CGAL::Aff_transformation_3<CGAL_Gt>                        CGAL_Transform3;

/// Convert a CGAL_Point3 to OPoint3D
inline OPoint3D from_cgal(const CGAL_Point3& cp)
{ return OPoint3D(cp.x(), cp.y(), cp.z()); }
/// Convert a OPoint3D to a CGAL_Point3
inline CGAL_Point3 to_cgal(const OPoint3D& op)
{ return CGAL_Point3(op._x, op._y, op._z); }
/// Convert a CGAL_Vector3 to OVector3D
inline OVector3D from_cgal(const CGAL_Vector3& cp)
{ return OVector3D(cp.x(), cp.y(), cp.z()); }
/// Convert a OVector3D to CGAL_Vector3
inline CGAL_Vector3 to_cgal(const OVector3D& op)
{ return CGAL_Vector3(op._x, op._y, op._z); }
/// Convert a OPlan to CGAL_Plane
CGAL_Plane to_cgal(const OPlan& oplan);

/**
 * @brief return 4 points defining a 3D parallelepiped
 *
 * @param w the width of the parallelepiped
 * @param h the height of the parallelepiped
 * @param pta the center of one of the two faces of dimension w x h
 * @param ptb the center of the other face of dimension w x h
 *
 * @return a deque containing the 4 vertices defining the 3D parallelepiped
 *
 * Points A and B delimit the length of the parallelepiped and form a segment placed in its center
 *
 * The vertices of the box that are returned are chosen so that (P1 - P0), (P2 - P0) and (P3 - P0)
 * represent the axes of a non-orthonormal system of reference.
 */
std::deque<CGAL_Point3> build_box(float w, float h, CGAL_Point3 pta, CGAL_Point3 ptb);

/**
 * @brief Find the triangles from `triangle_soup` that are intersected by the 3D box including the
 * points of `query_triangle`
 *
 * @param l expected length of the 3D box
 * @param w expected width of the 3D box
 * @param h expected height of the 3D box
 * @param triangle_soup triangles whose intersection with query_triangle bounding box will be tested
 * @param query_triangle 3 points forming a triangle representing a 3D (bounding) box
 *
 * The indices of these triangles are returned.
 **/
std::deque<size_t> intersected_triangles(CGAL_Triangles & triangle_soup,
        std::deque<CGAL_Point3> query_triangle, float l, float w, float h);

/**
 * @brief normalize vector v
 */
CGAL_Vector3 normalize(CGAL_Vector3 v);

/**
 * @brief This class provides triangulating simple polygons without holes
 */
class PolygonTriangulator
{

public:

    struct VertexInfo
    {
    	/// Constructor
        VertexInfo(int i_ = -1) : i(i_) {}
        int i; //!< This is the index of the vertice within the initial polygon.
    };

    struct FaceInfo {};

    /* Please refer to the following example in CGAL doc for meaning of those typedef
     * http://www.cgal.org/Manual/latest/doc_html/cgal_manual/Triangulation_2/Chapter_main.html#Subsection_37.8.2
     */
    typedef CGAL::Triangulation_vertex_base_with_info_2<VertexInfo, CGAL_Gt>        Vbb;
    typedef CGAL::Triangulation_face_base_with_info_2<FaceInfo, CGAL_Gt>            Fbb;
    typedef CGAL::Constrained_triangulation_face_base_2<CGAL_Gt, Fbb>               Fb;
    typedef CGAL::Triangulation_data_structure_2<Vbb, Fb>                           TDS;
    typedef CGAL::Exact_predicates_tag                                              Itag;
    typedef CGAL::Constrained_Delaunay_triangulation_2<CGAL_Gt, TDS, Itag>          CDT;
    // typedef CGAL::Constrained_triangulation_plus_2<CDT>                          CDTplus;
    typedef CDT::Vertex_handle                                                      Vertex_handle;
    typedef CDT::Face_handle                                                        Face_handle;
    typedef CDT::Triangle                                                           Triangle;
    typedef std::deque<CDT::Vertex_handle>                                          Vertex_handles_container;

public:
    typedef boost::array<unsigned, 3>                                               Tri_indices;
    /// Constructor from a CGAL_Polygon
    PolygonTriangulator(const CGAL_Polygon& poly_);
    /// Destructor
    virtual ~PolygonTriangulator();
    /// Return CDT object
    const CDT& triangulation() const {return cdt;}
    /// Return the ith vertex handle
    const Vertex_handle& vertex_handle(unsigned i) const;
    /// Return the polygon vertex handles
    const Vertex_handles_container& vertex_handles() const {return poly_vh;}


    /**
     * @brief Exports the triangles inside the polygon
     *
     * The vertices are identified by their coordinates
     * NB : This function expect empty deque and will clear the deque passed.
     *
     */
    void
    exportTriangles(std::deque<CDT::Triangle>& triangles) const;

    /**
     * @brief Exports the triangles inside the polygon
     *
     * The vertices are identified by their vertice handles and thus
     * triangles are simply triples
     * NB : This function expect empty deque and will clear the deque passed.
     *
     */
    void
    exportTrianglesIndices(std::deque<Tri_indices>& triangles) const;


protected:

    Vertex_handles_container poly_vh;  //!< Polygon vertex handles
    const CGAL_Polygon& poly;	//!< Reference to the CGAL_Polygon to triangulate
    CDT cdt; //!< CDT (Constrained Delaunay Triangulation) is the type of the main CGAL object to build the triangulation.

}; //  PolygonTriangulator


} //namespace tympan


namespace tympan   // Templates and inline implementations
{


} // namespace tympan {

#endif // TYMPAN__CGAL_TOOLS_HPP__INCLUDED
