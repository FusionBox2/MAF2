cmake_minimum_required(VERSION 3.27)
#
# Copyright and description to be placed here
#

PROJECT(mafVME)

DBG_MESSAGE("Configuring project ${PROJECT_NAME}")

# Set your list of sources here.
SET(PROJECT_SRCS
  mafDataVector.cpp
  mafDataVector.h
  mafMatrixInterpolator.cpp
  mafMatrixInterpolator.h
  mafMatrixVector.cpp
  mafMatrixVector.h
  mafPipeFactoryVME.cpp
  mafPipeFactoryVME.h
  mafScalarVector.cpp
  mafScalarVector.h
  mafVMEExternalData.cpp
  mafVMEExternalData.h
  mafVMEFactory.cpp
  mafVMEFactory.h
  mafVMEGenericAbstract.cpp
  mafVMEGenericAbstract.h
  mafVMEGroup.cpp
  mafVMEGroup.h
  mafVMEItem.cpp
  mafVMEItem.h
  mafVMEInfoText.cpp
  mafVMEInfoText.h
  mafMSFImporter.cpp
  mafMSFImporter.h
  mafPipePolylineSlice.cpp
  mafPipePolylineSlice.h
  mafPipeMesh.cpp
  mafPipeMesh.h
  mafPipeMeshSlice.cpp
  mafPipeMeshSlice.h
  mafPipeScalar.cpp
  mafPipeScalar.h
  mafVisualPipeVolumeRayCasting.cpp
  mafVisualPipeVolumeRayCasting.h
  mafVMEOutputScalar.cpp
  mafVMEOutputScalar.h
  mafVMEScalar.cpp
  mafVMEScalar.h












  medVMEFactory.cpp 
  medVMEFactory.h
  medVMELabeledVolume.cpp
  medVMELabeledVolume.h
  medPipeFactoryVME.cpp
  medPipeFactoryVME.h

  medPipeVolumeMIP.cpp
  medPipeVolumeMIP.h
  medPipeDensityDistance.cpp
  medPipeDensityDistance.h
  medPipeTrajectories.cpp
  medPipeTrajectories.h
  medPipeVolumeDRR.cpp
  medPipeVolumeDRR.h
  medPipeVolumeVR.cpp
  medPipeVolumeVR.h

  # mafPolylineGraph.cpp
  # mafPolylineGraph.h
  medVMEPolylineGraph.cpp
  medVMEPolylineGraph.h
  medVMEPolylineEditor.cpp
  medVMEPolylineEditor.h
  medPipePolylineGraphEditor.cpp
  medPipePolylineGraphEditor.h
  medVMEOutputPolylineEditor.cpp
  medVMEOutputPolylineEditor.h
  medVMEMaps.cpp
  medVMEMaps.h
  
  medVMEWrappedMeter.cpp
  medVMEWrappedMeter.h
  medVMEOutputWrappedMeter.cpp
  medVMEOutputWrappedMeter.h
  medPipeWrappedMeter.cpp
  medPipeWrappedMeter.h

  medVMESurfaceEditor.cpp
  medVMESurfaceEditor.h
  medPipeSurfaceEditor.cpp
  medPipeSurfaceEditor.h
  medVMEOutputSurfaceEditor.cpp
  medVMEOutputSurfaceEditor.h
  
  medVisualPipeSlicerSlice.cpp
  medVisualPipeSlicerSlice.h
  
  medVisualPipePolylineGraph.cpp
  medVisualPipePolylineGraph.h

  medVMEComputeWrapping.h
  medVMEComputeWrapping.cpp
  medPipeComputeWrapping.h
  medPipeComputeWrapping.cpp
  medVMEOutputComputeWrapping.cpp
  medVMEOutputComputeWrapping.h 
  
  medPipeVectorField.cpp
  medPipeVectorField.h
  medPipeVectorFieldMapWithArrows.cpp
  medPipeVectorFieldMapWithArrows.h
  medPipeVectorFieldGlyphs.cpp
  medPipeVectorFieldGlyphs.h
  medPipeVectorFieldSurface.cpp
  medPipeVectorFieldSurface.h
  medPipeVectorFieldSlice.cpp
  medPipeVectorFieldSlice.h
  medPipeTensorField.cpp
  medPipeTensorField.h
  medPipeTensorFieldGlyphs.cpp
  medPipeTensorFieldGlyphs.h
  medPipeTensorFieldSurface.cpp
  medPipeTensorFieldSurface.h
  medPipeTensorFieldSlice.cpp
  medPipeTensorFieldSlice.h
  medPipeCompound.cpp
  medPipeCompound.h
  medPipeCompoundVolume.cpp
  medPipeCompoundVolume.h
  medPipeCompoundVolumeFixedScalars.cpp
  medPipeCompoundVolumeFixedScalars.h
  medVMESegmentationVolume.cpp
  medVMESegmentationVolume.h
  medDataPipeCustomSegmentationVolume.cpp
  medDataPipeCustomSegmentationVolume.h
  medAttributeSegmentationVolume.cpp
  medAttributeSegmentationVolume.h
  
  
  ### TO BE COMMITTED DOWN ###
  mafPipeSlice.cpp
  mafPipeSlice.h
  ############################
  
  mafGizmoROI_BES.cpp
  mafGizmoROI_BES.h
  mafPipeMeshSlice_BES.cpp
  mafPipeMeshSlice_BES.h
  mafPipePolylineSlice_BES.cpp
  mafPipePolylineSlice_BES.h
  mafPipeSurfaceSlice_BES.cpp
  mafPipeSurfaceSlice_BES.h
  mafPipeVolumeSlice_BES.cpp
  mafPipeVolumeSlice_BES.h
  
  ############# VolumeLarge Files - TO BE COMMITTED DOWN ########
  ../BES_Beta/IO/mafBrickedFile.cpp
  ../BES_Beta/IO/mafBrickedFile.h
  ../BES_Beta/IO/mafBrickedFileReader.cpp
  ../BES_Beta/IO/mafBrickedFileReader.h
  ../BES_Beta/IO/mafBrickedFileWriter.cpp
  ../BES_Beta/IO/mafBrickedFileWriter.h
  ../BES_Beta/IO/mafVolumeLargeReader.cpp
  ../BES_Beta/IO/mafVolumeLargeReader.h
  ../BES_Beta/IO/mafVolumeLargeWriter.cpp
  ../BES_Beta/IO/mafVolumeLargeWriter.h
  mafVMEVolumeLarge.cpp
  mafVMEVolumeLarge.h
  mafVMEVolumeLargeUtils.cpp
  mafVMEVolumeLargeUtils.h  
  ###############################################################

  medPipeVolumeSliceBlend.cpp
  medPipeVolumeSliceBlend.h

  medVisualPipeCollisionDetection.cpp
  medVisualPipeCollisionDetection.h

  medPipeVolumeSliceNotInterpolated.cpp
  medPipeVolumeSliceNotInterpolated.h
  
  medPipeRayCast.cpp
  medPipeRayCast.h
  





  
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
  mafPipeScalarMatrix.cpp
  mafPipeScalarMatrix.h
  mafVMEOutputScalarMatrix.cpp
  mafVMEOutputScalarMatrix.h
  mafVMEScalarMatrix.cpp
  mafVMEScalarMatrix.h
  mafVMEItemScalarMatrix.cpp
  mafVMEItemScalarMatrix.h










  mafVMERawMotionData.cpp
  mafVMERawMotionData.h
  medPipeGraph.cpp
  medPipeGraph.h
  medVMEAnalog.cpp
  medVMEAnalog.h
  
 #add for stent
  medVMEStent.cpp
  medVMEStent.h

  #stent used tool, to find nearest point
  kdtree.c
  kdtree.h

  



	lhpVMESurfaceScalarVarying.cpp
	lhpVMESurfaceScalarVarying.h

  )
ENDIF (MAF_USE_ITK)

IF (MAF_USE_VTK)
  SET(PROJECT_SRCS
  ${PROJECT_SRCS}
  mafPipeGizmo.cpp
  mafPipeGizmo.h
  mafPipeImage3D.cpp
  mafPipeImage3D.h
  mafPipeIsosurface.cpp
  mafPipeIsosurface.h
  mafPipeLandmarkCloud.cpp
  mafPipeLandmarkCloud.h
  mafPipeMeter.cpp
  mafPipeMeter.h
  mafPipePointSet.cpp
  mafPipePointSet.h
  mafPipePolyline.cpp
  mafPipePolyline.h
  mafPipeSurface.cpp
  mafPipeSurface.h
  mafPipeSurfaceSlice.cpp
  mafPipeSurfaceSlice.h
  mafPipeSurfaceTextured.cpp
  mafPipeSurfaceTextured.h
  mafPipeVector.cpp
  mafPipeVector.h
  mafPipeVolumeProjected.cpp
  mafPipeVolumeProjected.h
  mafPipeVolumeSlice.cpp
  mafPipeVolumeSlice.h
  mafPipeMuscleWrapper.cpp
  mafPipeMuscleWrapper.h
  mafVMELineSeg.cpp
  mafVMELineSeg.h
  mafVMECenterLine.cpp
  mafVMECenterLine.h
  mafVMEGravityLine.cpp
  mafVMEGravityLine.h
  mafVMEOutputLineSeg.cpp
  mafVMEOutputLineSeg.h
  mafVMEOutputMuscleWrapping.cpp
  mafVMEOutputMuscleWrapping.h
  mafVMEOutputMuscleWrapper.cpp
  mafVMEOutputMuscleWrapper.h
  mafVMEQuadricSurfaceFitting.cpp
  mafVMEQuadricSurfaceFitting.h
  mafVMEHyperboloid2S.cpp
  mafVMEHyperboloid2S.h
  mafVMEHyperboloid.cpp
  mafVMEHyperboloid.h
  mafVMEPlane.cpp
  mafVMEPlane.h
  mafVMEOsteometricBoard.cpp
  mafVMEOsteometricBoard.h
  mafVMECylinder.cpp
  mafVMECylinder.h    
  mafVMEEllipsoid.cpp
  mafVMEEllipsoid.h
  mafLandmark.cpp
  mafLandmark.hpp
  mafDataElement.cpp
  mafDataElement.hpp
  mafQuadraticSurface.cpp
  mafQuadraticSurface.hpp
  mafMathtools.cpp
  mafMathtools.hpp
  mafVMEMuscleWrapper.cpp
  mafVMEMuscleWrapper.h
  mafVMEMuscleWrapping.cpp
  mafVMEMuscleWrapping.h
  mafPointCloud.cpp
  mafPointCloud.hpp
  mafdmLineSegment.cpp
  mafdmLineSegment.hpp 
  mafVMEGeneric.cpp
  mafVMEGeneric.h
  mafVMEGizmo.cpp
  mafVMEGizmo.h
  mafVMEImage.cpp
  mafVMEImage.h
  mafVMEItemVTK.cpp
  mafVMEItemVTK.h
  mafVMELandmark.cpp
  mafVMELandmark.h
  mafVMELandmarkCloud.cpp
  mafVMELandmarkCloud.h
  mafVMEMesh.cpp
  mafVMEMesh.h
  mafVMEMeter.cpp
  mafVMEMeter.h
  mafVMEOutputImage.cpp
  mafVMEOutputImage.h
  mafVMEOutputLandmarkCloud.cpp
  mafVMEOutputLandmarkCloud.h
  mafVMEOutputMesh.cpp
  mafVMEOutputMesh.h
  mafVMEOutputMeter.cpp
  mafVMEOutputMeter.h
  mafVMEOutputPointSet.cpp
  mafVMEOutputPointSet.h
  mafVMEOutputPolyline.cpp
  mafVMEOutputPolyline.h
  mafVMEOutputSurface.cpp
  mafVMEOutputSurface.h
  mafVMEOutputVolume.cpp
  mafVMEOutputVolume.h
  mafVMEOutputVTK.cpp
  mafVMEOutputVTK.h
  mafVMEPointSet.cpp
  mafVMEPointSet.h
  mafVMEPolyline.cpp
  mafVMEPolyline.h
  mafVMEPolylineSpline.cpp
  mafVMEPolylineSpline.h
  mafVMEProber.cpp
  mafVMEProber.h
  mafVMERefSysAbstract.cpp
  mafVMERefSysAbstract.h
  mafVMERefSys.cpp
  mafVMERefSys.h
  mafVMESlicer.cpp
  mafVMESlicer.h
  mafVMESurface.cpp
  mafVMESurface.h
  mafVMESurfaceParametric.cpp
  mafVMESurfaceParametric.h
  mafVMEVector.cpp
  mafVMEVector.h
  mafVMEVolume.cpp
  mafVMEVolume.h
  mafVMEVolumeGray.cpp
  mafVMEVolumeGray.h
  mafVMEVolumeRGB.cpp
  mafVMEVolumeRGB.h
  mmaMeter.cpp
  mmaMeter.h
  )
ENDIF (MAF_USE_VTK)

IF(WIN32)
  SET(PROJECT_SRCS
  ${PROJECT_SRCS}
  mafPipeIsosurfaceGPU.cpp
  mafPipeIsosurfaceGPU.h
  )
ENDIF(WIN32)

SUBDIRS(DataPipes)

# List libraries that are needed by this project.
#SET(PROJECT_LIBS mafCore vtkMAF)
SET(PROJECT_LIBS mafDataPipes "mafInteraction" "vtkMAF")

IF (MAF_USE_VTK)
#  SET(PROJECT_LIBS ${PROJECT_LIBS} vtkIO vtkRendering vtkGraphics vtkHybrid)
ENDIF (MAF_USE_VTK)

# Create the library.
ADD_LIBRARY(${PROJECT_NAME} ${PROJECT_SRCS})
TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${PROJECT_LIBS})
