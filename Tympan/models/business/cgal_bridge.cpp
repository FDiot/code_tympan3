/**
 * @file cgal_bridge.cpp
 *
 * @brief Bridges TY* types with CGAL functionalities exposed by @f cgal_tools.hpp
 *
 * @author Anthony Truchet <anthony.truchet@logilab.fr>
 */

#include "cgal_bridge.h"

#include <boost/foreach.hpp>

#include "Tympan/models/common/cgal_tools.h"
#include "Tympan/models/business/geometry/TYPolygon.h"
#include "Tympan/models/business/exceptions.h"

namespace tympan
{

    class TYPolygonTriangulator
        : public ITYPolygonTriangulator
    {
    public:

        TYPolygonTriangulator(const TYPolygon& poly_);

        virtual void
        exportMesh(
                   std::deque<OPoint3D>& points,
                   std::deque<OTriangle>& triangles) const;

        const TYPolygon& poly;
    }; // class TYPolygonTriangulator


    TYPolygonTriangulator::TYPolygonTriangulator(const TYPolygon& poly_)
        : poly(poly_) {}

    void
    TYPolygonTriangulator::exportMesh(
               std::deque<OPoint3D>& points,
               std::deque<OTriangle>& triangles) const
    {

        assert(points.size() == 0 &&
               "Output arguments 'points' is expected to be initially empty");
        assert(triangles.size() == 0 &&
               "Output arguments 'triangles' is expected to be initially empty");

        TYPolygon checked_poly(poly);
        checked_poly.updateNormal(); // This actually updates the associated plane
        if(!checked_poly.checkCoplanar())
        {
            std::deque<LPTYElement> elements;
            LPTYElement element(new TYPolygon(poly));
            elements.push_back(element);
            throw tympan::invalid_data("Polygon is not planar.")
                << tympan_source_loc << tympan::elements_implied_errinfo(elements);
        }
        // NB The triangulation happen in the local r/ frame
        // We build a polygon in the plane (aka 2D) so as to be able to triangulate it
        CGAL_Plane plane(to_cgal(checked_poly.getPlan()));
        CGAL_Polygon poly2d;
        BOOST_FOREACH(const OPoint3D& op, checked_poly.getPoints())
        {
            // Projection of `op` onto `plane`
            const CGAL_Point3 pp3d = plane.projection(to_cgal(op));
            // Conversion into a 2D point
            const CGAL_Point2 pp2d = plane.to_2d(pp3d);
            poly2d.push_back(pp2d);
            points.push_back(op);
        }

        if(!poly2d.is_simple())
        {
            std::deque<LPTYElement> elements;
            LPTYElement element(new TYPolygon(poly));
            elements.push_back(element);
            throw tympan::invalid_data("Face can not be triangulated (invalid plane?).")
                << tympan_source_loc << tympan::elements_implied_errinfo(elements);
        }
        // Actual triangulation done by CGAL
        PolygonTriangulator triangulator(poly2d);
        // Use information from triangulator.vertice_handles and the triangulation itself.
        // to level-up from 2D CGAL representation to our 3D OPoints world
        std::deque<PolygonTriangulator::Tri_indices> tri_indices;
        triangulator.exportTrianglesIndices(tri_indices);
        BOOST_FOREACH(const auto& tri_idx, tri_indices)
        {
            OTriangle tri(tri_idx[0], tri_idx[1], tri_idx[2]);
            for (unsigned i = 0; i < 3; ++i)
            {
                const OPoint3D& p = points[tri_idx[i]];
                tri.vertex(i) = p;
            }
            triangles.push_back(tri);
            assert(triangles.back().checkConsistencyWrtPointsTab(points));
        }
    } // void TYPolygonTriangulator::exportMesh(...)

    std::unique_ptr<ITYPolygonTriangulator>
    make_polygon_triangulator(const TYPolygon& poly)
    {
        return  std::unique_ptr<ITYPolygonTriangulator>(
            new TYPolygonTriangulator(poly));
    } // ITYPolygonTriangulator* make_polygon_triangulator()

    // Convert a TYPolygon into a counter-clockwise oriented CGAL_Polygon.
    CGAL_Polygon cgal_ccw_polygon(const TYPolygon& poly) {
        CGAL_Polygon cpoly;
        std::vector<TYPoint> pts = poly.getPoints();
        for (int i = 0; i < pts.size(); i++) {
            cpoly.push_back(CGAL_Point2(pts[i]._x, pts[i]._y));
        }
        if (cpoly.is_counterclockwise_oriented()) {
            return cpoly;
        }
        // Reverse the polygon to make it counter-clockwise oriented.
        CGAL_Polygon cpoly_;
        for (CGAL_Polygon::Vertex_iterator vi = cpoly.vertices_begin(); vi != cpoly.vertices_end(); ++vi) {
            cpoly_.insert(cpoly_.vertices_begin(), *vi);
        }
        return cpoly_;
    }

    void join_polygons(std::deque<TYPolygon> polygons, TYPolygon& out) {
        std::vector<CGAL_Polygon> cpolys(polygons.size());
        std::vector<TYPoint>& points = out.getPoints();
        assert (points.size() == 0 &&
                "join_polygons got an output polygon with points already defined");
        if (polygons.size() == 1) {
            // Only one polygon, no need to join, just copy its point.
            points = polygons[0].getPoints();
            return;
        }
        for (int i = 0; i < polygons.size(); i++) {
            CGAL_Polygon cpoly = cgal_ccw_polygon(polygons[i]);
            cpolys.push_back(cpoly);
        }

        std::list<CGAL_Polygon_with_holes_2> joined;
        CGAL::join(cpolys.begin(), cpolys.end(), std::back_inserter(joined));

        assert (joined.size() == 1 && "polygons join resulted in more than one polygon");
        CGAL_Polygon boundary = joined.begin()->outer_boundary();

        for (CGAL_Polygon::Vertex_iterator vi = boundary.vertices_begin();
                vi != boundary.vertices_end(); ++vi) {
            points.push_back(TYPoint(vi->x(), vi->y(), 0));
        }
    }
} // namespace tympan
