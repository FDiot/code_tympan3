/*
 * Copyright (C) <2012> <EDF-R&D> <FRANCE>
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
*/

/**
 * @file AltimetryBuilder.cpp
 *
 * @brief The \c AltimetryBuilder is responsible to build a altimetry compatilbe
 *        with the groud material
 */

#include <strstream>

#include <CGAL/intersections.h>

#include <boost/foreach.hpp>
#include <boost/bind.hpp>


#include "Tympan/MetierSolver/SolverDataModel/AltimetryBuilder.hpp"


namespace tympan
{

const double unspecified_altitude = -1000.0;


MaterialPolygon::MaterialPolygon(const TYTerrain& terrain)
{
    //TODO
}

AltimetryBuilder::AltimetryBuilder()
{
}

AltimetryBuilder::~AltimetryBuilder()
{
}

// Main methods

void
AltimetryBuilder::process(TYTopographie& topography)
{
    // No need to handle the 'emprise' it is implicitely handled by the merging
    // of the sub-sites, which is expected to be done before entering this method.
    BOOST_FOREACH(LPTYCourbeNiveauGeoNode p_geo_node, topography.getListCrbNiv())
    {
        TYCourbeNiveau* p_curve =
            TYCourbeNiveau::safeDownCast(p_geo_node->getElement());
        process(*p_curve);
    }
    BOOST_FOREACH(LPTYTerrainGeoNode p_geo_node, topography.getListTerrain())
    {
        TYTerrain* p_ground =
            TYTerrain::safeDownCast(p_geo_node->getElement());
        process(*p_ground);
    }
}

void
AltimetryBuilder::process(const TYTerrain& zone_terrain)
{
    material_polygons.push_back(new MaterialPolygon(zone_terrain));
}

void
AltimetryBuilder::process(TYCourbeNiveau& courbe_niveau, bool closed)
{
    TYTabPoint& ty_tab_points = courbe_niveau.getListPoints();
    double alti = courbe_niveau.getAltitude();
    /* We build a boost::Range of CGAL points on the fly  by calling
     * to_cgal_info(alti, p) for each point p in ty_tab_points,
     * and we pass this range to addAsConstraint.
     * (This is akin to using itertools in Python)
     *
     * Cf. doc for boost::bind and  boost::range */
    addAsConstraint(
        ty_tab_points | transformed(boost::bind(to_cgal_info, alti, _1)),
        closed);
}

double
AltimetryBuilder::computeAltitude(const CGAL_Point& p)
{
    // Query the triangulation for the location of p
    CDT::Face_handle fh = alti_cdt.locate(p);
    if (alti_cdt.is_infinite(fh))
    {
        throw AlgorithmicError();
    }
    // We rely on CGAL intersection computation in 3D between the triangle
    // and a vertical line going through p
    double x[3], y[3], z[3];
    for (int i = 0; i < 3; i++)
    {
        Gt::Point_2 q = fh->vertex(i)->point();
        x[i] = q.x();
        y[i] = q.y();
        z[i] = fh->vertex(i)->info().altitude;
    }
    Gt::Triangle_3 tri3(
        Gt::Point_3(x[0], y[0], z[0]),
        Gt::Point_3(x[1], y[1], z[1]),
        Gt::Point_3(x[2], y[2], z[2]));
    Gt::Line_3 line(Gt::Point_3(p.x(), p.y(), 0), Gt::Vector_3(0, 0, 1));

    CGAL::Object result = CGAL::intersection(tri3, line);
    // We check that the intersection is actually a point as it should
    if (const Gt::Point_3* ip = CGAL::object_cast<Gt::Point_3>(&result))
    {
        assert(std::max(fabs(ip->x() - p.x()), fabs(ip->y() - p.y())) <= 1e-3);
        return ip->z();
    }
    else
    {
        throw AlgorithmicError();
    }
}

void
AltimetryBuilder::insertMaterialPolygonsInTriangulation()
{
    // Do check that alti_cdt is clear
    assert(alti_cdt.number_of_vertices() == 0 &&
           "Altitude can't be modified afterwards");
    // We copy the current triangulation, which will be used to compute alitudes
    alti_cdt = cdt;
    BOOST_FOREACH(MaterialPolygon & poly, material_polygons)
    {
        addAsConstraint(poly |
                        transformed(boost::bind(&AltimetryBuilder::build_point_info, this, _1)),
                        true);
    }
    if (!cdt.is_valid())
    {
        throw AlgorithmicError();
    }
    // Give an altitude to the points inserted as intersections of contraints.
    for (CDT::Vertex_iterator vit = cdt.vertices_begin();
         vit != cdt.vertices_end(); ++vit)
    {
        CGAL_Point p = vit->point();
        if (vit->info().altitude != unspecified_altitude)
        {
            continue;
        }
        vit->info().altitude = computeAltitude(p);
    }
}

void
AltimetryBuilder::indexFacesMaterial()
{
    if (cdt.number_of_faces() == 0)
    {
        throw AlgorithmicError();
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
        throw AlgorithmicError();
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
        assert(min_p_it != p.second && "This should never happen.");
        face_handle_t face_h = min_p_it->first;
        material_polygon_handle_t poly_h = min_p_it->second;
        // Extract the associated material.
        material_t mater = material_polygons[poly_h].material;
        face_h->info().material = mater;
    }
}

std::pair<unsigned, unsigned>
AltimetryBuilder::count_edges()
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
    for (CDT::Finite_vertices_iterator vit = cdt.finite_vertices_begin();
         vit != cdt.finite_vertices_end(); ++vit)
    {
        CDT::Point p = vit->point();
        std::stringstream txt;
        double alti = vit->info().altitude;
        if (alti == unspecified_altitude)
        {
            txt << "-";
        }
        else
        {
            txt << alti;
        }
        drawText(scene, p, txt.str());
    }

}

void
AltimetryBuilder::addFacesInfo(QGraphicsScene* scene) const
{
    for (CDT::Finite_faces_iterator fit = cdt.finite_faces_begin();
         fit != cdt.finite_faces_end(); ++fit)
    {
        std::stringstream txt;
        material_t material = fit->info().material;
        txt << material[0]; // First character of the material name
        drawText(scene, CGAL::centroid(cdt.triangle(fit)), txt.str());
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
    view->fitInView(xmin, ymin, xmax, ymax, Qt::KeepAspectRatio);
    return std::make_pair(view, scene);
}

#endif // TY_USE_CGAL_QT_IFACE



} /* namespace tympan */

