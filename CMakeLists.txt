project(Code_TYMPAN)
cmake_minimum_required(VERSION 2.8)

# Set path for additionnal CMake modules provides with Tympan
list(APPEND CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake)

set(${PROJECT_NAME}_VERSION_MAJOR 4)
set(${PROJECT_NAME}_VERSION_MINOR 1)
set(${PROJECT_NAME}_VERSION_PATCH 14)

# This file specifies several build options
include(tympan-build-options NO_POLICY_SCOPE)
# Detect and handle the platform
include(tympan-platform)
include(tympan-utils) 
# This file configures the dependencies of Code_TYMPAN 
include(tympan-deps)

include_directories(${CMAKE_SOURCE_DIR})
include_directories(${CMAKE_BINARY_DIR})

set(TYMPAN_PythonModules         "python")
# Recurse into main sources directory...
add_subdirectory(Tympan)

# ... and in the test sources directory...
if (TYMPAN_BUILD_TEST)
  enable_testing () # Activate CTest			
  add_subdirectory(tests)
endif (TYMPAN_BUILD_TEST)

# ... and in the python extension sources directory.
if (TYMPAN_BUILD_PYTHON)
  add_subdirectory(python)
endif (TYMPAN_BUILD_PYTHON)

include(tympan-install)
