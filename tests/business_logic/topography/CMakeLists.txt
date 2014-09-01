file (GLOB TESTS_SRC ${CMAKE_CURRENT_SOURCE_DIR}/test_*.cpp )

# Configure PYTHONPATH and CGAL_BINDINGS_PATH (test_TYAltimetrie uses python altimetry)
set(PYTHONPATH
   "${CMAKE_BINARY_DIR}/${TYMPAN_CythonModules_${CMAKE_BUILD_TYPE}}")
build_native_path_list(NATIVE_PYTHONPATH "${PYTHONPATH}")
build_native_path_list(NATIVE_CGAL_BINDINGS_PYTHON_DIR "${CGAL_BINDINGS_PYTHON_DIR}")

# Main loop for all CPP test files.
foreach (testfile ${TESTS_SRC})
  get_filename_component (basename ${testfile} NAME_WE)
  add_executable (${basename} ${testfile} ../../test_main.cpp)
  configure_gtest_target(TARGET ${basename}
             FOLDER ${CURRENT_FOLDER}
             RUNTIME_PATH "${TYMPAN_3RDPARTY_DLL_NATIVE_DIRS}"
             LIBS ${TYMPAN_MODULES} )
endforeach (testfile)
set_property(TEST test_TYAltimetrie APPEND PROPERTY ENVIRONMENT
    "PYTHONPATH=${NATIVE_PYTHONPATH}")
set_property(TEST test_TYAltimetrie APPEND PROPERTY ENVIRONMENT
    "CGAL_BINDINGS_PATH=${NATIVE_CGAL_BINDINGS_PYTHON_DIR}")

# XML files is an existing study used for testing.
file (GLOB XML_FILES ${CMAKE_CURRENT_SOURCE_DIR}/*.xml)
foreach (xmlfile ${XML_FILES})
  get_filename_component (filename ${xmlfile} NAME)
  file (COPY ${xmlfile} DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
endforeach (xmlfile)
