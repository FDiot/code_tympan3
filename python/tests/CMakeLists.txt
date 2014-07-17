set(CURRENT_FOLDER "Tests for Python")

enable_testing()

# Configure and optionnaly report the PYTHONPATH for the tests
set(PYTHONPATH 
  "${PROJECT_BINARY_DIR}/${TYMPAN_PythonModules}"
  "${PROJECT_BINARY_DIR}/${TYMPAN_CythonModules_${CMAKE_BUILD_TYPE}}")
if(TYMPAN_DEBUG_CMAKE)
  build_native_path_list(NATIVE_PYTHONPATH "${_PYTHONPATH}")  
  message(STATUS "TYMPAN_DEBUG: native PYTHONPATH for tests : " "${NATIVE_PYTHONPATH}" )
  unset(NATIVE_PYTHONPATH)
endif()

# Copy all python source into the build directory
file(
  COPY        "${CMAKE_CURRENT_SOURCE_DIR}/"
  DESTINATION "${CMAKE_CURRENT_BINARY_DIR}/"
  FILES_MATCHING PATTERN "*.py" )

file(GLOB TESTS_SCRIPTS ${CMAKE_CURRENT_BINARY_DIR}/test_*.py)

foreach(testfile ${TESTS_SCRIPTS})
  get_filename_component (basename ${testfile} NAME_WE)
  add_python_test(TARGET ${basename} 
    SCRIPT ${testfile} 
    FOLDER ${CURRENT_FOLDER}
    PYTHONPATH "${PYTHONPATH}"
    RUNTIME_PATH "${TYMPAN_3RDPARTY_DLL_NATIVE_DIRS}") 
endforeach (testfile)

# Add CGAL_BINDINGS_PATH environment variable (for the altimetry-related tests)
set_property(TEST test_altimetry APPEND PROPERTY ENVIRONMENT 
  "CGAL_BINDINGS_PATH=${CGAL_BINDINGS_DIR}")
