# Ensure that the system runtime (including the debug version) are installed
set(CMAKE_INSTALL_DEBUG_LIBRARIES ON)
set(CMAKE_INSTALL_SYSTEM_RUNTIME_DESTINATION ${CMAKE_INSTALL_PREFIX})
include (InstallRequiredSystemLibraries)

# This function is expected to be defined by the platform specific settings
platform_install_hook()

# Add an uninstall target, adapted from
# http://www.cmake.org/Wiki/CMake_FAQ#Can_I_do_.22make_uninstall.22_with_CMake.3F
configure_file(
    "${PROJECT_SOURCE_DIR}/cmake/cmake_uninstall.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake"
    IMMEDIATE @ONLY)
add_custom_target(uninstall
    COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake)
get_property(cmake_folder GLOBAL PROPERTY PREDEFINED_TARGETS_FOLDER)
set_property(TARGET uninstall PROPERTY FOLDER ${cmake_folder})
# Configure the script setting the environment variables
set(SetEnvTympan "SetEnvTympan")
FILE(TO_NATIVE_PATH ${CMAKE_INSTALL_PREFIX} TYMPAN_EXE_FULL_PATH)
if(SYS_NATIVE_WIN)
  configure_file(${PROJECT_SOURCE_DIR}/cmake/${SetEnvTympan}.bat.in
    ${CMAKE_INSTALL_PREFIX}/${SetEnvTympan}.bat @ONLY)
elseif(SYS_LINUX)
  configure_file(${PROJECT_SOURCE_DIR}/cmake/${SetEnvTympan}.sh.in
     ${CMAKE_INSTALL_PREFIX}/${SetEnvTympan}.sh @ONLY)
endif()
