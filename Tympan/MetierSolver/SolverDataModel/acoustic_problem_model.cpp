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

using namespace std;

#include "acoustic_problem_model.hpp"

namespace tympan
{

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

material_ptr_t AcousticProblemModel::make_material(const string& name, double resistivity)
{
    material_ptr_t p_mat = tympan::static_pointer_cast<AcousticMaterialBase>(
                               tympan::make_shared<AcousticGroundMaterial>(name, resistivity));
    all_materials.push_back(p_mat);
    return p_mat;
}

material_ptr_t AcousticProblemModel::make_material(const string& name, const Spectrum& spectrum)
{
    material_ptr_t p_mat = tympan::static_pointer_cast<AcousticMaterialBase>(
                               tympan::make_shared<AcousticBuildingMaterial>(name, spectrum));
    all_materials.push_back(p_mat);
    return p_mat;
}

source_idx AcousticProblemModel::make_source(
        const Point& point,
        const Spectrum& spectrum)
{

    all_sources.push_back(AcousticSource(point, spectrum));
    return all_sources.size() - 1;
}

} // namespace tympan
