path_relative_to_python(destination)

# Handles pure python sources
file(
    COPY .
    DESTINATION "${PROJECT_BINARY_DIR}/${destination}"
    FILES_MATCHING PATTERN "*.py" )
install(DIRECTORY .
    DESTINATION "${destination}"
    FILES_MATCHING PATTERN "*.py")
