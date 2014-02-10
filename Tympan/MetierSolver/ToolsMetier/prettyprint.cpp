/**
 * @file prettyprint.cpp
 *
 * @brief Utilities to pretty-print value
 * @author Anthony Truchet <anthony.truchet@logilab.org>
 *
 */

#include "Tympan/MetierSolver/ToolsMetier/prettyprint.hpp"

::std::ostream& operator<<(::std::ostream& os, const OCoord3D& c)
{
    return os << "(" << c._x << ", " << c._y << ", " << c._z << ")";
}

::std::ostream& operator<<(::std::ostream& os, const OVector3D& v)
{
    return os << "OVector3D"
           << static_cast<const OCoord3D&>(v);
}


::std::ostream& operator<<(::std::ostream& os, const OPoint3D& v)
{
    return os << "OPoint3D"
           << static_cast<const OCoord3D&>(v);
}

::std::ostream& operator<<(::std::ostream& os, const OPlan& p)
{
    return os << "OPlan(" << p._a << ", " << p._b << ", " << p._c << ", " << p._d << ")";
}
