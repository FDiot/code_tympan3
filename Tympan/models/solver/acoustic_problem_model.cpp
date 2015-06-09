/**
 * \file acoustic_problem_model.cpp
 * \brief Implementation for top-level of the model
 *
 * \date jul. 12 2013
 * \author Anthony Truchet <anthony.truchet@logilab.fr>
 */

#include <iostream>
#include <fstream>
#include <iomanip>

#include "Tympan/models/common/cgal_tools.h"
#include "Tympan/models/common/triangle.h"
#include "acoustic_problem_model.hpp"

using namespace std;

namespace tympan
{

deque<triangle_idx> scene_volume_intersection(const triangle_pool_t & triangle_soup,
                                              const nodes_pool_t & nodes, const OBox2 & vol)
{
    // Build CGAL triangles from the triangle soup of the scene
    deque<CGAL_Point3> cgal_nodes;
    for(nodes_pool_t::const_iterator it = nodes.begin(); it != nodes.end(); it ++)
    {
        cgal_nodes.push_back(CGAL_Point3(it->_x, it->_y, it->_z));
    }
    CGAL_Triangles cgal_triangles;
    for(triangle_pool_t::const_iterator it = triangle_soup.begin(); it != triangle_soup.end(); it ++)
    {
        cgal_triangles.push_back(CGAL_Triangle(cgal_nodes[it->n[0]], cgal_nodes[it->n[1]],
                                               cgal_nodes[it->n[2]]));
    }
    return intersected_triangles(cgal_triangles, vol);
}

node_idx AcousticProblemModel::make_node(const Point& p)
{
    all_nodes.push_back(p);
    return all_nodes.size() - 1;
}

AcousticTriangle::AcousticTriangle(node_idx n1, node_idx n2, node_idx n3)
{
    n[0] = n1;
    n[1] = n2;
    n[2] = n3;
}

/*
bool AcousticProblemModel::less_than (const node_idx & lhs, const node_idx & rhs)
{
    if (lhs==rhs)
        return false;

    const Point& lp = *lhs;
    const Point& rp = *rhs;

    return less_than(lp, rp);
}

bool AcousticProblemModel::less_than (const Point& lp, const Point& rp)
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

triangle_idx AcousticProblemModel::make_triangle(node_idx n1, node_idx n2, node_idx n3)
{
    /*
    AcousticProblemModel::node_idx tab[3] = {n1, n2, n3};
    assert(tab[1] == n2);
    assert(&tab[1] != &n2);
    const unsigned k0 = std::min_element(&tab[0], &tab[3], AcousticProblemModel::less_than) - &tab[0];
    assert( k0<3 );

    AcousticProblemModel::triangle tri;
    // We fill the triangle array starting from k0
    for(unsigned int k=k0; k<k0+3; k++)
        tri.n[k%3] = tab[k];
    return tri;
    */
    all_triangles.push_back(AcousticTriangle(n1, n2, n3));
    return all_triangles.size() - 1;
}

material_ptr_t AcousticProblemModel::make_material(const string& name, double resistivity, double deviation, double length)
{
    material_ptr_t p_mat = tympan::static_pointer_cast<AcousticMaterialBase>(
                               tympan::make_shared<AcousticGroundMaterial>(name, resistivity, deviation, length));
    all_materials.push_back(p_mat);
    return p_mat;
}

material_ptr_t AcousticProblemModel::make_material(const string& name, const ComplexSpectrum& spectrum)
{
    material_ptr_t p_mat = tympan::static_pointer_cast<AcousticMaterialBase>(
                               tympan::make_shared<AcousticBuildingMaterial>(name, spectrum));
    all_materials.push_back(p_mat);
    return p_mat;
}

source_idx AcousticProblemModel::make_source(
        const Point& point,
        const Spectrum& spectrum,
        SourceDirectivityInterface* directivity)
{

    all_sources.push_back(AcousticSource(point, spectrum, directivity));
    return all_sources.size() - 1;
}

receptor_idx AcousticProblemModel::make_receptor(
        const Point& position_)
{
    all_receptors.push_back(AcousticReceptor(position_));
    return all_receptors.size() - 1;
}

    std::unique_ptr<AcousticProblemModel> make_AcousticProblemModel()
    { return std::unique_ptr<AcousticProblemModel>(new AcousticProblemModel()); }

} // namespace tympan
