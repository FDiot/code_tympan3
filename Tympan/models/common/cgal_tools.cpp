/**
 * @file cgal_tool.cpp
 *
 * @brief Utilities to ease (and wrap) use of CGAL
 *
 * @author Anthony Truchet <anthony.truchet@logilab.fr>
 *
 */

#include <cassert>
#include <algorithm>
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

CGAL_Vector3 normalize(CGAL_Vector3 v)
{
    return v / sqrt(v.squared_length());
}

std::deque<CGAL_Point3> build_box(float w, float h, CGAL_Point3 pta, CGAL_Point3 ptb)
{
    // AB vector
    CGAL_Vector3 pta_ptb(pta, ptb);
    // We need to build a plane containing pt A and pt B. A third point is needed.
    CGAL_Point3 some_point(0, 1, 0);  // have a try (Y axis)
    // If AB vector and Asome_point vectors are colinear, use X axis instead of Y axis
    if(CGAL::cross_product(pta_ptb, CGAL_Vector3(pta, some_point)) == CGAL::NULL_VECTOR)
        some_point = CGAL_Point3(1, 0, 0);
    // Build a plane with pt A, pt B and the arbitrary point and compute the orthogonal vector
    CGAL_Vector3 ortho = normalize(CGAL::orthogonal_vector(pta, ptb, some_point));
    // Compute another orthogonal vector
    CGAL_Vector3 ortho2 = CGAL::cross_product(normalize(pta_ptb), ortho);
    // Build the face containing pt A (add 2 perpendicular unit vectors respectively multiplied by
    // w/2 and h/2 since the face dimension is l x h)
    std::deque<CGAL_Point3> vertices;
    vertices.push_back(pta - ortho*h/2 + ortho2*w/2); // orig
    vertices.push_back(pta - ortho*h/2 + ortho2*w/2 + pta_ptb); // x
    vertices.push_back(pta - ortho*h/2 - ortho2*w/2); // y
    vertices.push_back(pta + ortho*h/2 + ortho2*w/2); // z
    return vertices;
}

void intersection_report(std::deque<size_t>* intersected, CGAL_Triangles::iterator start_index,
                         const CGAL_TBox& a, const CGAL_TBox& b)
{
    // here we know the 2 boxes intersect, but we don't know for sure that the triangle of
    // box a intersects with box b. Make sure it is the case before inserting the triangle
    if(CGAL::do_intersect(*a.handle(), b.bbox()))
    {
        intersected->push_back(a.handle() - start_index);
    }
}

// This implementation is inspired from http://doc.cgal.org/4.4/Box_intersection_d/index.html
// (see parts "4 Minimal Example for Intersecting Boxes" and
// "5 Example for Finding Intersecting 3D Triangles")
std::deque<size_t> intersected_triangles(CGAL_Triangles & triangle_soup,
        std::deque<CGAL_Point3> query_box, float length, float width, float height)
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
    CGAL_Triangles query_triangle;
    query_triangle.push_back(CGAL_Triangle(query_box[0], query_box[1], query_box[2]));
    std::deque<CGAL_TBox> query_tboxes;
    CGAL::Bbox_3 box = query_triangle.begin()->bbox();
    // Make sure the new query box has the same dimensions as "query" OBox2
    double epsilon = 0.0001;
    double x_dim = abs(box.xmax() - box.xmin());
    double y_dim = abs(box.ymax() - box.ymin());
    double z_dim = abs(box.zmax() - box.zmin());
    assert(abs(x_dim - length) < epsilon
            && "The dimension X of CGAL query box doesn't match query parameter.");
    assert(abs(y_dim - width) < epsilon
            && "The dimension Y CGAL query box doesn't match query parameter.");
    assert(abs(z_dim - height) < epsilon
            && "The dimension Z of CGAL query box doesn't match query parameter.");
    query_tboxes.push_back(CGAL_TBox(box, query_triangle.begin()));
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
    CGAL::box_intersection_d(boxes.begin(), boxes.end(), query_tboxes.begin(), query_tboxes.end(),
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
