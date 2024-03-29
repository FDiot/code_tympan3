cmake_minimum_required(VERSION 2.8)

# from cmake 2.8.12, COMPILE_DEFINITION_DEBUG/RELEASE shouldn't be used anymore and should be
# replaced by generator expressions (see https://cmake.org/cmake/help/v3.0/policy/CMP0043.html)
# CMP0043 policy exists from major version 3.
if (NOT(${CMAKE_MAJOR_VERSION} VERSION_LESS "3"))
    cmake_policy(SET CMP0043 OLD)
endif()

# Adding GTest include in the non-test tree is required to enable gtest_prod.h
include_directories (${GTEST_INCLUDE_DIR})
include_directories(${CMAKE_SOURCE_DIR} ${CMAKE_SOURCE_DIR}/${TYMPAN_ACOUSTICRAYTRACER})
set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
  APPEND PROPERTY COMPILE_DEFINITIONS_DEBUG _DEBUG)


# Configuring source code ######################################################

# use find_package from CMake module directory instead of the one found in
# CMAKE_MODULE_PATH coming from CGAL
cmake_policy(SET CMP0017 NEW)
find_package(PythonInterp 3)

# Generate initial solver parameters in DefaultSolverConfig.h
set(GEN_SOLVER_PARAMS_HEADER_PYSCRIPT "${CMAKE_SOURCE_DIR}/tools/generate_solver_config_header.py")
set(SOLVER_PARAMS_JSON "resources/solver_config_datamodel.json")
set(SOLVER_PARAMS_HEADER "${CMAKE_CURRENT_SOURCE_DIR}/models/business/DefaultSolverConfig.h")
add_custom_command(OUTPUT ${SOLVER_PARAMS_HEADER}
                  COMMAND ${PYTHON_EXECUTABLE} ${GEN_SOLVER_PARAMS_HEADER_PYSCRIPT}
                          -p "${CMAKE_SOURCE_DIR}/${SOLVER_PARAMS_JSON}" -o ${SOLVER_PARAMS_HEADER}
                  COMMENT "Generating models/business/DefaultSolverConfig.h ..."
                  WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
                  DEPENDS "${CMAKE_SOURCE_DIR}/${SOLVER_PARAMS_JSON}")

# Update some files into AcousticRaytracer/Geometry from models/common to create an external AcousticRayTracer library:
set(SOURCE_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/models/common")
set(TARGET_DIRECTORY "${CMAKE_SOURCE_DIR}/${TYMPAN_ACOUSTICRAYTRACER}/Geometry")
foreach(file 3d.cpp;3d.h;mathlib.h;config.h)
add_custom_command(OUTPUT ${TARGET_DIRECTORY}/${file}
                   COMMAND ${PYTHON_EXECUTABLE} ${CMAKE_SOURCE_DIR}/tools/copy_add_header.py -r "${CMAKE_SOURCE_DIR}" -i "${SOURCE_DIRECTORY}/${file}" -o "${TARGET_DIRECTORY}/${file}"
                   COMMENT "Updating ${TARGET_DIRECTORY}/${file} ..."
                   WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
                   DEPENDS "${CMAKE_SOURCE_DIR}/tools/copy_add_header.py" "${SOURCE_DIRECTORY}/${file}"
)
endforeach()

set(SOLVE_PYSCRIPT "bin/solve_tympan_project.py")
set(ALTIMETRY_PYSCRIPT "bin/process_site_altimetry.py")
set(SOLVER_PARAMS_GUI_PYSCRIPT "bin/solver_config_gui.py")
configure_file(
  ${CMAKE_CURRENT_SOURCE_DIR}/core/config.h.in
  ${CMAKE_CURRENT_BINARY_DIR}/core/config.h
  @ONLY
  )

# XXX is it useful here and part of source code configuration ?
include_directories(${CMAKE_BINARY_DIR})


# common models ################################################################

file(GLOB TYMPAN_COMMON_SRCS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
    *.cpp *.h
    core/*.cpp core/*.h
    models/common/*.cpp models/common/*.h
)
add_library(tympan_common ${TYMPAN_COMPONENT_TYPE} ${TYMPAN_COMMON_SRCS})
set_property(TARGET tympan_common PROPERTY FOLDER "Tympan/common")
target_link_libraries(tympan_common
    # qstring in core/logging, quuid in core/idgen, qlist in models/common/delaunay_maker.h
    ${QT_QTCORE_LIBRARY}
    # Boost isn't linked as the "header only library" part is used
    # CGAL for mesh/volume intersection and triangulation of the infrastructure
    ${CGAL_LIBRARIES}
)
if (WIN32)
    target_link_libraries(tympan_common winmm.lib)
endif ()
install_tympan_component(tympan_common)


# main library #################################################################

file(GLOB TYMPAN_LIB_SRCS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
    models/business/*.cpp models/business/*.h
    models/business/*/*.cpp models/business/*/*.h
    gui/*.cpp gui/*.h
    gui/tools/*.cpp gui/tools/*.h
    gui/widgets/*.cpp gui/widgets/*.h
    gui/gl/*.cpp gui/gl/*.h
)

if(NOT TYMPAN_USE_NMPB2008)
    list(REMOVE_ITEM TYMPAN_LIB_SRCS
        models/business/infrastructure/TYRoute.h
        models/business/infrastructure/TYRoute.cpp
        gui/gl/TYRouteGraphic.h
        gui/gl/TYRouteGraphic.cpp
        gui/widgets/TYRouteWidget.cpp
        gui/widgets/TYRouteWidget.h
        models/business/acoustic/TYTrafic.h
        models/business/acoustic/TYTrafic.cpp
        gui/widgets/TYTraficWidget.h
        gui/widgets/TYTraficWidget.cpp
    )
endif()

# cf. https://qt-project.org/quarterly/view/using_cmake_to_build_qt_projects
# Headers requiring MOC pre-processing...
file(GLOB TYMPAN_LIB_MOC_HDRS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} gui/widgets/*.h)
# ... all in this directory but for the two below
list(REMOVE_ITEM TYMPAN_LIB_MOC_HDRS
    gui/widgets/TYIncludesQT.h
)
if(NOT TYMPAN_USE_NMPB2008)
    list(REMOVE_ITEM TYMPAN_LIB_MOC_HDRS
        gui/widgets/TYRouteWidget.h
        gui/widgets/TYTraficWidget.h
    )
endif()
# Call moc on the listed headers, thus generating new sources
qt4_wrap_cpp(TYMPAN_LIB_MOC_SRCS ${TYMPAN_LIB_MOC_HDRS})
# Qt Designer UIC files
file(GLOB TYMPAN_LIB_FORMS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} gui/widgets/*.ui)
# Call moc on the listed UI file, thus generating new sources
qt4_wrap_ui(TYMPAN_LIB_FORMS_HDRS ${TYMPAN_LIB_FORMS})
add_library(tympan_lib ${TYMPAN_COMPONENT_TYPE}
    ${TYMPAN_LIB_SRCS}
    ${TYMPAN_LIB_MOC_SRCS} # Sources generated by MOC
    ${TYMPAN_LIB_FORMS_HDRS} # Sources generated by UIC
)
set_property(TARGET tympan_lib PROPERTY FOLDER "Tympan/lib")
set_property(TARGET tympan_lib
  APPEND PROPERTY INCLUDE_DIRECTORIES ${RPLY_INCLUDE_DIRECTORY})
add_dependencies(tympan_lib
    GTest # XXX pour que les tests aient accès à des membres protégés
)
if(TYMPAN_USE_NMPB2008)
  add_dependencies(tympan_lib NMPB2008)
endif(TYMPAN_USE_NMPB2008)
target_link_libraries(tympan_lib
    ${QT_LIBRARIES}
    ${OPENGL_LIBRARIES}
    tympan_common
    ${RPLY_LIB}
)
if(TYMPAN_USE_NMPB2008)
    target_link_libraries(tympan_lib RoadEmissionNMPB)
endif()
install_tympan_component(tympan_lib)


# solvers ######################################################################

######### Solver model #########################################################
file(GLOB TYMPAN_SOLVER_MODEL_SRCS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
    models/solver/*.cpp models/solver/*.h models/solver/*.hpp
    )
add_library(tympan_solver_model ${TYMPAN_COMPONENT_TYPE} ${TYMPAN_SOLVER_MODEL_SRCS})
set_property(TARGET tympan_solver_model PROPERTY FOLDER "Tympan/solver")
install_tympan_component(tympan_solver_model)


######### Solver plugins #######################################################
######### DefaultSolver #######################################################
file(GLOB TYMPAN_SOLVER_DEFAULT_SRCS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
    solvers/DefaultSolver/*.cpp solvers/DefaultSolver/*.h
    solvers/DefaultSolver/*/*.cpp solvers/DefaultSolver/*/*.h
)
set_property(SOURCE ${TYMPAN_SOLVER_DEFAULT_SRCS} APPEND PROPERTY COMPILE_DEFINITIONS _PLUGIN_DLL)
link_directories(${CGAL_LIBRARIES_DIR})
add_library(tympan_solver_default MODULE ${TYMPAN_SOLVER_DEFAULT_SRCS})
set_property(TARGET tympan_solver_default PROPERTY FOLDER "Tympan/solvers/DefaultSolver")
target_link_libraries(tympan_solver_default
    ${QT_QTCORE_LIBRARY} # threading QMutex, QThread, QWaitCondition
    tympan_common
    tympan_solver_model
    tympan_acousticraytracer
    tympan_convexhullfinder
)
configure_tympan_plugin(tympan_solver_default)
install_tympan_plugin(tympan_solver_default)

######### ANIME3DSolver plugin #######################################################
file(GLOB TYMPAN_SOLVER_ANIME3D_SRCS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
    solvers/ANIME3DSolver/*.cpp solvers/ANIME3DSolver/*.h
    solvers/ANIME3DSolver/*/*.cpp solvers/ANIME3DSolver/*/*.h
)
set_property(SOURCE ${TYMPAN_SOLVER_ANIME3D_SRCS} APPEND PROPERTY COMPILE_DEFINITIONS _PLUGIN_DLL)
link_directories(${CGAL_LIBRARIES_DIR})
add_library(tympan_solver_anime3d ${TYMPAN_COMPONENT_TYPE} MODULE ${TYMPAN_SOLVER_ANIME3D_SRCS})
set_property(TARGET tympan_solver_anime3d PROPERTY FOLDER "Tympan/solvers/ANIME3DSolver")
target_link_libraries(tympan_solver_anime3d
    ${QT_QTCORE_LIBRARY} # qlist, QTime, QMutex QSharedPointer QTime
    tympan_common
    tympan_solver_model
    tympan_analyticraytracer
    tympan_acousticraytracer
)
configure_tympan_plugin(tympan_solver_anime3d)
install_tympan_plugin(tympan_solver_anime3d)

######### Geometric libraries #######################################################
######### AnalyticRayTracer #######################################################
file(GLOB TYMPAN_ANALYTICRAYTRACER_SRCS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
    geometric_methods/AnalyticRayTracer/*.cpp geometric_methods/AnalyticRayTracer/*.h
    geometric_methods/AnalyticRayTracer/*/*.cpp geometric_methods/AnalyticRayTracer/*/*.h
)
add_library(tympan_analyticraytracer ${TYMPAN_COMPONENT_TYPE} ${TYMPAN_ANALYTICRAYTRACER_SRCS})
set_property(TARGET tympan_analyticraytracer PROPERTY FOLDER "Tympan/geometric_methods/AnalyticRayTracer")
target_link_libraries(tympan_analyticraytracer
    ${QT_QTCORE_LIBRARY} # threading QMutex, QThread, QWaitCondition
    tympan_common
    tympan_acousticraytracer
)
install_tympan_component(tympan_analyticraytracer)

######### ConvexHullFinder #######################################################
file(GLOB TYMPAN_CONVEXHULLFINDER_SRCS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
    geometric_methods/ConvexHullFinder/*.cpp geometric_methods/ConvexHullFinder/*.h
)
add_library(tympan_convexhullfinder ${TYMPAN_COMPONENT_TYPE} ${TYMPAN_CONVEXHULLFINDER_SRCS})
set_property(TARGET tympan_convexhullfinder PROPERTY FOLDER "Tympan/geometric_methods/ConvexHullFinder")
target_link_libraries(tympan_convexhullfinder
    ${QT_QTCORE_LIBRARY} # threading QMutex, QThread, QWaitCondition
    tympan_common
)
install_tympan_component(tympan_convexhullfinder)

# anime3d lib for tests  #####################################################################
file(GLOB TYMPAN_SOLVER_ANIME3D_SRCS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
    solvers/ANIME3DSolver/*.cpp solvers/ANIME3DSolver/*.h
    solvers/ANIME3DSolver/*/*.cpp solvers/ANIME3DSolver/*/*.h
)

link_directories(${CGAL_LIBRARIES_DIR})
add_library(tympan_solver_anime3d_lib ${TYMPAN_COMPONENT_TYPE} ${TYMPAN_SOLVER_ANIME3D_SRCS})
set_property(TARGET tympan_solver_anime3d_lib PROPERTY FOLDER "Tympan/solvers/ANIME3DSolver")
target_link_libraries(tympan_solver_anime3d_lib
    ${QT_QTCORE_LIBRARY} # qlist, QTime, QMutex QSharedPointer QTime
    tympan_common
    tympan_solver_model
    tympan_analyticraytracer
    tympan_acousticraytracer
)

install_tympan_component(tympan_solver_anime3d_lib)
# DefaultSolver lib for tests   #####################################################################
file(GLOB TYMPAN_SOLVER_DEFAULT_SRCS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
    solvers/DefaultSolver/*.cpp solvers/DefaultSolver/*.h
    solvers/DefaultSolver/*/*.cpp solvers/DefaultSolver/*/*.h
)

link_directories(${CGAL_LIBRARIES_DIR})
add_library(tympan_solver_default_lib ${TYMPAN_COMPONENT_TYPE} ${TYMPAN_SOLVER_DEFAULT_SRCS})
set_property(TARGET tympan_solver_default_lib PROPERTY FOLDER "Tympan/solvers/DefaultSolver")
target_link_libraries(tympan_solver_default_lib
    ${QT_QTCORE_LIBRARY} # threading QMutex, QThread, QWaitCondition
    tympan_common
    tympan_solver_model
    tympan_acousticraytracer
    tympan_convexhullfinder
)

install_tympan_component(tympan_solver_default_lib)

# main app #####################################################################

file(GLOB TYMPAN_APP_SRCS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
    gui/app/*.cpp gui/app/*.h
)

if(NOT TYMPAN_USE_NMPB2008)
    list(REMOVE_ITEM TYMPAN_APP_SRCS
        gui/app/TYRouteEditor.cpp
        gui/app/TYRouteEditor.h
    )
endif()

# Headers requiring MOC pre-processing...
file(GLOB TYMPAN_APP_MOC_HDRS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} gui/app/*.h)
# ... all in this directory but for the one below
list(REMOVE_ITEM TYMPAN_APP_MOC_HDRS
    gui/app/OSplashScreen.h
    gui/app/TYAction.h
    gui/app/TYActions.h
    gui/app/TYAppDefines.h
    gui/app/TYCustomPopupMenu.h
    gui/app/TYDXFBDTOPOImporter.h
    gui/app/TYDXFImporter.h
    gui/app/TYDXFBDTOPOSpecifications.h
    gui/app/TYElementListItem.h
    gui/app/TYMenuItem.h
    gui/app/TYMessageManager.h
    gui/app/TYOpenGLRenderer.h
)
if(NOT TYMPAN_USE_NMPB2008)
    list(REMOVE_ITEM TYMPAN_APP_MOC_HDRS
        gui/app/TYRouteEditor.h
    )
endif()
# Needed to import DXF files.
add_definitions (-DTY_USE_DXF -DDIME_NOT_DLL) # XXX Add DIME_NOT_DLL too?? See
                                              # the define and the comment in
                                              # 'TYDXFImporter.h'.
# Call moc on the listed headers, thus generating new sources
qt4_wrap_cpp(TYMPAN_APP_MOC_SRCS ${TYMPAN_APP_MOC_HDRS})
add_executable(tympan_app
    ${TYMPAN_APP_SRCS}
    ${TYMPAN_APP_MOC_SRCS}  # Sources generated by MOC
)
set_property(TARGET tympan_app PROPERTY FOLDER "Tympan/app")
add_dependencies(tympan_app
    Dime
)
# Set the actual executable name to 'Tympan' or Tympan_d for debug version
set_property(TARGET tympan_app PROPERTY OUTPUT_NAME Tympan)
set_property(TARGET tympan_app PROPERTY DEBUG_POSTFIX "_d")
set_property(TARGET tympan_app
    APPEND PROPERTY COMPILE_DEFINITIONS_DEBUG  "PLUGINS_PATH=\"${TYMPAN_INSTALL_PLUGINS_Debug}\"")
set_property(TARGET tympan_app
    APPEND PROPERTY COMPILE_DEFINITIONS_RELEASE  "PLUGINS_PATH=\"${TYMPAN_INSTALL_PLUGINS_Release}\"")
link_directories(${CGAL_LIBRARIES_DIR})
target_link_libraries(tympan_app
    ${QT_LIBRARIES}
    ${OPENGL_LIBRARIES}
    tympan_common
    tympan_lib
)

set(ADDITIONNAL_LIBRARIES "dime")
if (WIN32)
    LIST(APPEND ADDITIONNAL_LIBRARIES winmm.lib)
endif ()
foreach(lib ${ADDITIONNAL_LIBRARIES})
   target_link_libraries(tympan_app ${lib})
endforeach()

install(TARGETS tympan_app DESTINATION . )

if(SYS_NATIVE_WIN)
  # From http://www.cmake.org/Wiki/BundleUtilitiesExample
  #--------------------------------------------------------------------------------
  # Use BundleUtilities to get all other dependencies for the application to work.
  # It takes a bundle or executable along with possible plugins and inspects it
  # for dependencies.  If they are not system dependencies, they are copied.
  # directories to look for dependencies
  SET(APPS_Debug   "\${CMAKE_INSTALL_PREFIX}/Tympan_d.exe")
  SET(APPS_Release "\${CMAKE_INSTALL_PREFIX}/Tympan.exe")
  SET(DLL_FROM_DIRS ${TYMPAN_3RDPARTY_DLL_DIRS})
  # Now the work of copying dependencies into the bundle/package
  # The quotes are escaped and variables to use at install time have their $ escaped
  # An alternative is the do a configure_file() on a script and use install(SCRIPT  ...).
  INSTALL(CODE "
    # Takes the configuration into account
    set(TYMPAN_INSTALL_PLUGINS_Release ${TYMPAN_INSTALL_PLUGINS_Release})
    set(TYMPAN_INSTALL_PLUGINS_Debug ${TYMPAN_INSTALL_PLUGINS_Debug})
    set(APPS_Release ${APPS_Release})
    set(APPS_Debug ${APPS_Debug})
    set(_plugins_dir \${TYMPAN_PLUGINS_\${CMAKE_INSTALL_CONFIG_NAME}})
    set(_apps \${APPS_\${CMAKE_INSTALL_CONFIG_NAME}})

    file(GLOB_RECURSE PLUGINS
      \"\${CMAKE_INSTALL_PREFIX}/\${_plugins_dir}/*${CMAKE_SHARED_LIBRARY_SUFFIX}\")
    include(BundleUtilities)
    fixup_bundle(\"\${_apps}\" \"\${PLUGINS}\" \"${DLL_FROM_DIRS}\")
    " COMPONENT Runtime )
 endif(SYS_NATIVE_WIN)
