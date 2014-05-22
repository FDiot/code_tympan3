# Setting specific to the Visual Studio / Windows platform

# Assumes ${MSVC} is set

# Code_TYMPAN uses a lot of 'unsecure' functions like vsprintf
add_definitions(/DUSE_STANDARD_FILE_FUNCTIONS /D_CRT_SECURE_NO_WARNINGS)
# The `throw()` compiler specification triggers an annoying warning in VS2010
# (standard and C++'11 problems inside) but they are required in custom exception classes
add_definitions(/wd4290)

# Put here Visual Studio specific stuff
# Enable some level of build parallelisation
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /MP")

# Reserve more memory esp. for precompiled headers
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /Zm200")

## Building components as shared libs is unpossible on Windows by lack
## of DDL_EXPORTS/DLL_IMPORTS specifiers in the code for supporting it.
## We thus choose to force the 'working' way to compile depending on
## the platform, awaiting a real fix when splitting the application
## into several applications, libraries and solvers. (TODO)
set(TYMPAN_COMPONENT_TYPE STATIC CACHE STRING 
  "For now, on Windows, it is only possible to build the components as static libs." 
  FORCE)
set_property(CACHE TYMPAN_COMPONENT_TYPE PROPERTY STRINGS STATIC)


function(configure_RunTympan_scripts)
  FILE(TO_NATIVE_PATH ${CMAKE_INSTALL_PREFIX} TYMPAN_EXE_FULL_PATH)

  if(TYMPAN_DEBUG_CMAKE)
    message(STATUS "INFO CMAKE_INSTALL_PREFIX: " ${CMAKE_INSTALL_PREFIX})
    message(STATUS "INFO TYMPAN_EXE_FULL_PATH: " ${TYMPAN_EXE_FULL_PATH})
  endif()
  
  
  configure_file(${PROJECT_SOURCE_DIR}/cmake/runTympanDebug.bat.in
    ${PROJECT_BINARY_DIR}/runTympanDebug.bat @ONLY)
  install(PROGRAMS ${PROJECT_BINARY_DIR}/runTympanDebug.bat
    DESTINATION .
    CONFIGURATIONS Debug)
  
  configure_file(${PROJECT_SOURCE_DIR}/cmake/runTympanRelease.bat.in
    ${PROJECT_BINARY_DIR}/runTympanRelease.bat @ONLY)
  install(PROGRAMS ${PROJECT_BINARY_DIR}/runTympanRelease.bat
    DESTINATION .
    CONFIGURATIONS Release)
endfunction(configure_RunTympan_scripts)

function(platform_install_hook)
  if(TYMPAN_DEBUG_CMAKE)
    message(STATUS "INFO Running platform specific hooks for Windows")
  endif()
  configure_RunTympan_scripts()
endfunction(platform_install_hook)