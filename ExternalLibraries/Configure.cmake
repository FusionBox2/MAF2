# this defines LHPExternalLibraries_SUBPROJECT
INCLUDE (${LHPExternalLibraries_SOURCE_DIR}/modules/LHPExternalLibrariesConfigMacro.cmake)
SET (DEBUG FALSE)

# options for configuring LHPExternalLibraries libraries
OPTION(LHP_USE_YOURLIBRARY "Find and Link YOURLIBRARY library." ON)

#
# configure CMake modules for LHPExternalLibraries
#
LHPExternalLibraries_SUBPROJECT (LHPExternalLibraries_MODULES modules)

#
# Sample code to include an external library
#
# IF (LHP_USE_YOURLIBRARY)
#   LHPExternalLibraries_SUBPROJECT(YOURLIBRARY YOURLIBRARY)
# ENDIF(LHP_USE_YOURLIBRARY)

# Create module files in build directory
IF (NOT CONFIGURE_ERROR)
  CONFIGURE_FILE(${LHPExternalLibraries_SOURCE_DIR}/UseLHPExternalLibraries.cmake.in ${LHPExternalLibraries_BINARY_DIR}/UseLHPExternalLibraries.cmake @ONLY IMMEDIATE)
ENDIF (NOT CONFIGURE_ERROR)

MARK_AS_ADVANCED (
  LHPExternalLibraries_BUILD_TARGETS
)

#Write the list of variables and their values that are defined by CMake when running this CMakeList file
IF (DEBUG_MESSAGES)
  #FILE(WRITE ${LHPExternalLibraries_BINARY_DIR}/AllVariables.txt "")
  GET_CMAKE_PROPERTY(VARS VARIABLES)
  FOREACH(var ${VARS})
  	FILE(APPEND ${LHPExternalLibraries_BINARY_DIR}/AllVariables.txt 
  							"${var} \"${${var}}\"\n")
  ENDFOREACH(var ${VARS})
ENDIF (DEBUG_MESSAGES)
