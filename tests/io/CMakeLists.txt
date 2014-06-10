# CMakeLists for test cases with GoogleTest framework.
# TODO (dag) The macro to compile a test could be factored-out above and used below

# Needed to import DXF files.
add_definitions (-DTY_USE_DXF -DDIME_NOT_DLL)

# Find all CPP test files.
file (GLOB TESTS_SRC ${CMAKE_CURRENT_SOURCE_DIR}/test_*.cpp )

# Needed for DXF files import.
set (DXF_DEPEND_FILES ${PROJECT_SOURCE_DIR}/Tympan/Appli/TympanApp/TYDXFBDTOPOImporter.cpp
  ${PROJECT_SOURCE_DIR}/Tympan/Appli/TympanApp/TYDXFImporter.cpp)

set (TYMPAN_MODULES tympan_dm_metier tympan_dm_graphics tympan_dm_ihm
     tympan_dm_core tympan_tools_metier tympan_tools_graphics tympan_tools)

# Main loop for all CPP test files.
foreach (testfile ${TESTS_SRC})
  get_filename_component (basename ${testfile} NAME_WE)
  add_executable (${basename} ${testfile} ${DXF_DEPEND_FILES} ../test_main.cpp)
  configure_gtest_target(TARGET ${basename} 
             FOLDER ${CURRENT_FOLDER}
             RUNTIME_PATH "${TYMPAN_3RDPARTY_DLL_NATIVE_DIRS}" 
             LIBS test_utils ${TYMPAN_MODULES} dime)
endforeach (testfile)
