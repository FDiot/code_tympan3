cmake_minimum_required(VERSION 2.8)
set(MODULE_NAME tympan_dm_metier)

file(GLOB_RECURSE ${MODULE_NAME}_SRCS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} *.cpp *.h *.hpp)
# Some problematic / unused source files to be removed
list(REMOVE_ITEM ${MODULE_NAME}_SRCS
  EltInfrastructure/TYRouteDeblai.cpp EltInfrastructure/TYRouteDeblai.h
  )

set_precompiled_header(${MODULE_NAME} CXX
  ${CMAKE_CURRENT_SOURCE_DIR}/TYPHMetier.h ${MODULE_NAME}_PCH_SRCS)
add_library(${MODULE_NAME} ${TYMPAN_COMPONENT_TYPE} ${${MODULE_NAME}_SRCS} ${${MODULE_NAME}_PCH_SRCS} )
set_property(TARGET ${MODULE_NAME} PROPERTY FOLDER ${CURRENT_FOLDER})

use_precompiled_header(${MODULE_NAME} ${MODULE_NAME}_SRCS)

if(LOGILAB_USE_PRECOMPILED_HEADER)
endif(LOGILAB_USE_PRECOMPILED_HEADER)
add_dependencies(${MODULE_NAME}
    NMPB2008
    tympan_tools_metier tympan_solver_data_model tympan_tools tympan_dm_core
    tympan_tools_graphics
    GTest)

target_link_libraries(${MODULE_NAME} ${QT_LIBRARIES} ${CGAL_LIBRARIES} RoadEmissionNMPB tympan_tools_metier tympan_solver_data_model tympan_tools tympan_dm_core tympan_tools_graphics tympan_analytic_raytracer)

install_tympan_component(${MODULE_NAME})
