#
# Program:   MULTIMOD APPLICATION FRAMEWORK (MAF)
# Module:    $RCSfile: CMakeLists.txt,v $
# Language:  CMake 1.2
# Date:      $Date: 2007-08-22 14:01:40 $
# Version:   $Revision: 1.1 $
#
# Description:
# Project file of the Multimod Fundation Layer.

MESSAGE(STATUS "LHPExternalLibraries: Configuring LHPExternalLibraries")
PROJECT (LHPExternalLibraries)

# place all binaries inside the same output directory
SET (LIBRARY_OUTPUT_PATH ${LHPExternalLibraries_BINARY_DIR}/bin/ CACHE INTERNAL 
   "Single output directory for building all libraries.")

SET (EXECUTABLE_OUTPUT_PATH ${LHPExternalLibraries_BINARY_DIR}/bin/ CACHE INTERNAL 
     "Single output directory for building all executables.")

MARK_AS_ADVANCED (
  LIBRARY_OUTPUT_PATH 
  EXECUTABLE_OUTPUT_PATH
)

INCLUDE (${LHPExternalLibraries_SOURCE_DIR}/Configure.cmake)

IF (CONFIGURE_ERROR)
  MESSAGE (FATAL_ERROR "LHPExternalLibraries: Error configuring LHPExternalLibraries library, cannot build!")
ENDIF (CONFIGURE_ERROR)
