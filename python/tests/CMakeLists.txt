enable_testing()

# Copy all python source into the build directory
file(
  COPY        "${CMAKE_CURRENT_SOURCE_DIR}/"
  DESTINATION "${CMAKE_CURRENT_BINARY_DIR}/"
  FILES_MATCHING PATTERN "*.py" )

add_test(NAME test_toto 
  # [CONFIGURATIONS [Debug|Release|...]]
  # [WORKING_DIRECTORY dir]
  COMMAND ${PYTHON_EXECUTABLE} test_toto.py)

# XXX Convert this to native paths just for the C++ runtime path
set(PYTHONPATH 
  "${PROJECT_BINARY_DIR}/${TYMPAN_PythonModules}:${PROJECT_BINARY_DIR}/${TYMPAN_CythonModules_${CMAKE_BUILD_TYPE}}")
set_property(TEST test_toto PROPERTY ENVIRONMENT "PYTHONPATH=${PYTHONPATH}")