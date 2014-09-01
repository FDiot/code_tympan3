# Find all CPP test files.
file (GLOB TESTS_SRC ${CMAKE_CURRENT_SOURCE_DIR}/test_*.cpp )

# The Xml / IO related test could be moved to IO section
# Thus enabling to get rid of this dependancy to QtXml here
find_package(Qt4 COMPONENTS QtCore QtXml REQUIRED)
include(${QT_USE_FILE})

# Main loop for all CPP test files.
foreach (testfile ${TESTS_SRC})
  get_filename_component (basename ${testfile} NAME_WE)
  add_executable (${basename} ${testfile} ../../test_main.cpp)
  configure_gtest_target(TARGET ${basename}
             FOLDER ${CURRENT_FOLDER}
             RUNTIME_PATH "${TYMPAN_3RDPARTY_DLL_NATIVE_DIRS}"
             LIBS testutils ${TYMPAN_MODULES} )
endforeach (testfile)
