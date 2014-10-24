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
include(tympan-deps-cgalbindings)
include(tympan-deps-ply)

if(SYS_NATIVE_WIN)
  set(LD_VARNAME "PATH")
endif()
if(SYS_LINUX)
  set(LD_VARNAME "LD_LIBRARY_PATH")
endif()

build_native_path_list(TYMPAN_3RDPARTY_DLL_NATIVE_DIRS "${TYMPAN_3RDPARTY_DLL_DIRS}")

message(STATUS "You will have to ensure that the following directories "
  "are searched for shared libratries / DLL when launching the program : "
  "${TYMPAN_3RDPARTY_DLL_NATIVE_DIRS}")

