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

