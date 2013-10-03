if(WIN32)
  include(tympan-install-win)
elseif(UNIX)
  # Nothing specific ? (for now XXX)
endif()

# uninstall target
# adapted from
# http://www.cmake.org/Wiki/CMake_FAQ#Can_I_do_.22make_uninstall.22_with_CMake.3F
configure_file(
    "${PROJECT_SOURCE_DIR}/cmake/cmake_uninstall.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake"
    IMMEDIATE @ONLY)

add_custom_target(uninstall
    COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake)

get_property(cmake_folder GLOBAL PROPERTY PREDEFINED_TARGETS_FOLDER)
set_property(TARGET uninstall PROPERTY FOLDER ${cmake_folder})

