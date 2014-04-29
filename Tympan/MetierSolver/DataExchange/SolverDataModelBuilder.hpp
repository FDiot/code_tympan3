/**
 * @file SolverDataModelBuilder.hpp
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
#include <QString>

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
    operator TYUUID() const;

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
    void processInfrastructure(LPTYSiteNode site_ptr);

    void processAltimetry(LPTYSiteNode site_ptr);

    /**
     *
     *
     */
    void setAcousticTriangle(const TYAcousticSurfaceGeoNode& acoust_surf_geo);


    //! Update material, spectrum and frequencies information.
    void setAcousticBuildMaterial(LPTYMateriauConstruction material_ptr);

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
    void processAcousticSources(LPTYSiteNode site_ptr);

    //! Update the \c tympan::AcousticReceptor
    /*! \todo Complete input args (if necessary). Link it with \c
        tympan::SiteElement which corresponds to a 'SiteAcousticReceptor( (do
        not occur in the schema) by the relation 'receptor'.
     \param site_ptr A pointer to a \c TYSiteNode.
     */
    void processAcousticReceptors(LPTYSiteNode site_ptr);

    // TODO Check what need to be handled related to diffraction edges.

protected:

    static
    const std::string
    to_std(const QString& qstr)
    { return std::string(qstr.toUtf8().constData()); } // NB : This does incur copy


    /**
     * @brief Creates the data model entities for a triangle soup.
     * @param points The points of the mesh.
     * @param triangles The triangles of the mesh.
     * @return void
     *
     * Creates nodes and AcousticTriangles in the \c model to
     * represent the mesh given as arguments.
     */
    void
    processMesh(
        const std::deque<OPoint3D>& points,
        const std::deque<OTriangle>& triangles);

protected:
    SolverModel& model;
}; // class SolverDataModelBuilder

} /* namespace tympan */

#endif /* TYMPAN__SOLVER_DATA_MODEL_BUILDER_HPP__INCLUDED */
