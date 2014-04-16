# Handles pure python sources
file(
  COPY .
  DESTINATION "${PROJECT_BINARY_DIR}/${TYMPAN_PythonModules}" 
  FILES_MATCHING PATTERN "*.py" )

install(DIRECTORY ./ 
  DESTINATION ${TYMPAN_PythonModules}
  FILES_MATCHING PATTERN "*.py")

# Handles cython modules
file(GLOB CYTHON_SOURCES *.pyx)

if(TYMPAN_DEBUG_CMAKE)
  message(STATUS "TYMPAN_DEBUG: CYTHON_SOURCES: " "${CYTHON_SOURCES}" )
endif()

# Specifies the pyx files are a C++ file
set_source_files_properties(${CYTHON_SOURCES}
  PROPERTIES CYTHON_IS_CXX TRUE )

# This is only a toy module to validate the CMake / Cython integration
cython_add_module(toto toto.pyx totolib.cpp)
configure_cython_module(toto)
install_cython_module(toto)


cython_add_module(pytam pytam.pyx Loader.cpp)
configure_cython_module(pytam)
target_link_libraries(pytam 
    QtGui QtXml tympan_dm_core tympan_dm_metier tympan_dm_ihm tympan_dm_graphics
    tympan_solver_data_model tympan_data_exchange)
install_cython_module(pytam)

