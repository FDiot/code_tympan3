enable_testing()

# Copy all python source into the build directory
file(
  COPY        "${CMAKE_CURRENT_SOURCE_DIR}/"
  DESTINATION "${CMAKE_CURRENT_BINARY_DIR}/"
  FILES_MATCHING PATTERN "*.py" )

set(PYTHONPATH 
  "${PROJECT_BINARY_DIR}/${TYMPAN_PythonModules}"
  "${PROJECT_BINARY_DIR}/${TYMPAN_CythonModules_${CMAKE_BUILD_TYPE}}")

add_python_test(TARGET test_toto SCRIPT test_toto.py PYTHONPATH "${PYTHONPATH}") 

if(TYMPAN_DEBUG_CMAKE)
  build_native_path_list(NATIVE_PYTHONPATH "${_PYTHONPATH}")  
  message(STATUS "TYMPAN_DEBUG: native PYTHONPATH for tests : " "${NATIVE_PYTHONPATH}" )
  unset(NATIVE_PYTHONPATH)
endif()
