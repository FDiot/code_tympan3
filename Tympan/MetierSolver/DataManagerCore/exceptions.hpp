/**
 * \file exceptions.hpp
 *
 * \brief Define some exception related utilities relying on DataManagerMetier classes
 */

#ifndef __TYMPAN__DMM_EXCEPTIONS_HPP__INCLUDED__
#define __TYMPAN__DMM_EXCEPTIONS_HPP__INCLUDED__

#include <deque>
#include "Tympan/MetierSolver/CommonTools/exceptions.hpp"

namespace tympan
{
    typedef boost::error_info < struct tag_elements_implied,
                                std::deque<LPTYElement> > elements_implied_errinfo;
    typedef boost::error_info < struct tag_position,
                                OPoint3D > position_errinfo;
} // namespace tympan


#endif // __TYMPAN__DMM_EXCEPTIONS_HPP__INCLUDED__
