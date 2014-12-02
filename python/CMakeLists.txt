set(CURRENT_FOLDER "python")

find_package(PythonInterp REQUIRED)
# Cython integration adapted from https://github.com/thewtex/cython-cmake-example/
# Include the CMake script UseCython.cmake.  This defines add_cython_module().
# Instruction for use can be found at the top of cmake/UseCython.cmake.
include(UseCython)

include(tympan-utils-python)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)
include_directories (${GTEST_INCLUDE_DIR})
if(SYS_NATIVE_WIN)
    execute_process(COMMAND "${PYTHON_EXECUTABLE}" "-c"
        "import numpy; print numpy.get_include()"
        OUTPUT_VARIABLE NUMPY_INCLUDE_DIR)
    include_directories(${NUMPY_INCLUDE_DIR})
endif()

add_subdirectory( tympan )
add_subdirectory( bin )

# Process the CMakeLists.txt in the 'tests' subdirectory.
add_subdirectory( tests )
