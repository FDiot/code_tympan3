FILE(TO_NATIVE_PATH ${CMAKE_INSTALL_PREFIX} TYMPAN_EXE_FULL_PATH)


message(STATUS "  ## CMAKE_INSTALL_PREFIX: " ${CMAKE_INSTALL_PREFIX})
message(STATUS "  ## TYMPAN_EXE_FULL_PATH: " ${TYMPAN_EXE_FULL_PATH})


configure_file(${PROJECT_SOURCE_DIR}/cmake/runTympanDebug.bat.in
	  ${PROJECT_BINARY_DIR}/runTympanDebug.bat @ONLY)
install(PROGRAMS ${PROJECT_BINARY_DIR}/runTympanDebug.bat
        DESTINATION .
	CONFIGURATIONS Debug)

configure_file(${PROJECT_SOURCE_DIR}/cmake/runTympanRelease.bat.in
	  ${PROJECT_BINARY_DIR}/runTympanRelease.bat @ONLY)
install(PROGRAMS ${PROJECT_BINARY_DIR}/runTympanRelease.bat
        DESTINATION .
	CONFIGURATIONS Release)