file (GLOB TESTS_SRC ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp )

add_library(test_utils STATIC ${TESTS_SRC} )
set_property(TARGET test_utils PROPERTY FOLDER ${CURRENT_FOLDER})

add_dependencies(test_utils GTest ${TYMPAN_MODULES} )
# 'CMAKE_THREAD_LIBS_INIT' can be empty on Windows.
target_link_libraries(test_utils gtest ${CMAKE_THREAD_LIBS_INIT} ${TYMPAN_MODULES})

# Unit tests dedicated to this directory only.
add_subdirectory(tests)
