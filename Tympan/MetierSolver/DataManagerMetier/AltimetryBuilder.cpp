/**
 * @file AltimetryBuilder.cpp
 *
 * @brief The \c AltimetryBuilder is responsible to build a altimetry compatilbe
 *        with the groud material
 *
 * @author Anthony Truchet <anthony.truchet@logilab.fr>
 *
 */

#include <strstream>
#include <limits>

#include <CGAL/intersections.h>

#include <boost/foreach.hpp>
#include <boost/bind.hpp>
#include <boost/math/special_functions/fpclassify.hpp>

#include "Tympan/Tools/OMessageManager.h"
#include "Tympan/MetierSolver/DataManagerMetier/AltimetryBuilder.hpp"
#include "Tympan/MetierSolver/DataManagerMetier/Site/TYSiteNode.h"


namespace tympan
{

const double unspecified_altitude = std::numeric_limits<double>::quiet_NaN();

bool is_valid_altitude(double alti)
{ return !boost::math::isnan(alti); } // Could and should use std::isnan in C++ '11


MaterialPolygon::MaterialPolygon(const TYTerrain& terrain, const OMatrix& matrix) :
		material( terrain.getSol() )
{
	BOOST_FOREACH(const TYPoint& point, terrain.getListPoints())
		CGAL_Polygon::push_back( to_cgal2_transform(matrix, point) );
	assert(terrain.getListPoints().empty() || this->is_simple());
}

MaterialPolygon::MaterialPolygon(const TYTabPoint& contour, material_t ground, const OMatrix& matrix):
		material(ground)
{
	BOOST_FOREACH(const TYPoint& point, contour)
		CGAL_Polygon::push_back( to_cgal2_transform(matrix, point) );
	assert(contour.empty() || this->is_simple());
}


AltimetryBuilder::AltimetryBuilder()
{
}

AltimetryBuilder::~AltimetryBuilder()
{
}

// Main methods

void
AltimetryBuilder::process(TYTopographie& topography, bool use_emprise_as_level_curve)
{
    // This should be compared to the old method 'TYTopographie::collectPointsForAltimetrie'

    // No need to handle the 'emprise' it is implicitely handled by the merging
    // of the sub-sites, which is expected to be done before entering this method.
    // XXX This seems no to be exact
    TYTabCourbeNiveauGeoNode& level_curves = topography.getListCrbNiv();
    // If we don't have any level curve we force the emprise to become one.
    if (level_curves.empty())
        process_emprise(topography, true);
    else
        process_emprise(topography, use_emprise_as_level_curve);

    BOOST_FOREACH(LPTYCourbeNiveauGeoNode p_geo_node, level_curves)
    {
        // Handle the elements' transform
    	p_geo_node->updateMatrix();
        const OMatrix& matrix = p_geo_node->getMatrix();
        TYCourbeNiveau* p_curve =
            TYCourbeNiveau::safeDownCast(p_geo_node->getElement());
        process(*p_curve, matrix);
    }

    BOOST_FOREACH(LPTYPlanEauGeoNode p_geo_node, topography.getListPlanEau())
    {
        TYPlanEau* p_water =
            TYPlanEau::safeDownCast(p_geo_node->getElement());
    	p_geo_node->updateMatrix();
        const OMatrix& matrix = p_geo_node->getMatrix();
        // This assumes the _pCrbNiv is up to date (TODO to be checked)
        process(*p_water->getCrbNiv(), matrix); // Process the underlying level curve
        process(*p_water, matrix); // TYPlanEau inherits from TYTerain
    }

    BOOST_FOREACH(LPTYTerrainGeoNode p_geo_node, topography.getListTerrain())
    {
        TYTerrain* p_ground =
            TYTerrain::safeDownCast(p_geo_node->getElement());
    	p_geo_node->updateMatrix();
        const OMatrix& matrix = p_geo_node->getMatrix();
        process(*p_ground, matrix);
    }
}

void
AltimetryBuilder::process_emprise(TYTopographie& topography, bool as_level_curve)
{
	TYTabPoint& ty_tab_points = topography.getEmprise();
	// If required, we process the emprise as a level curve
    if (as_level_curve)
    {
    	/* cf. AltimetryBuilder::process(TYCourbeNiveau&, bool)*/
        // Fetch parent TYSite altitude
        TYSiteNode* pSiteNode = TYSiteNode::safeDownCast(topography.getParent());
        assert(pSiteNode && "This TYTopographie is expected to have a TYSiteNode as parent.");
        double alti = pSiteNode->getAltiEmprise();
        addAsConstraint(
        		ty_tab_points | transformed(boost::bind(to_cgal2_info, alti, _1)),
        		true);
    }
    // and we process it as a default terrain
    material_polygons.push_back(new MaterialPolygon(
    		ty_tab_points, topography.getDefTerrain()->getSol() ));
}

void
AltimetryBuilder::process(const TYTerrain& zone_terrain, const OMatrix& matrix)
{
    // Pass the GeoNode transform to MaterialPolygon
    material_polygons.push_back(new MaterialPolygon(zone_terrain, matrix));
}

void
AltimetryBuilder::process(TYCourbeNiveau& courbe_niveau, const OMatrix& matrix, bool closed)
{
    TYTabPoint& ty_tab_points = courbe_niveau.getListPoints();
    double alti = courbe_niveau.getAltitude();
    /* We build a boost::Range of CGAL points on the fly  by calling
     * to_cgal2_info(alti, p) for each point p in ty_tab_points,
     * and we pass this range to addAsConstraint.
     * (This is akin to using itertools in Python)
     *
     * Cf. doc for boost::bind and  boost::range */

    addAsConstraint(
        ty_tab_points | transformed(boost::bind(to_cgal2_info, alti, matrix, _1)),
        closed);
}

double
AltimetryBuilder::computeAltitude(const CGAL_Point& p)
{
    // Query the triangulation for the location of p
    CDT::Locate_type lt;
    int li;
    CDT::Face_handle fh = alti_cdt.locate(p,lt, li);
    switch(lt)
    {
    case CDT::VERTEX:
        // The point IS a vertex of the known altimetry triangulation
        return fh->vertex(li)->info().altitude;
    case CDT::EDGE:
        // We test that the face is a finite face or we change for the opposite face
        // (which must be finite) then we fall back on the finite face case
        if (alti_cdt.is_infinite(fh))
        {
            fh = fh->neighbor(li) ;
            if (alti_cdt.is_infinite(fh))
                throw AlgorithmicError("AltimetryBuilder::computeAltitude: degenerate triangulation");
        } // no break : we DO want to fall back on the finite face case

    case CDT::FACE:
    {
        // This is the simple case of an inner/finite face
        // We rely on CGAL intersection computation in 3D between the triangle
	// and a vertical line going through p
	double x[3], y[3], z[3];
	for(int i=0; i<3; i++)
	{
		CGAL_Gt::Point_2 q = fh->vertex(i)->point();
		x[i] = q.x();
		y[i] = q.y();
		z[i] = fh->vertex(i)->info().altitude;
	}
	CGAL_Gt::Triangle_3 tri3(
			CGAL_Gt::Point_3(x[0], y[0], z[0]),
			CGAL_Gt::Point_3(x[1], y[1], z[1]),
			CGAL_Gt::Point_3(x[2], y[2], z[2]));
	CGAL_Gt::Line_3 line(CGAL_Gt::Point_3(p.x(), p.y(), 0), CGAL_Gt::Vector_3(0, 0, 1));

        CGAL::Object result = CGAL::intersection(tri3, line);
        // We check that the intersection is actually a point as it should
        const CGAL_Gt::Point_3 *ip = CGAL::object_cast<CGAL_Gt::Point_3>(&result);
        if (!ip)
            throw AlgorithmicError("Geometrical inconsistency !");
        // We assert the (x,y) coordinate of the intersection match those of the point
        assert( std::max(fabs(ip->x()-p.x()), fabs(ip->y()-p.y())) <= 1e-3 );
        return ip->z();
    }
    case CDT::OUTSIDE_CONVEX_HULL:
        // The point passed is outside the convex hull of the points
        // of all the level curves : it is not possible to give it
        // a significant altitude : please adjust your `emprise`
        OMessageManager::get()->warning(
            "%s : The point at (%lf, %lf) is outside the convex hull of point of known altitude : returning NaN",
            "AltimetryBuilder::computeAltitude", p.x(), p.y());
        return unspecified_altitude;

    case CDT::OUTSIDE_AFFINE_HULL:
        throw AlgorithmicError("This should never happen (is the altimetry empty or in 1D ?)");
    }
    throw AlgorithmicError("The above switch should be exhaustive !");
}

void
AltimetryBuilder::insertMaterialPolygonsInTriangulation()
{
	// Do check that alti_cdt is clear
	assert(alti_cdt.number_of_vertices() == 0 &&
			"Altitude can't be modified afterwards");
	// We copy the current triangulation, which will be used to compute alitudes
    alti_cdt = cdt;
    BOOST_FOREACH(MaterialPolygon& poly, material_polygons)
    {
    	addAsConstraint(poly |
    		transformed( boost::bind(&AltimetryBuilder::build_point_info, this, _1)),
    		true);
    }
    if (!cdt.is_valid())
    {
        throw AlgorithmicError("AltimetryBuilder::insertMaterialPolygonsInTriangulation: invalid triangulation");
    }
    // Give an altitude to the points inserted as intersections of contraints.
	for(CDT::Vertex_iterator vit = cdt.vertices_begin();
            vit != cdt.vertices_end(); ++vit)
	{
		CGAL_Point p = vit->point();
		if( !is_valid_altitude(vit->info().altitude) )
                    vit->info().altitude = computeAltitude(p);
	}
}

void
AltimetryBuilder::indexFacesMaterial()
{
    if (cdt.number_of_faces() == 0)
    {
        throw AlgorithmicError("AltimetryBuilder::indexFacesMaterial: empty triangulation");
    }
    unsigned N_poly = material_polygons.size();
    poly_to_faces.resize(N_poly);
    // For each face in the triangulation
    for (CDT::Finite_faces_iterator f_it = cdt.finite_faces_begin();
         f_it != cdt.finite_faces_end();
         ++f_it)
    {
        // Extract a representative inner point
        CGAL_Point center = CGAL::centroid(cdt.triangle(f_it));
        // For each polygon in the material polygons
        for (int i_poly = 0; i_poly < N_poly; ++i_poly)
        {
            // Test whether the Face is in the polygon
            const CGAL_Polygon& poly = material_polygons[i_poly];
            switch (poly.bounded_side(center))
            {
                case CGAL::ON_UNBOUNDED_SIDE:
                    continue;
                case CGAL::ON_BOUNDED_SIDE :
                case CGAL::ON_BOUNDARY: //XXX Check special case
                    face_to_poly.insert(std::make_pair(f_it, i_poly)); // face_to_poly[f_it] '+=' i_poly
                    poly_to_faces[i_poly].insert(f_it);
                    break;
            }
        }
    }
}

std::pair<bool, bool>
AltimetryBuilder::compare_polygons(material_polygon_handle_t p1, material_polygon_handle_t p2, face_set_t* p_intersect)
{
    face_set_t& fs1 = poly_to_faces[p1];
    face_set_t& fs2 = poly_to_faces[p2];
    bool one_in_two = std::includes(fs2.begin(), fs2.end(), fs1.begin(), fs1.end());
    bool two_in_one = std::includes(fs1.begin(), fs1.end(), fs2.begin(), fs2.end());
    if (!(one_in_two || two_in_one) && p_intersect)
    {
        p_intersect->clear();
    }
    std::set_intersection(fs1.begin(), fs1.end(), fs2.begin(), fs2.end(),
                          std::inserter(*p_intersect, p_intersect->begin()));
    return std::make_pair(one_in_two, two_in_one);
}


bool AltimetryBuilder::poly_comparator::operator()(face_to_material_poly_t::value_type& fp1,
                                                   face_to_material_poly_t::value_type& fp2) const
{
    material_polygon_handle_t& p1(fp1.second);
    material_polygon_handle_t& p2(fp2.second);
    face_set_t intersect;
    std::pair<bool, bool> p = parent.compare_polygons(p1, p2, &intersect);
    if (p.first | p.second)
        // If comparable, test for strict inclusion of p1 in p2
    {
        return p.first & !p.second;
    }
    else
    {
        throw NonComparablePolygons(p1, p2, intersect);
    }
}


void
AltimetryBuilder::labelFaces()
{
    if (face_to_poly.size() == 0)
    {
        throw AlgorithmicError("AltimetryBuilder::labelFaces: empty triangulation");
    }
    // For each face in the triangulation
    for (CDT::Finite_faces_iterator f_it = cdt.finite_faces_begin();
         f_it != cdt.finite_faces_end();
         ++f_it)
    {
    	// Extract the range of material polygon (handles) containing the face f_it
        std::pair<face_to_material_poly_t::iterator, face_to_material_poly_t::iterator> p =
            face_to_poly.equal_range(f_it);
        // Creates a comparator function object to compare polygons for inclusion
        poly_comparator cmp(*this);
        // Search for the minimal - ie most specific - polygon amoung them
        face_to_material_poly_t::iterator min_p_it = std::min_element(p.first, p.second, cmp);
        assert( min_p_it != p.second && "This should never happen.");
        face_handle_t face_h = min_p_it->first;
        material_polygon_handle_t poly_h = min_p_it->second;
        // Extract the associated material.
        material_t mater = material_polygons[poly_h].material;
        face_h->info().material = mater;
    }
}

std::pair<unsigned, unsigned>
AltimetryBuilder::count_edges() const
{
    unsigned edges = 0;
    unsigned constrained = 0;
    for (CDT::Finite_edges_iterator eit = cdt.finite_edges_begin();
         eit != cdt.finite_edges_end();
         ++eit)
    {
        ++edges;
        if (cdt.is_constrained(*eit))
        {
            ++constrained;
        }
    }
    return std::make_pair(edges, constrained);
}

void AltimetryBuilder::exportMesh(
    std::deque<OPoint3D>& points,
    std::deque<OTriangle>& triangles,
    std::deque<material_t>* p_materials) const
{
	assert(points.size()==0 &&
               "Output arguments 'points' is expected to be initially empty");
	assert(triangles.size()==0 &&
               "Output arguments 'triangles' is expected to be initially empty");
	assert(!p_materials || p_materials->size()==0 &&
               "Output arguments '*p_materials' is expected to be initially empty");

	std::map<CDT::Vertex_handle, unsigned> handle_to_index;

	for(CDT::Finite_vertices_iterator vit = cdt.finite_vertices_begin();
		vit	!= cdt.finite_vertices_end(); ++vit)
	{
		const unsigned i = points.size();
                // TODO Should extract this in a dedicated method - cf. ticket #1469664
		Point p = this->from_cgal(vit);
                if (!is_valid_altitude(p._z))
                {
                    OMessageManager::get()->error(
                        "%s : Invalid altitude for point %d (%lf, %lf, %lf)\n",
                        "AltimetryBuilder::exportMesh", i,  p._x, p._y, p._z);
                    // Try to recompute the altitude even if is
                    // supposedly computed on the fly and this late
                    // attempt is likely pointless.
                    double alti = computeAltitude(vit->point());
                    // NB Raising an exception here in case the altitude would still be invalid
                    // is tempting but a bit too strong : let the caller check as
                    // invalid altitudes seems to be better handled at the caller's level.
                }

		points.push_back(p);
		const bool ok = handle_to_index.insert( std::make_pair(vit, i) ).second;
		assert(ok && "Vertex handle should be unique.");
	}

	// Handle triangles
	for(CDT::Finite_faces_iterator fit = cdt.finite_faces_begin();
		fit	!= cdt.finite_faces_end(); ++fit)
	{
            if(p_materials) {
		const material_t& material = fit->info().material;
                p_materials->push_back(material);
            }
            // Build an OTriangle from 3 indices
            unsigned i[3], j;
            for(j=0; j<3; ++j)
                i[j] = handle_to_index[fit->vertex(j)];
            // Store it and update its associated OPoints
            triangles.push_back(OTriangle(i[0], i[1], i[2]));
            OTriangle& tri = triangles.back();
            for(j=0; j<3; ++j)
            {
                tri.vertex(j) = points[i[j]];
                assert( tri.vertex(j)== from_cgal(fit->vertex(j)));
            }
	}
	assert( points.size() == number_of_vertices());
	assert( triangles.size() == number_of_faces());
	assert( !p_materials || p_materials->size() == number_of_faces());
}

// Vizualization helpers
// If we use the IHM, we include some visualization utilities
#if TY_USE_CGAL_QT_IFACE

void
AltimetryBuilder::addMaterialLinesToScene(QGraphicsScene* scene) const
{
    BOOST_FOREACH(const CGAL_Polygon & p, material_polygons)
    {
        addPolygonToScene(scene, p, QPen(Qt::green, 0.05, Qt::DotLine));
    }
}

void
AltimetryBuilder::addTriangulationToScene(QGraphicsScene* scene) const
{
    // http://www.cgal.org/Manual/latest/doc_html/cgal_manual/GraphicsView_ref/Class_Qt--ConstrainedTriangulationGraphicsItem.html
    CGAL::Qt::ConstrainedTriangulationGraphicsItem<const CDT>* p_cdt_gitem =
        new CGAL::Qt::ConstrainedTriangulationGraphicsItem<const CDT>(&cdt);
    p_cdt_gitem->setConstraintsPen(QPen(Qt::blue, 0.05));
    scene->addItem(p_cdt_gitem);
}

void
AltimetryBuilder::addPolygonToScene(QGraphicsScene* scene, const CGAL_Polygon& p, QPen pen)
{
    CGAL::Qt::PolygonGraphicsItem<const CGAL_Polygon>* p_gi =
        new CGAL::Qt::PolygonGraphicsItem<const CGAL_Polygon>(&p);
    p_gi->setEdgesPen(pen);
    scene->addItem(p_gi);
}

QGraphicsSimpleTextItem*
AltimetryBuilder::drawText(QGraphicsScene* scene, const CGAL_Point& pos, const std::string& text, double scale)
{
	QGraphicsSimpleTextItem* item = scene->addSimpleText(
			QString::fromStdString(text));
	item->setPos(pos.x(), pos.y());
	item->setScale(scale);
	return item;
}

void
AltimetryBuilder::addVerticesInfo(QGraphicsScene* scene) const
{
	for(CDT::Finite_vertices_iterator vit = cdt.finite_vertices_begin();
		vit	!= cdt.finite_vertices_end(); ++vit)
	{
		CDT::Point p = vit->point();
		std::stringstream txt;
		double alti = vit->info().altitude;
		if (!is_valid_altitude(alti))
			txt << "-";
		else
			txt << alti;
		drawText(scene, p, txt.str() );
	}
}

void
AltimetryBuilder::addFacesInfo(QGraphicsScene* scene) const
{
	for(CDT::Finite_faces_iterator fit = cdt.finite_faces_begin();
		fit	!= cdt.finite_faces_end(); ++fit)
	{
		std::stringstream txt;
		material_t material = fit->info().material;
		txt << material->getName().at(0).toAscii(); // First character of the material name
		drawText(scene, CGAL::centroid(cdt.triangle(fit)), txt.str() );
	}

}


std::pair<QGraphicsView*, QGraphicsScene*>
AltimetryBuilder::buildView(double xmin, double ymin, double xmax, double ymax)
{
    QGraphicsScene* scene = new QGraphicsScene(xmin, ymin, xmax, ymax);
    //scene->addRect(xmin, ymin, xmax, ymax, QPen(Qt::gray, 0.2, Qt::DotLine));
    addTriangulationToScene(scene);
    addMaterialLinesToScene(scene);

    QGraphicsView* view = new QGraphicsView(scene);
    CGAL::Qt::GraphicsViewNavigation* navigation = new CGAL::Qt::GraphicsViewNavigation();
    view->installEventFilter(navigation);
    view->viewport()->installEventFilter(navigation);
    view->setRenderHint(QPainter::Antialiasing);
    view->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    view->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    view->fitInView(xmin, ymin, xmax, ymax,Qt::KeepAspectRatio);
    return std::make_pair(view, scene);
}

#endif // TY_USE_CGAL_QT_IFACE



} /* namespace tympan */
