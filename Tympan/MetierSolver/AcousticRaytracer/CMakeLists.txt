cmake_minimum_required(VERSION 2.8)
set(MODULE_NAME tympan_acoustic_raytracer)

file(GLOB_RECURSE ${MODULE_NAME}_SRCS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} *.cpp *.h *.hpp)
list(REMOVE_ITEM ${MODULE_NAME}_SRCS "Acoustic/NMPBSolver.h" "Acoustic/NMPBSolver.cpp" "main.cpp")

#set_precompiled_header(${MODULE_NAME} CXX
#  ${CMAKE_CURRENT_SOURCE_DIR}/XXX.h ${MODULE_NAME}_PCH_SRCS)
add_library(${MODULE_NAME} ${TYMPAN_COMPONENT_TYPE} ${${MODULE_NAME}_SRCS} ${${MODULE_NAME}_PCH_SRCS} )
set_property(TARGET ${MODULE_NAME} PROPERTY FOLDER ${CURRENT_FOLDER})
#use_precompiled_header(${MODULE_NAME} ${MODULE_NAME}_SRCS)

add_dependencies(${MODULE_NAME} tympan_common_tools)
target_link_libraries(${MODULE_NAME} ${QT_LIBRARIES})

install_tympan_component(${MODULE_NAME})