project(Code_TYMPAN)
cmake_minimum_required(VERSION 2.8)

if (${CMAKE_VERSION} VERSION_GREATER 2.8.4)
  # This is necessary as of CMake version 2.8.8
  cmake_policy(SET CMP0017 NEW)
  # the way CGAL provides a lot of CMake modules makes the new behaviour preferable
endif(${CMAKE_VERSION} VERSION_GREATER 2.8.4)

# Set path for additionnal CMake modules provides with Tympan
list(APPEND CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake)
include(tympan-platform)

set(${PROJECT_NAME}_VERSION_MAJOR 4)
set(${PROJECT_NAME}_VERSION_MINOR 1)
set(${PROJECT_NAME}_VERSION_PATCH 14)

# This file specifies several build options
include(build-options)
# This file configures the dependencies of Code_TYMPAN 
include(tympan-dependencies)

include_directories(${CMAKE_SOURCE_DIR})
include_directories(${CMAKE_BINARY_DIR})

# Recurse into main sources directory...
add_subdirectory(Tympan)

# ... and in the test sources directory.
if (TYMPAN_BUILD_TEST)
  add_subdirectory(tests)
endif (TYMPAN_BUILD_TEST)

include(tympan-install)
