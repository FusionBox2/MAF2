project(vtkMED)
cmake_minimum_required(VERSION 2.8)

CONFIGURE_FILE(${PROJECT_SOURCE_DIR}/vtkMEDConfigure.h.in
               ${PROJECT_BINARY_DIR}/${PROJECT_NAME}Configure.h
               @ONLY IMMEDIATE)
# Source files.
SET(vtkMED_SRCS
  vtkDicomUnPacker.cxx
  vtkDicomUnPacker.h
  vtkImageUnPacker.cxx
  vtkImageUnPacker.h
  vtkPackedImage.cxx
  vtkPackedImage.h
  vtkXRayVolumeMapper.cxx
  vtkXRayVolumeMapper.h
  vtkWeightedLandmarkTransform.cxx
  vtkWeightedLandmarkTransform.h
  vtkTriangleQualityRatio.cxx
  vtkTriangleQualityRatio.h
  vtkMAFCellLocator.cxx
  vtkMAFCellLocator.h
  vtkMEDAddScalarsFilter.cxx
  vtkMEDAddScalarsFilter.h
  vtkMEDExtrudeToCircle.cxx
  vtkMEDExtrudeToCircle.h
  vtkMEDFillingHole.cxx
  vtkMEDFillingHole.h
  vtkMEDFixTopology.cxx
  vtkMEDFixTopology.h
  vtkMEDImageUnsharpFilter.cxx
  vtkMEDImageUnsharpFilter.h
  vtkMEDMatrixVectorMath.h
  vtkMEDMatrixVectorMath.cxx
  vtkMEDPoissonSurfaceReconstruction.cxx
  vtkMEDPoissonSurfaceReconstruction.h
  vtkMEDPolyDataMirror.cxx
  vtkMEDPolyDataMirror.h
  vtkMEDPolyDataNavigator.h
  vtkMEDPolyDataNavigator.cxx
  vtkMEDRegionGrowingLocalGlobalThreshold.cxx
  vtkMEDRegionGrowingLocalGlobalThreshold.h
  vtkMEDHalfTubeRemoval.cxx
  vtkMEDHalfTubeRemoval.h
  vtkMEDInteractorStyleEnableRotation.cxx
  vtkMEDInteractorStyleEnableRotation.h
  vtkMEDSubdividePolylines.cxx
  vtkMEDSubdividePolylines.h
  vtkMEDLandmarkCloudPolydataFilter.cxx
  vtkMEDLandmarkCloudPolydataFilter.h
  vtkMaskPolyDataFilter.cxx
  vtkMaskPolyDataFilter.h
  vtkHoleConnectivity.cxx
  vtkHoleConnectivity.h
  vtkPatchLine.h
  vtkPatchLine.cpp
  
  ############# VolumeLarge Files - TO BE COMMITTED DOWN ########
  ../BES_Beta/vtkMAF/vtkMAFDataArrayDescriptor.cxx
  ../BES_Beta/vtkMAF/vtkMAFDataArrayDescriptor.h
  ../BES_Beta/vtkMAF/vtkMAFFileDataProvider.cxx
  ../BES_Beta/vtkMAF/vtkMAFFileDataProvider.h
  ../BES_Beta/vtkMAF/vtkMAFLargeImageData.cxx
  ../BES_Beta/vtkMAF/vtkMAFLargeImageData.h
  ../BES_Beta/vtkMAF/vtkMAFLargeImageReader.cxx
  ../BES_Beta/vtkMAF/vtkMAFLargeImageReader.h
  ../BES_Beta/vtkMAF/vtkMAFLargeImageSource.cxx
  ../BES_Beta/vtkMAF/vtkMAFLargeImageSource.h
  ../BES_Beta/vtkMAF/vtkMAFFile.cxx
  ../BES_Beta/vtkMAF/vtkMAFFile.h	
  ../BES_Beta/vtkMAF/vtkMAFDataArrayLayout.cpp
  ../BES_Beta/vtkMAF/vtkMAFDataArrayLayout.h
  ../BES_Beta/vtkMAF/vtkMAFMultiFileDataProvider.cpp
  ../BES_Beta/vtkMAF/vtkMAFMultiFileDataProvider.h
  ###############################################################
  
  ########## Should be committed down ###########
  vtkMAFVolumeRayCastMapper.cxx
  vtkMAFVolumeRayCastMapper.h
  vtkMAFVolumeTextureMapper2D.cxx
  vtkMAFVolumeTextureMapper2D.h
  vtkMAFVolumeSlicer_BES.cxx
  vtkMAFVolumeSlicer_BES.h
  vtkMAFMeshCutter_BES.cxx 
  vtkMAFMeshCutter_BES.h  	
  ###############################################
  
  ########## Probably could be committed down ###
  vtkMEDPolyDataDeformation.cpp
  vtkMEDPolyDataDeformation.h
  vtkMEDPolyDataDeformation_M1.cpp
  vtkMEDPolyDataDeformation_M1.h
  vtkMEDPolyDataDeformation_M2.cpp
  vtkMEDPolyDataDeformation_M2.h  
  ###############################################
    

  
  vtkMEDErythrocyteSource.cpp
  vtkMEDErythrocyteSource.h
  
  vtkMEDImageFillHolesRemoveIslands.cxx
  vtkMEDImageFillHolesRemoveIslands.h
  
  vtkMEDVolumeSlicerNotInterpolated.cxx
  vtkMEDVolumeSlicerNotInterpolated.h

  vtkMEDBinaryImageFloodFill.cxx
  vtkMEDBinaryImageFloodFill.h
  
  ######vtkbioeng################################
  vtkMEDCollisionDetectionFilter.cxx
  vtkMEDCollisionDetectionFilter.h
  
  vtkMEDRayCastCleaner.cpp
  vtkMEDRayCastCleaner.h
  
  vtkMEDVolumeToClosedSmoothSurface.cxx
  vtkMEDVolumeToClosedSmoothSurface.h
  
  
)

IF (MAF_USE_ITK)
SET(vtkMED_SRCS  ${vtkMED_SRCS}
  mafClassicICPRegistration.cxx
  mafClassicICPRegistration.h
  mafICPUtility.h
)
ENDIF (MAF_USE_ITK)

SET(vtkMED_SRCS
  ${vtkMED_SRCS}
  vtkMEDPastValuesList.cpp
  vtkMEDPastValuesList.h
  vtkMEDStentModelSource.cpp
  vtkMEDStentModelSource.h
  vtkMEDDeformableSimplexMeshFilter.h
  vtkMEDDeformableSimplexMeshFilter_Impl.h

  ############# VolumeLarge Files - TO BE COMMITTED DOWN ########
  vtkMAFIdType64.h
  ../BES_Beta/vtkMAF/vtkMAFLargeDataProvider.cxx
  ../BES_Beta/vtkMAF/vtkMAFLargeDataProvider.h
  ../BES_Beta/vtkMAF/vtkMAFLargeDataSet.cxx
  ../BES_Beta/vtkMAF/vtkMAFLargeDataSet.h
  ###############################################################
)

IF (MAF_USE_VTK)
  SET(PROJECT_LIBS ${PROJECT_LIBS} ${VTK_LIBRARIES})
ENDIF (MAF_USE_VTK)

# Create the library.
ADD_LIBRARY(${PROJECT_NAME} ${vtkMED_SRCS})
TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${PROJECT_LIBS})
