set(CYTHON_LIB_CURR_DIR "${CYTHON_LIB_CURR_DIR}/models")

# Handles __init__.py files
copy_init_files(${CYTHON_LIB_CURR_DIR})

file(GLOB CYTHON_SOURCES *.pyx)

# Specifies the pyx files are a C++ file
set_source_files_properties(${CYTHON_SOURCES}
  PROPERTIES CYTHON_IS_CXX TRUE )

cython_add_module(common common.pyx)
configure_cython_module(common ${CYTHON_LIB_CURR_DIR})
target_link_libraries(common tympan_common_tools)
install_cython_module(common ${CYTHON_LIB_CURR_DIR})

