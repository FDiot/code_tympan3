# CMakeLists for test cases with GoogleTest framework.
# TODO (dag) This integrative test whicjh requires the whole app should
#            be moved into a dedicated subdirectory.
#            The macro to compile a test could be factored-out above and used below

# Find all CPP test files.
file (GLOB TESTS_SRC ${CMAKE_CURRENT_SOURCE_DIR}/test_*.cpp )

# Main loop for all CPP test files.
foreach (testfile ${TESTS_SRC})
  get_filename_component (basename ${testfile} NAME_WE)

  add_executable (${basename} ${testfile})
  set_property(TARGET ${basename} PROPERTY FOLDER ${CURRENT_FOLDER})

  # add_dependencies (${basename} GTest tympan_dm_metier)
  add_dependencies (${basename} GTest test_utils ${TYMPAN_MODULES} )

  # 'CMAKE_THREAD_LIBS_INIT' can be empty on Windows.
  target_link_libraries (${basename} gtest_main gtest test_utils ${CMAKE_THREAD_LIBS_INIT}
    ${TYMPAN_MODULES})

  add_test (${basename} ${basename})
  set_property(TEST ${basename} PROPERTY ENVIRONMENT
             "${LD_VARNAME}=${ADD_PATH_STRING}")
endforeach (testfile)

add_executable(app_testQT_CGAL app_test_QT_CGAL.cpp)
set_property(TARGET app_testQT_CGAL PROPERTY FOLDER ${CURRENT_FOLDER})

add_dependencies(app_testQT_CGAL tympan_solver_data_model tympan_tools_metier)
target_link_libraries(app_testQT_CGAL ${CMAKE_THREAD_LIBS_INIT} ${TYMPAN_MODULES})
