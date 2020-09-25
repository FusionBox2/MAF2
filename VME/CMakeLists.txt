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
  mafPipeMuscleWrapper.cpp
  mafPipeMuscleWrapper.h
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
  mafVMELineSeg.cpp
  mafVMELineSeg.h
  mafVMECenterLine.cpp
  mafVMECenterLine.h
  mafVMEGravityLine.cpp
  mafVMEGravityLine.h
  mafVMEOutputImage.cpp
  mafVMEOutputImage.h
  mafVMEOutputLandmarkCloud.cpp
  mafVMEOutputLandmarkCloud.h
  mafVMEOutputMesh.cpp
  mafVMEOutputMesh.h
  mafVMEOutputLineSeg.cpp
  mafVMEOutputLineSeg.h
  mafVMEOutputMeter.cpp
  mafVMEOutputMeter.h
  mafVMEOutputMuscleWrapping.cpp
  mafVMEOutputMuscleWrapping.h
  mafVMEOutputMuscleWrapper.cpp
  mafVMEOutputMuscleWrapper.h
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
  mafVMEQuadricSurfaceFitting.cpp
  mafVMEQuadricSurfaceFitting.h
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
SET(PROJECT_LIBS mafDataPipes)

IF (MAF_USE_VTK)
  SET(PROJECT_LIBS ${PROJECT_LIBS} vtkIO vtkRendering vtkGraphics vtkHybrid)
ENDIF (MAF_USE_VTK)

# Create the library.
ADD_LIBRARY(${PROJECT_NAME} ${PROJECT_SRCS})
TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${PROJECT_LIBS})

IF (MAF_USE_VTK)
  ADD_DEPENDENCIES(${PROJECT_NAME} BUILD_VTK_LIBRARY)
ENDIF (MAF_USE_VTK)

IF (MAF_USE_WX)
  ADD_DEPENDENCIES(${PROJECT_NAME} BUILD_WXWIN_LIBRARY)
ENDIF (MAF_USE_WX)

IF (MAF_USE_ITK)
  ADD_DEPENDENCIES(${PROJECT_NAME} BUILD_ITK_LIBRARY)
ENDIF (MAF_USE_ITK)
