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


    class IAltimetryBuilder
    {
    protected:
        IAltimetryBuilder() {};

    public:
        virtual ~IAltimetryBuilder() {};

        /**
         * @brief Walk through a TYTopograpy and process all its elements
         * @param topography ref. to a TYTopography to be processed.
         * @param use_emprise_as_level_curve if the emprise should be use as a level curve.
         */
        virtual
        void
        process(TYTopographie& topography, bool use_emprise_as_level_curve = true) = 0;

        /**
         * @brief Take \a ground_area into account.
         *
         * Adapt \a ground_area into a \c MaterialPolygon and store it for later
         * processing by \c insertMaterialPolygonsInTriangulation().
         *
         * @param matrix a transform to be applied to \c ground_area before processing
         * @param ground_area ref. to a \c TYTerrain to be processed.
         */
        void
        virtual
        process(const TYTerrain& ground_area, const OMatrix& matrix = OMatrix()) = 0;

        /**
         * @brief Take \a level_curve into account.
         *
         * This method immediately adds the \a level_curve as contraints to the
         * triangulaiton
         *
         * @param matrix a transform to be applied to \c ground_area before processing
         * @param closed if true the last point will be linked to the first
         * @param level_curve ref. to a level curve to be processed.
         */
        virtual
        void
        process(TYCourbeNiveau& level_curve,
                const OMatrix& matrix = OMatrix(), bool closed = false) = 0;

        /**
         * @brief Processes the emprise
         * @param topography
         * @param as_level_curve
         */
        virtual
        void
        process_emprise(TYTopographie& topography, bool as_level_curve = true) = 0;

        /**
         * @brief Insert the stored material polygons into the triangulation
         *
         * When the \c process method is called on TYTerrain instances, they  are
         * only adapted into \c material_polygons which are then stored, but not taken
         * into account in the triangulation.
         *
         * This method actually integrates them into the triangulation AND as a side
         * effect *freezes*  the altimetry (it is no longer possible to insert level curves.)
         *
         */
        virtual
        void
        insertMaterialPolygonsInTriangulation() = 0;

        /**
         * @brief Build an index of which face belong to which Meterial polygon
         *
         * This method builds the \c face_to_poly and \c poly_to_faces members
         */
        virtual
        void
        indexFacesMaterial() = 0;

        /**
         * @brief Label each face with its material.
         *
         * Can be called only once \c insertMaterialPolygonsInTriangulation() has
         * been called. This method associate each face (aka triangle) with its
         * material : ie the material of the most specific MaterialPolygon.
         */
        virtual
        void
        labelFaces(LPTYSol default_material) = 0;

        /**
         * @brief Export the altimetry as a triangular mesh
         *
         * This function expect empty deques and will clear the deque passed.
         *
         * @param points output argument filled with the vertices of the triangulation
         * @param triangles output argument filled with the faces of the triangulation
         * @param p_materials optional output argument filled with the materials of the faces
         */
        virtual
        void
        exportMesh(std::deque<OPoint3D>& points,
                   std::deque<OTriangle>& triangles,
                   std::deque<LPTYSol>* p_materials = NULL) const = 0;

        /**
         * @brief Counts the number of (constrained) edges
         * @return a pair (#edges, #constrained edges)
         */
        virtual
        std::pair<unsigned, unsigned>
        count_edges() const = 0;

        /**
         * @brief Get number of vertices
         * @return number of vertices (aka points) of the altimetry
         */
        virtual
        unsigned number_of_vertices() const = 0;

        /**
         * @brief Get number of faces
         * @return number of faces (aka triangles) of the altimetry
         */
        virtual
        unsigned number_of_faces() const = 0;


        class NonComparablePolygons
            : public tympan::invalid_data
        {
        protected:
            NonComparablePolygons() {};
            NonComparablePolygons(const std::string& desc);
        public:
            virtual ~NonComparablePolygons() BOOST_NOEXCEPT {};
        };


    }; // class IAltimetryBuilder

    std::unique_ptr<IAltimetryBuilder>
    make_altimetry_builder();


} // namespace tympan

#endif // TYMPAN__CGAL_BRIDGE_HPP__INCLUDED
