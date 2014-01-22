# Find all CPP test files.
file (GLOB TESTS_SRC ${CMAKE_CURRENT_SOURCE_DIR}/test_*.cpp )

set (TYMPAN_MODULES tympan_dm_metier tympan_dm_graphics tympan_dm_ihm
     tympan_dm_core tympan_tools_metier tympan_tools_graphics tympan_tools)

# The Xml / IO related test could be moved to IO section
# Thus enabling to get rid of this dependancy to QtXml here
find_package(Qt4 COMPONENTS QtCore QtXml REQUIRED)
include(${QT_USE_FILE})

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
