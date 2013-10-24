/**
 * @file SolverDataModelBuilder.cpp
 *
 * @brief Implementation of how to build a solver data model from a site.
 *
 * @author Anthony Truchet <anthony.truchet@logilab.fr>
 *
 */

#include <boost/foreach.hpp>

#include <cassert>
#include "Tympan/MetierSolver/DataExchange/SolverDataModelBuilder.hpp"

namespace tympan
{

UuidAdapter::UuidAdapter(const TYUUID& rhs)
{
    uuid.s.data1 = rhs.quid.data1;
    uuid.s.data2 = rhs.quid.data2;
    uuid.s.data3 = rhs.quid.data3;
    for (unsigned int i = 0; i < 8; ++i)
    {
        uuid.s.data4[i] = rhs.quid.data4[i];
    }
}

UuidAdapter::operator TYUUID() const
{
    TYUUID element_uid;
    element_uid.quid.data1 = uuid.s.data1;
    element_uid.quid.data2 = uuid.s.data2;
    element_uid.quid.data3 = uuid.s.data3;
    for (unsigned int i = 0; i < 8; ++i)
    {
        element_uid.quid.data4[i] = uuid.s.data4[i];
    }
    return element_uid;
}


SolverDataModelBuilder::SolverDataModelBuilder(SolverModel& model_):
		model(model_)
{
    // We store a simple reference to the model this builder is responsible to update.
}

SolverDataModelBuilder::~SolverDataModelBuilder()
{
    // The simple reference to the model is forgotten but we DO NOT touch to the model
    // as the whole point of the builder is to... build it !
}


void SolverDataModelBuilder::processAltimetry(LPTYSiteNode site_ptr)
{
    /* We do NOT use the list of faces in the Altimetry : this is a legacy
     * data-structure which provides polygons without their materials.
     */
    std::deque<OPoint3D> points;
    std::deque<OTriangle> triangles;
    std::deque<LPTYSol> materials;

    const LPTYTopographie p_topo = site_ptr->getTopographie();
    UuidAdapter element_uid(p_topo->getID());
    p_topo->exportMesh(points, triangles, &materials);

    processMesh(points, triangles);
    // Set the UUID of the topography and the material of each triangle
    unsigned tri_idx = 0;
    BOOST_FOREACH(const OTriangle& tri, triangles)
    {
        AcousticTriangle& ac_tri =  model.triangle(tri_idx);
        ac_tri.uuid = element_uid.getUuid();
        const LPTYSol p_sol = materials[tri_idx];
        material_ptr_t p_mat = model.make_material(
            to_std(p_sol->getName()),
            p_sol->getResistivite());
        ac_tri.made_of = p_mat;
        ++tri_idx;
    }
}


void SolverDataModelBuilder::walkTroughtSite(LPTYSiteNode site_ptr)
{
    // TODO There should be some error handling in here...
    processAltimetry(site_ptr);
    processInfrastructure(site_ptr);
    // processAcousticSources(site_ptr); // TODO Dummy implementation for now
    // processAcousticReceptors(site_ptr);// TODO Dummy implementation for now
}



void SolverDataModelBuilder::processInfrastructure(LPTYSiteNode site_ptr)
{

    // Get \c AcousticTriangle.
    unsigned int nb_building_faces = 0;
    TYTabAcousticSurfaceGeoNode face_list;
    std::vector<bool> estUnIndexDeFaceEcran; // The notion of `faceEcran` is obsolete now
    site_ptr->getListFaces(true, face_list, nb_building_faces,
                           estUnIndexDeFaceEcran);

    // Faces d'infrastructure
    TYAcousticSurface* pSurf = NULL;

    for (unsigned int i = 0 ; i < nb_building_faces ; i++)
    {
        // 'face_list' can contain topography elements. Not relevant here.
    	if (pSurf = TYAcousticSurface::safeDownCast(face_list[i]->getElement()))
    		setAcousticTriangle(pSurf);
    	else
    		continue; // The face was not an acoustic surface, it is ignored.
    }
}

void SolverDataModelBuilder::processMesh(
    const std::deque<OPoint3D>& points,
    const std::deque<OTriangle>& triangles)
{
    // Create all nodes related to the triangles.
    std::vector<node_idx> map_to_model_node_idx(points.size(), 0);
    size_t i = 0;
    BOOST_FOREACH(const Point& point, points)
    {
    	map_to_model_node_idx[i++] = model.make_node(point); // add the points
    }

    BOOST_FOREACH(const OTriangle& tri, triangles)
    {
        // Assert consistency of the OPoint3D given in the mesh
    	assert(tri._A == points[tri._p1]);
    	assert(tri._B == points[tri._p2]);
    	assert(tri._C == points[tri._p3]);
        // add the triangles
    	triangle_idx tri_idx = model.make_triangle(
    			map_to_model_node_idx[tri._p1],
    			map_to_model_node_idx[tri._p2],
    			map_to_model_node_idx[tri._p3]);
    }
}

void SolverDataModelBuilder::setAcousticTriangle(const TYAcousticSurfaceGeoNode& acoust_surf_geo)
{
    const TYAcousticSurface* pSurf = dynamic_cast<TYAcousticSurface*> ( acoust_surf_geo.getElement() );

    // Get the uid for the site element bearing the current acoustic surface
    UuidAdapter element_uid(pSurf->getParent()->getID());

    // Use the triangulating interface of TYSurfaceInterface to get triangles
    // and convert them to Nodes and AcousticTriangles (beware of mapping
    // TYPoints to Node in the correct way.)
    std::deque<OPoint3D> points;
    std::deque<OTriangle> triangles;
    pSurf->exportMesh(points, triangles, acoust_surf_geo );
    processMesh(points, triangles);

    // Get the building material for the surface.
    LPTYMateriauConstruction p_build_mat = pSurf->getMateriau();
    const TYSpectre& ty_spectre = p_build_mat->getSpectreAbso();
    material_ptr_t p_mat = model.make_material(
        to_std(p_build_mat->getName()),
        ty_spectre);

    // Set the UUID of the site element and the material of the surface
    unsigned tri_idx = 0;
    BOOST_FOREACH(const OTriangle& tri, triangles)
    {
        AcousticTriangle& ac_tri =  model.triangle(tri_idx);
        ac_tri.uuid = element_uid.getUuid();
        ac_tri.made_of = p_mat;
        ++tri_idx;
    }
}

void SolverDataModelBuilder::setAcousticBuildMaterial(LPTYMateriauConstruction material_ptr)
{
    // TODO is a kind of registry needed for Materials ? Cf. ticket #1469657

//    // Building material.
//    AcousticBuildingMaterial::pointer
//        build_mat_ptr(new AcousticBuildingMaterial());
//
//    // Spectrum from the \c TYMateriauConstruction.
//    const TYSpectre& ty_spectrum = material_ptr->getSpectreAbso();
//
//    // Create the entity \c tympan::AcousticSpectrum
//    AcousticSpectrum::pointer acoustic_spectrum_ptr(new AcousticSpectrum());
//
//    // Link AcousticBuildingMaterial with a \c AcousticSpectrum.
//    build_mat_ptr->add<reflection_spectrum_rdef>(acoustic_spectrum_ptr);
}


void SolverDataModelBuilder::processAcousticSources(LPTYSiteNode site_ptr)
{
//    // Need project and calcul to get all source points.
//    LPTYInfrastructure infrastructure_ptr = site_ptr->getInfrastructure();
//    LPTYProjet project_ptr = site_ptr->getProjet();
//    LPTYCalcul current_calcul = project_ptr->getCurrentCalcul();
//
//    // Get all localised acoustic points. key: TYelement*; value: table of
//    // points (i.e. TYTabSourcePonctuelleGeoNode, aka a table of TYGeometryNode).
//    TYMapElementTabSources source_point_map;
//    infrastructure_ptr->getAllSrcs(current_calcul, source_point_map);
//
//    // XXX Missing the geolocation of an AcousticSource (via attribute or
//    // relation?).
//
//    // Walk trough all source points and create entity.
//    TYMapElementTabSources::const_iterator map_it;
//    for (map_it = source_point_map.begin(); map_it != source_point_map.end();
//         ++map_it)
//    {
//        TYUUID ty_uid(map_it->first->getID());
//        // Stored the TYElement related to an acoustic source.
//        if (!elementsUID_to_SiteElement.contains(ty_uid))
//        {
//            UuidAdapter element_uid(ty_uid);
//            SiteElement::pointer element_ptr(new SiteElement(element_uid.getUuid()));
//            elementsUID_to_SiteElement.insert(ty_uid, element_ptr);
//        }
//
//        BOOST_FOREACH(const SmartPtr<TYGeometryNode>& source, map_it->second)
//        {
//            AcousticSource::pointer source_ptr(new AcousticSource());
//            SiteElement::pointer element_ptr =
//                elementsUID_to_SiteElement[ty_uid];
//            acoustic_sources.push_back(source_ptr);
//            // Link to a \c tympan::SiteElement
//            source_ptr->add<from_element_rdef>(element_ptr);
//
//
//            // Get TYSource in order to get spectrum.
//            TYSource* ty_source_ptr = NULL;
//            if (ty_source_ptr = TYSource::safeDownCast(source->getElement()))
//            {
//                TYSpectre* ty_spectrum_ptr = ty_source_ptr->getCurrentSpectre();
//                if (!ty_spectrum_ptr)
//                    assert(false && "NULL spectrum pointer from TYSource.");
//                // Create the entity \c tympan::AcousticSpectrum
//                AcousticSpectrum::pointer acoustic_spectrum_ptr(new AcousticSpectrum());
//                source_ptr->add<emission_spectrum_rdef>(acoustic_spectrum_ptr);
//                updateSpectrumRelations(ty_spectrum_ptr,
//                                        acoustic_spectrum_ptr);
//            }
//            else
//    		assert(false && "Should be a TYSource at least.");
//        }
//    }
}

void SolverDataModelBuilder::processAcousticReceptors(LPTYSiteNode site_ptr)
{
//    // Need project and calcul to get all receptors.
//    LPTYInfrastructure infrastructure_ptr = site_ptr->getInfrastructure();
//    LPTYProjet project_ptr = site_ptr->getProjet();
//    LPTYCalcul current_calcul = project_ptr->getCurrentCalcul();
//
//    TYTabPointCalculGeoNode receptor_table;
//    current_calcul->getAllRecepteurs(receptor_table);
//
//    BOOST_FOREACH(const SmartPtr<TYGeometryNode>& receptor, receptor_table)
//    {
//        TYElement* ty_element = receptor->getElement();
//        TYUUID ty_uid(ty_element->getID());
//        // Stored the TYElement related to an acoustic source.
//        if (!elementsUID_to_SiteElement.contains(ty_uid))
//        {
//            UuidAdapter element_uid(ty_uid);
//            SiteElement::pointer element_ptr(new SiteElement(element_uid.getUuid()));
//            elementsUID_to_SiteElement.insert(ty_uid, element_ptr);
//        }
//
//        // Get the coordinate of the receptor, create the entity and link with
//        // \c tympan::SiteElement.
//        TYPointCalcul* ty_point_calcul_ptr = NULL;
//        if (ty_point_calcul_ptr = TYPointCalcul::safeDownCast(ty_element))
//        {
//            Point point(*ty_point_calcul_ptr);
//            AcousticReceptor::pointer receptor_ptr(new AcousticReceptor(point));
//            SiteElement::pointer element_ptr =
//                elementsUID_to_SiteElement[ty_uid];
//            // Link to a \c tympan::SiteElement
//            receptor_ptr->add<from_user_receptor_rdef>(element_ptr);
//            acoustic_receptors.push_back(receptor_ptr);
//        }
//        else
//            assert(false && "Should be a TYPointCalcul at least.");
//    }
}


} /* namespace tympan */

// bool operator == (const TYPoint& point, const tympan::Node::pointer& node)
