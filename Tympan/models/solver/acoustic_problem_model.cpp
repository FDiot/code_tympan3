/**
* \file acoustic_problem_model.cpp
* \brief Implementation for top-level of the model
*
* \date jul. 12 2013
* \author Anthony Truchet <anthony.truchet@logilab.fr>
*/

#include <iostream>
#include <iomanip>

#include "Tympan/models/common/cgal_tools.h"
#include "Tympan/models/common/triangle.h"
#include "acoustic_problem_model.hpp"

using namespace std;

namespace tympan
{

deque<triangle_idx> scene_volume_intersection(const triangle_pool_t & triangle_soup,
                                              const nodes_pool_t & nodes, float w, float h,
                                              OPoint3D source, OPoint3D receptor)
{
    CGAL_Point3 _source = to_cgal(source);
    CGAL_Point3 _receptor = to_cgal(receptor);
    deque<CGAL_Point3> vertices = build_box(w, h, _source, _receptor);
    // we just want to apply rotation on mesh triangles and no translation --> center the box system
    // on (0, 0, 0)
    CGAL_Vector3 vx = normalize(CGAL_Vector3(vertices[0], vertices[1]));
    CGAL_Vector3 vy = normalize(CGAL_Vector3(vertices[0], vertices[2]));
    CGAL_Vector3 vz = normalize(CGAL_Vector3(vertices[0], vertices[3]));
    CGAL_Transform3 to_box_system(vx.x(), vx.y(), vx.z(),
                                  vy.x(), vy.y(), vy.z(),
                                  vz.x(), vz.y(), vz.z());
    // Move triangles from the triangle soup of the scene to the volume reference systeme and
    // build CGAL triangles out of them
    deque<CGAL_Point3> cgal_nodes;
    for(nodes_pool_t::const_iterator it = nodes.begin(); it != nodes.end(); it ++)
    {
        cgal_nodes.push_back(CGAL_Point3(it->_x, it->_y, it->_z).transform(to_box_system));
    }
    CGAL_Triangles cgal_triangles;
    for(triangle_pool_t::const_iterator it = triangle_soup.begin(); it != triangle_soup.end(); it++)
    {
        cgal_triangles.push_back(CGAL_Triangle(cgal_nodes[it->n[0]], cgal_nodes[it->n[1]],
                                               cgal_nodes[it->n[2]]));
    }
    float l = sqrt(CGAL_Vector3(_source, _receptor).squared_length());
    // these 3 points delimit the bounds of the fresnel box (in other terms, the bounding
    // box of this triangle should have the dimensions of the fresnel box). This will be checked
    // anyway by intersected_triangles() thanks to the expected dimensions passed: l, w and h
    std::deque<CGAL_Point3> box_triangle;
    box_triangle.push_back(vertices[1].transform(to_box_system));
    box_triangle.push_back(vertices[2].transform(to_box_system));
    box_triangle.push_back(vertices[3].transform(to_box_system));
    return intersected_triangles(cgal_triangles, box_triangle, l, w, h);
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

triangle_idx AcousticProblemModel::make_triangle(node_idx n1, node_idx n2, node_idx n3)
{
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
