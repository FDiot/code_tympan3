project(Code_TYMPAN)
cmake_minimum_required(VERSION 2.8)

set(${PROJECT_NAME}_VERSION_MAJOR 3)
set(${PROJECT_NAME}_VERSION_MINOR 9)
set(${PROJECT_NAME}_VERSION_PATCH 0)

if (${CMAKE_VERSION} VERSION_GREATER 2.8.4)
  # This is necessary as of CMake version 2.8.8
  cmake_policy(SET CMP0017 NEW)
  # the way CGAL provides a lot of CMake modules makes the new behaviour preferable
endif(${CMAKE_VERSION} VERSION_GREATER 2.8.4)

# Set path for additionnal CMake modules provides with Tympan
list(APPEND CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake)
include(tympan-platform)

# Configure where to fetch 3rd party dependencies
# Please cf. the file "3rdparty/README"
set(TYMPAN_3RDPARTY_GTEST "${CMAKE_SOURCE_DIR}/3rdparty/gtest-1.6.0.zip"
  CACHE STRING "Where to fetch the external project GTest")

set(TYMPAN_3RDPARTY_DIME "${CMAKE_SOURCE_DIR}/3rdparty/dime-137851b034f6.zip"
  CACHE STRING "Where to fetch the external project Dime")
set(TYMPAN_3RDPARTY_DIME_MD5  "8d144a016b3fb1cb1ac606d39ec7fccb"
    CACHE STRING "Checksum for the TYMPAN_3RDPARTY_DIME archive.")

set(TYMPAN_3RDPARTY_NMPB2008 "${CMAKE_SOURCE_DIR}/3rdparty/NMPB2008.zip"
  CACHE STRING "Where to fetch the external project NMBP2008")
set(TYMPAN_3RDPARTY_NMPB2008_MD5  "419f04812a8d3cd75c68cfee59b0c51b"
    CACHE STRING "Checksum for the TYMPAN_3RDPARTY_NMPB2008 archive.")

## If you are having trouble getting CMake find boost you probably want to set
# set(Boost_DEBUG TRUE)

## You *can* also set the following variable to tell explicitely CMake
## where to find Boost but a *better* pratice is to set
## the BOOST_ROOT *environment* variable instead.
# set(BOOST_ROOT ""
#     CACHE PATH "The prefix where boost is installed" )

include(tympan-dependencies)

option(LOGILAB_USE_PRECOMPILED_HEADER "Enable pre-compiled headers" OFF)

## For now, some modules of Code_TYMPAN do not compile when this macro is not defined.
## => We Discard the option and set TRUE by default.
# option(TYMPAN_USE_IHM "Builds TYMPAN with GUI support" ON)
## TODO : rewrite the TYStuff/TYStuffWidget to make them work.
set (TYMPAN_USE_IHM TRUE CACHE BOOL "Shall the GUI support be included (broken without)" FORCE)
if (TYMPAN_USE_IHM)
  add_definitions(-DTY_USE_IHM)
endif(TYMPAN_USE_IHM)

include_directories(${CMAKE_SOURCE_DIR})
include_directories(${CMAKE_BINARY_DIR})

# Recurse intro main sources directory
add_subdirectory(Tympan)

# Building test.
option(TYMPAN_BUILD_TEST "Build Tympan tests" ON)
if (TYMPAN_BUILD_TEST)
  add_subdirectory(tests)
endif (TYMPAN_BUILD_TEST)

include(tympan-install)
