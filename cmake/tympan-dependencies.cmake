# Looks for Code TYMPAN dependencies
set(TYMPAN_3RDPARTY_DLL_DIRS "")

include(tympan-deps-boost-qt)
include(tympan-deps-cgal)


# Load the module to add an external project.
include (ExternalProject)
# Set default 'ExternalProject' root directory.
set_directory_properties(PROPERTIES EP_BASE ${PROJECT_BINARY_DIR}/3rdparty)

include(tympan-deps-gtest)
include(tympan-deps-dime)
include(tympan-deps-nmpb)


## We now process this 3rd party list of directories according to
## http://www.mail-archive.com/cmake@cmake.org/msg21493.html
if(SYS_NATIVE_WIN)
  set(LD_VARNAME "PATH")
  set(sep ";")
endif()
if(SYS_LINUX)
  set(LD_VARNAME "LD_LIBRARY_PATH")
  set(sep ":")
endif()

list(REMOVE_DUPLICATES TYMPAN_3RDPARTY_DLL_DIRS)
set(TYMPAN_3RDPARTY_DLL_NATIVE_DIRS "")
foreach(dir ${TYMPAN_3RDPARTY_DLL_DIRS})
    FILE(TO_NATIVE_PATH  ${dir} nativedir)
    set(TYMPAN_3RDPARTY_DLL_NATIVE_DIRS "${TYMPAN_3RDPARTY_DLL_NATIVE_DIRS}${sep}${nativedir}")
endforeach(dir)

# Remove the leading separator and then make a string again of the list
list(REMOVE_AT TYMPAN_3RDPARTY_DLL_NATIVE_DIRS 0)
set(TYMPAN_3RDPARTY_DLL_NATIVE_DIRS "${TYMPAN_3RDPARTY_DLL_NATIVE_DIRS}")

message(STATUS "You will have to ensure that the following directories "
  "are searched for shared libratries / DLL when launching the program : "
  "${TYMPAN_3RDPARTY_DLL_NATIVE_DIRS}")


## Some final display
if(TRUE) # XXX Debug, set to false in production
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
endif()