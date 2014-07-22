set(CURRENT_FOLDER "python")

find_package(PythonInterp REQUIRED)
# Cython integration adapted from https://github.com/thewtex/cython-cmake-example/
# Include the CMake script UseCython.cmake.  This defines add_cython_module().
# Instruction for use can be found at the top of cmake/UseCython.cmake.
include(UseCython)

include(tympan-utils-python)

set(TYMPAN_CythonModules_Debug   "cython_d")
set(TYMPAN_CythonModules_Release "cython")

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)
include_directories (${GTEST_INCLUDE_DIR})

add_subdirectory( tympan )
add_subdirectory( bin )

# Process the CMakeLists.txt in the 'tests' subdirectory.
add_subdirectory( tests )
