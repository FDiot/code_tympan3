# CMakeLists for test cases with GoogleTest framework.

# Find all CPP test files.
file (GLOB TESTS_SRC ${CMAKE_CURRENT_SOURCE_DIR}/test_*.cpp )

set (TYMPAN_MODULES tympan_dm_metier tympan_dm_graphics tympan_dm_ihm
     tympan_dm_core tympan_tools_metier tympan_tools_graphics tympan_tools)

# adapted from : http://qt-project.org/wiki/Unit_Testing
macro(add_qt_test testname testsrc)

  set(${basename}_SRCS ${testsrc})

  include_directories(${QT_QTTEST_INCLUDE_DIR} ${CMAKE_CURRENT_BINARY_DIR})
  qt4_automoc(${${basename}_SRCS})
  add_executable(${basename} ${${basename}_SRCS})
  target_link_libraries(${basename} ${TYMPAN_MODULES}
    ${QT_QTCORE_LIBRARY} ${QT_QTTEST_LIBRARY} ${QT_QTGUI_LIBRARY}) # ${GSOAP_LIBRARIES} ${QT_QTLOCATION_LIBRARY}

  set_property(TARGET ${basename} PROPERTY FOLDER ${CURRENT_FOLDER})
  add_test(${basename} ${basename})
  set_property(TEST ${basename} PROPERTY ENVIRONMENT
             "${LD_VARNAME}=${ADD_PATH_STRING}")
endmacro(add_qt_test)

# Main loop for all CPP test files.
foreach (testfile ${TESTS_SRC})
  get_filename_component (basename ${testfile} NAME_WE)
  add_qt_test(${basename} ${testfile})
endforeach (testfile)
