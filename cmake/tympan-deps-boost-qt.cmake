# Simple Boost - Qt incompatibility work-around :
# There is some bad interaction between Qt and Boost around the token 'foreach'
# https://svn.boost.org/trac/boost/ticket/6455
# add_definitions(-DQT_NO_KEYWORDS) # This is even worse
#
# The following define prevents Qt from defining the `foreach` macro
# which is a source of problem because boost uses the `foreach` as
# namespace identifier without interfering any further.
add_definitions(-Dforeach=foreach)


# Find and configure for using Qt
find_package(Qt4 4.6.0 COMPONENTS QtCore QtGui QtOpenGL QtXml QtTest REQUIRED)
message(STATUS "INFO including QT_USE_FILE at " "${QT_USE_FILE}")
include(${QT_USE_FILE})
if(SYS_NATIVE_WIN)
  list(APPEND TYMPAN_3RDPARTY_DLL_DIRS  ${QT_BINARY_DIR})
endif()
if(SYS_LINUX)
  list(APPEND TYMPAN_3RDPARTY_DLL_DIRS  ${QT_LIBRARY_DIR})
endif()

#set(Boost_DEBUG TRUE) # Activate me to debug problems finding Boost
set(Boost_DETAILED_FAILURE_MESSAGE TRUE)
# Find Boost
set(Boost_ADDITIONAL_VERSIONS "1.46" "1.46.1" "1.49" "1.49.0")
#set(Boost_USE_MULTITHREADED ON) # You might need to activate me later
find_package(Boost COMPONENTS thread REQUIRED)
include_directories(${Boost_INCLUDE_DIRS})

# This is useless (but harmless) in case we link statically with Boost.
list(APPEND TYMPAN_3RDPARTY_DLL_DIRS  ${Boost_LIBRARY_DIRS})

find_package(OpenGL REQUIRED) # XXX Shouldn't this be implied by finding QtOpenGL ?
include_directories(${OPENGL_INCLUDE_DIR})
