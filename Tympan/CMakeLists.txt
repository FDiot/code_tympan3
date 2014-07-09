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

# models #######################################################################

set(MODULE_NAME tympan_models_common)
file(GLOB ${MODULE_NAME}_SRCS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
    *.cpp *.h
    core/*.cpp core/*.h
    models/common/*.cpp models/common/*.h
)
add_library(${MODULE_NAME} ${TYMPAN_COMPONENT_TYPE} ${${MODULE_NAME}_SRCS} ${${MODULE_NAME}_PCH_SRCS})
set_property(TARGET ${MODULE_NAME} PROPERTY FOLDER "Tympan/models/common")
install_tympan_component(${MODULE_NAME})


set(MODULE_NAME tympan_models_solver)
file(GLOB ${MODULE_NAME}_SRCS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}/models/solver
    *.cpp *.h *.hpp
)
set_precompiled_header(${MODULE_NAME} CXX
  ${CMAKE_CURRENT_SOURCE_DIR}/std_boost_utils.hpp ${MODULE_NAME}_PCH_SRCS)
add_library(${MODULE_NAME} ${TYMPAN_COMPONENT_TYPE} ${${MODULE_NAME}_SRCS} ${${MODULE_NAME}_PCH_SRCS})
set_property(TARGET ${MODULE_NAME} PROPERTY FOLDER "Tympan/models/solver")
use_precompiled_header(${MODULE_NAME} ${MODULE_NAME}_SRCS)
target_link_libraries(${MODULE_NAME}
    tympan_models_common
)
install_tympan_component(${MODULE_NAME})


set(MODULE_NAME tympan_models_business)
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
file(GLOB_RECURSE ${MODULE_NAME}_SRCS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}/solvers/AcousticRaytracer
    *.cpp *.h *.hpp
)
list(REMOVE_ITEM ${MODULE_NAME}_SRCS "Acoustic/NMPBSolver.h" "Acoustic/NMPBSolver.cpp" "main.cpp")
add_library(${MODULE_NAME} ${TYMPAN_COMPONENT_TYPE} ${${MODULE_NAME}_SRCS} ${${MODULE_NAME}_PCH_SRCS})
set_property(TARGET ${MODULE_NAME} PROPERTY FOLDER "Tympan/solvers")
add_dependencies(${MODULE_NAME}
    tympan_models_common
)
target_link_libraries(${MODULE_NAME}
    ${QT_LIBRARIES} # XXX
)
install_tympan_component(${MODULE_NAME})

set(MODULE_NAME tympan_analytic_raytracer)
file(GLOB_RECURSE ${MODULE_NAME}_SRCS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}/solvers/AnalyticRayTracer
    *.cpp *.h *.hpp
)
add_library(${MODULE_NAME} ${TYMPAN_COMPONENT_TYPE} ${${MODULE_NAME}_SRCS} ${${MODULE_NAME}_PCH_SRCS})
set_property(TARGET ${MODULE_NAME} PROPERTY FOLDER "Tympan/solvers")
target_link_libraries(${MODULE_NAME}
    ${QT_LIBRARIES} # XXX
    tympan_acoustic_raytracer
)
install_tympan_component(${MODULE_NAME})

set(MODULE_NAME tympan_convexhull_finder)
file(GLOB_RECURSE ${MODULE_NAME}_SRCS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}/solvers/ConvexHullFinder
    *.cpp *.h *.hpp
)
add_library(${MODULE_NAME} ${TYMPAN_COMPONENT_TYPE} ${${MODULE_NAME}_SRCS} ${${MODULE_NAME}_PCH_SRCS})
set_property(TARGET ${MODULE_NAME} PROPERTY FOLDER "Tympan/solvers")
add_dependencies(${MODULE_NAME}
    tympan_models_common
)
install_tympan_component(${MODULE_NAME})


set(MODULE_NAME tympan_default_solver)
file(GLOB_RECURSE ${MODULE_NAME}_SRCS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}/solvers/DefaultSolver
    *.cpp *.h *.hpp
)
set_property(SOURCE ${${MODULE_NAME}_SRCS} APPEND PROPERTY COMPILE_DEFINITIONS _PLUGIN_DLL)
link_directories(${CGAL_LIBRARIES_DIR})
add_library(${MODULE_NAME} MODULE ${${MODULE_NAME}_SRCS} ${${MODULE_NAME}_PCH_SRCS} )
set_property(TARGET ${MODULE_NAME} PROPERTY FOLDER "Tympan/solvers")
target_link_libraries(${MODULE_NAME}
    tympan_models_solver
    tympan_models_business # XXX
    tympan_convexhull_finder
    ${QT_LIBRARIES} # XXX
    ${OPENGL_LIBRARIES} # XXX
)
configure_tympan_plugin(${MODULE_NAME})
install_tympan_plugin(${MODULE_NAME})


set(MODULE_NAME tympan_ANIME3D_solver)
file(GLOB_RECURSE ${MODULE_NAME}_SRCS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}/solvers/ANIME3D
    *.cpp *.h *.hpp
)
set_property(SOURCE ${${MODULE_NAME}_SRCS} APPEND PROPERTY COMPILE_DEFINITIONS _PLUGIN_DLL)
link_directories(${CGAL_LIBRARIES_DIR})
add_library(${MODULE_NAME} ${TYMPAN_COMPONENT_TYPE} MODULE ${${MODULE_NAME}_SRCS} ${${MODULE_NAME}_PCH_SRCS})
set_property(TARGET ${MODULE_NAME} PROPERTY FOLDER "Tympan/solvers")
target_link_libraries(${MODULE_NAME}
    tympan_acoustic_raytracer tympan_analytic_raytracer
    tympan_models_solver
    tympan_models_business # XXX
    ${QT_LIBRARIES} # XXX
    ${OPENGL_LIBRARIES} # XXX
)
configure_tympan_plugin(${MODULE_NAME})
install_tympan_plugin(${MODULE_NAME})


# gui ##########################################################################

set(MODULE_NAME tympan_gui)
file(GLOB ${MODULE_NAME}_SRCS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}/gui
    *.cpp *.h
    tools/*.cpp tools/*.h
)
add_library(${MODULE_NAME} ${TYMPAN_COMPONENT_TYPE} ${${MODULE_NAME}_SRCS} ${${MODULE_NAME}_PCH_SRCS})
set_property(TARGET ${MODULE_NAME} PROPERTY FOLDER "Tympan/gui")
target_link_libraries(${MODULE_NAME}
    ${QT_LIBRARIES}
	${OPENGL_LIBRARIES}
)
install_tympan_component(${MODULE_NAME})


set(MODULE_NAME tympan_gui_widgets)
file(GLOB ${MODULE_NAME}_SOURCES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}/gui/widgets
    *.cpp *.h *.hpp)
# cf. https://qt-project.org/quarterly/view/using_cmake_to_build_qt_projects
# Headers requiring MOC pre-processing...
file(GLOB ${MODULE_NAME}_MOC_HEADERS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} gui/widgets/*.h)
# ... all in this directory but for the two below
list(REMOVE_ITEM ${MODULE_NAME}_MOC_HEADERS TYIncludesQT.h TYPHIHM.h)
# Call moc on the listed headers, thus generating new sources
qt4_wrap_cpp(${MODULE_NAME}_MOC_SOURCES ${${MODULE_NAME}_MOC_HEADERS})
# Qt Designer UIC files
file(GLOB ${MODULE_NAME}_FORMS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} gui/widgets/*.ui)
# Call moc on the listed UI file, thus generating new sources
qt4_wrap_ui(${MODULE_NAME}_FORMS_HEADERS ${${MODULE_NAME}_FORMS})
add_library(${MODULE_NAME} ${TYMPAN_COMPONENT_TYPE}
  ${${MODULE_NAME}_SOURCES}
  ${${MODULE_NAME}_PCH_SOURCES}
  ${${MODULE_NAME}_MOC_SOURCES} # Sources generated by MOC
  ${${MODULE_NAME}_FORMS_HEADERS} # Sources generated by UIC
  )
set_property(TARGET ${MODULE_NAME} PROPERTY FOLDER "Tympan/GUI")
add_dependencies(${MODULE_NAME} tympan_gui)
target_link_libraries(${MODULE_NAME} ${QT_LIBRARIES})
install_tympan_component(${MODULE_NAME})


set(MODULE_NAME tympan_gui_gl)
file(GLOB ${MODULE_NAME}_SRCS/gui/gl
    *.cpp *.h *.hpp)
set_precompiled_header(${MODULE_NAME} CXX
  ${CMAKE_CURRENT_SOURCE_DIR}/TYPHGraphic.h ${MODULE_NAME}_PCH_SRCS)
add_library(${MODULE_NAME} ${TYMPAN_COMPONENT_TYPE} ${${MODULE_NAME}_SRCS} ${${MODULE_NAME}_PCH_SRCS} )
set_property(TARGET ${MODULE_NAME} PROPERTY FOLDER "Tympan/GUI")
use_precompiled_header(${MODULE_NAME} ${MODULE_NAME}_SRCS)
add_dependencies(${MODULE_NAME} tympan_gui)
target_link_libraries(${MODULE_NAME} ${QT_LIBRARIES})
install_tympan_component(${MODULE_NAME})


set(MODULE_NAME tympan_qt_app)
file(GLOB ${MODULE_NAME}_SRCS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} *.cpp *.h *.hpp
)
# Headers requiring MOC pre-processing...
file(GLOB ${MODULE_NAME}_MOC_HDRS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} *.h )
# ... all in this directory but for the one below
list(REMOVE_ITEM ${MODULE_NAME}_MOC_HDRS
    OSplashScreen.h
    TYAction.h
    TYActions.h
    TYAppDefines.h
    TYCustomPopupMenu.h
    TYDOMErrorHandler.h
    TYDXFBDTOPOImporter.h
    TYDXFImporter.h
    TYDXFBDTOPOSpecifications.h
    TYElementCheckListItem.h
    TYElementListItem.h
    TYMenuItem.h
    TYMessageManager.h
    TYOpenGLRenderer.h
    TYPHTympanApp.h
    TYVersion.h
)
# Needed to import DXF files.
add_definitions (-DTY_USE_DXF -DDIME_NOT_DLL) # XXX Add DIME_NOT_DLL too?? See
                                              # the define and the comment in
                                              # 'TYDXFImporter.h'.
# Call moc on the listed headers, thus generating new sources
qt4_wrap_cpp(${MODULE_NAME}_MOC_SRCS ${${MODULE_NAME}_MOC_HDRS})
set_precompiled_header(${MODULE_NAME} CXX
  ${CMAKE_CURRENT_SOURCE_DIR}/TYPHTympanApp.h ${MODULE_NAME}_PCH_SRCS)
add_executable(${MODULE_NAME}  ${${MODULE_NAME}_SRCS} ${${MODULE_NAME}_PCH_SRCS}
                               ${${MODULE_NAME}_MOC_SRCS} )    #Sources generated by MOC
set_property(TARGET ${MODULE_NAME} PROPERTY FOLDER "Tympan/GUI_Application")
use_precompiled_header(${MODULE_NAME} ${MODULE_NAME}_SRCS)
add_dependencies(${MODULE_NAME}
    Dime
)
# Set the actual executble name to 'Tympan' or Tympand for debug version
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
    tympan_gui_gl tympan_gui_widgets
    tympan_gui
    tympan_models_common
)

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
