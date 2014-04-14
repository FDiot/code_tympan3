# Detect platform and handle some platform specific issues 

if (${CMAKE_VERSION} VERSION_GREATER 2.8.4)
  # This is necessary as of CMake version 2.8.8
  cmake_policy(SET CMP0017 NEW)
  # the way CGAL provides a lot of CMake modules makes the new behaviour preferable
endif(${CMAKE_VERSION} VERSION_GREATER 2.8.4)


# Perform detection of the platform 
if(WIN32)
  if(CYGWIN)
    message(FATAL "Cygwin emulation environment will NOT work.")
  endif(CYGWIN)
  if(MINGW OR MSYS)
    message(WARNING "MINGW/MSYS is not supported : it might happen to work... or not.")
  endif(MINGW OR MSYS)
  if(NOT MSVC)
    message(FATAL "Compilation on Windows with something else than MS toolchain" 
      "is not supported.")
  endif(NOT MSVC)
  set(SYS_NATIVE_WIN TRUE)
else(WIN32)
  if(NOT UNIX)
    message(FATAL "Your system is unknow : only Windows, Linux and Mac OS X are detected")
  endif(NOT UNIX)
  if(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    set(SYS_MACOSX TRUE)
  endif(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
  if(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
    set(SYS_LINUX TRUE)
  endif(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
endif(WIN32)

# Include the common then platform specific settings
include(tympan-platform-common)
if(SYS_NATIVE_WIN)
  include(tympan-platform-windows)
elseif(SYS_LINUX)
  include(tympan-platform-linux)
elseif(SYS_MACOSX)
  message(WARNING "Mac OS X / g++ is not supported (yet) but might work when handled as linux")
  include(tympan-platform-linux)
else()
  message(FATAL "Your system has not been recognized...")
endif(SYS_NATIVE_WIN)
