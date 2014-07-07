# Add the CGAL-bindings 3rd party as an external project.
ExternalProject_Add(CGAL_bindings
  URL "${TYMPAN_3RDPARTY_CGAL_BINDINGS}"
  URL_MD5 "${TYMPAN_3RDPARTY_CGAL_BINDINGS_MD5}"
  CMAKE_ARGS -DBUILD_JAVA=OFF
  BUILD_COMMAND ${CMAKE_COMMAND} --build . --config Release
  INSTALL_COMMAND ""
  BUILD_IN_SOURCE 0
)
# Get BINARY_DIR property from CGAL_bindings project.
ExternalProject_Get_Property(CGAL_bindings BINARY_DIR)
set(CGAL_BINDINGS_DIR ${BINARY_DIR}/build-python)
set(CGAL_BINDINGS_LIB_DIR ${CGAL_BINDINGS_DIR}/CGAL)

link_directories(${CGAL_BINDINGS_LIB_DIR})

if(SYS_NATIVE_WIN)
    install(FILES ${CGAL_BINDINGS_LIB_DIR}/CGAL_HalfedgeDS.dll
      ${CGAL_BINDINGS_LIB_DIR}/CGAL_Kernel.dll
      ${CGAL_BINDINGS_LIB_DIR}/CGAL_Mesh_2.dll
      ${CGAL_BINDINGS_LIB_DIR}/CGAL_Triangulation_2_cpp.dll
    DESTINATION .)
list(APPEND TYMPAN_3RDPARTY_DLL_DIRS ${CGAL_BINDINGS_LIB_DIR})
else()
    set(filename _CGAL_HalfedgeDS${CMAKE_SHARED_LIBRARY_SUFFIX})
    install(FILES ${CGAL_BINDINGS_LIB_DIR}/${filename} DESTINATION lib)
    set(filename _CGAL_Kernel${CMAKE_SHARED_LIBRARY_SUFFIX})
    install(FILES ${CGAL_BINDINGS_LIB_DIR}/${filename} DESTINATION lib)
    set(filename _CGAL_Mesh_2${CMAKE_SHARED_LIBRARY_SUFFIX})
    install(FILES ${CGAL_BINDINGS_LIB_DIR}/${filename} DESTINATION lib)
    set(filename _CGAL_Triangulation_2${CMAKE_SHARED_LIBRARY_SUFFIX})
    install(FILES ${CGAL_BINDINGS_LIB_DIR}/${filename} DESTINATION lib)
    set(filename)
endif(SYS_NATIVE_WIN)
