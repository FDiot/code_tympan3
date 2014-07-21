path_relative_to_python(destination)

copy_and_install_python_files(${destination})
# Specifies the pyx files are a C++ file
file(GLOB CYTHON_SOURCES *.pyx)
set_source_files_properties(${CYTHON_SOURCES} PROPERTIES CYTHON_IS_CXX TRUE)

cython_add_module(common common.pyx)
configure_cython_module(common ${destination})
target_link_libraries(common tympan_common)
install_cython_module(common ${destination})

cython_add_module(business business.pyx)
configure_cython_module(business ${destination})
target_link_libraries(business tympan_common tympan_lib)
install_cython_module(business ${destination})

cython_add_module(solver solver.pyx)
configure_cython_module(solver ${destination})
target_link_libraries(solver tympan_common tympan_lib)
install_cython_module(solver ${destination})
