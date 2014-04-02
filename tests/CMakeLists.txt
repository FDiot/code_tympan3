# CMakeLists for test cases with GoogleTest framework.
set(CURRENT_FOLDER "Tests")

enable_testing ()

# GTest has already been loaded as external project by tympan-deps-gtest.cmake
include_directories (${GTEST_INCLUDE_DIR})
link_directories (${GTEST_BINARY_DIR})

# Find Thread. Using some 'CMAKE_THREAD_*' variables to check the thread lib.
find_package (Threads)

include_directories(.)

set(TYMPAN_CTEST_ENV )

# XXX It should be possible to make this much lighter
set (TYMPAN_MODULES
  tympan_data_exchange tympan_solver_data_model
  tympan_dm_metier tympan_dm_graphics tympan_dm_ihm  tympan_dm_core
  tympan_tools_metier tympan_tools_graphics tympan_tools)

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
