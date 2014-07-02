cmake_minimum_required(VERSION 2.8)
set(MODULE_NAME tympan_default_solver)

file(GLOB_RECURSE ${MODULE_NAME}_SRCS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} *.cpp *.h *.hpp)

set_property(SOURCE ${${MODULE_NAME}_SRCS} APPEND PROPERTY COMPILE_DEFINITIONS _PLUGIN_DLL)
link_directories(${CGAL_LIBRARIES_DIR})

#set_precompiled_header(${MODULE_NAME} CXX
#  ${CMAKE_CURRENT_SOURCE_DIR}/XXX.h ${MODULE_NAME}_PCH_SRCS)
add_library(${MODULE_NAME} MODULE ${${MODULE_NAME}_SRCS} ${${MODULE_NAME}_PCH_SRCS} )
set_property(TARGET ${MODULE_NAME} PROPERTY FOLDER ${CURRENT_FOLDER})
#use_precompiled_header(${MODULE_NAME} ${MODULE_NAME}_SRCS)

add_dependencies(${MODULE_NAME} ${TYMPAN_MODULES} tympan_convexhull_finder)

target_link_libraries(${MODULE_NAME} ${QT_LIBRARIES} ${TYMPAN_MODULES}
                      tympan_convexhull_finder ${OPENGL_LIBRARIES} )

configure_tympan_plugin(${MODULE_NAME})
install_tympan_plugin(${MODULE_NAME})
