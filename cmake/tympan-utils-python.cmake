# Utilities related to pyhton and cython


function(configure_cython_module module)
  set_property(TARGET ${module} PROPERTY DEBUG_POSTFIX "")
  set_property(TARGET ${module} PROPERTY LIBRARY_OUTPUT_DIRECTORY_DEBUG
    "${PROJECT_BINARY_DIR}/${TYMPAN_CythonModules_Debug}")
  set_property(TARGET ${module} PROPERTY LIBRARY_OUTPUT_DIRECTORY_RELEASE
    "${PROJECT_BINARY_DIR}/${TYMPAN_CythonModules_Release}")
endfunction()

function(install_cython_module module)
  install(TARGETS ${module}
    DESTINATION ${TYMPAN_CythonModules_Release}  
    CONFIGURATIONS Release)
  
  install(TARGETS ${module}
    DESTINATION ${TYMPAN_CythonModules_Debug} 
    CONFIGURATIONS Debug)  
endfunction()

# This function creates a new python test
function(add_python_test)
  set(options "")
  set(oneValueArgs "TARGET" "SCRIPT" "FOLDER")
  set(multiValueArgs "RUNTIME_PATH" "PYTHONPATH" "DEPS")
  cmake_parse_arguments("" "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

  add_test(NAME ${_TARGET}
    # [CONFIGURATIONS [Debug|Release|...]]
    # [WORKING_DIRECTORY dir]
    COMMAND ${PYTHON_EXECUTABLE} "${_SCRIPT}")
  _common_test_config()

  build_native_path_list(native_pythonpath "${_PYTHONPATH}")  
  set_property(TEST ${_TARGET} APPEND PROPERTY ENVIRONMENT "PYTHONPATH=${native_pythonpath}")

  if(_UNPARSED_ARGUMENTS)
    message(WARNING "add_qtest_executable: unknown arguments remaining unparsed "
      "for target ${_TARGET}: " ${_UNPARSED_ARGUMENTS})
  endif()
endfunction()


