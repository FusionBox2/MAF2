#
# This file sets up include directories and link directories for a project
# to use C3DReaderAPI.
# It should not be included directly, but rather through the
# C3DReaderAPI_USE_FILE setting obtained from C3DReaderAPIConfig.cmake.
#
#
# Both C3DReaderAPI_INCLUDE_DIRS and C3DReaderAPI_LIBRARY_DIRS
# are defined in C3DReaderAPIConfig.cmake file
#

SUBDIRS(${C3DReaderAPI_LIBRARY_DIRS})

# Add include directories needed to use C3DReaderAPI.
INCLUDE_DIRECTORIES(${C3DReaderAPI_INCLUDE_DIRS})

# Add link directories needed to use C3DReaderAPI.
LINK_DIRECTORIES(${C3DReaderAPI_LIBRARY_DIRS})
