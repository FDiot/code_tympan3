# CMakeLists for test cases with GoogleTest framework.
set(CURRENT_FOLDER "Tests")

set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
  APPEND PROPERTY COMPILE_DEFINITIONS_DEBUG _DEBUG)

# GTest has already been loaded as external project by tympan-deps-gtest.cmake
include_directories (${GTEST_INCLUDE_DIR})
link_directories (${GTEST_BINARY_DIR})

# Find Thread. Using some 'CMAKE_THREAD_*' variables to check the thread lib.
find_package (Threads)

include_directories(.)

set (TYMPAN_MODULES tympan_common tympan_lib)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_BINARY_DIR})
# Configure the headers for the tests
set(TYMPAN_TESTS_DATA_DIR "${CMAKE_CURRENT_BINARY_DIR}/data")
set(TYMPAN_PYTHON_SCRIPTS "${CMAKE_CURRENT_BINARY_DIR}/../bin")
file(TO_NATIVE_PATH  ${TYMPAN_TESTS_DATA_DIR} TYMPAN_TESTS_DATA_DIR_NATIVE)
file(TO_NATIVE_PATH  ${TYMPAN_PYTHON_SCRIPTS} TYMPAN_PYTHON_SCRIPTS_NATIVE)
string(REPLACE "\\" "\\\\" TYMPAN_TESTS_DATA_DIR_NATIVE ${TYMPAN_TESTS_DATA_DIR_NATIVE})
string(REPLACE "\\" "\\\\" TYMPAN_PYTHON_SCRIPTS_NATIVE ${TYMPAN_PYTHON_SCRIPTS_NATIVE})
configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/TympanTestsConfig.hpp.in
    ${CMAKE_CURRENT_BINARY_DIR}/TympanTestsConfig.hpp
    @ONLY
)

file(COPY data DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
file(COPY ${CMAKE_SOURCE_DIR}/resources DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

file(GLOB TESTS_SRC ${CMAKE_CURRENT_SOURCE_DIR}/testutils.cpp )
add_library(testutils STATIC ${TESTS_SRC} )
set_property(TARGET testutils PROPERTY FOLDER ${CURRENT_FOLDER})
add_dependencies(testutils GTest ${TYMPAN_MODULES} )
# 'CMAKE_THREAD_LIBS_INIT' can be empty on Windows.
target_link_libraries(testutils gtest ${CMAKE_THREAD_LIBS_INIT} ${TYMPAN_MODULES})

# Unit tests dedicated to this directory only.
add_subdirectory(tests)

add_subdirectory(business_logic)
add_subdirectory(gui)
add_subdirectory(io)
add_subdirectory(SolverDataModel)

file (GLOB TESTS_SRC ${CMAKE_CURRENT_SOURCE_DIR}/test_*.cpp )

# Main loop for all CPP test files.
foreach (testfile ${TESTS_SRC})
  get_filename_component (basename ${testfile} NAME_WE)
  add_executable(${basename} ${testfile} test_main.cpp)
  configure_gtest_target(TARGET ${basename}
             FOLDER ${CURRENT_FOLDER}
             RUNTIME_PATH "${TYMPAN_3RDPARTY_DLL_NATIVE_DIRS}"
             LIBS testutils ${TYMPAN_MODULES})
endforeach (testfile)

target_link_libraries(test_3rdparty_nmpb2008 RoadEmissionNMPB)
target_link_libraries(test_3rdparty_rply ${RPLY_LIB})
set_property(TARGET test_3rdparty_rply
  APPEND PROPERTY INCLUDE_DIRECTORIES ${RPLY_INCLUDE_DIRECTORY})