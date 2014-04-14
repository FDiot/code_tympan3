# Settings specific to the GCC (or clang) / Linux platform

# Assumes {CMAKE_COMPILER_IS_GNUCXX}
# TODO check behaviour for clang

# The objective is to get rid of all the -Wno-xxx
set(IGNORED_WARNINGS "-Wno-sign-compare -Wno-unknown-pragmas -Wno-comment -Wno-unused-variable")
set(WARNINGS_SETTINGS "-Wall ${IGNORED_WARNINGS} -Winvalid-pch")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --std=c++11 -frounding-math ${WARNINGS_SETTINGS}")

## Building as static libs on Linux mess-up the GUI, maybe because
## widget parenting problems related to singletons.
## We thus choose to force the 'working' way to compile depending on
## the platform, awaiting a real fix when splitting the application
## into several applications, libraries and solvers. (TODO)
set(TYMPAN_COMPONENT_TYPE SHARED CACHE STRING 
  "For now, on Linux, it is only possible to build the components as shared libs." 
  FORCE)
set_property(CACHE TYMPAN_COMPONENT_TYPE PROPERTY STRINGS STATIC)


# Some objects are also used by Solvers and as such need to be build as PIC
# This is a hack and this option should be located in ToolsMetier CMakeLists.txt only
# See https://extranet.logilab.fr/ticket/627148 and https://extranet.logilab.fr/ticket/609920
if(CMAKE_COMPILER_IS_GNUCXX)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC")
endif(CMAKE_COMPILER_IS_GNUCXX)
