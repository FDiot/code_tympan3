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


# XXX It should be possible to make this much lighter
set (TYMPAN_MODULES
   tympan_solver_data_model
  tympan_dm_metier tympan_dm_graphics tympan_dm_ihm  tympan_dm_core
  tympan_tools_metier tympan_tools_graphics tympan_tools)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_BINARY_DIR})
# Configure the headers for the tests
set(TYMPAN_TESTS_DATA_DIR "${CMAKE_CURRENT_BINARY_DIR}/data")
file(TO_NATIVE_PATH  ${TYMPAN_TESTS_DATA_DIR} TYMPAN_TESTS_DATA_DIR_NATIVE)
string(REPLACE "\\" "\\\\" TYMPAN_TESTS_DATA_DIR_NATIVE ${TYMPAN_TESTS_DATA_DIR_NATIVE})
configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/TympanTestsConfig.hpp.in
    ${CMAKE_CURRENT_BINARY_DIR}/TympanTestsConfig.hpp
    @ONLY
)
add_subdirectory(test_utils)

add_executable(test_example test_example.cpp)
configure_gtest_target(TARGET test_example 
  RUNTIME_PATH "${TYMPAN_3RDPARTY_DLL_NATIVE_DIRS}" 
  FOLDER ${CURRENT_FOLDER})

add_executable(print_environment test_environment.cpp)
configure_gtest_target(TARGET print_environment 
  RUNTIME_PATH "${TYMPAN_3RDPARTY_DLL_NATIVE_DIRS}" 
  FOLDER ${CURRENT_FOLDER})

file(COPY data DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
file(COPY ${CMAKE_SOURCE_DIR}/resources DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

add_subdirectory(business_logic)
add_subdirectory(gui)
add_subdirectory(io)
add_subdirectory(SolverDataModel)
add_subdirectory(data_exchange)
add_subdirectory(3rd-party)
