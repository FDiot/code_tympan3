project(TYMPAN_Python)
cmake_minimum_required(VERSION 2.8.5)

find_package(PythonInterp REQUIRED)
# Cython integration adapted from https://github.com/thewtex/cython-cmake-example/
# Include the CMake script UseCython.cmake.  This defines add_cython_module().
# Instruction for use can be found at the top of cmake/UseCython.cmake.
include(UseCython)

include(tympan-utils-python)

set(TYMPAN_PythonModules         "pymodules")
set(TYMPAN_CythonModules_Debug   "cython_d")
set(TYMPAN_CythonModules_Release "cython")

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

# Process the CMakeLists.txt in the 'src' subdirectory.
add_subdirectory( src )

# Process the CMakeLists.txt in the 'tests' subdirectory.
add_subdirectory( tests )
