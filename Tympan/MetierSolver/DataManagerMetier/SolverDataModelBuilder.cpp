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
#include "Tympan/MetierSolver/DataManagerMetier/SolverDataModelBuilder.hpp"

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


SolverDataModelBuilder::SolverDataModelBuilder()
{
}

SolverDataModelBuilder::~SolverDataModelBuilder()
{
}

} /* namespace tympan */

// bool operator == (const TYPoint& point, const tympan::Node::pointer& node)

