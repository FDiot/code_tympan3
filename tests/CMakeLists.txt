# CMakeLists for test cases with GoogleTest framework.

# from cmake 2.8.12, COMPILE_DEFINITION_DEBUG/RELEASE shouldn't be used anymore and should be
# replaced by generator expressions (see https://cmake.org/cmake/help/v3.0/policy/CMP0043.html)
# CMP0043 policy exists from major version 3.
if (NOT(${CMAKE_MAJOR_VERSION} VERSION_LESS "3"))
    cmake_policy(SET CMP0043 OLD)
endif()

set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} APPEND PROPERTY
    COMPILE_DEFINITIONS_DEBUG _DEBUG)
# GTest has already been loaded as external project by tympan-deps-gtest.cmake
include_directories(${GTEST_INCLUDE_DIR})
link_directories(${GTEST_BINARY_DIR})

include_directories(.)
include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_BINARY_DIR})
include_directories(${CMAKE_SOURCE_DIR} ${CMAKE_SOURCE_DIR}/${TYMPAN_ACOUSTICRAYTRACER})

# Configure the headers for the tests
set(TYMPAN_TESTS_DATA_DIR "${CMAKE_CURRENT_BINARY_DIR}/data")
set(TYMPAN_PYTHON_SCRIPTS "${CMAKE_CURRENT_BINARY_DIR}/../bin")
file(TO_NATIVE_PATH  ${TYMPAN_TESTS_DATA_DIR} TYMPAN_TESTS_DATA_DIR_NATIVE)
file(TO_NATIVE_PATH  ${TYMPAN_PYTHON_SCRIPTS} TYMPAN_PYTHON_SCRIPTS_NATIVE)
string(REPLACE "\\" "\\\\" TYMPAN_TESTS_DATA_DIR_NATIVE ${TYMPAN_TESTS_DATA_DIR_NATIVE})
string(REPLACE "\\" "\\\\" TYMPAN_PYTHON_SCRIPTS_NATIVE ${TYMPAN_PYTHON_SCRIPTS_NATIVE})
configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/TympanTestsConfig.h.in
    ${CMAKE_CURRENT_BINARY_DIR}/TympanTestsConfig.h
    @ONLY
)

# Copy data
file(COPY data DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

# Build testutils library
file(GLOB TESTS_SRC ${CMAKE_CURRENT_SOURCE_DIR}/testutils.cpp )
add_library(testutils STATIC ${TESTS_SRC} )
set_property(TARGET testutils PROPERTY FOLDER "Tests")
add_dependencies(testutils GTest tympan_common tympan_lib tympan_solver_model )
target_link_libraries(testutils gtest tympan_common tympan_lib tympan_solver_model)

# Specific tests configuration
# Needed to import DXF files.
add_definitions(-DTY_USE_DXF -DDIME_NOT_DLL)
# Needed for DXF files import.
set(DXF_DEPEND_FILES
    ${PROJECT_SOURCE_DIR}/Tympan/gui/app/TYDXFBDTOPOImporter.cpp
    ${PROJECT_SOURCE_DIR}/Tympan/gui/app/TYDXFImporter.cpp)
set(PYTHONPATH
   "${CMAKE_BINARY_DIR}/${TYMPAN_CythonModules_${CMAKE_BUILD_TYPE}}")
# Configure PYTHONPATH and CGAL_BINDINGS_PATH (test_m_b_altimetrie uses python
# altimetry)
build_native_path_list(NATIVE_PYTHONPATH "${PYTHONPATH}")
build_native_path_list(NATIVE_CGAL_BINDINGS_PYTHON_DIR "${CGAL_BINDINGS_PYTHON_DIR}")

# Find all CPP test files.
file(GLOB TESTS_SRC ${CMAKE_CURRENT_SOURCE_DIR}/test_*.cpp)
if(NOT TYMPAN_USE_NMPB2008)
    list(REMOVE_ITEM TESTS_SRC
        ${CMAKE_CURRENT_SOURCE_DIR}/test_3rdparty_nmpb2008.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/test_m_b_road.cpp
    )
endif()
# Need specific handling, see below
list(REMOVE_ITEM TESTS_SRC ${CMAKE_CURRENT_SOURCE_DIR}/test_gui_bdtopoimporter.cpp)
# XXX doesn't compile
list(REMOVE_ITEM TESTS_SRC ${CMAKE_CURRENT_SOURCE_DIR}/test_acousticraytracer_ray.cpp)
# Main loop for all CPP test files.
foreach (testfile ${TESTS_SRC})
    get_filename_component (basename ${testfile} NAME_WE)
    add_executable(${basename} ${testfile} testmain.cpp)
    configure_gtest_target(TARGET ${basename}
        FOLDER "Tests"
        RUNTIME_PATH "${TYMPAN_3RDPARTY_DLL_NATIVE_DIRS}"
        LIBS testutils tympan_common tympan_lib tympan_solver_anime3d_lib)
endforeach (testfile)
# More specific tests configuration
if(TYMPAN_USE_NMPB2008)
    target_link_libraries(test_3rdparty_nmpb2008 RoadEmissionNMPB)
endif()
target_link_libraries(test_3rdparty_rply ${RPLY_LIB})
set_property(TARGET test_3rdparty_rply APPEND PROPERTY
    INCLUDE_DIRECTORIES ${RPLY_INCLUDE_DIRECTORY})
set_property(TEST test_m_b_altimetrie APPEND PROPERTY
    ENVIRONMENT "PYTHONPATH=${NATIVE_PYTHONPATH}")
set_property(TEST test_m_b_altimetrie APPEND PROPERTY
    ENVIRONMENT "CGAL_BINDINGS_PATH=${NATIVE_CGAL_BINDINGS_PYTHON_DIR}")
# Specific case of test_gui_bdtopoimporter.cpp
add_executable(test_gui_bdtopoimporter
    test_gui_bdtopoimporter.cpp ${DXF_DEPEND_FILES} testmain.cpp)
configure_gtest_target(TARGET test_gui_bdtopoimporter
    FOLDER "Tests"
    RUNTIME_PATH "${TYMPAN_3RDPARTY_DLL_NATIVE_DIRS}"
    LIBS testutils ${TYMPAN_MODULES} dime)




