project(vtkLHP)
cmake_minimum_required(VERSION 2.8)

CONFIGURE_FILE(${PROJECT_SOURCE_DIR}/vtkLHPConfigure.h.in
               ${PROJECT_BINARY_DIR}/${PROJECT_NAME}Configure.h
               @ONLY IMMEDIATE)
# Source files.
SET(vtkLHP_SRCS
	vtkMAFMuscleDecomposition.cpp	
	vtkMAFMuscleDecomposition.h	
	vtkMAFMuscleFibers.cpp	
	vtkMAFMuscleFibers.h
)

IF (MAF_USE_VTK)
  SET(PROJECT_LIBS ${PROJECT_LIBS} ${VTK_LIBRARIES})
ENDIF (MAF_USE_VTK)

# Create the library.
ADD_LIBRARY(${PROJECT_NAME} ${vtkLHP_SRCS})
TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${PROJECT_LIBS})
