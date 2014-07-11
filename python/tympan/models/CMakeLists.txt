set(CYTHON_LIB_CURR_DIR "${CYTHON_LIB_CURR_DIR}/models")

# Handles __init__.py files
copy_init_files(${CYTHON_LIB_CURR_DIR})

file(GLOB CYTHON_SOURCES *.pyx)

# Specifies the pyx files are a C++ file
set_source_files_properties(${CYTHON_SOURCES}
  PROPERTIES CYTHON_IS_CXX TRUE )

cython_add_module(common common.pyx)
configure_cython_module(common ${CYTHON_LIB_CURR_DIR})
target_link_libraries(common tympan_models_common)
install_cython_module(common ${CYTHON_LIB_CURR_DIR})

cython_add_module(business business.pyx)
configure_cython_module(business ${CYTHON_LIB_CURR_DIR})
target_link_libraries(business tympan_models_business tympan_models_common tympan_gui_widgets
                      tympan_gui_gl)
install_cython_module(business ${CYTHON_LIB_CURR_DIR})

cython_add_module(solver solver.pyx)
configure_cython_module(solver ${CYTHON_LIB_CURR_DIR})
target_link_libraries(solver tympan_models_solver)
install_cython_module(solver ${CYTHON_LIB_CURR_DIR})
