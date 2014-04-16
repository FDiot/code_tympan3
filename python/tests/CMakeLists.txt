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

set(PYTHONPATH 
  "${PROJECT_BINARY_DIR}/${TYMPAN_PythonModules}"
  "${PROJECT_BINARY_DIR}/${TYMPAN_CythonModules_${CMAKE_BUILD_TYPE}}")

build_native_path_list(NATIVE_PYTHONPATH "${PYTHONPATH}")

set_property(TEST test_toto PROPERTY ENVIRONMENT "PYTHONPATH=${NATIVE_PYTHONPATH}")

if(TYMPAN_DEBUG_CMAKE)
message(STATUS "TYMPAN_DEBUG: native PYTHONPATH for tests : " "${NATIVE_PYTHONPATH}" )
endif()
