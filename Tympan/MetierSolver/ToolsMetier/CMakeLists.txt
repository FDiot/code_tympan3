cmake_minimum_required(VERSION 2.8)
set(MODULE_NAME tympan_tools_metier)

file(GLOB ${MODULE_NAME}_SRCS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} *.cpp *.h)

# Those objects are also used by Solvers and as such need to be build as PIC
# Once the solvers will be independents modules / executable this directory
# should build a shared library use by the GUI and the SolverDataModel

#set_precompiled_header(${MODULE_NAME} CXX
#  ${CMAKE_CURRENT_SOURCE_DIR}/XXX.h ${MODULE_NAME}_PCH_SRCS)
add_library(${MODULE_NAME} ${TYMPAN_COMPONENT_TYPE} ${${MODULE_NAME}_SRCS} ${${MODULE_NAME}_PCH_SRCS} )
set_property(TARGET ${MODULE_NAME} PROPERTY FOLDER ${CURRENT_FOLDER})
#use_precompiled_header(${MODULE_NAME} ${MODULE_NAME}_SRCS)


add_dependencies(${MODULE_NAME} tympan_tools)

target_link_libraries(${MODULE_NAME} tympan_tools ${QT_LIBRARIES})

install_tympan_component(${MODULE_NAME})