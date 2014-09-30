file (GLOB TESTS_SRC ${CMAKE_CURRENT_SOURCE_DIR}/test_*.cpp )

# XXX It should be possible to make TYMPAN_MODULES much lighter
set(TYMPAN_MODULES
  tympan_dm_metier tympan_dm_graphics tympan_dm_ihm tympan_dm_core
  tympan_tools_metier tympan_tools_graphics tympan_tools
  tympan_acoustic_raytracer
)

# Main loop for all CPP test files.
foreach (testfile ${TESTS_SRC})
  get_filename_component (basename ${testfile} NAME_WE)

  add_executable (${basename} ${testfile})
  set_property(TARGET ${basename} PROPERTY FOLDER ${CURRENT_FOLDER})

  # add_dependencies (${basename} GTest tympan_dm_metier)
  add_dependencies (${basename} tympan_solver_data_model tympan_tools_metier GTest )

  # 'CMAKE_THREAD_LIBS_INIT' can be empty on Windows.
  target_link_libraries (${basename} gtest_main gtest ${CMAKE_THREAD_LIBS_INIT}
    ${TYMPAN_MODULES})

  add_test (${basename} ${basename})
  set_property(TEST ${basename} PROPERTY ENVIRONMENT
             "${LD_VARNAME}=${ADD_PATH_STRING}")
endforeach (testfile)

# XML files is an existing study used for testing.
file (GLOB XML_FILES ${CMAKE_CURRENT_SOURCE_DIR}/*.xml)
foreach (xmlfile ${XML_FILES})
  get_filename_component (filename ${xmlfile} NAME)
  file (COPY ${xmlfile} DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
endforeach (xmlfile)
