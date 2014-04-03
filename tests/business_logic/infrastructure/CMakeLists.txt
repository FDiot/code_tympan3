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
  configure_gtest_target(TARGET ${basename} 
             FOLDER ${CURRENT_FOLDER}
             RUNTIME_PATH "${TYMPAN_3RDPARTY_DLL_NATIVE_DIRS}" 
             LIBS test_utils ${TYMPAN_MODULES} )
endforeach (testfile)
