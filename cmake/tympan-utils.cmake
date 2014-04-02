# Various utility macro

macro(install_tympan_plugin PLUGIN_NAME)
install(TARGETS ${PLUGIN_NAME}
        LIBRARY DESTINATION ${TYMPAN_INSTALL_PLUGINS_Release} CONFIGURATIONS Release)
install(TARGETS ${PLUGIN_NAME}
        LIBRARY DESTINATION ${TYMPAN_INSTALL_PLUGINS_Debug} CONFIGURATIONS Debug)
endmacro(install_tympan_plugin)

macro(install_tympan_component TARGET)
install(TARGETS ${TARGET} ARCHIVE DESTINATION lib
                          LIBRARY DESTINATION lib
                          RUNTIME DESTINATION . )

endmacro(install_tympan_component)
# This function configures an existing executable target as a GTest test
# It registers it with CTest, add the dependencies for GTest and others
# and configures a runtime path 
function(configure_gtest_target)
  set(options "")
  set(oneValueArgs "TARGET" "FOLDER")
  set(multiValueArgs "RUNTIME_PATH" "DEPS" "LIBS")
  cmake_parse_arguments("" "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

  add_dependencies(${_TARGET} GTest ${_DEPS})
  target_link_libraries(${_TARGET} gtest_main gtest ${CMAKE_THREAD_LIBS_INIT} ${_LIBS})
  add_test(${_TARGET} ${_TARGET})

  if(_RUNTIME_PATH)
    # From http://www.mail-archive.com/cmake@cmake.org/msg21493.html
    #
    # IMPORTANT NOTE: The set_tests_properties(), below, internally
    # stores its name/value pairs with a semicolon delimiter.
    # because of this we must protect the semicolons in the path
    string(REPLACE ";" "\\;" _rt_path "${_RUNTIME_PATH}")  
    set_property(TEST ${_TARGET} PROPERTY ENVIRONMENT "${LD_VARNAME}=${_rt_path}")
  endif()

  if(_FOLDER)
    set_property(TARGET ${_TARGET} PROPERTY FOLDER ${_FOLDER})
  endif() 

  if(_UNPARSED_ARGUMENTS)
    message(WARNING "configure_gtest_target: unknown arguments remaining unparsed "
      "for target ${_TARGET}: " ${_UNPARSED_ARGUMENTS})
  endif()
endfunction(configure_gtest_target)