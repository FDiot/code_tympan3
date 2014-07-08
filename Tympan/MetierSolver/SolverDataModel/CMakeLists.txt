cmake_minimum_required(VERSION 2.8)
set(MODULE_NAME tympan_solver_data_model)

file(GLOB ${MODULE_NAME}_SRCS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} *.cpp *.hpp)

set_precompiled_header(${MODULE_NAME} CXX
  ${CMAKE_CURRENT_SOURCE_DIR}/std_boost_utils.hpp ${MODULE_NAME}_PCH_SRCS)
add_library(${MODULE_NAME} ${TYMPAN_COMPONENT_TYPE} ${${MODULE_NAME}_SRCS} ${${MODULE_NAME}_PCH_SRCS} )
set_property(TARGET ${MODULE_NAME} PROPERTY FOLDER ${CURRENT_FOLDER})
use_precompiled_header(${MODULE_NAME} ${MODULE_NAME}_SRCS)

add_dependencies( ${MODULE_NAME} tympan_tools_metier tympan_models_common)

target_link_libraries( ${MODULE_NAME} tympan_tools_metier tympan_models_common)

install_tympan_component(${MODULE_NAME})
