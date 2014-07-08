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


inline OPoint3D from_cgal(const CGAL_Point3& cp)
{ return OPoint3D(cp.x(), cp.y(), cp.z()); }

inline CGAL_Point3 to_cgal(const OPoint3D& op)
{ return CGAL_Point3(op._x, op._y, op._z); }

inline OVector3D from_cgal(const CGAL_Vector3& cp)
{ return OVector3D(cp.x(), cp.y(), cp.z()); }

inline CGAL_Vector3 to_cgal(const OVector3D& op)
{ return CGAL_Vector3(op._x, op._y, op._z); }

CGAL_Plane to_cgal(const OPlan& oplan);

/**
 * @brief This class provides triangulating simple polygons without holes
 */
class PolygonTriangulator
{

public:

    struct VertexInfo
    {
        VertexInfo(int i_ = -1) : i(i_) {}
        int i; // This is the index of the vertice within the intial polygon.
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
    /// \c CDT is the type of the main CGAL object to build the triangulation.
    typedef CGAL::Constrained_Delaunay_triangulation_2<CGAL_Gt, TDS, Itag>          CDT;
    // typedef CGAL::Constrained_triangulation_plus_2<CDT>                          CDTplus;
    typedef CDT::Vertex_handle                                                      Vertex_handle;
    typedef CDT::Face_handle                                                        Face_handle;
    typedef CDT::Triangle                                                           Triangle;
    typedef std::deque<CDT::Vertex_handle>                                          Vertex_handles_container;

public:
    typedef boost::array<unsigned, 3>                                               Tri_indices;

    PolygonTriangulator(const CGAL_Polygon& poly_);
    virtual ~PolygonTriangulator();

    const CDT& triangulation() const {return cdt;}
    const Vertex_handle& vertex_handle(unsigned i) const;
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

    Vertex_handles_container poly_vh;
    const CGAL_Polygon& poly;
    CDT cdt;

}; //  PolygonTriangulator


} //namespace tympan


namespace tympan   // Templates and inline implementations
{


} // namespace tympan {

#endif // TYMPAN__CGAL_TOOLS_HPP__INCLUDED
