/**
 * @file prettyprint.hpp
 *
 * @brief Utilities to pretty-print value
 *
 */

#ifndef TY_PRETTYPRINT
#define TY_PRETTYPRINT

#include <string>
#include <sstream>

// Beware : Stuff::operator<< must be defined in the same namespace as Stuff

// For GTest related pretty-printing please see :
// http://code.google.com/p/googletest/wiki/AdvancedGuide#Teaching_Google_Test_How_to_Print_Your_Values

// For boost::exception diagnostic please see:
// http://www.boost.org/doc/libs/1_54_0/libs/exception/doc/diagnostic_information.html


template<typename T>
std::string tympan_to_string(const T& o)
{
    using namespace std;
    stringstream ss;
    ss << o;
    return ss.str();
}

#endif // TY_PRETTYPRINT
