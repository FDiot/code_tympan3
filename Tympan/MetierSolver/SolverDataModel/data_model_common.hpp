/**
 * \file data_model_common.hpp
 * \brief Common utilities and includes for all data models
 *
 *  Created on: 6 nov. 2012
 *      Author: Anthony Truchet <anthony.truchet@logilab.fr>
 */

#ifndef TYMPAN__DATA_MODEL_COMMON_HPP__INCLUDED
#define TYMPAN__DATA_MODEL_COMMON_HPP__INCLUDED


#include "std_boost_utils.hpp"

#include "Tympan/MetierSolver/ToolsMetier/OPoint3D.h"
#include "Tympan/MetierSolver/ToolsMetier/OVector3D.h"
#include "Tympan/MetierSolver/ToolsMetier/OSpectreComplex.h"


// This section import some types from ToolsMetier
// under a more generic name into the tympan namespace. Indeed,
// there must be no dependency to TY-classes and no direct
// ones to O-classes from within the solver data model.
namespace tympan
{
typedef OPoint3D   Point;
typedef OVector3D  Vector;
typedef OSpectreComplex Spectrum;

typedef std::deque<Point> nodes_pool_t;
typedef size_t node_idx;
} //namespace tympan


namespace tympan
{

/**
 * @brief The base of all entity classes.
 *
 * This class is \em required to be \em virtually inherited by all
 * entity classes. It provide some convenient dynamic access and cast method.
 */
class BaseEntity
{
protected:
    BaseEntity() {};

public:
	virtual ~BaseEntity() {};

//    virtual tympan::shared_ptr<BaseEntity> ptr_any() = 0;
//    virtual tympan::shared_ptr<BaseEntity const> ptr_any() const = 0;
//
//    template <class T>
//    tympan::shared_ptr<T> ptr()
//    { return boost::dynamic_pointer_cast<T>(ptr_any()); }
//
//    template <class T>
//    tympan::shared_ptr<T const> ptr() const
//    { return boost::dynamic_pointer_cast<T const>(ptr_any()); }
};

//! Simple representation of an UUID (Universal Unique Identifier).
/*! Do the correspondance between \c TYElement uuid and the uuid of schema
    solver entities. It can match with a \c QUuid (UUID from Qt) and thus with a
    \c TYUUID (aka \c OGenID).
 */
struct binary_uuid
{
    union
    {
        struct
        {
            unsigned int   data1;
            unsigned short data2;
            unsigned short data3;
            unsigned char  data4[8];
        } s;
        unsigned char t[16];
    };
};


} /* namespace tympan */

#endif /* TYMPAN__DATA_MODEL_COMMON_HPP__INCLUDED */
