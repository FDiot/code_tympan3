/**
 * @file UuidAdapter.hpp
 *
 * @author Anthony Truchet <anthony.truchet@logilab.fr>
 *
 */
#ifndef TYMPAN__UUID_ADAPTER_HPP__INCLUDED
#define TYMPAN__UUID_ADAPTER_HPP__INCLUDED

#include "Tympan/MetierSolver/DataManagerCore/TYElement.h"
#include "Tympan/MetierSolver/SolverDataModel/data_model_common.hpp"

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
} /* namespace tympan */

#endif /* TYMPAN__UUID_ADAPTER_HPP__INCLUDED */
