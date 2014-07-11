file (GLOB TESTS_SRC ${CMAKE_CURRENT_SOURCE_DIR}/test_*.cpp )

# Main loop for all CPP test files.
foreach (testfile ${TESTS_SRC})
  get_filename_component (basename ${testfile} NAME_WE)
  add_executable (${basename} ${testfile} ../test_main.cpp)
  configure_gtest_target(TARGET ${basename} 
             FOLDER ${CURRENT_FOLDER}
             RUNTIME_PATH "${TYMPAN_3RDPARTY_DLL_NATIVE_DIRS}" 
             LIBS test_utils ${TYMPAN_MODULES} )
endforeach (testfile)

add_executable(app_testQT_CGAL app_test_QT_CGAL.cpp)
set_property(TARGET app_testQT_CGAL PROPERTY FOLDER ${CURRENT_FOLDER})

add_dependencies(app_testQT_CGAL tympan_models_solver tympan_models_common)
target_link_libraries(app_testQT_CGAL ${CMAKE_THREAD_LIBS_INIT} ${TYMPAN_MODULES})
