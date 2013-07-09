# Looks for Code TYMPAN dependencies
set(TYMPAN_3RDPARTY_DLL_DIRS "")

# Simple Boost - Qt incompatibility work-around :
# There is some bad interaction between Qt and Boost around the token 'foreach'
# https://svn.boost.org/trac/boost/ticket/6455
# add_definitions(-DQT_NO_KEYWORDS) # This is even worse
#
# The following define prevents Qt from defining the `foreach` macro
# which is a source of problem because boost uses the `foreach` as
# namespace identifier without interfering any further.
add_definitions(-Dforeach=foreach)


# Find and configure for using Qt
find_package(Qt4 4.6.0 COMPONENTS QtCore QtGui QtOpenGL QtXml QtTest REQUIRED)
message(STATUS "  ## including QT_USE_FILE at " "${QT_USE_FILE}")
include(${QT_USE_FILE})
if(WIN32)
  list(APPEND TYMPAN_3RDPARTY_DLL_DIRS  ${QT_BINARY_DIR})
endif(WIN32)
if(UNIX)
  list(APPEND TYMPAN_3RDPARTY_DLL_DIRS  ${QT_LIBRARY_DIR})
endif(UNIX)

find_package(OpenGL REQUIRED) # XXX Shouldn't this be implied by finding QtOpenGL ?
include_directories(${OPENGL_INCLUDE_DIR})

#set(CGAL_Boost_USE_STATIC_LIBS ON) #XXX Check me
option( CGAL_DONT_OVERRIDE_CMAKE_FLAGS
        "We do not want for CGAL to overwrite CMake flags"
        TRUE
      )

find_package(CGAL COMPONENTS Qt4 REQUIRED)
if(CGAL_USE_FILE)
  message(STATUS "  ## including CGAL_USE_FILE at " "${CGAL_USE_FILE}")
  include(${CGAL_USE_FILE})
else(CGAL_USE_FILE)
  message(WARNING "  ## CGAL_USE_FILE is not set hence the configuration for CGAL might be incomplete")
endif(CGAL_USE_FILE)

if(WIN32)
  # Tries to put pre-built GMP and MPFR in the PATH
  # ( GMP and MPFR are exact and arbitrary arithmetic libraries needed by CGAL
  #   while they are provided by the system package manager on Linux
  #   they are provided along with CGAL for windows as a specific binary build )
  set(cgal_dir_env $ENV{CGAL_DIR})
  if(cgal_dir_env)
    set(GMP_MPFR_DIR $ENV{CGAL_DIR}/auxiliary/gmp/lib)
    list(APPEND TYMPAN_3RDPARTY_DLL_DIRS ${GMP_MPFR_DIR})
  else(cgal_dir_env)
    message(WARNING "The pre-built GMP anf MPFR needs to be put on the PATH")
  endif(cgal_dir_env)

  list(APPEND TYMPAN_3RDPARTY_DLL_DIRS ${CGAL_3RD_PARTY_LIBRARIES_DIRS})
  ## When using the DLL version of CGAL the dll are in bin instead of lib
  list(APPEND TYMPAN_3RDPARTY_DLL_DIRS  ${CGAL_LIBRARIES_DIR}/../bin)
endif(WIN32)
if(UNIX)
  list(APPEND TYMPAN_3RDPARTY_DLL_DIRS  ${CGAL_LIBRARIES_DIR})
endif(UNIX)

#set(Boost_DEBUG TRUE) # Activate me to debug problems finding Boost
set(Boost_DETAILED_FAILURE_MESSAGE TRUE)
# Find Boost
set(Boost_ADDITIONAL_VERSIONS "1.46" "1.46.1" "1.49" "1.49.0")
#set(Boost_USE_MULTITHREADED ON) # You will need to activate me later
find_package(Boost COMPONENTS thread REQUIRED)
include_directories(${Boost_INCLUDE_DIRS})

# This is useless but harmless if we link statically with Boost.
list(APPEND TYMPAN_3RDPARTY_DLL_DIRS  ${Boost_LIBRARY_DIRS})

# Yams++ dependency.
# Load the module to add an external project.
include (ExternalProject)
# Set default 'ExternalProject' root directory.
set_directory_properties(PROPERTIES EP_BASE ${PROJECT_BINARY_DIR}/3rdparty)

# Configuration step. Disable tests, examples and doc buildings.
set(ExternalYamsCMakeArgs -DYAMS_BUILD_EXAMPLE=OFF -DYAMS_BUILD_TESTS=OFF DYAMS_BUILD_DOC=OFF)
set(ExternalYamsCMakeArgs ${ExternalYamsCMakeArgs} -DCMAKE_CXX_FLAGS=${CMAKE_CXX_FLAGS} )
# set(ExternalYamsCMakeArgs ${ExternalYamsCMakeArgs} -DYAMS_3RDPARTY_GTEST=${TYMPAN_3RDPARTY_GTEST})
# Transmit the Boost setup already found
# Ensure BOOST_ROOT is defined, from the environment if needed.
if (NOT BOOST_ROOT AND NOT $ENV{BOOST_ROOT} STREQUAL "")
  set(BOOST_ROOT $ENV{BOOST_ROOT})
endif(NOT BOOST_ROOT AND NOT $ENV{BOOST_ROOT} STREQUAL "")
set(ExternalYamsCMakeArgs ${ExternalYamsCMakeArgs} -DBOOST_ROOT=${BOOST_ROOT} -DBoost_DEBUG=${Boost_DEBUG})


# Add the Yams++ project as an external project.
if (TYMPAN_USE_DEVEL_YAMS) # Development version Yams++ as a (probably versionned) directory

  if (EXISTS "${TYMPAN_3RDPARTY_YAMS}" AND IS_DIRECTORY "${TYMPAN_3RDPARTY_YAMS}")
    ExternalProject_Add(Yams
      # Where is Yams++ source dir.
      SOURCE_DIR "${TYMPAN_3RDPARTY_YAMS}"
      BUILD_IN_SOURCE 0

      CMAKE_ARGS ${ExternalYamsCMakeArgs}
      # Installation step. Disable install step.
      INSTALL_COMMAND ""
      ) # ExternalProject_Add Yams
  else (EXISTS "${TYMPAN_3RDPARTY_YAMS}" AND IS_DIRECTORY "${TYMPAN_3RDPARTY_YAMS}")
    if (TYMPAN_3RDPARTY_YAMS MATCHES "^${CMAKE_SOURCE_DIR}/3rdparty/yams(-[0-9.]+)?.zip$")
      message(WARNING
	"When TYMPAN_USE_DEVEL_YAMS is set, TYMPAN_3RDPARTY_YAMS must be set "
	"to a directory holding the development sources of Yams++ instead of "
	"an URL to an archive containing a Yams++ release.")
    endif (TYMPAN_3RDPARTY_YAMS MATCHES "^${CMAKE_SOURCE_DIR}/3rdparty/yams(-[0-9.]+)?.zip$")
    set(TYMPAN_3RDPARTY_YAMS TYMPAN_3RDPARTY_YAMS-NOTFOUND CACHE PATH
      "Where to fetch the external project Yams++" FORCE)
    message(FATAL_ERROR
      "Please set the _path_ to Yams++ development source directory in TYMPAN_3RDPARTY_YAMS variable.")
  endif (EXISTS "${TYMPAN_3RDPARTY_YAMS}" AND IS_DIRECTORY "${TYMPAN_3RDPARTY_YAMS}")

else (TYMPAN_USE_DEVEL_YAMS) # Yams++ release as ZIP file.

  ExternalProject_Add(Yams
    # Where to fetch Yams++
    URL "${TYMPAN_3RDPARTY_YAMS}"
    URL_MD5 "${TYMPAN_3RDPARTY_YAMS_MD5}"

    CMAKE_ARGS ${ExternalYamsCMakeArgs}
    # Installation step. Disable install step.
    INSTALL_COMMAND ""
    ) # ExternalProject_Add Yams
endif (TYMPAN_USE_DEVEL_YAMS)

# Get some properties from GTest project.
ExternalProject_Get_Property (Yams SOURCE_DIR BINARY_DIR)

# SOURCE_DIR is related to the Yams++ project.
set (YAMS_INCLUDE_DIR ${SOURCE_DIR}/include)
# BINARY_DIR is related to the Yams++ project.
set (YAMS_LIBRARIES_DIR ${BINARY_DIR}/src)

#XXX Move me to the usage point and / or write a FindYams.cmake
include_directories (${YAMS_INCLUDE_DIR})
link_directories (${YAMS_LIBRARIES_DIR})

set (YAMS_LIBRARIES "yams++") #XXX This should be a target name

## We now process this 3rd party list of directories according to
## http://www.mail-archive.com/cmake@cmake.org/msg21493.html

if(WIN32)
  set(LD_VARNAME "PATH")
  set(sep ";")
else(WIN32)
  set(LD_VARNAME "LD_LIBRARY_PATH")
  set(sep ":")
endif(WIN32)

list(REMOVE_DUPLICATES TYMPAN_3RDPARTY_DLL_DIRS)
set(TYMPAN_3RDPARTY_DLL_NATIVE_DIRS "")
foreach(dir ${TYMPAN_3RDPARTY_DLL_DIRS})
    FILE(TO_NATIVE_PATH  ${dir} nativedir)
    set(TYMPAN_3RDPARTY_DLL_NATIVE_DIRS "${TYMPAN_3RDPARTY_DLL_NATIVE_DIRS}${sep}${nativedir}")
endforeach(dir)
# Remove the leading separator and then make a string again of the list
list(REMOVE_AT TYMPAN_3RDPARTY_DLL_NATIVE_DIRS 0)
set(TYMPAN_3RDPARTY_DLL_NATIVE_DIRS "${TYMPAN_3RDPARTY_DLL_NATIVE_DIRS}")


# Add the Dime library 3rd party as an external project.
ExternalProject_Add(Dime
  URL "${TYMPAN_3RDPARTY_DIME}"
  URL_MD5 "${TYMPAN_3RDPARTY_DIME_MD5}"
  BUILD_IN_SOURCE 0

  # Installation step. Disable install step.
  INSTALL_COMMAND ""
)

# Get some properties from Dime project.
ExternalProject_Get_Property (Dime SOURCE_DIR BINARY_DIR)

# SOURCE_DIR is related to the Dime project.
include_directories (${SOURCE_DIR}/include)

# BINARY_DIR is related to the Dime project.
link_directories (${BINARY_DIR}/src)


message(STATUS "  ## TYMPAN_3RDPARTY_DLL_DIRS: " "${TYMPAN_3RDPARTY_DLL_DIRS}")
message(STATUS "  ## TYMPAN_3RDPARTY_DLL_NATIVE_DIRS: " "${TYMPAN_3RDPARTY_DLL_NATIVE_DIRS}")
message(STATUS "  ## Boost_INCLUDE_DIRS: " "${Boost_INCLUDE_DIRS}")
message(STATUS "  ## Boost_LIBRARIES   : " "${Boost_LIBRARIES}")
message(STATUS "  ## CGAL_INCLUDE_DIRS : " "${CGAL_INCLUDE_DIRS}" )
message(STATUS "  ## CGAL_3RD_PARTY_INCLUDE_DIRS : " "${CGAL_3RD_PARTY_INCLUDE_DIRS}" )
message(STATUS "  ## CGAL_LIBRARIES_DIR: " "${CGAL_LIBRARIES_DIR}" )
message(STATUS "  ## GMP_MPFR_DIR : " "${GMP_MPFR_DIR}" )
message(STATUS "  ## CGAL_LIBRARIES    : " ${CGAL_LIBRARIES})
message(STATUS "  ## QT_LIBRARIES      : " ${QT_LIBRARIES})
message(STATUS "  ## OPENGL_LIBRARIES  : " ${OPENGL_LIBRARIES})

