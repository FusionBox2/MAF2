PROJECT(lhpVME)

# Set your list of sources here.
SET(PROJECT_SRCS
	lhpPipeInfo.cpp
	lhpPipeInfo.h
	lhpPipeIntGraph.cpp
	lhpPipeIntGraph.h
	lhpPipeIntGraphAbstract.cpp
	lhpPipeIntGraphAbstract.h
	lhpPipeIntGraphAnalog.cpp
	lhpPipeIntGraphAnalog.h
	lhpPipeIntGraphEuler.cpp
	lhpPipeIntGraphEuler.h
	lhpPipeIntGraphFixed.cpp
	lhpPipeIntGraphFixed.h
	lhpPipeIntGraphGlobal.cpp
	lhpPipeIntGraphGlobal.h
	lhpPipeIntGraphHAxis.cpp
	lhpPipeIntGraphHAxis.h
	lhpPipeIntGraphLocal.cpp
	lhpPipeIntGraphLocal.h
	lhpPipeIntGraphOVPGES.cpp
	lhpPipeIntGraphOVPGES.h
	lhpPipeIntGraphPolyline.cpp
	lhpPipeIntGraphPolyline.h
	lhpPipeIntGraphTime.cpp
	lhpPipeIntGraphTime.h
	lhpPipeLeverArm.cpp
	lhpPipeLeverArm.h
	lhpVMEKMInfo.cpp
	lhpVMEKMInfo.h
	lhpVMELeverArm.cpp
	lhpVMELeverArm.h
	lhpVMELMCLines.cpp
	lhpVMELMCLines.h
	mafVMEArrow.cpp
	mafVMEArrow.h
	mafVMEBSplineLine.cpp
	mafVMEBSplineLine.h
	mafVMEBSplineSurface.cpp
	mafVMEBSplineSurface.h
	mafVMEBSplineVolume.cpp
	mafVMEBSplineVolume.h
	mafVMEPGDData.cpp
	mafVMEPGDData.h
	mafVMESurfaceRegParam.cpp
	mafVMESurfaceRegParam.h
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

