set(CURRENT_FOLDER "Tests for Python")

enable_testing()

# Configure and optionnaly report the PYTHONPATH for the tests
set(PYTHONPATH 
  "${CMAKE_BINARY_DIR}/${TYMPAN_CythonModules_${CMAKE_BUILD_TYPE}}")
if(TYMPAN_DEBUG_CMAKE)
  build_native_path_list(NATIVE_PYTHONPATH "${PYTHONPATH}")  
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

file(GLOB ALTIMETRY_TESTS_SCRIPTS ${CMAKE_CURRENT_BINARY_DIR}/test_altimetry_*.py)
foreach(testfile ${ALTIMETRY_TESTS_SCRIPTS})
  get_filename_component (basename ${testfile} NAME_WE)
  set_property(TEST ${basename} APPEND PROPERTY ENVIRONMENT 
    "CGAL_BINDINGS_PATH=${CGAL_BINDINGS_DIR}")
endforeach (testfile)

