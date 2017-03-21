/*
 * \file tympan_wrap_python.h
 *
 * This file is adapted from boost/python/detail/wrap_python.hpp in
 * order to solve the same problem described here :
 * http://www.boost.org/doc/libs/1_53_0/libs/python/doc/building.html#id19
 */

#ifndef __TYMPAN_WRAP_PYCONFIG_H__
#define __TYMPAN_WRAP_PYCONFIG_H__

#ifdef _DEBUG
# ifndef BOOST_DEBUG_PYTHON
#  ifdef _MSC_VER
    // VC8.0 will complain if system headers are #included both with
    // and without _DEBUG defined, so we have to #include all the
    // system headers used by pyconfig.h right here.
#   include <stddef.h>
#   include <stdarg.h>
#   include <stdio.h>
#   include <stdlib.h>
#   include <assert.h>
#   include <errno.h>
#   include <ctype.h>
#   include <wchar.h>
#   include <basetsd.h>
#   include <io.h>
#   include <limits.h>
#   include <float.h>
#   include <string.h>
#   include <math.h>
#   include <time.h>
#  endif
#  undef _DEBUG // Don't let Python force the debug library just because we're debugging.
#  define DEBUG_UNDEFINED_FROM_WRAP_PYTHON_H
# endif
#endif

# include <pyconfig.h>

#ifdef DEBUG_UNDEFINED_FROM_WRAP_PYTHON_H
# undef DEBUG_UNDEFINED_FROM_WRAP_PYTHON_H
# define _DEBUG
# ifdef _CRT_NOFORCE_MANIFEST_DEFINED_FROM_WRAP_PYTHON_H
#  undef _CRT_NOFORCE_MANIFEST_DEFINED_FROM_WRAP_PYTHON_H
#  undef _CRT_NOFORCE_MANIFEST
# endif
#endif

#  undef hypot // undo the evil #define left by Python.

#endif // __TYMPAN_WRAP_PYCONFIG_H__
