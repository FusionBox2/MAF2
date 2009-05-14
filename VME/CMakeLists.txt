PROJECT(lhpVME)

# Set your list of sources here.
SET(PROJECT_SRCS
	mafPipeIntGraph.cpp
	mafPipeIntGraph.h
	mafVMEAFRefSys.cpp
	mafVMEAFRefSys.h
	mafVMEC3DData.cpp
	mafVMEC3DData.h
	mafVMEHelAxis.cpp
	mafVMEHelAxis.h
	
	lhpVisualPipeSurfaceScalar.cpp
	lhpVisualPipeSurfaceScalar.h
	
	medVMEMuscleWrapper.cpp	
	medVMEMuscleWrapper.h	
)

IF (MAF_USE_ITK)
SET(PROJECT_SRCS  ${PROJECT_SRCS}
	lhpVMESurfaceScalarVarying.cpp
	lhpVMESurfaceScalarVarying.h
)
ENDIF (MAF_USE_ITK)


# List libraries that are needed by this project.
# SET(PROJECT_LIBS "mafVME" "mafInteraction" "vtkMAF")

# SET(PROJECT_LIBS ${PROJECT_LIBS} vtkMED)

# Create the library.
ADD_LIBRARY(${PROJECT_NAME} ${PROJECT_SRCS})
# TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${PROJECT_LIBS})

