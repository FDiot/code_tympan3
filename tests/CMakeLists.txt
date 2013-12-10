# CMakeLists for test cases with GoogleTest framework.
set(CURRENT_FOLDER "Tests")

enable_testing ()

# GTest should have already been loaded as external project by tympan-dependencies.cmake
# Get some properties from GTest project.
ExternalProject_Get_Property (GTest SOURCE_DIR BINARY_DIR)
# SOURCE_DIR is related to the GTest project.
include_directories (${SOURCE_DIR}/include)
# BINARY_DIR is related to the GTest project.
link_directories (${BINARY_DIR})

# Find Thread. Using some 'CMAKE_THREAD_*' variables to check the thread lib.
find_package (Threads)

  # From http://www.mail-archive.com/cmake@cmake.org/msg21493.html
  #
  # IMPORTANT NOTE: The set_tests_properties(), below, internally
  # stores its name/value pairs with a semicolon delimiter.
  # because of this we must protect the semicolons in the path
  #
  string(REPLACE ";" "\\;" ADD_PATH_STRING "${TYMPAN_3RDPARTY_DLL_NATIVE_DIRS}")
  #$ENV{${LD_VARNAME}}${sep}

set(TYMPAN_CTEST_ENV )

# XXX It should be possible to make this much lighter
set (TYMPAN_MODULES
  tympan_data_exchange tympan_solver_data_model
  tympan_dm_metier tympan_dm_graphics tympan_dm_ihm  tympan_dm_core
  tympan_tools_metier tympan_tools_graphics tympan_tools)

include_directories(.)
add_subdirectory(test_utils)

add_executable(test_example test_example.cpp)
set_property(TARGET test_example PROPERTY FOLDER ${CURRENT_FOLDER})

target_link_libraries(test_example gtest_main gtest ${CMAKE_THREAD_LIBS_INIT})
add_test(test_example test_example)
set_property(TEST test_example PROPERTY ENVIRONMENT "${LD_VARNAME}=${ADD_PATH_STRING}")



file(COPY data DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
file(COPY ${CMAKE_SOURCE_DIR}/resources DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

add_subdirectory(business_logic)
add_subdirectory(gui)
add_subdirectory(io)
add_subdirectory(SolverDataModel)
add_subdirectory(data_exchange)
