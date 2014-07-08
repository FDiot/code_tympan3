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
set(CURRENT_FOLDER "Tympan/GUI")

set(MODULE_NAME tympan_tools_graphics)
file(GLOB ${MODULE_NAME}_SRCS/gui/tools
    *.cpp *.h *.hpp)
add_library(${MODULE_NAME} ${TYMPAN_COMPONENT_TYPE} ${${MODULE_NAME}_SRCS} ${${MODULE_NAME}_PCH_SRCS} )
set_property(TARGET ${MODULE_NAME} PROPERTY FOLDER ${CURRENT_FOLDER})
target_link_libraries(${MODULE_NAME} tympan_gui
				     ${QT_LIBRARIES}
				     ${OPENGL_LIBRARIES})
install_tympan_component(${MODULE_NAME})

set(MODULE_NAME tympan_dm_ihm)
file(GLOB ${MODULE_NAME}_SOURCES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}/gui/widgets
    *.cpp *.h *.hpp)
# cf. https://qt-project.org/quarterly/view/using_cmake_to_build_qt_projects
# Headers requiring MOC pre-processing...
file(GLOB ${MODULE_NAME}_MOC_HEADERS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}/gui/widgets *.h )
# ... all in this directory but for the two below
list(REMOVE_ITEM ${MODULE_NAME}_MOC_HEADERS TYIncludesQT.h TYPHIHM.h)
# Call moc on the listed headers, thus generating new sources
qt4_wrap_cpp(${MODULE_NAME}_MOC_SOURCES ${${MODULE_NAME}_MOC_HEADERS})
# Qt Designer UIC files
file(GLOB ${MODULE_NAME}_FORMS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} *.ui )
# Call moc on the listed UI file, thus generating new sources
qt4_wrap_ui(${MODULE_NAME}_FORMS_HEADERS ${${MODULE_NAME}_FORMS})
add_library(${MODULE_NAME} ${TYMPAN_COMPONENT_TYPE}
  ${${MODULE_NAME}_SOURCES}
  ${${MODULE_NAME}_PCH_SOURCES}
  ${${MODULE_NAME}_MOC_SOURCES} # Sources generated by MOC
  ${${MODULE_NAME}_FORMS_HEADERS} # Sources generated by UIC
  )
set_property(TARGET ${MODULE_NAME} PROPERTY FOLDER ${CURRENT_FOLDER})
#use_precompiled_header(${MODULE_NAME} ${MODULE_NAME}_SOURCES)
#add_dependencies(${MODULE_NAME}_pch ${MODULE_NAME}_FORMS_HEADERS)
add_dependencies(${MODULE_NAME} tympan_tools_graphics)
target_link_libraries(${MODULE_NAME} ${QT_LIBRARIES})
install_tympan_component(${MODULE_NAME})

set(MODULE_NAME tympan_dm_graphics)
file(GLOB ${MODULE_NAME}_SRCS/gui/gl
    *.cpp *.h *.hpp)
set_precompiled_header(${MODULE_NAME} CXX
  ${CMAKE_CURRENT_SOURCE_DIR}/TYPHGraphic.h ${MODULE_NAME}_PCH_SRCS)
add_library(${MODULE_NAME} ${TYMPAN_COMPONENT_TYPE} ${${MODULE_NAME}_SRCS} ${${MODULE_NAME}_PCH_SRCS} )
set_property(TARGET ${MODULE_NAME} PROPERTY FOLDER ${CURRENT_FOLDER})
use_precompiled_header(${MODULE_NAME} ${MODULE_NAME}_SRCS)
add_dependencies(${MODULE_NAME} tympan_gui tympan_tools_graphics)
target_link_libraries(${MODULE_NAME} ${QT_LIBRARIES})
install_tympan_component(${MODULE_NAME})


set(CURRENT_FOLDER "Tympan/GUI_Application")
add_subdirectory(gui/app)

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

