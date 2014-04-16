
# Specifies the pyx files are a C++ file
set_source_files_properties(${TESTS_SCRIPTS}
  PROPERTIES CYTHON_IS_CXX TRUE )

function(configure_cython_module module)
  set_property(TARGET ${module} PROPERTY DEBUG_POSTFIX "")
  set_property(TARGET ${module} PROPERTY LIBRARY_OUTPUT_DIRECTORY_DEBUG
    "${PROJECT_BINARY_DIR}/${TYMPAN_CythonModules_Debug}")
  set_property(TARGET ${module} PROPERTY LIBRARY_OUTPUT_DIRECTORY_RELEASE
    "${PROJECT_BINARY_DIR}/${TYMPAN_CythonModules_Release}")
endfunction()

function(install_cython_module module)
  install(TARGETS ${module}
    DESTINATION ${TYMPAN_CythonModules_Release}  
    CONFIGURATIONS Release)
  
  install(TARGETS ${module}
    DESTINATION ${TYMPAN_CythonModules_Debug} 
    CONFIGURATIONS Debug)  
endfunction()

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


