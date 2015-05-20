/**
 * @file cgal_tool.cpp
 *
 * @brief Utilities to ease (and wrap) use of CGAL
 *
 * @author Anthony Truchet <anthony.truchet@logilab.fr>
 *
 */

#include <cassert>
#include<algorithm>
#include <functional>

#include "cgal_tools.h"

namespace tympan
{

CGAL_Plane to_cgal(const OPlan& oplan)
{
    CGAL_Point3 o(to_cgal(oplan.rframe._origin));
    CGAL_Vector3 n(to_cgal(oplan.rframe._vecK));
    return CGAL_Plane(o, n);
}

void intersection_report(std::deque<size_t>* intersected, CGAL_Triangles::iterator start_index,
                         const CGAL_TBox& a, const CGAL_TBox& b)
{
    // XXX here we know the 2 boxes intersect, but we don't know for sure that the triangle of
    // box a intersects with box b.
    intersected->push_back(a.handle() - start_index);
}

// This implementation is inspired from http://doc.cgal.org/4.4/Box_intersection_d/index.html
// (see parts "4 Minimal Example for Intersecting Boxes" and
// "5 Example for Finding Intersecting 3D Triangles")
std::deque<size_t> intersected_triangles(CGAL_Triangles & triangle_soup, OBox2 query)
{
    std::deque<CGAL_TBox> boxes;
    // Put the triangles of the scene (triangle soup) in iso-oriented bounded boxes
    for (CGAL_Triangles::iterator it = triangle_soup.begin(); it != triangle_soup.end(); it++)
    {
        boxes.push_back(CGAL_TBox(it->bbox(), it));
    }
    // Create a triangle whose bounding box will have the dimensions of the "query" box,
    // choosing 3 nodes of the box as the triangle nodes (works because CGAL bounding box is
    // iso-oriented)
    CGAL_Triangles query_triangles;
    query_triangles.push_back(CGAL_Triangle(CGAL_Point3(query._C._x, query._C._y, query._C._z),
                CGAL_Point3(query._A._x, query._A._y, query._A._z),
                CGAL_Point3(query._E._x, query._E._y, query._E._z)));
    std::deque<CGAL_TBox> query_boxes;
    CGAL::Bbox_3 box = query_triangles.begin()->bbox();
    // Make sure the new query box has the same dimensions as "query" OBox2
    double epsilon = 0.0001;
    double x_dim = abs(box.xmax() - box.xmin());
    double y_dim = abs(box.ymax() - box.ymin());
    double z_dim = abs(box.zmax() - box.zmin());
    assert(abs(x_dim - query._length) < epsilon
            && "The dimensions of CGAL query box doesn't match query parameter.");
    assert(abs(y_dim - query._width) < epsilon
            && "The dimensions of CGAL query box doesn't match query parameter.");
    assert(abs(z_dim - query._height) < epsilon
            && "The dimensions of CGAL query box doesn't match query parameter.");
    query_boxes.push_back(CGAL_TBox(query_triangles.begin()->bbox(), query_triangles.begin()));
    // Compute intersection between the triangles from the triangle soup and the box
    // from the query.
    // "intersected" will contain the indices of the triangles of the triangle soup
    // that intersect with the triangle of the query deque.
    std::deque<size_t> intersected;
    // use currying to specify container and triangle start index from here and therefore
    // avoid using global variables (these variables must be accessible from the callback function,
    // but CGAL::box_intersection_d requires a callback function that only deals with 2 boxes).
    auto cgal_callback = std::bind(intersection_report, &intersected, triangle_soup.begin(),
                                   std::placeholders::_1, std::placeholders::_2);
    CGAL::box_intersection_d(boxes.begin(), boxes.end(), query_boxes.begin(), query_boxes.end(),
                             cgal_callback);
    // remove duplicates (first order the triangle ids since std::unique only works on
    // an ordered container)
    std::sort(intersected.begin(), intersected.end());
    auto last = std::unique(intersected.begin(), intersected.end());
    intersected.erase(last, intersected.end());
    return intersected;
}

/* **** class PolygonTriangulator **** */

PolygonTriangulator::PolygonTriangulator(
    const CGAL_Polygon& poly_
) :
    poly(poly_)
{
    assert(poly.size() > 2);
    assert(poly.is_simple() && "The polygon need to be simple - no 8 shape");
    unsigned i = 0; // index of the current ve
    BOOST_FOREACH(const CGAL_Point2 & current, poly.container())
    {
        Vertex_handle current_vh = cdt.insert(current);
        current_vh->info() = VertexInfo(i);
        poly_vh.push_back(current_vh);
        if (i > 0)
        {
            cdt.insert_constraint(poly_vh[i - 1], poly_vh[i]);
        }
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
    assert(poly_vh.size() == poly.size() && "Inconsistency btw points and vertices vectors.");
    // Let the checked access std exception trigger for out-of-bound indices
    return poly_vh.at(i);
}

void PolygonTriangulator::exportTriangles(std::deque<PolygonTriangulator::Triangle>& triangles) const
{
    assert(triangles.size() == 0 && "triangles output arguments expected to be empty");
    for (CDT::Finite_faces_iterator it = cdt.finite_faces_begin();
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

    assert(triangles.size() == 0 && "triangles output arguments expected to be empty");
    for (CDT::Finite_faces_iterator it = cdt.finite_faces_begin();
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
                for (unsigned i = 0; i < 3; ++i)
                {
                    tri[i] = it->vertex(i)->info().i;
                }
                triangles.push_back(tri);
                break;
        }
    }
}

} //namespace tympan
