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

#include "Tympan/MetierSolver/DataManagerMetier/Site/TYSiteNode.h"


// Solver data model header files.
#include "entities.hpp"
#include "relations.hpp"


namespace tympan
{

class SolverDataModelBuilder
{
public:
    // Constructor & destructor.
    SolverDataModelBuilder();
    virtual ~SolverDataModelBuilder();


    ////////////////////
    // Access methods //
    ////////////////////


    /////////////////////
    // Setting methods //
    /////////////////////

    //! Walk trought \c TYSiteNode and fill some attributes.
    void walkTroughtSite(LPTYSiteNode site_ptr);

    //! Set a few 'geometric' entities such as \c tympan::Node.
    /*! Create geometric entities, fill dedicated container and relate them
        according to the relation definitions.
      \param site_ptr A pointer to a \c TYSideNode.
     */
    void setGeometricEntities(LPTYSiteNode site_ptr);

    //! Fill the \c acoustic_surfaces attribute.
    /*! Create & relate an \c tympan::AcousticSurface with a \c
        tympan::SiteElement. Update the relation \c tympan::surface_has_node'
        relation.
     \param element_uid the unique \c TYElement ID (from QUuid to QString).
     \param point_list List of points related to the surface.
     */
    void setAcousticSurface(const string& element_uid,
                            const TYTabPoint& point_list);

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
          - AcousticSurface
     */
    void setAcousticBuildMaterial();

    //! Update the \c tympan::AcousticSource
    /*! \warning Not implemented yet. Complete input args (if necessary). Link
        it with:
           - AcousticSurface
           - AcousticSpectrum
           - SiteUserAcousticSource
           - GlobalContribution (solver output?)
           - AcousticPath (solver output)
           - DefaultSolverSourceDirectivity
     */
    void setAcousticSource(LPTYSiteNode site_ptr);

    //! Update the \c tympan::AcousticReceptor
    /*! \warning Not implemented yet. Complete input args (if necessary). Link
        it with:
           - AcousticSurface
           - AcousticSpectrum
           - SiteUserAcousticSource
           - GlobalContribution (solver output?)
           - AcousticPath (solver output)
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

    ////////////////
    // Attributes //
    ////////////////

    //! Stores the entity \c tympan::Site (root & childs).
    // std::deque<boost::shared_ptr<Site> > sites;

    //! Stores all nodes of a site.
    std::deque<boost::shared_ptr<Node> > nodes;

    //! Stores all \c tympan::AcousticSurface of a site.
    std::deque<boost::shared_ptr<AcousticSurface> > acoustic_surfaces;

    //! Stores \c tympan::SpectrumSample.
    std::deque<boost::shared_ptr<SpectrumSample> > spectrum_samples;

    //! Stores \c tympan::Frequency.
    std::deque<boost::shared_ptr<Frequency> > frequencies;

    //! Is frequencies table loaded from the static table for a single a \c
    //! TYCalcul.
    bool is_frequency;

    //! According to the data model, the \c tympan::SpectrumSample is unique.
    // \warning XXX use a deque container. Remove if from the
    // SolverDataModelBuilder constructor! See 'setAcousticBuildMaterial' which
    // updates/uses this entity.
    std::deque<boost::shared_ptr<AcousticSpectrum> > acoustic_spectrums;

    //! Stores \c tympan::AcousticSource.
    std::deque<boost::shared_ptr<AcousticSource> > acoustic_sources;

    //! Stores \c tympan::AcousticReceptor.
    std::deque<boost::shared_ptr<AcousticReceptor> > acoustic_receptors;
};

} /* namespace tympan */
#endif /* TYMPAN__SOLVER_DATA_MODEL_BUILDER_HPP__INCLUDED */
