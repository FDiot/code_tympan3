# Common configuration for all platforms and targets

if (${CMAKE_VERSION} VERSION_GREATER 2.8.4)
  # This is necessary as of CMake version 2.8.8
  cmake_policy(SET CMP0017 NEW)
  set(CMAKE_ECLIPSE_MAKE_ARGUMENTS "-j8")
endif(${CMAKE_VERSION} VERSION_GREATER 2.8.4)

# Enable using Solution folder to visualy group targets
set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set(CURRENT_FOLDER "")
set_property(GLOBAL PROPERTY PREDEFINED_TARGETS_FOLDER "CMake Targets")

if(WIN32)
  if(CYGWIN)
    message(FATAL "Cygwin emulation environment will NOT work.")
  endif(CYGWIN)
  if(MINGW OR MSYS)
    message(WARNING "MINGW/MSYS is not supporter : it might happen to work... or not.")
  endif(MINGW OR MSYS)
  if(NOT MSVC)
    message(WARNING "Compilation on Windows with something else than MS toolchain is not supported : it might happen to work... or not.")
  endif(NOT MSVC)
  # Code_TYMPAN uses a lot of 'unsecure' functions like vsprintf
  add_definitions(/DUSE_STANDARD_FILE_FUNCTIONS /D_CRT_SECURE_NO_WARNINGS)
  # The `throw()` compiler specification triggers an annoying warning in VS2010
  # (standard and C++'11 problems inside) but they are required in custom exception classes
  add_definitions(/wd4290)
  set(SYS_NATIVE_WIN TRUE)
else(WIN32)
  if(NOT (UNIX AND CMAKE_COMPILER_IS_GNUCXX))
    message(ERROR "On UNIXes (and esp. Linux) only G++-compatible toolchain is supported : seem your system does not match...")
  endif(NOT (UNIX AND CMAKE_COMPILER_IS_GNUCXX))
  if(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    message(WARNING "Mac OS X / g++ is not supported (yet) but should work...")
    set(SYS_MACOSX TRUE)
  endif(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
  if(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
    set(SYS_LINUX TRUE)
  endif(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
endif(WIN32)

# The Tympan 'components' are build according to the TYMPAN_COMPONENT_TYPE custom variable
# or to the standard CMake default BUILD_SHARED_LIBS if TYMPAN_COMPONENT_TYPE is empty
# NB : The plugins are always build as MODULEs
# An empty string means relying on CMake standard way (BUILD_SHARED_LIBS) to choose a default
# option(BUILD_SHARED_LIBS "Builds using shared libraries by default" ON)
set(TYMPAN_COMPONENT_TYPE_DESCRIPTION "How to build Tympan components : SHARED or STATIC. '' means to rely on defaul CMake behaviour (cf. BUILD_SHARED_LIBS).")
set(TYMPAN_COMPONENT_TYPE ""
    CACHE STRING ${TYMPAN_COMPONENT_TYPE_DESCRIPTION})


## Building components as shared libs is unpossible on Windows by lack
## of DDL_EXPORTS/DLL_IMPORTS specifiers in the code for supporting it.
## Building as static libs on Linux mess-up the GUI, probably because
## widget parenting problems related to singletons.

## We thus choose to force the 'working' way to compile depending on
## the platform, awaiting a real fix when splitting the application
## into several applications, libraries and solvers plugins or
## executables. (TODO)

mark_as_advanced(TYMPAN_COMPONENT_TYPE)
if(SYS_NATIVE_WIN)
set(TYMPAN_COMPONENT_TYPE STATIC
    CACHE STRING "For now, on Windows, it is only possible to build the components a static libs." FORCE)
set_property(CACHE TYMPAN_COMPONENT_TYPE PROPERTY STRINGS STATIC)
elseif(SYS_MACOSX OR SYS_LINUX)
set(TYMPAN_COMPONENT_TYPE SHARED
    CACHE STRING "For now, on Linux/OS X, it is only possible to build the components a shared libs." FORCE)
  set_property(CACHE TYMPAN_COMPONENT_TYPE PROPERTY STRINGS SHARED)
else()
  message(WARNING "Your system has not been recognized...")
endif(SYS_NATIVE_WIN)

# Some objects are also used by Solvers and as such need to be build as PIC
# This is a hack and this option should be located in ToolsMetier CMakeLists.txt only
# See https://extranet.logilab.fr/ticket/627148 and https://extranet.logilab.fr/ticket/609920
if(CMAKE_COMPILER_IS_GNUCXX)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC")
endif(CMAKE_COMPILER_IS_GNUCXX)

# Set a default build type for single-configuration
# CMake generators if no build type is set.
if(NOT CMAKE_CONFIGURATION_TYPES AND NOT CMAKE_BUILD_TYPE)
   set(CMAKE_BUILD_TYPE Debug
       CACHE STRING "Build configuration" FORCE)
endif(NOT CMAKE_CONFIGURATION_TYPES AND NOT CMAKE_BUILD_TYPE)

set(CMAKE_DEBUG_POSTFIX "_d")
set_property(DIRECTORY ${PROJECT_SOURCE_DIR} APPEND PROPERTY COMPILE_DEFINITIONS_DEBUG _DEBUG)

## XXX This would be a cleaner way (to be checked)
#include(CheckCCompilerFlag)
#check_c_compiler_flag(...)

if(MSVC)
  # Put here Visual Studio specific stuff
  set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}"
       CACHE STRING "MS VC Compiler Flags for All Builds")
endif(MSVC)
if(CMAKE_COMPILER_IS_GNUCXX)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -frounding-math")
  # XXX -Wall would be FAR better than -fpermissive -w -Winvalid-pch
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fpermissive -w -Winvalid-pch")
endif(CMAKE_COMPILER_IS_GNUCXX)

# NB: add_definitions(...)
# -DLINUX used only by Tympan/Appli/PyTympan/PyTympan.cpp
# -DTYMPAN_PRECOMPILED_HEADERS sems not to be used

macro(install_tympan_plugin PLUGIN_NAME)
install(TARGETS ${PLUGIN_NAME}
        LIBRARY DESTINATION plugins  CONFIGURATIONS Release)
install(TARGETS ${PLUGIN_NAME}
        LIBRARY DESTINATION pluginsd CONFIGURATIONS Debug)
endmacro(install_tympan_plugin)

macro(install_tympan_component TARGET)
install(TARGETS ${TARGET} ARCHIVE DESTINATION lib
                          LIBRARY DESTINATION lib
                          RUNTIME DESTINATION . )

endmacro(install_tympan_component)

install(DIRECTORY     resources/
        DESTINATION   resources)
