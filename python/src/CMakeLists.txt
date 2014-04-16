# If the pyx file is a C++ file, we should specify that here.
set_source_files_properties(
    ${CMAKE_CURRENT_SOURCE_DIR}/toto.pyx
  PROPERTIES CYTHON_IS_CXX TRUE )

cython_add_module(toto toto.pyx totolib.cpp)

set_property(TARGET toto PROPERTY DEBUG_POSTFIX "")

