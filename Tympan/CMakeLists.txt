cmake_minimum_required(VERSION 2.8)

# Adding GTest include in the non-test tree is required to enable gtest_prod.h
include_directories (${GTEST_INCLUDE_DIR})

set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
  APPEND PROPERTY COMPILE_DEFINITIONS_DEBUG _DEBUG)


# Configuring source code ######################################################

set(PYMODULES_DIR "pymodules")
set(PYSCRIPT "${PYMODULES_DIR}/solve_project.py")
configure_file(
  ${CMAKE_CURRENT_SOURCE_DIR}/core/config.h.in
  ${CMAKE_CURRENT_BINARY_DIR}/core/config.h
  @ONLY
  )
# XXX is it useful here and part of source code configuration ?
include_directories(${CMAKE_BINARY_DIR})

# models #######################################################################

set(MODULE_NAME tympan_models_common)
file(GLOB ${MODULE_NAME}_SRCS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
    *.cpp *.h
    core/*.cpp core/*.h
    models/common/*.cpp models/common/*.h
)
add_library(${MODULE_NAME} ${TYMPAN_COMPONENT_TYPE} ${${MODULE_NAME}_SRCS})
set_property(TARGET ${MODULE_NAME} PROPERTY FOLDER "Tympan/models/common")
install_tympan_component(${MODULE_NAME})


set(MODULE_NAME tympan_models_solver)
file(GLOB ${MODULE_NAME}_SRCS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
    models/solver/*.cpp models/solver/*.h models/solver/*.hpp
)
add_library(${MODULE_NAME} ${TYMPAN_COMPONENT_TYPE} ${${MODULE_NAME}_SRCS})
set_property(TARGET ${MODULE_NAME} PROPERTY FOLDER "Tympan/models/solver")
target_link_libraries(${MODULE_NAME}
    tympan_models_common
)
install_tympan_component(${MODULE_NAME})


set(MODULE_NAME tympan_models_business)
file(GLOB ${MODULE_NAME}_SRCS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
    models/business/*.cpp models/business/*.h
    models/business/*/*.cpp models/business/*/*.h
)
add_library(${MODULE_NAME} ${TYMPAN_COMPONENT_TYPE} ${${MODULE_NAME}_SRCS})
set_property(TARGET ${MODULE_NAME} PROPERTY FOLDER "Tympan/models/business")
add_dependencies(${MODULE_NAME}
    NMPB2008
    NMPB2008 # XXX?
    GTest # XXX
)
target_link_libraries(${MODULE_NAME}
    ${QT_LIBRARIES}
    ${CGAL_LIBRARIES}
    RoadEmissionNMPB # XXX?
    tympan_models_common
    tympan_models_solver # XXX
    tympan_gui tympan_gui_widgets # XXX
)
install_tympan_component(${MODULE_NAME})


# solvers ######################################################################

set(MODULE_NAME tympan_acoustic_raytracer)
file(GLOB ${MODULE_NAME}_SRCS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
    solvers/AcousticRaytracer/*.cpp solvers/AcousticRaytracer/*.h
    solvers/AcousticRaytracer/*/*.cpp solvers/AcousticRaytracer/*/*.h
)
list(REMOVE_ITEM ${MODULE_NAME}_SRCS
    "solvers/AcousticRaytracer/Acoustic/NMPBSolver.h"
    "solvers/AcousticRaytracer/Acoustic/NMPBSolver.cpp"
    "solvers/AcousticRaytracer/main.cpp")
add_library(${MODULE_NAME} ${TYMPAN_COMPONENT_TYPE} ${${MODULE_NAME}_SRCS})
set_property(TARGET ${MODULE_NAME} PROPERTY FOLDER "Tympan/solvers/AcousticRaytracer")
add_dependencies(${MODULE_NAME}
    tympan_models_common
)
install_tympan_component(${MODULE_NAME})

set(MODULE_NAME tympan_analytic_raytracer)
file(GLOB ${MODULE_NAME}_SRCS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
    solvers/AnalyticRayTracer/*.cpp solvers/AnalyticRayTracer/*.h
    solvers/AnalyticRayTracer/*/*.cpp solvers/AnalyticRayTracer/*/*.h
)
add_library(${MODULE_NAME} ${TYMPAN_COMPONENT_TYPE} ${${MODULE_NAME}_SRCS})
set_property(TARGET ${MODULE_NAME} PROPERTY FOLDER "Tympan/solvers/AnalyticRayTracer")
target_link_libraries(${MODULE_NAME}
    ${QT_LIBRARIES} # XXX
    tympan_acoustic_raytracer
)
install_tympan_component(${MODULE_NAME})

set(MODULE_NAME tympan_convexhull_finder)
file(GLOB ${MODULE_NAME}_SRCS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
    solvers/ConvexHullFinder/*.cpp solvers/ConvexHullFinder/*.h
    solvers/ConvexHullFinder/*/*.cpp solvers/ConvexHullFinder/*/*.h
)
add_library(${MODULE_NAME} ${TYMPAN_COMPONENT_TYPE} ${${MODULE_NAME}_SRCS})
set_property(TARGET ${MODULE_NAME} PROPERTY FOLDER "Tympan/solvers/ConvexHullFinder")
add_dependencies(${MODULE_NAME}
    tympan_models_common
)
install_tympan_component(${MODULE_NAME})


set(MODULE_NAME tympan_default_solver)
file(GLOB ${MODULE_NAME}_SRCS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
    solvers/DefaultSolver/*.cpp solvers/DefaultSolver/*.h
    solvers/DefaultSolver/*/*.cpp solvers/DefaultSolver/*/*.h
)
set_property(SOURCE ${${MODULE_NAME}_SRCS} APPEND PROPERTY COMPILE_DEFINITIONS _PLUGIN_DLL)
link_directories(${CGAL_LIBRARIES_DIR})
add_library(${MODULE_NAME} MODULE ${${MODULE_NAME}_SRCS})
set_property(TARGET ${MODULE_NAME} PROPERTY FOLDER "Tympan/solvers/DefaultSolver")
target_link_libraries(${MODULE_NAME}
    tympan_models_solver
    tympan_models_business # XXX
    tympan_convexhull_finder
    ${QT_LIBRARIES} # threading support
)
configure_tympan_plugin(${MODULE_NAME})
install_tympan_plugin(${MODULE_NAME})


set(MODULE_NAME tympan_ANIME3D_solver)
file(GLOB ${MODULE_NAME}_SRCS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
    solvers/ANIME3DSolver/*.cpp solvers/ANIME3DSolver/*.h
    solvers/ANIME3DSolver/*/*.cpp solvers/ANIME3DSolver/*/*.h
)
set_property(SOURCE ${${MODULE_NAME}_SRCS} APPEND PROPERTY COMPILE_DEFINITIONS _PLUGIN_DLL)
link_directories(${CGAL_LIBRARIES_DIR})
add_library(${MODULE_NAME} ${TYMPAN_COMPONENT_TYPE} MODULE ${${MODULE_NAME}_SRCS})
set_property(TARGET ${MODULE_NAME} PROPERTY FOLDER "Tympan/solvers/ANIME3DSolver")
target_link_libraries(${MODULE_NAME}
    tympan_acoustic_raytracer tympan_analytic_raytracer
    tympan_models_solver
    tympan_models_business # XXX
)
configure_tympan_plugin(${MODULE_NAME})
install_tympan_plugin(${MODULE_NAME})


# gui ##########################################################################

set(MODULE_NAME tympan_gui)
file(GLOB ${MODULE_NAME}_SRCS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
    gui/*.cpp gui/*.h
    gui/tools/*.cpp gui/tools/*.h
)
add_library(${MODULE_NAME} ${TYMPAN_COMPONENT_TYPE} ${${MODULE_NAME}_SRCS})
set_property(TARGET ${MODULE_NAME} PROPERTY FOLDER "Tympan/gui")
target_link_libraries(${MODULE_NAME}
    #tympan_models_business # XXX
    ${QT_LIBRARIES}
	${OPENGL_LIBRARIES}
)
install_tympan_component(${MODULE_NAME})


set(MODULE_NAME tympan_gui_widgets)
file(GLOB ${MODULE_NAME}_SRCS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
    gui/widgets/*.cpp gui/widgets/*.h
)
# cf. https://qt-project.org/quarterly/view/using_cmake_to_build_qt_projects
# Headers requiring MOC pre-processing...
file(GLOB ${MODULE_NAME}_MOC_HDRS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} gui/widgets/*.h)
# ... all in this directory but for the two below
list(REMOVE_ITEM ${MODULE_NAME}_MOC_HDRS
    gui/widgets/TYIncludesQT.h
    gui/widgets/TYPHIHM.h
)
# Call moc on the listed headers, thus generating new sources
qt4_wrap_cpp(${MODULE_NAME}_MOC_SRCS ${${MODULE_NAME}_MOC_HDRS})
# Qt Designer UIC files
file(GLOB ${MODULE_NAME}_FORMS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} gui/widgets/*.ui)
# Call moc on the listed UI file, thus generating new sources
qt4_wrap_ui(${MODULE_NAME}_FORMS_HDRS ${${MODULE_NAME}_FORMS})
add_library(${MODULE_NAME} ${TYMPAN_COMPONENT_TYPE}
  ${${MODULE_NAME}_SRCS}
  ${${MODULE_NAME}_MOC_SRCS} # Sources generated by MOC
  ${${MODULE_NAME}_FORMS_HDRS} # Sources generated by UIC
)
set_property(TARGET ${MODULE_NAME} PROPERTY FOLDER "Tympan/gui/widgets")
add_dependencies(${MODULE_NAME}
    tympan_gui
)
target_link_libraries(${MODULE_NAME}
    ${QT_LIBRARIES}
)
install_tympan_component(${MODULE_NAME})


set(MODULE_NAME tympan_gui_gl)
file(GLOB ${MODULE_NAME}_SRCS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
    gui/gl/*.cpp gui/gl/*.h
)
add_library(${MODULE_NAME} ${TYMPAN_COMPONENT_TYPE} ${${MODULE_NAME}_SRCS})
set_property(TARGET ${MODULE_NAME} PROPERTY FOLDER "Tympan/gui/gl")
add_dependencies(${MODULE_NAME}
    tympan_gui
)
target_link_libraries(${MODULE_NAME}
    ${QT_LIBRARIES}
)
install_tympan_component(${MODULE_NAME})


set(MODULE_NAME tympan_gui_app)
file(GLOB ${MODULE_NAME}_SRCS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
    gui/app/*.cpp gui/app/*.h
)
# Headers requiring MOC pre-processing...
file(GLOB ${MODULE_NAME}_MOC_HDRS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} gui/app/*.h)
# ... all in this directory but for the one below
list(REMOVE_ITEM ${MODULE_NAME}_MOC_HDRS
    gui/app/OSplashScreen.h
    gui/app/TYAction.h
    gui/app/TYActions.h
    gui/app/TYAppDefines.h
    gui/app/TYCustomPopupMenu.h
    gui/app/TYDXFBDTOPOImporter.h
    gui/app/TYDXFImporter.h
    gui/app/TYDXFBDTOPOSpecifications.h
    gui/app/TYElementCheckListItem.h
    gui/app/TYElementListItem.h
    gui/app/TYMenuItem.h
    gui/app/TYMessageManager.h
    gui/app/TYOpenGLRenderer.h
    gui/app/TYPHTympanApp.h
)
# Needed to import DXF files.
add_definitions (-DTY_USE_DXF -DDIME_NOT_DLL) # XXX Add DIME_NOT_DLL too?? See
                                              # the define and the comment in
                                              # 'TYDXFImporter.h'.
# Call moc on the listed headers, thus generating new sources
qt4_wrap_cpp(${MODULE_NAME}_MOC_SRCS ${${MODULE_NAME}_MOC_HDRS})
add_executable(${MODULE_NAME} ${${MODULE_NAME}_MOC_SRCS})  # Sources generated by MOC
set_property(TARGET ${MODULE_NAME} PROPERTY FOLDER "Tympan/gui/app")
add_dependencies(${MODULE_NAME}
    Dime
)
# Set the actual executble name to 'Tympan' or Tympan_d for debug version
set_property(TARGET ${MODULE_NAME} PROPERTY OUTPUT_NAME Tympan)
set_property(TARGET ${MODULE_NAME} PROPERTY DEBUG_POSTFIX "_d")
set(ADDITIONNAL_LIBRARIES "dime")
if (WIN32)
    LIST(APPEND ADDITIONNAL_LIBRARIES winmm.lib)
endif ()
set_property(TARGET ${MODULE_NAME}
    APPEND PROPERTY COMPILE_DEFINITIONS_DEBUG  "PLUGINS_PATH=\"${TYMPAN_INSTALL_PLUGINS_Debug}\"")
set_property(TARGET ${MODULE_NAME}
    APPEND PROPERTY COMPILE_DEFINITIONS_RELEASE  "PLUGINS_PATH=\"${TYMPAN_INSTALL_PLUGINS_Release}\"")
link_directories(${CGAL_LIBRARIES_DIR})
target_link_libraries(${MODULE_NAME}
    ${QT_LIBRARIES}
    ${OPENGL_LIBRARIES}
    tympan_models_business
    tympan_gui_gl
    tympan_gui_widgets)

foreach(lib ${ADDITIONNAL_LIBRARIES})
   target_link_libraries(${MODULE_NAME} ${lib})
endforeach()

install(TARGETS ${MODULE_NAME} DESTINATION . )

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
