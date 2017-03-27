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

cython_add_module(_business2solver _business2solver.pyx)
configure_cython_module(_business2solver ${destination})
include_directories(${CMAKE_SOURCE_DIR} ${TYMPAN_ACOUSTICRAYTRACER})
target_link_libraries(_business2solver tympan_lib tympan_common tympan_solver_model)
install_cython_module(_business2solver ${destination})

add_subdirectory(models)
