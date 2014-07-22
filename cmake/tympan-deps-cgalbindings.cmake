# Add the CGAL-bindings 3rd party as an external project.
ExternalProject_Add(CGAL_bindings
  URL "${TYMPAN_3RDPARTY_CGAL_BINDINGS}"
  URL_MD5 "${TYMPAN_3RDPARTY_CGAL_BINDINGS_MD5}"
  CMAKE_ARGS -DBUILD_JAVA=OFF -DCMAKE_BUILD_TYPE:STRING=Release -DCMAKE_INSTALL_PREFIX:PATH=<INSTALL_DIR>
  BUILD_COMMAND ${CMAKE_COMMAND} --build . --config Release
  INSTALL_COMMAND ${CMAKE_COMMAND} --build . --config Release --target install
  BUILD_IN_SOURCE 0
)
# Get BINARY_DIR property from CGAL_bindings project.
ExternalProject_Get_Property(CGAL_bindings INSTALL_DIR)
set(CGAL_BINDINGS_PYTHON_DIR ${INSTALL_DIR}/cgal-bindings/CGAL)
ExternalProject_Get_Property(CGAL_bindings BINARY_DIR)
set(CGAL_BINDINGS_LIB_DIR ${BINARY_DIR}/lib)

# Install language independent libraries from CGAL bindings
if(SYS_NATIVE_WIN)
install(DIRECTORY ${CGAL_BINDINGS_LIB_DIR}/Release/ #Significant trailing slash!
    DESTINATION .
    FILES_MATCHING PATTERN "*.dll")
else()
  install(DIRECTORY ${CGAL_BINDINGS_LIB_DIR}/ #Significant trailing slash!
    DESTINATION "lib"
    FILES_MATCHING PATTERN "*.so")
endif(SYS_NATIVE_WIN)
# Install Python modules 
install(DIRECTORY ${CGAL_BINDINGS_PYTHON_DIR} #Significant NO trailing slash!
    DESTINATION "${TYMPAN_CythonModules_Release}"
    CONFIGURATIONS Release)
install(DIRECTORY ${CGAL_BINDINGS_PYTHON_DIR} #Significant NO trailing slash!
    DESTINATION "${TYMPAN_CythonModules_Debug}"
    CONFIGURATIONS Debug)