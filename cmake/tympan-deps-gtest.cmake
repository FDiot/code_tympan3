# Add the Google Test project as an external project.
ExternalProject_Add(GTest
  URL ${TYMPAN_3RDPARTY_GTEST}
  URL_MD5 ${TYMPAN_3RDPARTY_GTEST_MD5}
  # Configuration step. GTest doc said "Use shared (DLL) run-time lib even when
  # Google Test is built as static lib."
  CMAKE_ARGS -Dgtest_force_shared_crt=ON  
  INSTALL_COMMAND ""
  BUILD_IN_SOURCE 0
)
ExternalProject_Get_Property(GTest SOURCE_DIR)
set(GTEST_SOURCE_DIR ${SOURCE_DIR})
set(GTEST_INCLUDE_DIR ${GTEST_SOURCE_DIR}/include)

include_directories (${GTEST_INCLUDE_DIR})
#NB Adding GTest include in the non-test tree is to enable gtest_prod.h

if(TYMPAN_DEBUG_CMAKE)
message(STATUS "INFO GTEST_INCLUDE_DIR: " "${GTEST_INCLUDE_DIR}" )
endif()
