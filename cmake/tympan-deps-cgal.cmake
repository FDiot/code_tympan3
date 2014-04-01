option( CGAL_DONT_OVERRIDE_CMAKE_FLAGS
        "We do not want for CGAL to overwrite CMake flags"
        TRUE
      )

#set(CGAL_Boost_USE_STATIC_LIBS ON) #XXX Check me

find_package(CGAL COMPONENTS Qt4 REQUIRED)
if(CGAL_USE_FILE)
  message(STATUS "INFO including CGAL_USE_FILE at " "${CGAL_USE_FILE}")
  include(${CGAL_USE_FILE})
else(CGAL_USE_FILE)
  message(WARNING "INFO CGAL_USE_FILE is not set hence the configuration for CGAL might be incomplete")
endif(CGAL_USE_FILE)

if(WIN32)
  # Tries to put pre-built GMP and MPFR in the PATH
  # ( GMP and MPFR are exact and arbitrary arithmetic libraries needed by CGAL
  #   while they are provided by the system package manager on Linux
  #   they are provided along with CGAL for windows as a specific binary build )
  set(cgal_dir_env $ENV{CGAL_DIR})
  if(cgal_dir_env)
    FILE(TO_CMAKE_PATH ${cgal_dir_env} GMP_MPFR_DIR)
    set(GMP_MPFR_DIR ${GMP_MPFR_DIR}/auxiliary/gmp/lib)
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

