/**
 * @file cgal_bridge.cpp
 *
 * @brief Bridges TY* types with CGAL functionalities exposed by @f cgal_tools.hpp
 *
 * @author Anthony Truchet <anthony.truchet@logilab.fr>
 */

#include "cgal_bridge.hpp"

#include <boost/foreach.hpp>

#include "Tympan/MetierSolver/CommonTools/cgal_tools.hpp"
#include "Tympan/MetierSolver/DataManagerMetier/ComposantGeometrique/TYPolygon.h"

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

        // NB The triangulation happen in the local r/ frame
        // We build a polygon in the plane (aka 2D) so as to be able to triangulate it
        CGAL_Plane plane(to_cgal(poly.getPlan()));
        CGAL_Polygon poly2d;
        BOOST_FOREACH(const OPoint3D& op, poly.getPoints())
        {
            // CHECKME the .to_2d method implement an unspecified *affine*
            // transform - NOT an isometry
            poly2d.push_back(plane.to_2d(plane.projection(to_cgal(op))));
            points.push_back(op);
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


} // namespace tympan
