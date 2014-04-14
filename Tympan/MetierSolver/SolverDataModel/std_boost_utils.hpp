/**
 * @file   std_boost_utils.hh
 * @author Anthony Truchet <anthony.truchet@logilab.fr>
 * @date   Tue Oct 30 11:23:41 2012
 *
 * @brief  Misc utilities used by SolverDataModel
 *
 * NB This header would be a nice candidate to pre-compilation
 */

#ifndef TYMPAN_STD_BOOST_UTILS_H__INCLUDED
#define TYMPAN_STD_BOOST_UTILS_H__INCLUDED

// Misc includes from the standard library related to memory or low-level aspects
#include <cassert>
#include <cstddef>
#include <memory>

// Types
#include <string>
using std::string;
#include <complex>
#include <cmath>

// Smart pointers
#include <boost/config.hpp>
#include <boost/tr1/memory.hpp>

#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/make_shared.hpp>

// Data structures
#include <deque>
#include <set>

#include <boost/tr1/unordered_set.hpp>
#include "boost/tuple/tuple.hpp"

// Import some names which could come either from std:: std::tr1 or boost:: into tympan::
namespace tympan
{

using boost::shared_ptr;
using boost::weak_ptr;
using boost::dynamic_pointer_cast;
using boost::static_pointer_cast;
using boost::make_shared;

} // namespace tympan


// Boost BiMaps are use for some relations between entities
#include <boost/bimap/bimap.hpp>
//#include <boost/bimap/list_of.hpp>
#include <boost/bimap/unordered_set_of.hpp>
#include <boost/bimap/unordered_multiset_of.hpp>



#include <iterator>
#include <algorithm>

// Ranges and associated utilities
#include <boost/foreach.hpp>
#include <boost/range.hpp>
//#include <boost/range/any_range.hpp>
//#include <boost/range/iterator_range.hpp>
//#include <boost/range/functions.hpp>
#include <boost/range/adaptors.hpp>
//#include <boost/range/adaptor/transformed.hpp>


// Boost::Serialize archive support
// #include <boost/archive/text_oarchive.hpp>
// #include <boost/archive/text_iarchive.hpp>



#endif // TYMPAN_STD_BOOST_UTILS_H__INCLUDED
