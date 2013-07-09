/**
 * \file test_entities.cpp
 * \test Test the representation of entities of the model.
 *
 * \date nov. 6 2012
 * \author Anthony Truchet <anthony.truchet@logilab.fr>
 * \author Damien Garaud <damien.garaud@logilab.fr>
 */
#include <boost/foreach.hpp>

#include "gtest/gtest.h"

#include "Tympan/MetierSolver/SolverDataModel/entities.hpp"
#include "Tympan/MetierSolver/SolverDataModel/relations.hpp"

using namespace tympan;

using std::cout;
using std::cerr;
using std::endl;

TEST(BasicInstanciationTests, dummy) {
	Point p1(0, 1, 0);
	Point p2(0, 1, 2);
	double angle = 1.2;
	DiffractionEdge edge(p1, p2, angle);
	AcousticTriangle surf();
	// XXX relation edge_of ?
	AcousticBuildingMaterial("béton");
	// XXX relation made_of
	ASSERT_TRUE(true);
}

TEST(BasicInstanciationTests, acoustic_diffraction_edge)
{
    // Normal vector for an acoustic surface.
    double x=1., y=1., z=0.;
    AcousticTriangle::pointer p_surface(new AcousticTriangle());

    // Diffraction edge.
    double a_x=0., a_y=0., a_z=0.;
    double b_x=0., b_y=1., b_z=0.;
    Point a(a_x, a_y, a_y);
    Point b(b_x, b_y, b_z);
    double angle = 0.56;
    DiffractionEdge::pointer p_diff_edge(new DiffractionEdge(a, b, angle));
    DiffractionEdge::pointer p_diff_edge_2(
        new DiffractionEdge(Point(a_x, a_y, a_z), Point(1., 0., b_z), angle + 0.5));
    DiffractionEdge::pointer p_diff_edge_3(
        new DiffractionEdge(Point(a_x, 1., a_z), Point(1., b_y, b_z), angle - 1.6));

    // Create the relation between previous instances.
    edge_of_rdef::add(p_diff_edge, p_surface);
    edge_of_rdef::add(p_diff_edge_2, p_surface);
    edge_of_rdef::add(p_diff_edge_3, p_surface);

    // Subject view, i.e. viewed from a 'DiffractionEdge'.
    edge_of_rdef::subject_view::type& edge_of = edge_of_rdef::by<subject>();
    EXPECT_EQ(edge_of.count(p_diff_edge), 1);

    // Object view view, i.e. viewed from an 'AcousticTriangle'.
    edge_of_rdef::object_view::type& contains = edge_of_rdef::by<object>();
    EXPECT_EQ(contains.count(p_surface), 3);

    // Loop on all diffraction edges related to a specific acoustic surface.
    BOOST_FOREACH(DiffractionEdge::pointer edge, p_surface->rev_iter<edge_of_rdef>())
        EXPECT_TRUE( (edge == p_diff_edge)  ||
                     (edge == p_diff_edge_2) ||
                     (edge == p_diff_edge_3) );

    // Loop on the single acoustic surface related to a specific diffraction edge.
    BOOST_FOREACH(AcousticTriangle::pointer surf, p_diff_edge->iter<edge_of_rdef>())
        EXPECT_TRUE( (surf == p_surface) );
}


TEST(BasicInstanciationTests, acousticSurfaceCreation) {
	Point p1(0, 1, 0);
	Point p2(0, 1, 2);
	Point p3(0, 0, 0);

	Node::pointer n1( new Node(p1) );
	Node::pointer n2( new Node(p2) );
	Node::pointer n3( new Node(p3) );

	AcousticTriangle::pointer surf =
			AcousticTriangle::make_triangle(n1, n2, n3);
	// TODO This is just a complicated way to get the one point
	// the Yams++ interface needs to be improved to handle 1 cardinalities.
	Node::pointer node0 (*boost::begin(surf->iter<has_node_0>()));
	ASSERT_EQ(n3->p, node0->p);
	ASSERT_EQ(n3, node0);
}
