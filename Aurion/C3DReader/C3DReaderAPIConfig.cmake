#-----------------------------------------------------------------------------
#
# C3DReaderAPIConfig.cmake -
# C3DReaderAPI CMake configuration file for external projects.
#
# This file is configured by C3DReaderAPI and used by the UseC3DReaderAPI.cmake module
# to load C3DReaderAPI's settings for an external project.

SET(NEW_INSTALLATION_DIRECTORY
	"C:/cvsMAF/builderAppNEWSTYLE/Aurion/C3DReader"
)

# The C3DReaderAPI source tree.
SET(C3DReaderAPI_SOURCE_DIR "C:/cvsMAF/builderAppNEWSTYLE/Aurion/C3DReader/")

# The C3DReaderAPI include file directories.
SET(C3DReaderAPI_INCLUDE_DIRS "C:/cvsMAF/builderAppNEWSTYLE/Aurion/C3DReader/include")

# The C3DReaderAPI library directories.
# Includes C3DReaderAPI libs directory and OTHER libs directories
SET(C3DReaderAPI_LIBRARY_DIRS "C:/cvsMAF/builderAppNEWSTYLE/Aurion/C3DReader/lib;")

# This includes all libraries (.lib) on which C3DReaderAPI depends
SET(C3DReaderAPI_OTHER_LIBRARIES "")

# This includes all libraries (.lib) that C3DReaderAPI generates.
SET(C3DReaderAPI_LIBRARIES "C3D_Reader")

# This includes all debug libraries (.lib) that C3DReaderAPI generates.
SET(C3DReaderAPI_DEBUG_LIBRARIES "")

# The location of the UseC3DReaderAPI.cmake file.
SET(C3DReaderAPI_USE_FILE "C:/cvsMAF/builderAppNEWSTYLE/Aurion/C3DReader/UseC3DReaderAPI.cmake")
