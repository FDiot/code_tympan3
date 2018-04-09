# Global settings common to all platforms

# Enable using Solution folder to visualy group targets
set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set(CURRENT_FOLDER "")
set_property(GLOBAL PROPERTY PREDEFINED_TARGETS_FOLDER "CMake Targets")

# Set a default build type for single-configuration
# CMake generators if no build type is set.
if(NOT CMAKE_CONFIGURATION_TYPES AND NOT CMAKE_BUILD_TYPE)
   set(CMAKE_BUILD_TYPE Debug
       CACHE STRING "Build configuration" FORCE)
endif(NOT CMAKE_CONFIGURATION_TYPES AND NOT CMAKE_BUILD_TYPE)

# Handle the differing output paths between Debug and Release
set(TYMPAN_INSTALL_PLUGINS_Release plugins)
set(TYMPAN_INSTALL_PLUGINS_Debug pluginsd)

# TYMPAN_SOLVERDIR in the environment file was wrong for Release (plugins and not pluginsd) 
if("${CMAKE_BUILD_TYPE}" STREQUAL "Release")
   set(TYMPAN_INSTALL_PLUGINS "${TYMPAN_INSTALL_PLUGINS_Release}")
else()
   set(TYMPAN_INSTALL_PLUGINS "${TYMPAN_INSTALL_PLUGINS_Debug}")
endif()

set(CMAKE_DEBUG_POSTFIX "_d")

install(DIRECTORY     resources/
        DESTINATION   resources)
