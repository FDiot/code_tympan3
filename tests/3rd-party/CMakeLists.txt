file (GLOB TESTS_SRC ${CMAKE_CURRENT_SOURCE_DIR}/test_*.cpp )

# Main loop for all CPP test files.
foreach (testfile ${TESTS_SRC})
  get_filename_component (basename ${testfile} NAME_WE)

  add_executable (${basename} ${testfile})
  set_property(TARGET ${basename} PROPERTY FOLDER ${CURRENT_FOLDER})

  # add_dependencies (${basename} GTest tympan_dm_metier)
  add_dependencies (${basename} GTest NMPB2008)

  # 'CMAKE_THREAD_LIBS_INIT' can be empty on Windows.
  target_link_libraries (${basename} test_utils gtest_main gtest ${CMAKE_THREAD_LIBS_INIT}
    RoadEmissionNMPB)

  add_test (${basename} ${basename})
  set_property(TEST ${basename} PROPERTY ENVIRONMENT
             "${LD_VARNAME}=")
endforeach (testfile)
