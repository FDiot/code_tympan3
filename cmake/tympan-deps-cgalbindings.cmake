# Add the CGAL-bindings 3rd party as an external project.
ExternalProject_Add(CGAL_bindings
  URL "${TYMPAN_3RDPARTY_CGAL_BINDINGS}"
  URL_MD5 "${TYMPAN_3RDPARTY_CGAL_BINDINGS_MD5}"
  CMAKE_ARGS -DBUILD_JAVA=OFF -DCMAKE_BUILD_TYPE:STRING=Release -DCMAKE_INSTALL_PREFIX:PATH=<INSTALL_DIR> -DPython_ADDITIONAL_VERSIONS=2.7
  BUILD_COMMAND ${CMAKE_COMMAND} --build . --config Release
  INSTALL_COMMAND ${CMAKE_COMMAND} --build . --config Release --target install
  BUILD_IN_SOURCE 0
)
# Get BINARY_DIR property from CGAL_bindings project.
ExternalProject_Get_Property(CGAL_bindings INSTALL_DIR)
set(CGAL_BINDINGS_PYTHON_DIR ${INSTALL_DIR}/cgal-bindings)
ExternalProject_Get_Property(CGAL_bindings BINARY_DIR)

# Install language independent libraries from CGAL bindings
if(SYS_NATIVE_WIN)
  set(CGAL_BINDINGS_LIB_DIR ${BINARY_DIR}/lib/Release)  
  install(DIRECTORY ${CGAL_BINDINGS_LIB_DIR}/ #Significant trailing slash!
    DESTINATION .
    FILES_MATCHING PATTERN "*.dll")
else()
  set(CGAL_BINDINGS_LIB_DIR ${BINARY_DIR}/lib)
  install(DIRECTORY ${CGAL_BINDINGS_LIB_DIR}/ #Significant trailing slash!
    DESTINATION "lib"
    FILES_MATCHING PATTERN "*.so")
endif(SYS_NATIVE_WIN)
list(APPEND TYMPAN_3RDPARTY_DLL_DIRS ${CGAL_BINDINGS_LIB_DIR})

# Creating the __init__.py in the install directory of the CGAL bindings
#  => Actually the CMakeLists of CGAL-bindings should be fixed
file(WRITE ${CGAL_BINDINGS_PYTHON_DIR}/CGAL/__init__.py "#created by CMake from Code_TYMPAN")

# Install Python modules 
install(DIRECTORY ${CGAL_BINDINGS_PYTHON_DIR}/CGAL #Significant NO trailing slash!
    DESTINATION "${TYMPAN_CythonModules_Release}"
    CONFIGURATIONS Release)
install(DIRECTORY ${CGAL_BINDINGS_PYTHON_DIR}/CGAL #Significant NO trailing slash!
    DESTINATION "${TYMPAN_CythonModules_Debug}"
    CONFIGURATIONS Debug)

