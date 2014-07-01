/**
 * @file prettyprint.hpp
 *
 * @brief Utilities to pretty-print value
 *
 */

#ifndef TYMPAN__PRETTYPRINT_HPP__INCLUDED
#define TYMPAN__PRETTYPRINT_HPP__INCLUDED

#include <cstring>
#include <string>
#include <ostream>
#include <sstream>

#include "Tympan/models/common/3d.h"
#include "OPlan.h"
#include "Tympan/models/common/spectre.h"

// Beware : Stuff::operator<< must be defined in the same namespace as Stuff

// For GTest related pretty-printing please see :
// http://code.google.com/p/googletest/wiki/AdvancedGuide#Teaching_Google_Test_How_to_Print_Your_Values

// For boost::exception diagnostic please see:
// http://www.boost.org/doc/libs/1_54_0/libs/exception/doc/diagnostic_information.html

namespace tympan
{

} // namespace tympan

::std::ostream& operator<<(::std::ostream& os, const OCoord3D& c);
::std::ostream& operator<<(::std::ostream& os, const OVector3D& v);
::std::ostream& operator<<(::std::ostream& os, const OPoint3D& v);
::std::ostream& operator<<(::std::ostream& os, const OPlan& p);
::std::ostream& operator<<(::std::ostream& os, const OSpectre& s);


/** Convert an object T to a string using operator<< */
template<typename T>
std::string tympan_to_string(const T& o);


///// Templates implementations /////

template<typename T>
std::string tympan_to_string(const T& o)
{
    using namespace std;
    stringstream ss;
    ss << o;
    return ss.str();
}

#endif // TYMPAN__PRETTYPRINT_HPP__INCLUDED
