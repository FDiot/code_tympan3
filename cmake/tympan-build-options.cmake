# This CMake file is only included from the main CMakeLists.txt and is
# the place where various compiler options are set and documented.

# Should you need to tune those options for your build, please de NOT
# modify this file but edit the CMake-cache instead (using ccmake or cmake-gui)

# Adjust some policies (cf http://www.cmake.org/Wiki/CMake/Policies)
if (${CMAKE_VERSION} VERSION_GREATER 2.8.4)
  # This is necessary as of CMake version 2.8.8
  cmake_policy(SET CMP0017 NEW)
  # the way CGAL provides a lot of CMake modules makes the new behaviour preferable
endif(${CMAKE_VERSION} VERSION_GREATER 2.8.4)

option(TYMPAN_BUILD_TEST "Build Tympan tests" ON)
option(TYMPAN_BUILD_PYTHON "Build Python / Cython related components" ON)
option(TYMPAN_DEBUG_CMAKE "Verbose information messages from CMake" ON)

# Configure where to fetch 3rd party dependencies
# Please cf. the file "3rdparty/README"
set(TYMPAN_3RDPARTY_GTEST "${CMAKE_SOURCE_DIR}/3rdparty/gtest-1.6.0.zip"
  CACHE STRING "Where to fetch the external project GTest")
set(TYMPAN_3RDPARTY_GTEST_MD5  "4577b49f2973c90bf9ba69aa8166b786"
  CACHE STRING "Checksum for the TYMPAN_3RDPARTY_GTEST archive.")

set(TYMPAN_3RDPARTY_DIME "${CMAKE_SOURCE_DIR}/3rdparty/dime-137851b034f6.zip"
  CACHE STRING "Where to fetch the external project Dime")
set(TYMPAN_3RDPARTY_DIME_MD5  "8d144a016b3fb1cb1ac606d39ec7fccb"
    CACHE STRING "Checksum for the TYMPAN_3RDPARTY_DIME archive.")

set(TYMPAN_3RDPARTY_NMPB2008 "${CMAKE_SOURCE_DIR}/3rdparty/NMPB2008.zip"
  CACHE STRING "Where to fetch the external project NMBP2008")
set(TYMPAN_3RDPARTY_NMPB2008_MD5  "419f04812a8d3cd75c68cfee59b0c51b"
    CACHE STRING "Checksum for the TYMPAN_3RDPARTY_NMPB2008 archive.")

#NB: Source URL for CGAL bindings with installation patch
# http://medionilaura-cgal-bindings.googlecode.com/archive/issue-50.zip
set(TYMPAN_3RDPARTY_CGAL_BINDINGS "${CMAKE_SOURCE_DIR}/3rdparty/cgal-bindings.zip"
    CACHE STRING "Where to fetch the external project CGAL_bindings")
set(TYMPAN_3RDPARTY_CGAL_BINDINGS_MD5  "ae7e309dc8507fd45d5d0fd98de18b94"
    CACHE STRING "Checksum for the TYMPAN_3RDPARTY_CGAL_BINDINGS archive.")

## If you are having trouble getting CMake find boost you probably want to set
# set(Boost_DEBUG TRUE)

## You *can* also set the following variable to tell explicitely CMake
## where to find Boost but a *better* pratice is to set
## the BOOST_ROOT *environment* variable instead.
# set(BOOST_ROOT ""
#     CACHE PATH "The prefix where boost is installed" )

## For now, some modules of Code_TYMPAN do not compile when this macro is not defined.
## => We Discard the option and set TRUE by default.
# option(TYMPAN_USE_IHM "Builds TYMPAN with GUI support" ON)
## TODO : rewrite the TYStuff/TYStuffWidget to make them work.
set (TYMPAN_USE_IHM TRUE CACHE BOOL "Shall the GUI support be included (broken without)" FORCE)
if (TYMPAN_USE_IHM)
  add_definitions(-DTY_USE_IHM)
endif(TYMPAN_USE_IHM)

# The Tympan 'components' are build according to the TYMPAN_COMPONENT_TYPE
# custom variable or to the standard CMake default BUILD_SHARED_LIBS if
# TYMPAN_COMPONENT_TYPE is empty. The plugins are always built as MODULEs 
set(TYMPAN_COMPONENT_TYPE ""
    CACHE STRING "How to build Tympan components : SHARED or STATIC."
                 " (empty means to rely on BUILD_SHARED_LIBS).")
mark_as_advanced(TYMPAN_COMPONENT_TYPE)
# NB Because this value is declared here but set in platform specific configuration.
