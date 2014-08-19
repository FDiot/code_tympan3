# Configure and install two minor dependencies for reading and writting PLY files
# PLY files are simple file for storing polygonal meshes.
#
# The PLY file format is documented here :
#     http://www.cs.virginia.edu/~gfx/Courses/2001/Advanced.spring.01/plylib/Ply.txt
# The following C library is used from the C++ side:
#     http://w3.impa.br/~diego/software/rply/
# The following Python module is used from the Python side:
#     https://github.com/dranjan/python-plyfile

set(RPLY_DIR "${CMAKE_SOURCE_DIR}/3rdparty/rply-1.1.3"
  CACHE PATH "The root directory for the RPLY source tree")
set(RPLY_SOURCES "${RPLY_DIR}/rply.c")
set(RPLY_INCLUDE_DIRECTORY "${RPLY_DIR}")
add_library(rply SHARED ${RPLY_SOURCES})
set(RPLY_LIB rply)

set(PYTHONPLYFILE_DIR "${CMAKE_SOURCE_DIR}/3rdparty/python-plyfile"
  CACHE PATH "The root directory for the plyfile python source tree")
set(PYTHONPLYFILE_SOURCES "${PYTHONPLYFILE_DIR}/plyfile.py")
file(COPY ${PYTHONPLYFILE_SOURCES} 
  DESTINATION "${PROJECT_BINARY_DIR}/${TYMPAN_CythonModules_Debug}")
file(COPY ${PYTHONPLYFILE_SOURCES} 
  DESTINATION "${PROJECT_BINARY_DIR}/${TYMPAN_CythonModules_Release}")
install(FILES ${PYTHONPLYFILE_SOURCES} 
  DESTINATION "${TYMPAN_CythonModules_Release}/"
  CONFIGURATIONS Release)
install(FILES ${PYTHONPLYFILE_SOURCES} 
  DESTINATION "${TYMPAN_CythonModules_Debug}/"
  CONFIGURATIONS Debug)

