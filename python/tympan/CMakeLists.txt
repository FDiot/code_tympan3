set(CYTHON_LIB_CURR_DIR "tympan")

# Handles pure python sources
file(
  COPY .
  DESTINATION "${PROJECT_BINARY_DIR}/${TYMPAN_PythonModules}" 
  FILES_MATCHING PATTERN "*.py" )

install(DIRECTORY ./ 
  DESTINATION ${TYMPAN_PythonModules}
  FILES_MATCHING PATTERN "*.py")

# Handles __init__.py files
copy_init_files(${CYTHON_LIB_CURR_DIR})

# Handles cython modules
file(GLOB CYTHON_SOURCES *.pyx)

if(TYMPAN_DEBUG_CMAKE)
  message(STATUS "TYMPAN_DEBUG: CYTHON_SOURCES: " "${CYTHON_SOURCES}" )
endif()

# Specifies the pyx files are a C++ file
set_source_files_properties(${CYTHON_SOURCES}
  PROPERTIES CYTHON_IS_CXX TRUE )

cython_add_module(pytam pytam.pyx Loader.cpp)
configure_cython_module(pytam ${CYTHON_LIB_CURR_DIR})
target_link_libraries(pytam
    tympan_dm_core tympan_dm_metier tympan_dm_ihm tympan_dm_graphics
    tympan_solver_data_model)
install_cython_module(pytam ${CYTHON_LIB_CURR_DIR})


add_subdirectory( models )
