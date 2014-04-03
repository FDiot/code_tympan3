# Various utility macro

## Related to installation

include(CMakeParseArguments)

# This function installs a plugin in the right directory depending on the configuration
function(install_tympan_plugin PLUGIN_NAME)
install(TARGETS ${PLUGIN_NAME}
        LIBRARY DESTINATION ${TYMPAN_INSTALL_PLUGINS_Release} 
        CONFIGURATIONS Release)
install(TARGETS ${PLUGIN_NAME}
        LIBRARY DESTINATION ${TYMPAN_INSTALL_PLUGINS_Debug} 
        CONFIGURATIONS Debug)
endfunction(install_tympan_plugin)

# This function installs a component (lib) in the right directory depending 
# on the library type and OS
function(install_tympan_component TARGET)
install(TARGETS ${TARGET} ARCHIVE DESTINATION lib
                          LIBRARY DESTINATION lib
                          RUNTIME DESTINATION . )
endfunction(install_tympan_component)

# This MACRO factors out some functionalities shared among
# add_qtest_executable and configure_gtest_target: it is NOT MEANT to
# be called DIRECTLY
macro(_register_test_target)
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
endmacro()


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

  _register_test_target()

  if(_UNPARSED_ARGUMENTS)
    message(WARNING "configure_gtest_target: unknown arguments remaining unparsed "
      "for target ${_TARGET}: " ${_UNPARSED_ARGUMENTS})
  endif()
endfunction(configure_gtest_target)


# This function creates a new executable target which is a QTest test
# It register it with CTest, adds the dependencies for QTest and
# configure a runtime path
# adapted from : http://qt-project.org/wiki/Unit_Testing
function(add_qtest_executable)
  set(options "")
  set(oneValueArgs "TARGET" "FOLDER")
  set(multiValueArgs "SOURCES" "RUNTIME_PATH" "DEPS" "LIBS")
  cmake_parse_arguments("" "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

  include_directories(${QT_QTTEST_INCLUDE_DIR} ${CMAKE_CURRENT_BINARY_DIR})
  qt4_automoc(${_SOURCES})
  add_executable(${_TARGET} ${_SOURCES})
  target_link_libraries(${_TARGET} ${TYMPAN_MODULES}
    ${QT_QTCORE_LIBRARY} ${QT_QTTEST_LIBRARY} ${QT_QTGUI_LIBRARY}) 

  _register_test_target()

  if(_UNPARSED_ARGUMENTS)
    message(WARNING "add_qtest_executable: unknown arguments remaining unparsed "
      "for target ${_TARGET}: " ${_UNPARSED_ARGUMENTS})
  endif()
endfunction()
