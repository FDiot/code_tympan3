# CMakeLists for test cases with GoogleTest framework.

# Find all CPP test files.
file (GLOB TESTS_SRC ${CMAKE_CURRENT_SOURCE_DIR}/test_*.cpp )

set (TYMPAN_MODULES tympan_models_business tympan_gui_gl tympan_gui_widgets
     tympan_models_common tympan_models_common tympan_gui tympan_models_common)

# Main loop for all CPP test files.
#foreach (testfile ${TESTS_SRC})
#  get_filename_component (basename ${testfile} NAME_WE)
#  add_qtest_executable(
#    TARGET ${basename}
#    SOURCES ${testfile}
#    FOLDER ${CURRENT_FOLDER}
#    RUNTIME_PATH "${TYMPAN_3RDPARTY_DLL_NATIVE_DIRS}"
#    LIBS ${TYMPAN_MODULES} )
#endforeach (testfile)
