set(MODULE_NAME tympan_tools)

file(GLOB ${MODULE_NAME}_SRCS *.cpp *.h *.hpp)

#set_precompiled_header(${MODULE_NAME} CXX
#  ${CMAKE_CURRENT_SOURCE_DIR}/XXX.h ${MODULE_NAME}_PCH_SRCS)
add_library(${MODULE_NAME} ${TYMPAN_COMPONENT_TYPE} ${${MODULE_NAME}_SRCS} ${${MODULE_NAME}_PCH_SRCS} )
set_property(TARGET ${MODULE_NAME} PROPERTY FOLDER ${CURRENT_FOLDER})
#use_precompiled_header(${MODULE_NAME} ${MODULE_NAME}_SRCS)

target_link_libraries(${MODULE_NAME} ${QT_LIBRARIES})

install_tympan_component(${MODULE_NAME})