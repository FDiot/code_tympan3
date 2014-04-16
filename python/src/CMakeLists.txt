

# Specifies the pyx files are a C++ file
set_source_files_properties(toto.pyx
  PROPERTIES CYTHON_IS_CXX TRUE )

cython_add_module(toto toto.pyx totolib.cpp)
configure_cython_module(toto)
install_cython_module(toto)

file(
  COPY .
  DESTINATION "${PROJECT_BINARY_DIR}/${TYMPAN_PythonModules}" 
  FILES_MATCHING PATTERN "*.py" )

install(DIRECTORY ./ 
  DESTINATION ${TYMPAN_PythonModules}
  FILES_MATCHING PATTERN "*.py")


