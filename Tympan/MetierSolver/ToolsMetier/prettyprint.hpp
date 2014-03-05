/**
 * @file prettyprint.hpp
 *
 * @brief Utilities to pretty-print value
 *
 */

#ifndef TYMPAN__PRETTYPRINT_HPP__INCLUDED
#define TYMPAN__PRETTYPRINT_HPP__INCLUDED

#include <cstring>
#include <ostream>

#include "Tympan/MetierSolver/ToolsMetier/OPoint3D.h"
#include "Tympan/MetierSolver/ToolsMetier/OVector3D.h"
#include "Tympan/MetierSolver/ToolsMetier/OPlan.h"
#include "Tympan/MetierSolver/ToolsMetier/OSpectre.h"

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

#endif // TYMPAN__PRETTYPRINT_HPP__INCLUDED
