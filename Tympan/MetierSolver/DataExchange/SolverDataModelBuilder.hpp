/**
 * @file SolverDataModelBuilder.hpp
 *
 * @brief The \c SolverDataModelBuilder is responsible to build a solver data
 *        model from a site.
 *
 * @author Anthony Truchet <anthony.truchet@logilab.fr>
 *
 */

#ifndef TYMPAN__SOLVER_DATA_MODEL_BUILDER_HPP__INCLUDED
#define TYMPAN__SOLVER_DATA_MODEL_BUILDER_HPP__INCLUDED

#include <deque>

#include <QtCore/qhash.h>

#include "Tympan/MetierSolver/DataManagerMetier/Site/TYSiteNode.h"


// Solver data model header files.
#include "Tympan/MetierSolver/SolverDataModel/entities.hpp"
#include "Tympan/MetierSolver/SolverDataModel/relations.hpp"
#include "Tympan/MetierSolver/SolverDataModel/model.hpp"


namespace tympan
{

//! Useful UUID (Universal Unique Identifier) adapter.
/*! Make a bridge between \c tympan::binary_uuid from the solver schema and \c
 TYUUID.

 \note that TYUUID contains a 'quid' attribute which is Qt Uuid type (i.e. \c
 QUuid).
 */
class UuidAdapter
{
public:
    UuidAdapter(const TYUUID& rhs);
    //! Allow conversion to \c TYUUID.
    operator TYUUID () const;

    //! Get the Uuid.
    binary_uuid getUuid() const { return uuid; }

private:
    //! Do not allow to build.
    UuidAdapter() {};

    //! Store unsigned char[16] in a way to easily share data with a \c QUuid.
    binary_uuid uuid;
};


class SolverDataModelBuilder
{
public:
    // Constructor & destructor.
    SolverDataModelBuilder(SolverModel& model_);
    virtual ~SolverDataModelBuilder();

    //! Walk trought \c TYSiteNode and fill some attributes.
    void walkTroughtSite(LPTYSiteNode site_ptr);

    //! Set a few 'geometric' entities such as \c tympan::Node.
    /*! Create geometric entities, fill dedicated container and relate them
        according to the relation definitions.
      \param site_ptr A pointer to a \c TYSideNode.
     */
    void setGeometricEntities(LPTYSiteNode site_ptr);

    //! Fill the \c acoustic_triangles attribute.
    /*! Create & relate an \c tympan::AcousticTriangle with a \c
        tympan::SiteElement. Update the relation \c tympan::surface_has_node'
        relation.
     \param element_uid the unique \c TYElement ID (from QUuid to QString).
     \param point_list List of points related to the surface.
     */
    void setAcousticTriangle(const TYAcousticSurface* pSurf);


//    //! Update some relations related to \c tympan::AcousticSpectrum.
//    /*! Update relations \c tympan::frequency_rdef and \c tympan::sample_of_rdef
//        between a \c tympan::AcousticSpectrum and \c tympan::Frequency & \c
//        tympan::SpectrumSample.
//      \param ty_spectrum A spectrum from a Tympan site
//      \param spectrum_ptr The entity 'spectrum' from the solver data model.
//     */
//    void updateSpectrumRelations(const TYSpectre* ty_spectrum_ptr,
//                                 const AcousticSpectrum::pointer spectrum_ptr);

    //! Set the frequencies.
    /*! Note that the frequencies are stored in a static attribute in Tympan
        (there is one frequencies table for a single computation).
     \param freq_tab The table with frequencies values.
    */
    void setFrequencyTab(const OTabFreq& freq_tab);

    //! Update material, spectrum and frequencies information.
    void setAcousticBuildMaterial(LPTYMateriauConstruction material_ptr);

    //! Update ground surface.
    /*! \warning Not implemented yet. Complete arguments. Link it with:
          - AcousticSpectrum
          - AcousticTriangle
     */
    void setAcousticBuildMaterial();

    //! Update the \c tympan::AcousticSource
    /*! \todo Complete input args (if necessary). Link
        it with:
           - AcousticTriangle
           - AcousticSpectrum
           - SiteUserAcousticSource
           - GlobalContribution (solver output?)
           - AcousticPath (solver output)
           - DefaultSolverSourceDirectivity (not yet)
     */
    void setAcousticSource(LPTYSiteNode site_ptr);

    //! Update the \c tympan::AcousticReceptor
    /*! \todo Complete input args (if necessary). Link it with \c
        tympan::SiteElement which corresponds to a 'SiteAcousticReceptor( (do
        not occur in the schema) by the relation 'receptor'.
     \param site_ptr A pointer to a \c TYSiteNode.
     */
    void setAcousticReceptor(LPTYSiteNode site_ptr);

    //! Update the \c tympan::DiffractionEdge entity.
    /*! \warning Not implemented yet. Complete arguments. Link it with a \c
        AcousticBuildingMaterial.
        \note For now, the DiffractionEdge is handled by the Solver.
     */
    void setDiffractionEdge();

    //! Update the \c tympan::DirectivityCoefficient entity.
    /*! \warning Not implemented yet. Complete arguments. Link it with a \c
        tympan::SpectrumSample.
        \note For now, we can avoid the DiffractionEdge.
     */
    void setDirectivityCoefficient();


    /**
     * @brief Maps a \c TYPoint as a \c tympan::Node
     * @param point the \c TYPoint to be mapped
     * @return a smart pointer to the corresponding Node (created on the fly if needed)
     */
    // Node::pointer node_for(const TYPoint& point); // TODO

protected:
    //! Stores a mapping from \c TYPoint-s' uuid to \c tympan::Node::pointer-s
    SolverModel& model;
}; // class SolverDataModelBuilder

} /* namespace tympan */

#endif /* TYMPAN__SOLVER_DATA_MODEL_BUILDER_HPP__INCLUDED */
