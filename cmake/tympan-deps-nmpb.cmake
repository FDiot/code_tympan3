# Add the NMPB2008 library 3rd party as an external project.
ExternalProject_Add(NMPB2008
  URL "${TYMPAN_3RDPARTY_NMPB2008}"
  # URL_MD5 "${TYMPAN_3RDPARTY_DIME_MD5}"
  CMAKE_ARGS -DCMAKE_INSTALL_PREFIX:PATH=<INSTALL_DIR>
  BUILD_IN_SOURCE 0
)
# Get some properties from NMPB2008 project.
ExternalProject_Get_Property(NMPB2008 INSTALL_DIR)
# INSTALL_DIR is now related to the NMPB2008 project.
set(NMPB2008_INSTALL_DIR ${INSTALL_DIR})
set(NMPB2008_INCLUDE_DIR ${NMPB2008_INSTALL_DIR}/include)
set(NMPB2008_LIBRARY_DIR ${NMPB2008_INSTALL_DIR}/lib)

include_directories(${NMPB2008_INCLUDE_DIR})
link_directories(${NMPB2008_LIBRARY_DIR})

if(MSVC)
  # It is required to install the prebuilt NMPB08 DLL on windows
  # along the executable and under the name `RoadEmissionNMPB.dll`
  install(FILES ${NMPB2008_INSTALL_DIR}/RoadEmissionNMPB.dll
    DESTINATION .)
  list(APPEND TYMPAN_3RDPARTY_DLL_DIRS ${NMPB2008_INSTALL_DIR})
else()
  set(filename ${CMAKE_SHARED_LIBRARY_PREFIX}RoadEmissionNMPB${CMAKE_SHARED_LIBRARY_SUFFIX})
  install(FILES ${NMPB2008_INSTALL_DIR}/lib/${filename} DESTINATION lib)
  set(filename)
endif(MSVC)
