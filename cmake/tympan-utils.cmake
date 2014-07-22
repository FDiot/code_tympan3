# Various utility macro

## Related to installation

include(CMakeParseArguments)


function(configure_tympan_plugin plugin)
  set_property(TARGET ${plugin} PROPERTY DEBUG_POSTFIX "")
  set_property(TARGET ${plugin} PROPERTY LIBRARY_OUTPUT_DIRECTORY_DEBUG
    "${PROJECT_BINARY_DIR}/${TYMPAN_INSTALL_PLUGINS_Debug}")
  set_property(TARGET ${plugin} PROPERTY LIBRARY_OUTPUT_DIRECTORY_RELEASE
    "${PROJECT_BINARY_DIR}/${TYMPAN_INSTALL_PLUGINS_Release}")
endfunction()

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

if(SYS_NATIVE_WIN)
  set(OS_PATH_SEPARATOR ";")
endif()
if(SYS_LINUX)
  set(OS_PATH_SEPARATOR ":")
endif()


# This function rewrite a CMake list of path into a string
# usable as native shell list of path like
# <user_home>/tympan/lib:<system>/lib on Linux
# c:\<user_home>\tympan\lib;c:\<system>\lib on Windows
function(build_native_path_list outvar inlist)
  # Handles the special case a an empty list
  if(NOT inlist)
    set(${outvar} "" PARENT_SCOPE)
    return()
  endif()


  ## We now process this 3rd party list of directories according to
  ## http://www.mail-archive.com/cmake@cmake.org/msg21493.html
  list(REMOVE_DUPLICATES inlist)

  set(native_list "")
  foreach(dir ${inlist})
    FILE(TO_NATIVE_PATH  ${dir} nativedir)
    set(native_list "${native_list}${OS_PATH_SEPARATOR}${nativedir}")
  endforeach(dir)

  # Remove the leading separator
  string(SUBSTRING "${native_list}" 1 -1 native_list )

  # Export the result in calling function scope
  set(${outvar} "${native_list}" PARENT_SCOPE)
endfunction(build_native_path_list)


# This MACRO factors out some functionalities shared among
# add_qtest_executable and configure_gtest_target: it is NOT MEANT to
# be called DIRECTLY
macro(_common_test_config)
  if(_RUNTIME_PATH)
    set(current_rt_path "$ENV{${LD_VARNAME}}")
    # Removes trailing directory separator before a path separator
    string(REPLACE "\\;" ";" current_rt_path "${current_rt_path}")
    set(extended_rt_path "${_RUNTIME_PATH}${OS_PATH_SEPARATOR}${current_rt_path}")
    # From http://www.mail-archive.com/cmake@cmake.org/msg21493.html
    #
    # IMPORTANT NOTE: The set_tests_properties(), below, internally
    # stores its name/value pairs with a semicolon delimiter.
    # because of this we must protect the semicolons in the path
    string(REPLACE ";" "\\;" extended_rt_path "${extended_rt_path}")

    set_property(TEST ${_TARGET} PROPERTY ENVIRONMENT "${LD_VARNAME}=${extended_rt_path}")
  endif()

  if(TARGET ${_TARGET})
    if(_FOLDER)
      set_property(TARGET ${_TARGET} PROPERTY FOLDER ${_FOLDER})
    endif()
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

  add_test(${_TARGET} ${_TARGET})
  _common_test_config()

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

  add_test(${_TARGET} ${_TARGET})
  _common_test_config()

  if(_UNPARSED_ARGUMENTS)
    message(WARNING "add_qtest_executable: unknown arguments remaining unparsed "
      "for target ${_TARGET}: " ${_UNPARSED_ARGUMENTS})
  endif()
endfunction()
