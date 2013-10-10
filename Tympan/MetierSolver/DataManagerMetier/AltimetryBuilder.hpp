/**
 * @file AltimetryBuilder.hpp
 *
 * @brief The \c AltimetryBuilder is responsible to build a altimetry compatilbe
 *        with the groud material.
 *
 *\b NB This module heavily relies on CGAL to perform the underlying contrained
 * triangulations.
 *
 * @author Anthony Truchet <anthony.truchet@logilab.fr>
 *
 */

#ifndef TYMPAN__ALTIMETRYBUILDER_HPP__INCLUDED
#define TYMPAN__ALTIMETRYBUILDER_HPP__INCLUDED

// Standard and Boost includes
#include <string>
#include <cassert>
#include <map>
#include <functional>
#include <algorithm>

// CGAL related Includes
#include "Tympan/MetierSolver/ToolsMetier/cgal_tools.hpp"

#include <boost/ptr_container/ptr_deque.hpp>
// http://www.boost.org/doc/libs/1_52_0/libs/ptr_container/doc/examples.html
#include <boost/foreach.hpp>
#include <boost/range.hpp>
#include <boost/range/iterator_range.hpp>
#include <boost/range/functions.hpp>
#include <boost/range/adaptors.hpp>
#include <boost/range/adaptor/transformed.hpp>
using boost::adaptors::transformed;


// Forward declaration for the benefit of TYAltimetrie and TYTopographie
namespace tympan {
class AltimetryBuilder;
class SolverDataModelBuilder;
}

// Tympan includes site side
#include "Tympan/MetierSolver/ToolsMetier/OPoint3D.h"
#include "Tympan/MetierSolver/DataManagerMetier/Site/TYTopographie.h"
#include "Tympan/MetierSolver/DataManagerMetier/EltTopographique/TYCourbeNiveau.h"
#include "Tympan/MetierSolver/DataManagerMetier/EltTopographique/TYTerrain.h"
#include "Tympan/MetierSolver/DataManagerMetier/EltTopographique/TYAltimetrie.h"

#undef max // XXX

// Tympan includes solver side
//#include "Tympan/MetierSolver/SolverDataModel/entities.hpp"
//#include "Tympan/MetierSolver/SolverDataModel/relations.hpp"
#include "Tympan/MetierSolver/SolverDataModel/data_model_common.hpp"


#define TY_USE_CGAL_QT_IFACE 1
#ifndef TY_USE_CGAL_QT_IFACE
#ifdef TY_USE_IHM
// If we use the IHM, we include some visualization utilities
#define TY_USE_CGAL_QT_IFACE 1
#else
#define TY_USE_CGAL_QT_IFACE 0
#endif // TY_USE_IHM
#endif // TY_USE_CGAL_QT_IFACE defined

#if TY_USE_CGAL_QT_IFACE
// We include some visualization utilities
#include <QPen>
#include <CGAL/Qt/PolygonGraphicsItem.h>
#include <CGAL/Qt/ConstrainedTriangulationGraphicsItem.h>
#include <CGAL/Qt/GraphicsViewNavigation.h>
#endif // TY_USE_CGAL_QT_IFACE


namespace tympan
{

bool is_valid_altitude(double alti);

/** @brief an alias to LPTYSol for representing a ground material */
typedef LPTYSol material_t;

/// @brief A constant representing yet unspecified altitude
extern const double unspecified_altitude;

/**
 * @brief Adaptor for \c TYTerrain
 */
class MaterialPolygon: public CGAL_Polygon
{
public:
    material_t material;

    /**
     * @brief Build a \c MaterialPolygon from a \c TYTerrain
     * @param terrain the TYTerrain to be adapted
     * @param matrix transform from the GeoNode holding \c terrain (default to identity)
     */
    MaterialPolygon(const TYTerrain& terrain, const OMatrix& matrix=OMatrix());

    /**
     * @brief Build a \c MaterialPolygon from a material and a polygon
     * @param contour a list of TYPoint representing the polygon
     * @param ground the ground material inside the polygon
     * @param matrix the transform from the GeoNode holding \c terrain (default to identity)
     */
    MaterialPolygon(const TYTabPoint& contour, material_t ground, const OMatrix& matrix=OMatrix());

    template <class InputRange>
    MaterialPolygon(InputRange poly, const material_t& material_):
        CGAL_Polygon(poly.begin(), poly.end()), material(material_)
    {
        assert(this->is_simple());
    }

    std::string material_name()
    { return material->getName().toStdString(); }

    CGAL_Polygon::Vertex_iterator begin() const { return vertices_begin(); }
    CGAL_Polygon::Vertex_iterator end() const {return vertices_end();}
};

/**
 * @brief the information to be associated with faces of the triangulation
 */
struct FaceInfo
{
    FaceInfo():
        material(new TYSol()) {}
    FaceInfo(const material_t mat):
        material(mat) {}
    material_t material;
};

/**
 * @brief the information to be associated with vertex of the triangulation
 */
struct VertexInfo
{
    VertexInfo():
        altitude(unspecified_altitude) {};

    VertexInfo(double altitude_):
        altitude(altitude_) {};

    VertexInfo(const VertexInfo& rhs) :
    	altitude(rhs.altitude) {}

    VertexInfo& operator=(const VertexInfo& rhs)
    {
        VertexInfo& lhs = *this;
        lhs.altitude = rhs.altitude;
        return lhs;
    }

    double altitude;
};

/* Please refer to the following example in CGAL doc for meaning of those typedef
http://www.cgal.org/Manual/latest/doc_html/cgal_manual/Triangulation_2/Chapter_main.html#Subsection_37.8.2
*/
typedef CGAL::Triangulation_vertex_base_with_info_2<VertexInfo, CGAL_Gt>Vbb;
typedef CGAL::Triangulation_face_base_with_info_2<FaceInfo, CGAL_Gt>    Fbb;
typedef CGAL::Constrained_triangulation_face_base_2<CGAL_Gt, Fbb>       Fb;
typedef CGAL::Triangulation_data_structure_2<Vbb, Fb>              TDS;
typedef CGAL::Exact_predicates_tag                                 Itag;
/// \c CDT is the type of the main CGAL object to build the triangulation.
typedef CGAL::Constrained_Delaunay_triangulation_2<CGAL_Gt, TDS, Itag>  CDT;
// typedef CDT::Point                                                 CGAL_Point;
typedef CGAL_Point2 CGAL_Point;


/**
 * @brief Conversion from a Tympan point to a CGAL point
 * @param p a \c OPoint3D
 * @return a \c CGAL_Point (2D) with  same x and y coordinates
 */
inline
CGAL_Point
to_cgal2(const OPoint3D& p)
{ return CGAL_Point(p._x, p._y); }

/**
 * @brief Conversion from a Tympan point to a CGAL point, applying a transformation before
 * @param matrix a transform to be applied to \c p before conversion
 * @param p  a \c OPoint3D
 * @return a \c CGAL_Point (2D) with  same x and y coordinates as \c matrix * \c p
 */
inline
CGAL_Point
to_cgal2_transform(const OMatrix& matrix, OPoint3D p)
{
	p = matrix * p;
	return CGAL_Point(p._x, p._y);
}


/**
 * @brief Conversion from a Tympan point to a CGAL point, setting the altitude
 * @param p  a \c OPoint3D
 * @return a pair of a 2D \c CGAL_Point and a \c VertexInfo holding the altitude.
 */
inline
std::pair<CGAL_Point, VertexInfo>
to_cgal2_info(double alti, OPoint3D& p)
{
	p._z = alti;
	return std::make_pair(to_cgal2(p), VertexInfo(alti));
}

/**
 * @brief Conversion from a Tympan point to a CGAL point,
 * @param matrix a transform to be applied to \c p before conversion
 * @param p  a \c OPoint3D
 * @return  a pair of a 2D \c CGAL_Point and a \c VertexInfo holding the altitude.
 */
inline
std::pair<CGAL_Point, VertexInfo>
to_cgal2_info(double alti, const OMatrix& matrix, OPoint3D& p)
{
	p._z = alti;
	return std::make_pair(to_cgal2_transform(matrix, p), VertexInfo(alti));
}


/**
 * @brief Conversion from a Tympan point to a CGAL point using its altitude
 * @param p  a \c OPoint3D
 * @return a pair of a 2D \c CGAL_Point and a \c VertexInfo holding the altitude.
 */
inline
std::pair<CGAL_Point, VertexInfo>
to_cgal2_info(const OPoint3D& p)
{ return std::make_pair(to_cgal2(p), VertexInfo(p._z)); }


/**
 * @brief Exception class to represent an algorithmic error and/or invalid data.
 */
struct AlgorithmicError :
        public std::exception
{
    AlgorithmicError() throw() {}
    AlgorithmicError(const char* msg_) : msg(msg_) {}
    virtual ~AlgorithmicError() throw() {}

    virtual const char * what() const throw() { return msg.c_str(); }

protected:
    const string msg;
};


/// @brief Test fixture used to test the AltimetryBuilder
class AltimetryFixture;

/**
 * @brief This class is responsible for building an Altimetry.
 *
 * An \c Altimetry is a triangulation of the ground compatible both with the
 * \c Topography (i.e. the level lines) and the ground aera defining the
 * materials.
 */
class AltimetryBuilder
{

public:
    // Types
    typedef boost::ptr_deque< MaterialPolygon >                         p_material_poly_container_t;
    typedef CDT                                                         triangulation_t;
    typedef unsigned                                                    material_polygon_handle_t;
    typedef triangulation_t::Face_handle                                face_handle_t;
    typedef std::set<triangulation_t::Face_handle>  face_set_t;
    typedef std::multimap< face_handle_t, material_polygon_handle_t>    face_to_material_poly_t;
    typedef std::vector< face_set_t >                                   material_poly_to_face_set_t;

    // Attributes
    /// Hold the whole triangulation
    triangulation_t cdt;
    /// Hold the sub-triangulation induced by the level curves only
    triangulation_t alti_cdt;
    p_material_poly_container_t material_polygons;

    // XXX Use boost::bimap for this ?
    /// Map a \c face_handle to the material polygon(s) it belongs to.
    face_to_material_poly_t face_to_poly;
    /// Map a material polygon to the set of the faces handles ocmposing it
    material_poly_to_face_set_t poly_to_faces;

public:
    AltimetryBuilder();
    virtual
    ~AltimetryBuilder();

    //TODO We should properly handle a simple state machine.

    // Main methods

    friend class SolverDataModelBuilder;

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
    void
    insertMaterialPolygonsInTriangulation();

    /**
     * @brief Walk through a TYTopograpy and process all its elements
     * @param topography ref. to a TYTopography to be processed.
     * @param use_emprise_as_level_curve if the emprise should be use as a level curve.
     */
    void
    process(TYTopographie& topography, bool use_emprise_as_level_curve = true);

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
    process(const TYTerrain& ground_area, const OMatrix& matrix=OMatrix());

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
    void
    process(TYCourbeNiveau& level_curve, const OMatrix& matrix=OMatrix(), bool closed = false);

    /**
     * @brief Processes the emprise
     * @param topography
     * @param as_level_curve
     */
    void
    process_emprise(TYTopographie& topography, bool as_level_curve = true);

    /**
     * @brief Add \c points_range as elements of the triangulation
     *
     * PointRange must satisfy the requirement for the boost Range concept
     * with value type beeing either CGAL_Point or pair of CGAL_Point
     * and VertexInfo.
     *
     * @param points_range A range over \c CGAL_Points
     * @param linked Whether to add consecutive points as contrained edges
     * @param closed Whether there is an edge between the last and first points
     * @tparam PointRange the type of the range of points
     */
    template <class PointRange>
    void
    addAsConstraint(PointRange points_range,
                         bool closed = false, bool linked = true);

    /**
     * @brief Compute altitudes from the alti_cdt triangulation and vertices info
     * @param p a 2D point
     * @return the altitude of \c p or \c unspecified_altitude if \c p is out-of-scope
     */
    double
    computeAltitude(const CGAL_Point& p) const;

    /**
     * @brief Auxilliary method used to insert points into the triangulation
     * @param p a CGAL point to be added
     * @return a pair of \c p and its associated VertexInfo
     */
    std::pair<CGAL_Point, VertexInfo>
    build_point_info(const CGAL_Point& p)
    { return std::make_pair(p, VertexInfo(computeAltitude(p))); }

public:

    /**
     * @brief Build an index of which face belong to which Meterial polygon
     *
     * This method builds the \c face_to_poly and \c poly_to_faces members
     */
    void
    indexFacesMaterial();

    /* TODO
     * Provide triangulation refinement following :
     * http://www.cgal.org/Manual/latest/doc_html/cgal_manual/Mesh_2/Chapter_main.html
     */

    /**
     * @brief This is a test for inclusion between polygons \a p1 and \a p2
     *
     * This method test whether \a p1 is included in \a p2 and/or the
     * reciprocal. \b NB : It requires the indexes computed by
     * \c indexFacesMaterial to be up to date !
     *
     * If the optional argument \a p_intersect is given, it must
     * point to a set of face handles. In case neither of the two polygons is
     * included in the other, this set will be cleared and their intersection
     * will be computed in it.
     *
     * @param p1 First polygon
     * @param p2 Second polygon
     * @param p_intersect
     * @return the pair (\a p1 in \a p2, \a p2 in \a p1)
     */
    std::pair<bool, bool>
    compare_polygons(material_polygon_handle_t p1, material_polygon_handle_t p2, face_set_t* p_intersect = NULL);

    /**
     * @ brief exception class raised by \c poly_comparator in case polygons
     * are not comparable.
     */
    struct NonComparablePolygons
    {
        material_polygon_handle_t p1, p2;
        face_set_t intersect;
        NonComparablePolygons(material_polygon_handle_t p1_, material_polygon_handle_t p2_, face_set_t intersect_)
            : p1(p1_), p2(p2_), intersect(intersect_) {}
    };


    /**
     * @brief Label each face with its material.
     *
     * Can be called only once \c insertMaterialPolygonsInTriangulation() has
     * been called. This method associate each face (aka triangle) with its
     * material : ie the material of the most specific MaterialPolygon.
     */
    void
    labelFaces();

    /**
     * @brief Insert a point and a constraint into the triangulation
     * @param p the point to be inserted
     * @param vh the Vertex handle of a point already inserted : a constraint
     *           is added between it and \c p if \c vh is specified.
     * @return the Vertex handle for the point \p
     */
    CDT::Vertex_handle
    insert(const CGAL_Point& p,
    	   CDT::Vertex_handle vh0 = CDT::Vertex_handle() // Default to a NULL-like handle
    ){
    	CDT::Vertex_handle vh1 = cdt.insert(p);
    	if(vh0 != CDT::Vertex_handle()) // If some contraint has been specified...
    		cdt.insert_constraint(vh0, vh1);
    	return vh1;
    }

    /**
     * @brief Insert a point and some information into the triangulation
     * @param p a pair made of the point to be inserted and its information
     * @param vh the Vertex handle of a point already inserted : a constraint
     *        is added between it and \c p
     * @return the Vertex handle for the point
     */
    CDT::Vertex_handle
    insert(const std::pair<CGAL_Point, VertexInfo>& p,
    		CDT::Vertex_handle vh0 = CDT::Vertex_handle()
    ){
    	CDT::Vertex_handle vh1 = insert(p.first, vh0);
    	vh1->info() = p.second;
    	return vh1;
    }

    /**
     * @brief insert points and return the handles of vertices
     * @param points_range a range of points to be inserted
     * @tparam PointRange a type satisfying the concept of a range of points.
     * @return a (Boost) range of vertices' handles
     */
    template <class PointRange>
    std::deque<CDT::Vertex_handle>
    insert_range(PointRange points_range);

    /**
     * @brief Counts the number of (constrained) edges
     * @return a pair (#edges, #constrained edges)
     */
    std::pair<unsigned, unsigned>
    count_edges() const;

    /**
     * @brief Get number of vertices
     * @return number of vertices (aka points) of the altimetry
     */
    unsigned number_of_vertices() const
    { return cdt.number_of_vertices(); }

    /**
     * @brief Get number of faces
     * @return number of faces (aka triangles) of the altimetry
     */
    unsigned number_of_faces() const
    { return cdt.number_of_faces(); }

    /**
     * @brief Conversion from a CGAL Vertex_handle to a Tympan point
     * @param vh the CGAL vertex handle
     * @return coordinates of \c vh as a tympan point
     */
    Point
    from_cgal(const CDT::Vertex_handle& vh) const
    {
		const CDT::Point& p = vh->point();
		const VertexInfo& i = vh->info();
    	return Point(p.x(), p.y(), i.altitude);
    }

    /**
     * @brief Export the altimetry as a triangular mesh
     *
     * This function expect empty deques and will clear the deque passed.
     *
     * @param points output argument filled with the vertices of the triangulation
     * @param triangles output argument filled with the faces of the triangulation
     * @param p_materials optional output argument filled with the materials of the faces
     */
    void exportMesh(std::deque<OPoint3D>& points,
                    std::deque<OTriangle>& triangles,
                    std::deque<material_t>* p_materials=NULL) const;

protected:
    /**
     * @brief Helper function object to compare material polygons for inclusion.
     */
    struct poly_comparator :
            std::binary_function <
                face_to_material_poly_t::value_type&,
                face_to_material_poly_t::value_type&,
                bool >
    {
        AltimetryBuilder& parent;

        poly_comparator(AltimetryBuilder& parent_) : parent(parent_) {}

        // This will be called on iterators on a face_to_material_poly_t
        // so takes pair (face, polygons) as arguments, only compares the polygons
        bool operator()(face_to_material_poly_t::value_type& fp1,
                        face_to_material_poly_t::value_type& fp2) const;
    };

#if TY_USE_CGAL_QT_IFACE
    // Vizualization helpers
public:
    void
    addLevelLinesToScene(QGraphicsScene* scene) const;

    void
    addMaterialLinesToScene(QGraphicsScene* scene) const;

    void
    addTriangulationToScene(QGraphicsScene* scene) const;

    static QGraphicsSimpleTextItem*
    drawText(QGraphicsScene* scene,
    		const CGAL_Point& pos, const std::string& text, double scale=0.03);

    void
    addVerticesInfo(QGraphicsScene* scene) const;

    void
    addFacesInfo(QGraphicsScene* scene) const;

    std::pair<QGraphicsView*, QGraphicsScene*>
    buildView(double xmin, double ymin, double xmax, double ymax);

protected:
    static void
    addPolygonToScene(QGraphicsScene* scene, const CGAL_Polygon& p, QPen pen);
#endif // TY_USE_CGAL_QT_IFACE

    // This declares the fixture friend to facilitate testing
    friend class AltimetryFixture;
};

// ********** Template implementations **********

template <class PointRange>
std::deque<CDT::Vertex_handle>
AltimetryBuilder::insert_range(PointRange points_range)
{
	std::deque<CDT::Vertex_handle> result;
	BOOST_FOREACH(
			typename boost::range_reference<const PointRange>::type point,
			points_range) {
		result.push_back(insert(point));
	}
	return result;
}

template <class PointRange>
void
AltimetryBuilder::addAsConstraint(const PointRange points_range, bool closed, bool linked)
{
	CDT::Vertex_handle prev_vertex; // Default contructed is analogous to NULL
	BOOST_FOREACH(
			typename boost::range_reference<const PointRange>::type current,
			points_range)
	{
		CDT::Vertex_handle current_vertex = insert(current, prev_vertex);
		if  (linked)
			prev_vertex = current_vertex;
	}
        // If we link successive points with a constraint and we want a closed polygon
        // (only relevant for polygon with strictly more than 2 vertices) then
        // we add a constraint between the last point inserted and the first one.
	if (linked && closed && (points_range.size() > 2) )
	{
            //XXX Should we check that the point is not inserted twice
            CDT::Vertex_handle current_vertex = insert(*(points_range.begin()), prev_vertex);
	}
}



} /* namespace tympan */

#endif /* TYMPAN__ALTIMETRYBUILDER_HPP__INCLUDED */
