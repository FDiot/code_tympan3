cmake_minimum_required(VERSION 2.8)

# This enable includes relative to current source file dir
# TODO required for now (esp. by PCH) but should ideally be deactivated
set(CMAKE_INCLUDE_CURRENT_DIR ON)

include_directories (${GTEST_INCLUDE_DIR})
#NB Adding GTest include in the non-test tree is required to enable gtest_prod.h

set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
  APPEND PROPERTY COMPILE_DEFINITIONS_DEBUG _DEBUG)


### Precompiled header handling ###
if(${CMAKE_VERSION} VERSION_LESS 2.8.9 AND LOGILAB_USE_PRECOMPILED_HEADER)

  set(LOGILAB_USE_PRECOMPILED_HEADER OFF
    CACHE BOOL "Pre-compiled headers forcibly disabled." FORCE)
  message(SEND_ERROR
    "Pre-compiled headers support requires CMake >= 2.8.9. "
    "PCH support forcibly disabled ; you can try to run cmake again.")
endif(${CMAKE_VERSION} VERSION_LESS 2.8.9 AND LOGILAB_USE_PRECOMPILED_HEADER)

# logilab-pch need to be included even when pre-compiled headers are disabled
# (i.e. LOGILAB_USE_PRECOMPILED_HEADER is OFF).
# Indeed, it then defines dummy compatibility macros.
include(logilab-pch)

if(LOGILAB_USE_PRECOMPILED_HEADER)
  # This triggers the inclusion of the TYPH*.h headers from the *.cpp files
  add_definitions(-DTYMPAN_USE_PRECOMPILED_HEADER)
else(LOGILAB_USE_PRECOMPILED_HEADER)
  message(STATUS
    "Because the way the headers are inappropriately handled, "
    "we had to force #define the TYMPAN_USE_PRECOMPILED_HEADER  "
    "and pay the price of excessive dependencies, at least for now...")
  add_definitions(-DTYMPAN_USE_PRECOMPILED_HEADER)
endif(LOGILAB_USE_PRECOMPILED_HEADER)


### Configuring source code ###
configure_file(
  ${CMAKE_CURRENT_SOURCE_DIR}/TYVersion.h.in
  ${CMAKE_CURRENT_BINARY_DIR}/TYVersion.h
  @ONLY
)
set(PYMODULES_DIR "pymodules")
set(PYSCRIPT "${PYMODULES_DIR}/solve_project.py")
configure_file(
  ${CMAKE_CURRENT_SOURCE_DIR}/Config.h.in
  ${CMAKE_CURRENT_BINARY_DIR}/Config.h
  @ONLY
  )
include_directories(${CMAKE_BINARY_DIR})

### Setting sub-directories and modules up ###
set(CURRENT_FOLDER "Tympan")

# The Tympan components must be enumerated here in reverse depence order
set(TYMPAN_MODULES  tympan_dm_metier tympan_dm_graphics tympan_dm_ihm
		    tympan_tools_graphics
		    tympan_models_common
)


set(MODULE_NAME tympan_models_common)
file(GLOB ${MODULE_NAME}_SRCS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
    *.cpp *.h
    core/*.cpp core/*.h
    models/common/*.cpp models/common/*.h
)
# Those objects are also used by Solvers and as such need to be build as PIC
# Once the solvers will be independents modules / executable this directory
# should build a shared library use by the GUI and the SolverDataModel
add_library(${MODULE_NAME} ${TYMPAN_COMPONENT_TYPE} ${${MODULE_NAME}_SRCS} ${${MODULE_NAME}_PCH_SRCS} )
set_property(TARGET ${MODULE_NAME} PROPERTY FOLDER "Tympan/models/common")
#target_link_libraries(${MODULE_NAME} ${QT_LIBRARIES})
install_tympan_component(${MODULE_NAME})

set(MODULE_NAME tympan_gui)
file(GLOB ${MODULE_NAME}_SRCS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
    gui/*.cpp gui/*.h
)
# Those objects are also used by Solvers and as such need to be build as PIC
# Once the solvers will be independents modules / executable this directory
# should build a shared library use by the GUI and the SolverDataModel
add_library(${MODULE_NAME} ${TYMPAN_COMPONENT_TYPE} ${${MODULE_NAME}_SRCS} ${${MODULE_NAME}_PCH_SRCS} )
set_property(TARGET ${MODULE_NAME} PROPERTY FOLDER "Tympan/gui")
target_link_libraries(${MODULE_NAME} ${QT_LIBRARIES})
install_tympan_component(${MODULE_NAME})


set(MODULE_NAME tympan_models_solver)
file(GLOB ${MODULE_NAME}_SRCS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}/models/solver
    *.cpp *.h *.hpp)
set_precompiled_header(${MODULE_NAME} CXX
  ${CMAKE_CURRENT_SOURCE_DIR}/std_boost_utils.hpp ${MODULE_NAME}_PCH_SRCS)
add_library(${MODULE_NAME} ${TYMPAN_COMPONENT_TYPE} ${${MODULE_NAME}_SRCS} ${${MODULE_NAME}_PCH_SRCS} )
set_property(TARGET ${MODULE_NAME} PROPERTY FOLDER "Tympan/models/solver")
use_precompiled_header(${MODULE_NAME} ${MODULE_NAME}_SRCS)
add_dependencies( ${MODULE_NAME} tympan_models_common)
target_link_libraries( ${MODULE_NAME} tympan_models_common)
install_tympan_component(${MODULE_NAME})


set(MODULE_NAME tympan_dm_metier)
file(GLOB_RECURSE ${MODULE_NAME}_SRCS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}/models/business
    *.cpp *.h *.hpp
)
set_precompiled_header(${MODULE_NAME} CXX
  ${CMAKE_CURRENT_SOURCE_DIR}/TYPHMetier.h ${MODULE_NAME}_PCH_SRCS)
add_library(${MODULE_NAME} ${TYMPAN_COMPONENT_TYPE} ${${MODULE_NAME}_SRCS} ${${MODULE_NAME}_PCH_SRCS} )
set_property(TARGET ${MODULE_NAME} PROPERTY FOLDER "Tympan/models/business")
use_precompiled_header(${MODULE_NAME} ${MODULE_NAME}_SRCS)
add_dependencies(${MODULE_NAME}
    NMPB2008
    tympan_models_solver
    tympan_tools_graphics tympan_dm_ihm
    GTest)
target_link_libraries(${MODULE_NAME} ${QT_LIBRARIES} ${CGAL_LIBRARIES}
    RoadEmissionNMPB
    tympan_models_solver tympan_tools_graphics)
install_tympan_component(${MODULE_NAME})


# The inclusion order seems to be significant for dependencies
add_subdirectory(GraphicIHM)
add_subdirectory(Appli)

set(CURRENT_FOLDER "Tympan/solvers")

set(MODULE_NAME tympan_analytic_raytracer)
file(GLOB_RECURSE ${MODULE_NAME}_SRCS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}/solvers/AnalyticRayTracer
    *.cpp *.h *.hpp)
add_library(${MODULE_NAME} ${TYMPAN_COMPONENT_TYPE} ${${MODULE_NAME}_SRCS} ${${MODULE_NAME}_PCH_SRCS} )
set_property(TARGET ${MODULE_NAME} PROPERTY FOLDER ${CURRENT_FOLDER})
target_link_libraries(${MODULE_NAME} ${QT_LIBRARIES} tympan_acoustic_raytracer)
install_tympan_component(${MODULE_NAME})

set(MODULE_NAME tympan_acoustic_raytracer)
file(GLOB_RECURSE ${MODULE_NAME}_SRCS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}/solvers/AcousticRaytracer
    *.cpp *.h *.hpp)
list(REMOVE_ITEM ${MODULE_NAME}_SRCS "Acoustic/NMPBSolver.h" "Acoustic/NMPBSolver.cpp" "main.cpp")
add_library(${MODULE_NAME} ${TYMPAN_COMPONENT_TYPE} ${${MODULE_NAME}_SRCS} ${${MODULE_NAME}_PCH_SRCS} )
set_property(TARGET ${MODULE_NAME} PROPERTY FOLDER ${CURRENT_FOLDER})
add_dependencies(${MODULE_NAME} tympan_models_common)
target_link_libraries(${MODULE_NAME} ${QT_LIBRARIES})
install_tympan_component(${MODULE_NAME})

set(MODULE_NAME tympan_convexhull_finder)
file(GLOB_RECURSE ${MODULE_NAME}_SRCS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}/solvers/ConvexHullFinder
    *.cpp *.h *.hpp)
list(REMOVE_ITEM ${MODULE_NAME}_SRCS "Acoustic/NMPBSolver.h" "Acoustic/NMPBSolver.cpp" "main.cpp")
add_library(${MODULE_NAME} ${TYMPAN_COMPONENT_TYPE} ${${MODULE_NAME}_SRCS} ${${MODULE_NAME}_PCH_SRCS} )
set_property(TARGET ${MODULE_NAME} PROPERTY FOLDER ${CURRENT_FOLDER})
add_dependencies(${MODULE_NAME} tympan_models_common)
target_link_libraries(${MODULE_NAME} ${QT_LIBRARIES})
install_tympan_component(${MODULE_NAME})

set(MODULE_NAME tympan_default_solver)
file(GLOB_RECURSE ${MODULE_NAME}_SRCS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}/solvers/DefaultSolver
    *.cpp *.h *.hpp)
set_property(SOURCE ${${MODULE_NAME}_SRCS} APPEND PROPERTY COMPILE_DEFINITIONS _PLUGIN_DLL)
link_directories(${CGAL_LIBRARIES_DIR})
add_library(${MODULE_NAME} MODULE ${${MODULE_NAME}_SRCS} ${${MODULE_NAME}_PCH_SRCS} )
set_property(TARGET ${MODULE_NAME} PROPERTY FOLDER ${CURRENT_FOLDER})
add_dependencies(${MODULE_NAME} ${TYMPAN_MODULES} tympan_convexhull_finder)
target_link_libraries(${MODULE_NAME} ${QT_LIBRARIES} ${TYMPAN_MODULES}
                      tympan_convexhull_finder ${OPENGL_LIBRARIES} )
configure_tympan_plugin(${MODULE_NAME})
install_tympan_plugin(${MODULE_NAME})


set(MODULE_NAME tympan_ANIME3D_solver)
file(GLOB_RECURSE ${MODULE_NAME}_SRCS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}/solvers/ANIME3D
    *.cpp *.h *.hpp)
set_property(SOURCE ${${MODULE_NAME}_SRCS} APPEND PROPERTY COMPILE_DEFINITIONS _PLUGIN_DLL)
link_directories(${CGAL_LIBRARIES_DIR})
add_library(${MODULE_NAME} ${TYMPAN_COMPONENT_TYPE} MODULE ${${MODULE_NAME}_SRCS} ${${MODULE_NAME}_PCH_SRCS} )
set_property(TARGET ${MODULE_NAME} PROPERTY FOLDER ${CURRENT_FOLDER})
add_dependencies(${MODULE_NAME} ${TYMPAN_MODULES} tympan_acoustic_raytracer tympan_analytic_raytracer)
target_link_libraries(${MODULE_NAME} ${QT_LIBRARIES}
                      tympan_acoustic_raytracer tympan_analytic_raytracer
                      ${TYMPAN_MODULES} ${OPENGL_LIBRARIES})
configure_tympan_plugin(${MODULE_NAME})
install_tympan_plugin(${MODULE_NAME})

