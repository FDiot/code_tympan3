/**
 * @file prettyprint.hpp
 *
 * @brief Utilities to pretty-print value
 *
 */

#ifndef TYMPAN__PRETTYPRINT_HPP__INCLUDED
#define TYMPAN__PRETTYPRINT_HPP__INCLUDED

#include <cstring>
#include <iostream>

#include "gtest/gtest.h"

#include "Tympan/MetierSolver/ToolsMetier/OPoint3D.h"
#include "Tympan/MetierSolver/ToolsMetier/OVector3D.h"
#include "Tympan/MetierSolver/ToolsMetier/OPlan.h"


// Beware : Stuff::operator<< must be defined in the same namespace as Stuff

// For GTest related pretty-printing plese see :
// http://code.google.com/p/googletest/wiki/AdvancedGuide#Teaching_Google_Test_How_to_Print_Your_Values

namespace tympan {

} // namespace tympan

::std::ostream& operator<<(::std::ostream& os, const OCoord3D& c);
::std::ostream& operator<<(::std::ostream& os, const OVector3D& v);
::std::ostream& operator<<(::std::ostream& os, const OPoint3D& v);
::std::ostream& operator<<(::std::ostream& os, const OPlan& p);

#endif // TYMPAN__PRETTYPRINT_HPP__INCLUDED
