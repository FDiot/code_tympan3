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
    SolverDataModelBuilder();
    virtual ~SolverDataModelBuilder();

}; // class SolverDataModelBuilder

} /* namespace tympan */

#endif /* TYMPAN__SOLVER_DATA_MODEL_BUILDER_HPP__INCLUDED */
