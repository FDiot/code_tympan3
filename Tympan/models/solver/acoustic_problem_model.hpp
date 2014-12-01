/**
 * \file acoustic_problem_model.hpp
 * \brief This file provides the top-level declaration for the acoustic problem
 * model
 *
 * \date jul. 12 2013
 * \author Anthony Truchet <anthony.truchet@logilab.fr>
 */

#ifndef TYMPAN__ACOUSTIC_PROBLEM_MODEL_H__INCLUDED
#define TYMPAN__ACOUSTIC_PROBLEM_MODEL_H__INCLUDED

#include <string>
#include <memory>

#include "data_model_common.hpp"
#include "entities.hpp"

namespace tympan
{

class AcousticProblemModel
{
public:

    AcousticProblemModel() {};
    virtual ~AcousticProblemModel() {};

    /**
     * @brief Maps a \c TYPoint or \c OPoint3D as a \c node_ref
     * @param point the \c TYPoint  or \c OPoint3D to be mapped
     * @return a smart pointer to the corresponding Node (created on the fly if needed)
     */
    node_idx make_node(const Point&);
    node_idx make_node(double x, double y, double z)
    {return make_node(Point(x, y, z)); }

    triangle_idx make_triangle(node_idx n1, node_idx n2, node_idx n3);

    size_t npoints() const //TODO move to implementation file
    { return  all_nodes.size(); }

    size_t ntriangles() const //TODO move to implementation file
    { return  all_triangles.size(); }

    size_t nsources() const //TODO move to implementation file
    { return  all_sources.size(); }

    size_t nreceptors() const //TODO move to implementation file
    { return  all_receptors.size(); }

    AcousticTriangle& triangle(triangle_idx tri_idx)
    { return all_triangles.at(tri_idx); }

    const AcousticTriangle& triangle(triangle_idx tri_idx) const
    { return all_triangles.at(tri_idx); }

    Point& node(node_idx idx)
    { return all_nodes.at(idx); }

    const Point& node(node_idx idx) const
    { return all_nodes.at(idx); }

    AcousticSource& source(source_idx idx)
    { return all_sources.at(idx); }

    const AcousticSource& source(source_idx idx) const
    { return all_sources.at(idx); }

    AcousticReceptor& receptor(receptor_idx idx)
    { return all_receptors.at(idx); }

    const AcousticReceptor& receptor(receptor_idx idx) const
    { return all_receptors.at(idx); }


    size_t nmaterials() const //TODO move to implementation file
    { return  all_materials.size(); }

    const AcousticMaterialBase& material(size_t mat_idx) const
    { return *all_materials.at(mat_idx); }

    AcousticMaterialBase& material(size_t mat_idx)
    { return *all_materials.at(mat_idx); }

    /**
     * @brief Push a representation of a ground material into the model
     * @param name the name of the material
     * @param resistivity the resistivity of the ground, expressed in kRayls (TO BE CONFIRMED)
     * @return a shared_ptr to the corresponding AcousticGroundMaterial instance
     */
    material_ptr_t make_material(const string& name, double resistivity);

    /**
     * @brief Push a representation of a building material into the model
     * @param name the name of the material
     * @param the absorption spectrum (TO BE PRECISED)
     * @return a shared_ptr to the corresponding AcousticBuildingMaterial instance
     */
    material_ptr_t make_material(const string& name, const ComplexSpectrum& spectrum);

    /**
     * @brief Push a new acoustic source into the model
     * TODO
     */
    source_idx make_source(
        const Point& point_,
        const Spectrum& spectrum_,
        SourceDirectivityInterface* directivity);

    /**
     * @brief Push a new acoustic receptor into the model
     */
    receptor_idx make_receptor(
        const Point& position_);


    const nodes_pool_t& nodes() const { return all_nodes; }
    const triangle_pool_t& triangles() const  { return all_triangles; }
    const material_pool_t& materials() const { return all_materials; }

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
    material_pool_t all_materials;
    source_pool_t all_sources;
    receptor_pool_t all_receptors;
};  // class AcousticProblemModel

    std::unique_ptr<AcousticProblemModel> make_AcousticProblemModel();

} // namespace tympan




#endif // TYMPAN__ACOUSTIC_PROBLEM_MODEL_H__INCLUDED
