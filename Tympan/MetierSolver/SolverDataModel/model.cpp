/**
 * \file model.cpp
 * \brief Implementation for top-level of the model
 *
 * \date jul. 12 2013
 * \author Anthony Truchet <anthony.truchet@logilab.fr>
 */

#include <iostream>
#include <fstream>
#include <iomanip>

using namespace std;

#include "model.hpp"

namespace tympan
{

node_idx SolverModel::make_node(const Point& p) {
	all_nodes.push_back(p);
	return all_nodes.size()-1;
}

AcousticTriangle::AcousticTriangle(node_idx n1, node_idx n2, node_idx n3 )
{
	n[0]=n1;
	n[1]=n2;
	n[2]=n3;
}

/*
bool SolverModel::less_than (const node_idx & lhs, const node_idx & rhs)
{
	if (lhs==rhs)
		return false;

	const Point& lp = *lhs;
	const Point& rp = *rhs;

	return less_than(lp, rp);
}

bool SolverModel::less_than (const Point& lp, const Point& rp)
{
	if(lp._x == rp._x)
	{
		if(lp._y == rp._y)
			return lp._z < rp._z;
		else
			return lp._y < rp._y;
	}
	else
		return lp._x < rp._x;
}
*/

triangle_idx SolverModel::make_triangle(node_idx n1, node_idx n2, node_idx n3)
{
	/*
	SolverModel::node_idx tab[3] = {n1, n2, n3};
	assert(tab[1] == n2);
	assert(&tab[1] != &n2);
	const unsigned k0 = std::min_element(&tab[0], &tab[3], SolverModel::less_than) - &tab[0];
	assert( k0<3 );

	SolverModel::triangle tri;
	// We fill the triangle array starting from k0
	for(unsigned int k=k0; k<k0+3; k++)
		tri.n[k%3] = tab[k];
	return tri;
	*/
    all_triangles.push_back(AcousticTriangle(n1, n2, n3));
    return all_triangles.size() - 1;
}


void  SolverModel::export_triangles_soup(const std::string& filename)
{
    const static string nodes_suffix("_nodes.csv");
    const static string faces_suffix("_faces.csv");
    ofstream nodes_f( (filename+nodes_suffix).c_str());
    ofstream faces_f( (filename+faces_suffix).c_str());

    nodes_f.setf(ios_base::fixed, ios_base::floatfield);
    nodes_f.precision(6);

    BOOST_FOREACH(const Point& p, all_nodes)
    {
        nodes_f<<setw(12)<<p._x<<"; ";
        nodes_f<<setw(12)<<p._y<<"; ";
        nodes_f<<setw(12)<<p._z<<endl;
    }

    BOOST_FOREACH(const AcousticTriangle& tri, all_triangles)
    {
        faces_f<<setw(4)<<tri.n[0]<<"; ";
        faces_f<<setw(4)<<tri.n[1]<<"; ";
        faces_f<<setw(4)<<tri.n[2]<<endl;
    }
}

} // namespace tympan
