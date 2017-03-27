cmake_minimum_required(VERSION 2.8)

######### AcousticRayTracer #######################################################
file(GLOB TYMPAN_ACOUSTICRAYTRACER_SRCS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
    *.cpp *.h
    */*.cpp */*.h
)
include_directories(${CMAKE_CURRENT_SOURCE_DIR})
add_library(tympan_acousticraytracer ${TYMPAN_COMPONENT_TYPE} ${TYMPAN_ACOUSTICRAYTRACER_SRCS})
set_property(TARGET tympan_acousticraytracer PROPERTY FOLDER ${TYMPAN_ACOUSTICRAYTRACER})
install_tympan_component(tympan_acousticraytracer)
