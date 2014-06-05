file (GLOB TESTS_SRC ${CMAKE_CURRENT_SOURCE_DIR}/test_*.cpp )

# XXX It should be possible to make TYMPAN_MODULES much lighter
set(TYMPAN_MODULES
  tympan_dm_metier tympan_dm_graphics tympan_dm_ihm tympan_dm_core
  tympan_solver_data_model
  tympan_tools_metier tympan_tools_graphics tympan_tools
)

# Main loop for all CPP test files.
foreach (testfile ${TESTS_SRC})
  get_filename_component (basename ${testfile} NAME_WE)
  add_executable (${basename} ${testfile} ../../test_main.cpp)
  configure_gtest_target(TARGET ${basename} 
             FOLDER ${CURRENT_FOLDER}
             RUNTIME_PATH "${TYMPAN_3RDPARTY_DLL_NATIVE_DIRS}" 
             LIBS ${TYMPAN_MODULES} )
endforeach (testfile)

# XML files is an existing study used for testing.
file (GLOB XML_FILES ${CMAKE_CURRENT_SOURCE_DIR}/*.xml)
foreach (xmlfile ${XML_FILES})
  get_filename_component (filename ${xmlfile} NAME)
  file (COPY ${xmlfile} DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
endforeach (xmlfile)
