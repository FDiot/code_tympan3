/**
 * @file cgal_tool.cpp
 *
 * @brief Utilities to ease (and wrap) use of CGAL
 *
 * @author Anthony Truchet <anthony.truchet@logilab.fr>
 *
 */

#include <cassert>

#include "cgal_tools.hpp"

namespace tympan {

CGAL_Plane to_cgal(const OPlan& oplan)
{
    CGAL_Point3 o(to_cgal(oplan.rframe._origin));
    CGAL_Vector3 n(to_cgal(oplan.rframe._vecK));
    return CGAL_Plane(o, n);
}

/* **** class PolygonTriangulator **** */

PolygonTriangulator::PolygonTriangulator(
    const CGAL_Polygon& poly_
) :
    poly(poly_)
{
    assert(poly.size() > 2);
    assert(poly.is_simple() && "The polygon need to be simple - no 8 shape");
    unsigned i=0; // index of the current ve
    BOOST_FOREACH( const CGAL_Point2& current, poly.container())
    {
        Vertex_handle current_vh = cdt.insert(current);
        current_vh->info()=VertexInfo(i);
        poly_vh.push_back(current_vh);
        if (i>0)
            cdt.insert_constraint(poly_vh[i-1], poly_vh[i]);
        ++i;
    }
    // Closes the polygon
    cdt.insert_constraint(poly_vh[--i], poly_vh[0]);
} // PolygonTriangulator::PolygonTriangulator()


PolygonTriangulator::~PolygonTriangulator()
{

} // PolygonTriangulator::~PolygonTriangulator()

const PolygonTriangulator::Vertex_handle& PolygonTriangulator::vertex_handle(unsigned i) const
{
    assert(poly_vh.size()==poly.size() && "Inconsistency btw points and vertices vectors." );
    // Let the checked access std exception trigger for out-of-bound indices
    return poly_vh.at(i);
}

void PolygonTriangulator::exportTriangles(std::deque<PolygonTriangulator::Triangle>& triangles) const
{
    assert(triangles.size()==0 && 'triangles output arguments expected to be empty');
    for(CDT::Finite_faces_iterator it = cdt.finite_faces_begin();
        it !=  cdt.finite_faces_end(); ++ it)
    {
        // Extract a representative inner point
        CGAL_Point2 center = CGAL::centroid(cdt.triangle(it));
        // Test whether it is inside the polygon
        switch (poly.bounded_side(center))
        {
        case CGAL::ON_UNBOUNDED_SIDE:
        case CGAL::ON_BOUNDARY: // this is the degenerate case of a flat triangle
            continue;
        case CGAL::ON_BOUNDED_SIDE :
            triangles.push_back(cdt.triangle(it)); // `it` is a Face_handle
            break;
        }
    }
}

void PolygonTriangulator::exportTrianglesIndices(std::deque<Tri_indices>& triangles) const
{

    assert(triangles.size()==0 && 'triangles output arguments expected to be empty');
    for(CDT::Finite_faces_iterator it = cdt.finite_faces_begin();
        it !=  cdt.finite_faces_end(); ++ it)
    {
        // Extract a representative inner point
        CGAL_Point2 center = CGAL::centroid(cdt.triangle(it));
        // Test whether it is inside the polygon
        switch (poly.bounded_side(center))
        {
        case CGAL::ON_UNBOUNDED_SIDE:
        case CGAL::ON_BOUNDARY: // this is the degenerate case of a flat triangle
            continue;
        case CGAL::ON_BOUNDED_SIDE :
            Tri_indices tri;
            for(unsigned i=0; i<3; ++i)
            {
                tri[i] = it->vertex(i)->info().i;
            }
            triangles.push_back(tri);
            break;
        }
    }
}

} //namespace tympan
