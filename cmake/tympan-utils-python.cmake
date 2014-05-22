# Utilities related to python and cython


function(configure_cython_module module)
  set_property(TARGET ${module} PROPERTY DEBUG_POSTFIX "")
  set_property(TARGET ${module} PROPERTY LIBRARY_OUTPUT_DIRECTORY_DEBUG
    "${PROJECT_BINARY_DIR}/${TYMPAN_CythonModules_Debug}")
  set_property(TARGET ${module} PROPERTY LIBRARY_OUTPUT_DIRECTORY_RELEASE
    "${PROJECT_BINARY_DIR}/${TYMPAN_CythonModules_Release}")
  # We do NOT want to depend on the debug version of the Python libs
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
    COMMAND ${PYTHON_EXECUTABLE} "${_SCRIPT}" $<CONFIGURATION>)
  _common_test_config()

  build_native_path_list(native_pythonpath "${_PYTHONPATH}")  
  set_property(TEST ${_TARGET} APPEND PROPERTY ENVIRONMENT "PYTHONPATH=${native_pythonpath}")

  if(_UNPARSED_ARGUMENTS)
    message(WARNING "add_python_test: unknown arguments remaining unparsed "
      "for target ${_TARGET}: " ${_UNPARSED_ARGUMENTS})
  endif()
endfunction()

set(CYTHON_PATCH_SCRIPT "${CMAKE_SOURCE_DIR}/tools/cython_patch.py"
  CACHE PATH "Where to find the cython_patch.py script ")
mark_as_advanced(CYTHON_PATCH_SCRIPT)

function(cython_patch)
  set(options "")
  set(oneValueArgs "OUTPUT" "FILE" )
  set(multiValueArgs "")
  cmake_parse_arguments("" "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})
    
  get_filename_component(dir   "${_FILE}" PATH )
  get_filename_component(fname "${_FILE}" NAME )
  set(patched_fname "${dir}/patched_${fname}")

  add_custom_command(OUTPUT ${patched_fname}
    COMMAND ${PYTHON_EXECUTABLE}
    ARGS ${CYTHON_PATCH_SCRIPT} "${_FILE}" "${patched_fname}" 
    WORKING_DIRECTORY ${_dir}
    DEPENDS "${_FILE}")


  if(_UNPARSED_ARGUMENTS)
    message(WARNING 
      "cython_patch: unknown arguments remaining unparsed: " 
      "${_UNPARSED_ARGUMENTS}")
  endif()
  set(${_OUTPUT} "${patched_fname}" PARENT_SCOPE)
endfunction()


