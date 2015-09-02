# Add the Dime library 3rd party as an external project.
ExternalProject_Add(Dime
  URL "${TYMPAN_3RDPARTY_DIME}"
  URL_MD5 "${TYMPAN_3RDPARTY_DIME_MD5}"
  BUILD_IN_SOURCE 0

  # Installation step. Disable install step.
  INSTALL_COMMAND ""
)

# Get some properties from Dime project.
ExternalProject_Get_Property (Dime SOURCE_DIR BINARY_DIR)
# SOURCE_DIR is related to the Dime project.
include_directories (${SOURCE_DIR}/include)
# BINARY_DIR is related to the Dime project.
link_directories (${BINARY_DIR}/src)
