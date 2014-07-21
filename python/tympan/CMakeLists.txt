path_relative_to_python(destination)
# Handles pure python sources
copy_and_install_python_files(${destination})

# Handles cython modules
file(GLOB CYTHON_SOURCES *.pyx)

if(TYMPAN_DEBUG_CMAKE)
  message(STATUS "TYMPAN_DEBUG: CYTHON_SOURCES: " "${CYTHON_SOURCES}" )
endif()

# Specifies the pyx files are a C++ file
set_source_files_properties(${CYTHON_SOURCES}
  PROPERTIES CYTHON_IS_CXX TRUE )

cython_add_module(business2solver business2solver.pyx)
configure_cython_module(business2solver ${destination})
target_link_libraries(business2solver tympan_lib)
install_cython_module(business2solver ${destination})

add_subdirectory(models)
