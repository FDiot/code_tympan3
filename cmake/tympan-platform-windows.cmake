# Setting specific to the Visula Studio / Windows platform

# Assumes ${MSVC} is set

# Code_TYMPAN uses a lot of 'unsecure' functions like vsprintf
add_definitions(/DUSE_STANDARD_FILE_FUNCTIONS /D_CRT_SECURE_NO_WARNINGS)
# The `throw()` compiler specification triggers an annoying warning in VS2010
# (standard and C++'11 problems inside) but they are required in custom exception classes
add_definitions(/wd4290)

# Put here Visual Studio specific stuff
# Enable some level of build parallelisation
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /MP")

## Building components as shared libs is unpossible on Windows by lack
## of DDL_EXPORTS/DLL_IMPORTS specifiers in the code for supporting it.
## We thus choose to force the 'working' way to compile depending on
## the platform, awaiting a real fix when splitting the application
## into several applications, libraries and solvers. (TODO)
set(TYMPAN_COMPONENT_TYPE STATIC CACHE STRING 
  "For now, on Windows, it is only possible to build the components as static libs." 
  FORCE)
set_property(CACHE TYMPAN_COMPONENT_TYPE PROPERTY STRINGS STATIC)
