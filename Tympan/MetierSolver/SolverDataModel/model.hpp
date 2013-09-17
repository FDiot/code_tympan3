/**
 * \file model.hpp
 * \brief This file provides the top-level declaration for the model
 *
 * \date jul. 12 2013
 * \author Anthony Truchet <anthony.truchet@logilab.fr>
 */

#ifndef TYMPAN__MODEL_H__INCLUDED
#define TYMPAN__MODEL_H__INCLUDED


#include "data_model_common.hpp"
#include "entities.hpp"

namespace tympan
{

class SolverModel
{
public:

	SolverModel(){};
	virtual ~SolverModel(){};

    /**
     * @brief Maps a \c TYPoint or \c OPoint3D as a \c node_ref
     * @param point the \c TYPoint  or \c OPoint3D to be mapped
     * @return a smart pointer to the corresponding Node (created on the fly if needed)
     */
	node_idx make_node(const Point&);
	node_idx make_node(double x, double y, double z)
	{return make_node(Point(x, y, z)); }

	triangle_idx make_triangle(node_idx n1, node_idx n2, node_idx n3);

	size_t num_points() const //TODO move to implementation file
	{ return  all_nodes.size(); }

	size_t num_triangles() const //TODO move to implementation file
		{ return  all_triangles.size(); }

    AcousticTriangle& triangle(triangle_idx tri_idx)
    { return all_triangles[tri_idx]; }

    const AcousticTriangle& triangle(triangle_idx tri_idx) const
    { return all_triangles[tri_idx]; }

// TODO

public: // XXX Could / should be protected but this complicates testing

/*
	// Comparison utilities
	static bool equal_to (const node_idx & lhs, const node_idx & rhs)
	{ return *lhs == *rhs; };
	static bool equal_to (const Point& point, const node_idx & pnode)
	{ return point == *pnode; }
	static bool equal_to (const node_idx & pnode, const Point& point)
	{ return equal_to(point, pnode); };

	static bool less_than (const node_idx & lhs, const node_idx & rhs);
	static bool less_than (const Point& lhs, const Point& rhs);
*/

protected: // data members

	nodes_pool_t all_nodes;
	triangle_pool_t all_triangles;
};  // class SolverModel


} // namespace tympan




#endif // TYMPAN__MODEL_H__INCLUDED
