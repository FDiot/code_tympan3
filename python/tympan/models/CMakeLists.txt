path_relative_to_python(destination)

copy_and_install_python_files(${destination})
# Specifies the pyx files are a C++ file
file(GLOB CYTHON_SOURCES *.pyx)
set_source_files_properties(${CYTHON_SOURCES} PROPERTIES CYTHON_IS_CXX TRUE)

cython_add_module(_common _common.pyx)
configure_cython_module(_common ${destination})
target_link_libraries(_common tympan_common)
install_cython_module(_common ${destination})

cython_add_module(_business _business.pyx)
configure_cython_module(_business ${destination})
target_link_libraries(_business tympan_common tympan_lib)
install_cython_module(_business ${destination})

cython_add_module(_solver _solver.pyx)
configure_cython_module(_solver ${destination})
target_link_libraries(_solver tympan_common tympan_solver_model)
install_cython_module(_solver ${destination})
