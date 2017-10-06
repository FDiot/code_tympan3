set(CURRENT_FOLDER "Tests for Python")

enable_testing()

# Configure and optionnaly report the PYTHONPATH for the tests
set(PYTHONPATH "")
list(APPEND PYTHONPATH "${CMAKE_BINARY_DIR}/${TYMPAN_CythonModules_${CMAKE_BUILD_TYPE}}/")
list(APPEND PYTHONPATH "${CMAKE_BINARY_DIR}/bin")
build_native_path_list(NATIVE_CGAL_BINDINGS_PYTHON_DIR "${CGAL_BINDINGS_PYTHON_DIR}")
if(TYMPAN_DEBUG_CMAKE)
  build_native_path_list(NATIVE_PYTHONPATH "${PYTHONPATH}")  
  message(STATUS "TYMPAN_DEBUG: native PYTHONPATH for tests : " "${NATIVE_PYTHONPATH}" )
  #unset(NATIVE_PYTHONPATH)
  message(STATUS "TYMPAN_DEBUG: native CGAL_BINDINGS_PYTHON_DIR for tests : " "${NATIVE_CGAL_BINDINGS_PYTHON_DIR}" )
endif()

# Copy all python source into the build directory
file(
  COPY        "${CMAKE_CURRENT_SOURCE_DIR}/"
  DESTINATION "${CMAKE_CURRENT_BINARY_DIR}/"
  FILES_MATCHING PATTERN "*.py" )
file(
  COPY        "${CMAKE_CURRENT_SOURCE_DIR}/data"
  DESTINATION "${CMAKE_CURRENT_BINARY_DIR}"
  FILES_MATCHING PATTERN "*" )


file(GLOB TESTS_SCRIPTS ${CMAKE_CURRENT_BINARY_DIR}/test_*.py)
foreach(testfile ${TESTS_SCRIPTS})
  get_filename_component (basename ${testfile} NAME_WE)
  add_python_test(TARGET ${basename} 
    SCRIPT ${testfile} 
    FOLDER ${CURRENT_FOLDER}
    PYTHONPATH "${PYTHONPATH}"
    RUNTIME_PATH "${TYMPAN_3RDPARTY_DLL_NATIVE_DIRS}") 
endforeach (testfile)

foreach(testfile ${TESTS_SCRIPTS})
  get_filename_component (basename ${testfile} NAME_WE)
  set_property(TEST ${basename} APPEND PROPERTY ENVIRONMENT 
    "PYTHONIOENCODING=UTF8")
  set_property(TEST ${basename} APPEND PROPERTY ENVIRONMENT 
    "CGAL_BINDINGS_PATH=${NATIVE_CGAL_BINDINGS_PYTHON_DIR}")
endforeach (testfile)
# Configure the script enabling to launch test manually from the command line
if(SYS_NATIVE_WIN)
  configure_file(${PROJECT_SOURCE_DIR}/cmake/SetEnvTympanTests.bat.in
    ${CMAKE_BINARY_DIR}/SetEnvTympanTests.bat @ONLY)
elseif(SYS_LINUX)
  configure_file(${PROJECT_SOURCE_DIR}/cmake/SetEnvTympanTests.sh.in
     ${CMAKE_BINARY_DIR}/SetEnvTympanTests.sh @ONLY)
endif()
