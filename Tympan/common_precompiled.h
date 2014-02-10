#ifndef TYMPAN__PRECOMPILED_H__INCLUDED
#define TYMPAN__PRECOMPILED_H__INCLUDED

#if defined TYMPAN_USE_PRECOMPILED_HEADER

/* Add C includes here */

#  if defined __cplusplus
// Add C++ includes here

// STL headers
#    include <cstdlib>
#    include <cmath>
#    include <cstring>
#    include <iostream>
#    include <vector>
#    include <deque>
#    include <iterator>
#    include <algorithm>

// Boost headers
#    include <boost/config.hpp>
#    include <boost/shared_ptr.hpp>
#    include <boost/enable_shared_from_this.hpp>

// Qt headers
#    include <Qt/QtCore>
#    include <Qt/QtGui>

#  else /* Add C ONLY includes here */
#    include <stdlib.h>
#    include <math.h>
#    include <string.h>
#  endif // __cplusplus

#endif // defined TYMPAN_USE_PRECOMPILED_HEADER

#endif // not defined TYMPAN__PRECOMPILED_H__INCLUDED
