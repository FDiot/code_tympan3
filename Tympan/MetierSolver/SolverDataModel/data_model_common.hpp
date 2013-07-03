/**
 * \file data_model_common.hpp
 * \brief Common utilities and includes for all data models
 *
 *  Created on: 6 nov. 2012
 *      Author: Anthony Truchet <anthony.truchet@logilab.fr>
 */

#ifndef TYMPAN__DATA_MODEL_COMMON_HPP__INCLUDED
#define TYMPAN__DATA_MODEL_COMMON_HPP__INCLUDED

#include <boost/config.hpp>

#include "yams/utils.hh"

#include "Tympan/MetierSolver/ToolsMetier/OPoint3D.h"
#include "Tympan/MetierSolver/ToolsMetier/OVector3D.h"
namespace tympan
{
typedef OPoint3D   Point;
typedef OVector3D  Vector;
} //namespace tympan


namespace tympan
{

/**
 * @brief A classical Singleton Design pattern
 *
 * The intended use is \code
class OnlyOne : public Singleton<OnlyOne>
{
// Make OnlyOne Ctor/Dtor accessible by Singleton<OnlyOne>
friend class Singleton<OnlyOne>;
//... your singleton class
};
\endcode
 *
 * @tparam The type of the Singleton - must be default constructible
 */
template<typename T> class Singleton
{
public:
    static T& Instance()
    {
        static std::auto_ptr<T> theSingleInstance(new T());
        return theSingleInstance;
    }
private:
    Singleton() {}
    Singleton(Singleton const&);      // Don't Implement
    void operator=(Singleton const&); // Don't implement
};



} /* namespace tympan */

#endif /* TYMPAN__DATA_MODEL_COMMON_HPP__INCLUDED */
