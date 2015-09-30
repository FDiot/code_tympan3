/**
 * @file cgal_bridge.h
 *
 * @brief Bridges TY* types with CGAL functionalities exposed by @f cgal_tools.hpp
 *
 * @author Anthony Truchet <anthony.truchet@logilab.fr>
 */

#ifndef TYMPAN__CGAL_BRIDGE_HPP__INCLUDED
#define TYMPAN__CGAL_BRIDGE_HPP__INCLUDED

#include <deque>
#include <memory>

#include "Tympan/core/exceptions.h"
#include "Tympan/models/common/triangle.h"
#include "Tympan/models/common/3d.h"
#include "Tympan/models/common/cgal_tools.h"
#include "Tympan/models/business/TYDefines.h"

// Forward declarations
#include "Tympan/core/smartptr.h"
class TYPolygon;
class TYTopographie;
class TYSol;
class TYTerrain;
class TYCourbeNiveau;
///Smart pointer sur TYSol.
typedef SmartPtr<TYSol> LPTYSol;

#ifndef BOOST_NOEXCEPT
#define BOOST_NOEXCEPT
#endif

namespace tympan
{

    class ITYPolygonTriangulator
    {
    public:
        virtual ~ITYPolygonTriangulator() {};

        /**
         * @brief Export the surface as a triangular mesh
         *
         * NB : This function expects empty deques and will clear the deque passed.
         * TODO : It would be better to use output iterators
         *
         * @param points output argument filled with the vertices of the triangulation
         * @param triangles output argument filled with the faces of the triangulation
         */
        virtual void
        exportMesh(
                   std::deque<OPoint3D>& points,
                   std::deque<OTriangle>& triangles) const = 0;

    }; // class ITYPolygonTriangulator

    std::unique_ptr<ITYPolygonTriangulator>
    make_polygon_triangulator(const TYPolygon& poly);

    /**
     * @brief Join a sequence of polygons into a single polygon.
     *
     * @param polygons input TYPolygon as a std::deque
     * @param output TYPolygon result of the union of input polygons
     */
    void join_polygons(const std::deque<TYPolygon> polygons, TYPolygon& res);
} // namespace tympan

#endif // TYMPAN__CGAL_BRIDGE_HPP__INCLUDED
