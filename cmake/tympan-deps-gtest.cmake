# Add the Google Test project as an external project.
ExternalProject_Add(GTest
  URL ${TYMPAN_3RDPARTY_GTEST}
  SOURCE_DIR "${PROJECT_SOURCE_DIR}/3rdparty/gtest-1.6.0"

  # Configuration step. GTest doc said "Use shared (DLL) run-time lib even when
  # Google Test is built as static lib."
  CMAKE_ARGS -Dgtest_force_shared_crt=ON
  # Installation step. Disable install step.
  INSTALL_COMMAND ""
)
ExternalProject_Get_Property (GTest SOURCE_DIR BINARY_DIR)
# SOURCE_DIR is related to the GTest project.
include_directories (${SOURCE_DIR}/include)
#NB Adding GTest include in the non-test tree is to enable gtest_prod.h

# TODO : install GTest and use it from the install dir instead of explicit source dir
