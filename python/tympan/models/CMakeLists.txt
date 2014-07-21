path_relative_to_python(destination)

copy_and_install_python_files(${destination})

file(GLOB CYTHON_SOURCES *.pyx)
# Specifies the pyx files are a C++ file
set_source_files_properties(${CYTHON_SOURCES}
  PROPERTIES CYTHON_IS_CXX TRUE )

cython_add_module(common common.pyx)
configure_cython_module(common ${destination})
target_link_libraries(common tympan_common_tools)
install_cython_module(common ${destination})

cython_add_module(business business.pyx)
configure_cython_module(business ${destination})
target_link_libraries(business tympan_dm_metier tympan_dm_core tympan_dm_ihm
                      tympan_dm_graphics)
install_cython_module(business ${destination})

cython_add_module(solver solver.pyx)
configure_cython_module(solver ${destination})
target_link_libraries(solver tympan_common_tools tympan_solver_data_model)
install_cython_module(solver ${destination})
