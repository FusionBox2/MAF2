cmake_minimum_required(VERSION 3.27)
#
# Copyright and description to be placed here
#

PROJECT(mafIO)

DBG_MESSAGE("Configuring project mafIO")

# Set your list of sources here.
SET(PROJECT_SRCS
  mafTo.h
  mafStorage.cpp
  mafStorage.h
  mafStorageElement.cpp
  mafStorageElement.h
)

# IO classes for VTK related objects
IF (MAF_USE_VTK)
  #SET(PROJECT_SRCS
  #${PROJECT_SRCS}
  #)
ENDIF (MAF_USE_VTK)

# List libraries that are needed by this project.

IF(BUILD_SHARED_LIBS)
ELSE (BUILD_SHARED_LIBS)
  SET(PROJECT_LIBS mafBase)
ENDIF (BUILD_SHARED_LIBS)


# IO classes for XML related objects
IF (MAF_USE_XercesC)
  SET(PROJECT_SRCS
    ${PROJECT_SRCS}
  )
  SET(PROJECT_LIBS ${PROJECT_LIBS} XercesC::XercesC)
ENDIF (MAF_USE_XercesC)

IF (MAF_USE_VTK)
#  SET(PROJECT_LIBS ${PROJECT_LIBS} vtkIO)
ENDIF (MAF_USE_VTK)

# Create the library.
ADD_LIBRARY(${PROJECT_NAME} ${PROJECT_SRCS})
TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${PROJECT_LIBS})

IF(BUILD_SHARED_LIBS)
  ADD_DEPENDENCIES(${PROJECT_NAME} mafBase) #substitute with  ${PROJECT_LIBS}   when external libraries are dynamic
ENDIF (BUILD_SHARED_LIBS)

