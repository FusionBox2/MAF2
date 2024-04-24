cmake_minimum_required(VERSION 3.27)
#
# Copyright and description to be placed here
#

PROJECT(mafBase)

DBG_MESSAGE("Configuring project mafCore")

# Set your list of sources here.
SET(PROJECT_SRCS
mafDecl.cpp
mafDecl.h
mafDefines.cpp
mafDefines.h
mafDirectory.cpp
mafDirectory.h
mafDynamicLoader.cpp
mafDynamicLoader.h
mafEventBase.cpp
mafEventBase.h
mafEventSender.cpp
mafEventSender.h
mafFilesDirs.cpp
mafFilesDirs.h
mafIncludeWIN32.h
mafIncludeWX.h
mafIndent.cpp
mafIndent.h
mafLUTLibrary.cpp
mafLUTLibrary.h
mafMTime.cpp
mafMTime.h
mafMatrix.cpp
mafMatrix.h
mafMatrix3x3.cpp
mafMatrix3x3.h
mafOBB.cpp
mafOBB.h
mafObject.cpp
mafObject.h
mafObjectFactory.cpp
mafObjectFactory.h
mafBaseEventHandler.cpp
mafBaseEventHandler.h
mafReferenceCounted.cpp
mafReferenceCounted.h
mafSmartPointer.h
mafString.cpp
mafString.h
mafTimeStamped.cpp
mafTimeStamped.h
mafTransform.cpp
mafTransform.h
mafTransformBase.cpp
mafTransformBase.h
mafTransformFrame.cpp
mafTransformFrame.h
mafVectors.h
mafVersion.h
mmuIdFactory.cpp
mmuIdFactory.h
mafZipUtility.cpp
mafZipUtility.h

)

IF (MAF_USE_VTK)
  SET(PROJECT_SRCS
    ${PROJECT_SRCS}
    vtkMAFToLinearTransform.cxx
    vtkMAFToLinearTransform.h
  )
ENDIF (MAF_USE_VTK)

IF (MAF_USE_CRYPTO)
SET(PROJECT_SRCS  ${PROJECT_SRCS}
  mafCrypt.cpp
  mafCrypt.h
  )
ENDIF (MAF_USE_CRYPTO)

# List libraries that are needed by this project.
#SET(PROJECT_LIBS "" )

IF (MAF_USE_CRYPTO)
  SET(PROJECT_LIBS ${PROJECT_LIBS} cryptopp-static)
ENDIF (MAF_USE_CRYPTO)

IF (MAF_USE_VTK)
#  SET(PROJECT_LIBS ${PROJECT_LIBS} vtkCommon)
ENDIF (MAF_USE_VTK)

# Create the library.
ADD_LIBRARY(${PROJECT_NAME} ${PROJECT_SRCS})
TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${PROJECT_LIBS})
