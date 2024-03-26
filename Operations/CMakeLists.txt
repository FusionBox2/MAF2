cmake_minimum_required(VERSION 3.27)
#
# Copyright and description to be placed here
#

PROJECT(mafOperations)

DBG_MESSAGE("Configuring project mafOperations")

# Set your list of sources here.
SET(PROJECT_SRCS
)

IF (MAF_USE_VTK)
  SET(PROJECT_SRCS
    ${PROJECT_SRCS}
    mafOpDecomposeTimeVarVME.cpp
    mafOpDecomposeTimeVarVME.h
    mafOpImporterMSF.cpp
    mafOpImporterMSF.h
    mafOpImporterExternalFile.cpp
    mafOpImporterExternalFile.h
    mafOpLabelExtractor.cpp
    mafOpLabelExtractor.h
    mafOpOpenExternalFile.cpp
    mafOpOpenExternalFile.h
    mafOp2DMeasure.cpp
    mafOp2DMeasure.h
    mafOpAddLandmark.cpp
    mafOpAddLandmark.h
    mafOpExporterBmp.cpp
    mafOpExporterBmp.h
    mafOpClipSurface.cpp
    mafOpClipSurface.h
    mafOpCreateGroup.cpp
    mafOpCreateGroup.h
    mafOpCreateMeter.cpp
    mafOpCreateMeter.h
    mafOpCreateSurfaceParametric.cpp
    mafOpCreateSurfaceParametric.h
    mafOpCreateGravityLine.cpp
    mafOpCreateGravityLine.h
    mafOpCreateMeter2.cpp
    mafOpCreateMeter2.h
    mafOpCreateCylinder.cpp
    mafOpCreateCylinder.h
    mafOpCreateHyperboloid2S.cpp
    mafOpCreateHyperboloid2S.h
    mafOpCreateHyperboloid.cpp
    mafOpCreateHyperboloid.h
    mafOpCreateEllipsoid.cpp
    mafOpCreateEllipsoid.h
    mafOpCreateQuadricSurfaceFitting.cpp
    mafOpCreateQuadricSurfaceFitting.h
    mafOpCreateMuscleWrapping2.cpp
    mafOpCreateMuscleWrapping2.h
    mafOpCreateMuscleWrapper.cpp
    mafOpCreateMuscleWrapper.h
    mafOpCreateCenterline.cpp
    mafOpCreateCenterline.h
    mafOpCreatePlane.cpp
    mafOpCreatePlane.h
    mafOpCreateOsteometricBoard.cpp
    mafOpCreateOsteometricBoard.h
    mafOpExporterVRML.cpp
    mafOpExporterVRML.h
    mafOpExporterGeomTex.cpp
    mafOpExporterGeomTex.h
    mafOpCrop3DSurface.cpp
    mafOpCrop3DSurface.h
    mafOpCreateProber.cpp
    mafOpCreateProber.h
    mafOpCreateRefSys.cpp
    mafOpCreateRefSys.h
    mafOpCreateSlicer.cpp
    mafOpCreateSlicer.h
    mafOpCreateSpline.cpp
    mafOpCreateSpline.h
    mafOpCreateVolume.cpp
    mafOpCreateVolume.h
	mafOpVolumeUnion.cpp
    mafOpVolumeUnion.h
    mafOpRemoveCells.cpp
    mafOpRemoveCells.h
    mafOpEditMetadata.cpp
    mafOpEditMetadata.h
    mafOpExplodeCollapse.cpp
    mafOpExplodeCollapse.h
    mafOpExtractIsosurface.cpp
    mafOpExtractIsosurface.h
    mafOpFilterSurface.cpp
    mafOpFilterSurface.h
    mafOpDecimateSurface.cpp
    mafOpDecimateSurface.h
    mafOpConnectivitySurface.cpp
    mafOpConnectivitySurface.h
    mafOpFilterVolume.cpp
    mafOpFilterVolume.h
    mafOpImporterImage.cpp
    mafOpImporterImage.h
    mafOpMAFTransform.cpp
    mafOpMAFTransform.h
    mafOpExporterMSF.cpp
    mafOpExporterMSF.h
    mafOpImporterMSF1x.cpp
    mafOpImporterMSF1x.h
    mafOpExporterRaw.cpp
    mafOpExporterRaw.h
    mafOpImporterRAWVolume.cpp
    mafOpImporterRAWVolume.h
    mafOpReparentTo.cpp
    mafOpReparentTo.h
    mafOpExporterSTL.cpp
    mafOpExporterSTL.h
    mafOpImporterSTL.cpp
    mafOpImporterSTL.h
    mafOpTransformInterface.cpp
    mafOpTransformInterface.h
    mafOpValidateTree.cpp
    mafOpValidateTree.h
    mafOpVolumeResample.cpp
    mafOpVolumeResample.h
    mafOpVOIDensity.cpp
    mafOpVOIDensity.h
    mafOpVOIDensityEditor.cpp
    mafOpVOIDensityEditor.h
    mafOpImporterVRML.cpp
    mafOpImporterVRML.h
    mafOpExporterVTK.cpp
    mafOpExporterVTK.h
    mafOpImporterVTK.cpp
    mafOpImporterVTK.h
    mafOpCrop.cpp
    mafOpCrop.h
    mafOpBooleanSurface.cpp
    mafOpBooleanSurface.h
    mafOpEditNormals.cpp
    mafOpEditNormals.h
    mafOpApplyTrajectory.cpp
    mafOpApplyTrajectory.h
	mafOpGarbageCollectMSFDir.cpp
	mafOpGarbageCollectMSFDir.h

















  medGeometryEditorPolylineGraph.cpp
  medGeometryEditorPolylineGraph.h
  medOpCreateLabeledVolume.cpp
  medOpCreateLabeledVolume.h
  medOpImporterLandmark.cpp
  medOpImporterLandmark.h
  medOpImporterLandmarkTXT.cpp
  medOpImporterLandmarkTXT.h
  medOpImporterLandmarkWS.cpp
  medOpImporterLandmarkWS.h

  medOpMatrixVectorMath.cpp
  medOpMatrixVectorMath.h

  medOpMML.cpp
  medOpMML.h
  medOpMMLContourWidget.cpp
  medOpMMLContourWidget.h
  medOpMMLModelView.cpp
  medOpMMLModelView.h
  medOpMMLParameterView.cpp
  medOpMMLParameterView.h

  medOpMML3.cpp
  medOpMML3.h
  medOpMML3ContourWidget.cpp
  medOpMML3ContourWidget.h
  medOpMML3GlobalRegistration.cpp
  medOpMML3GlobalRegistration.h
  medOpMML3ModelView.cpp
  medOpMML3ModelView.h
  medOpMML3ModelView2DPipe.cpp
  medOpMML3ModelView2DPipe.h
  medOpMML3ModelView3DPipe.cpp
  medOpMML3ModelView3DPipe.h
  medOpMML3NonUniformSlicePipe.cpp
  medOpMML3NonUniformSlicePipe.h
  medOpMML3ParameterView.cpp
  medOpMML3ParameterView.h



  medOpRegisterClusters.cpp
  medOpRegisterClusters.h
  medOpRegisterClusters2.cpp
  medOpRegisterClusters2.h
  medOpVolumeMeasure.cpp
  medOpVolumeMeasure.h
  medOpImporterRAWImages.cpp
  medOpImporterRAWImages.h
  medOpMeshQuality.cpp
  medOpMeshQuality.h
  medOpCropDeformableROI.cpp
  medOpCropDeformableROI.h
  medOpFlipNormals.cpp
  medOpFlipNormals.h
  medOpExtrusionHoles.cpp
  medOpExtrusionHoles.h
  medOpVolumeResample.cpp
  medOpVolumeResample.h
  medOpSplitSurface.cpp
  medOpSplitSurface.h
  medOpInteractiveClipSurface.cpp
  medOpInteractiveClipSurface.h
  medOpCreateEditSkeleton.cpp
  medOpCreateEditSkeleton.h
  medOpCreateSurface.cpp
  medOpCreateSurface.h
  medOpMove.cpp
  medOpMove.h
  medOpScaleDataset.cpp
  medOpScaleDataset.h

  medOpCreateWrappedMeter.cpp
  medOpSegmentation.cpp
  medOpSegmentation.h
  medOpExtractGeometry.cpp
  medOpExtractGeometry.h
  
  medOpCreateWrappedMeter.cpp
  medOpCreateWrappedMeter.h
  
  medOpComputeWrapping.h
  medOpComputeWrapping.cpp  
  
  medOpComputeInertialTensor.h
  medOpComputeInertialTensor.cpp  
  
  medOpExporterWrappedMeter.cpp
  medOpExporterWrappedMeter.h

  medOpLabelizeSurface.cpp
  medOpLabelizeSurface.h
  medOp2DMeasure.cpp
  medOp2DMeasure.h
  
  medOpFreezeVME.cpp
  medOpFreezeVME.h
  
  medOpIterativeRegistration.cpp
  medOpIterativeRegistration.h
  
  medOpSmoothSurface.cpp
  medOpSmoothSurface.h
  
  medOpCleanSurface.cpp
  medOpCleanSurface.h
  
  medOpTriangulateSurface.cpp
  medOpTriangulateSurface.h

  medOpSurfaceMirror.cpp
  medOpSurfaceMirror.h
  
  medOpImporterVTK.cpp
  medOpImporterVTK.h
  
  medOpImporterVTKXML.cpp
  medOpImporterVTKXML.h
  
  medOpExporterVTKXML.cpp
  medOpExporterVTKXML.h

  medOpSubdivide.cpp
  medOpSubdivide.h
    
  medOpFillHoles.cpp
  medOpFillHoles.h
  medOpMeshDeformation.cpp
  medOpMeshDeformation.h
  
  medOpMakeVMETimevarying.cpp
  medOpMakeVMETimevarying.h
  medOpSegmentationRegionGrowingConnectedThreshold.cpp
  medOpSegmentationRegionGrowingConnectedThreshold.h
  #### VolumeLarge - TO BE COMMITTED DOWN    ####
  mafOpImporterBBF.cpp
  mafOpImporterBBF.h
  mafOpImporterRAWVolume_BES.cpp
  mafOpImporterRAWVolume_BES.h  
  ###############################################
  medOpSegmentationRegionGrowingLocalAndGlobalThreshold.cpp
  medOpSegmentationRegionGrowingLocalAndGlobalThreshold.h
  
  medOpEqualizeHistogram.cpp
  medOpEqualizeHistogram.h
  
  medOpExporterAnalogWS.cpp
  medOpExporterAnalogWS.h
  medOpExporterGRFWS.cpp
  medOpExporterGRFWS.h
  medOpExporterLandmarkWS.cpp
  medOpExporterLandmarkWS.h
  
  medOpMergeDicomSeries.cpp
  medOpMergeDicomSeries.h
 
  


	#to be moved in core; need to solve some linking problems
	
	#
	# VMEUploaderDownloader (Production)
	#

	lhpFactoryTagHandler.cpp
	lhpFactoryTagHandler.h
	
	lhpTagHandlerContainer.cpp
	lhpTagHandlerContainer.h
	
	lhpTagHandlerDICOMContainer.cpp
	lhpTagHandlerDICOMContainer.h
	
	lhpTagHandler.cpp
	lhpTagHandler.h
	
	lhpOpUploadVME.cpp
	lhpOpUploadVME.h
	
	lhpOpUploadMultiVME.cpp
	lhpOpUploadMultiVME.h
	
	lhpOpDownloadVME.cpp
	lhpOpDownloadVME.h
	
	lhpOpEditTag.cpp
	lhpOpEditTag.h
	
	#
	# VMEUploaderDownloader Refactor Target
	#
	
	lhpOpUploadVMERefactor.cpp
	lhpOpUploadVMERefactor.h
	
	lhpOpUploadMultiVMERefactor.cpp
	lhpOpUploadMultiVMERefactor.h
	
	lhpOpDownloadVMERefactor.cpp
	lhpOpDownloadVMERefactor.h
	
	lhpOpEditTagRefactor.cpp
	lhpOpEditTagRefactor.h
	
	
	#######################################
	
	#
	# Google KeyCZar Integration Test
	#

	lhpOpKeyczarIntegrationTest.cpp
	lhpOpKeyczarIntegrationTest.h

	#######################################
	
	#
	lhpOpBonemat.cpp
	lhpOpBonemat.h
	lhpOpImporterAnsysInputFile.cpp
	lhpOpImporterAnsysInputFile.h
	lhpOpExporterAnsysInputFile.cpp
	lhpOpExporterAnsysInputFile.h

    lhpOpImporterAnsysCDBFile.cpp
	lhpOpImporterAnsysCDBFile.h
	
	# multiscale files
	lhpOpMultiscaleExplore.cpp
	lhpOpMultiscaleExplore.h
	lhpMultiscaleActor.cpp
	lhpMultiscaleActor.h
	lhpMultiscaleActorCoordsUtility.cpp
	lhpMultiscaleActorCoordsUtility.h
	lhpMultiscaleCallbacks.cpp
	lhpMultiscaleCallbacks.h
	lhpMultiscaleCameraParams.cpp
	lhpMultiscaleCameraParams.h
	lhpMultiscaleCameraUtility.cpp
	lhpMultiscaleCameraUtility.h
	lhpMultiscaleUtility.cpp
	lhpMultiscaleUtility.h
	lhpMultiscaleVectorMath.cpp
	lhpMultiscaleVectorMath.h
	lhpMultiscaleVisualPipes.cpp
	lhpMultiscaleVisualPipes.h

	# texture orientation files
	lhpCooccurrenceMatrixFilter.cpp
	lhpCooccurrenceMatrixFilter.h
	lhpHistogram1D.cpp
	lhpHistogram1D.h
	lhpHistogramEqualizationFilter.cpp
	lhpHistogramEqualizationFilter.h
	lhpIsotropicSampleTable.cpp
	lhpIsotropicSampleTable.h
	lhpMatrixVectorMath.cpp
	lhpMatrixVectorMath.h
	lhpOpTextureOrientation.cpp
	lhpOpTextureOrientation.h
	lhpTextureOrientationCalculator.cpp
	lhpTextureOrientationCalculator.h
	lhpTextureOrientationCallbacks.cpp
	lhpTextureOrientationCallbacks.h
	lhpTextureOrientationFilter.cpp
	lhpTextureOrientationFilter.h
	lhpTextureOrientationVectorGlyphPipe.cpp
	lhpTextureOrientationVectorGlyphPipe.h
	lhpTextureOrientationSlicePipe.cpp
	lhpTextureOrientationSlicePipe.h
	lhpTextureOrientationUseful.cpp
	lhpTextureOrientationUseful.h
	
	lhpProceduralDataset.cxx
	lhpProceduralDataset.h
	lhpProceduralElements.cxx
	lhpProceduralElements.h
	lhpTypes.h
	mafDictionary.cpp
	mafDictionary.h
	mafInpReader.cpp
	mafInpReader.h
	mafINPWriter.cpp
	mafINPWriter.h
	mafMtrLMCReader.cpp
	mafMtrLMCReader.h
	lhpOpAFSys.cpp
	lhpOpAFSys.h
	lhpOpAverageLM.cpp
	lhpOpAverageLM.h
	lhpOpBuildHierarchy.cpp
	lhpOpBuildHierarchy.h
	lhpOpHelAxis.cpp
	lhpOpHelAxis.h
	lhpOpINPExporter.cpp
	lhpOpINPExporter.h
	mafOpExporterOBJ.cpp
	mafOpExporterOBJ.h
	lhpOpINPImporter.cpp
	lhpOpINPImporter.h
	lhpOpMTRExporter.cpp
	lhpOpMTRExporter.h
	lhpOpMTRImporter.cpp
	lhpOpMTRImporter.h
	lhpOpStickPalpation.cpp
	lhpOpStickPalpation.h
	lhpOpTimeReduce.cpp
	lhpOpTimeReduce.h
	lhpOpLnSurf.cpp
	lhpOpLnSurf.h
	
	lhpOpImporterC3D.cpp
	lhpOpImporterC3D.h

	lhpOpImporterC3DBTK.cpp
	lhpOpImporterC3DBTK.h
	lhpOpExporterC3DBTK.cpp
	lhpOpExporterC3DBTK.h

	lhpOpExporterCSVGraph.cpp
	lhpOpExporterCSVGraph.h
	lhpOpImporterC3DFused.cpp
	lhpOpImporterC3DFused.h
	lhpOpImporterPressionCenter.cpp
	lhpOpImporterPressionCenter.h
	lhpOpImporterOBJ.cpp
	lhpOpImporterOBJ.h

	lhpOpImporterPLY.cpp
	lhpOpImporterPLY.h

	lhpOpICPRegFollow.cpp
	lhpOpICPRegFollow.h
	lhpOpImporterRSScan.cpp
	lhpOpImporterRSScan.h
	lhpOpCreateMetersScripted.cpp
	lhpOpCreateMetersScripted.h
	lhpOpJoinSurf.cpp
	lhpOpJoinSurf.h
	lhpOpLMMirror.cpp
	lhpOpLMMirror.h
	lhpOpLMProj.cpp
	lhpOpLMProj.h
	lhpOpMergeClouds.cpp
	lhpOpMergeClouds.h        
	lhpOpSolidify.cpp
	lhpOpSolidify.h
	lhpVMEMeshRSScanImporter.cpp
	lhpVMEMeshRSScanImporter.h
	mafMtrReader.cpp
	mafMtrReader.h
	lhpOpCreateObject.h	
	lhpOpCutSurface.cpp
	lhpOpCutSurface.h
	lhpOpFingerStick.cpp
	lhpOpFingerStick.h
	lhpOpMTRULBImporter.cpp
	lhpOpMTRULBImporter.h
	lhpOpRegistration.cpp
	lhpOpRegistration.h
	lhpOpRegression.cpp
	lhpOpRegression.h	
	lhpOpRegSurfWithCloud.cpp
	lhpOpRegSurfWithCloud.h
	lhpOpRepresentInAF.cpp
	lhpOpRepresentInAF.h
	lhpOpSoftReg.cpp
	lhpOpSoftReg.h
	lhpOpRegisterLMScripted.cpp
	lhpOpRegisterLMScripted.h
	lhpOpFuseLMScripted.cpp
	lhpOpFuseLMScripted.h
	lhpOpFindCentroid.cpp
	lhpOpFindCentroid.h
	lhpOpComputeTensor.cpp
	lhpOpComputeTensor.h
	lhpCOmputeTensor.cpp
	lhpComputeTensor.h
	lhpOpKinectUtil.cpp
	lhpOpKinectUtil.h
	lhpOpKinectAFs.cpp
	lhpOpKinectAFs.h
	lhpOpKinectModel.cpp
	lhpOpKinectModel.h
	lhpOpTimeShift.cpp
	lhpOpTimeShift.h        
	medOpCreateMuscleWrapper.cpp	
	medOpCreateMuscleWrapper.h
  )
ENDIF (MAF_USE_VTK)

IF (MAF_USE_ITK)
  SET(PROJECT_SRCS
    ${PROJECT_SRCS}
    mafASCIIImporterUtility.cpp
    mafASCIIImporterUtility.h
    mafOpImporterASCII.cpp
    mafOpImporterASCII.h
    mafVMEDataSetAttributesImporter.cpp
    mafVMEDataSetAttributesImporter.h
    mafVMEMeshAnsysTextImporter.cpp
    mafVMEMeshAnsysTextImporter.h
    mafVMEMeshAnsysTextExporter.cpp
    mafVMEMeshAnsysTextExporter.h
    mafParabolicMeshToLinearMeshFilter.cxx
    mafParabolicMeshToLinearMeshFilter.h
    mafOpImporterMesh.cpp
    mafOpImporterMesh.h
    mafOpExporterMesh.cpp
    mafOpExporterMesh.h
    mafOpScalarToSurface.cpp
    mafOpScalarToSurface.h
    mafOpImporterVMEDataSetAttributes.cpp
    mafOpImporterVMEDataSetAttributes.h









  # itkRawMotionImporterUtility.cxx
  # itkRawMotionImporterUtility.h
  medOpImporterMotionData.h
  medOpExporterLandmark.cpp
  medOpExporterLandmark.h
  medOpClassicICPRegistration.cpp
  medOpClassicICPRegistration.h
  medOpImporterC3D.cpp
  medOpImporterC3D.h
  medOpImporterGRFWS.cpp
  medOpImporterGRFWS.h
  medOpImporterAnalogWS.cpp
  medOpImporterAnalogWS.h
  medOpSmoothSurfaceCells.cpp
  medOpSmoothSurfaceCells.h
  medOpExporterMeters.cpp
  medOpExporterMeters.h
  medOpInteractionDebugger.cpp
  medOpInteractionDebugger.h



	lhpOpCreateSurfaceScalar.cpp
	lhpOpCreateSurfaceScalar.h

  )
ENDIF (MAF_USE_ITK)

IF (MAF_USE_Offis)
SET(PROJECT_SRCS ${PROJECT_SRCS}
    medOpImporterDicomOffis.h
    medOpImporterDicomOffis.cpp
  
    medDicomCardiacMRIHelper.cpp
    medDicomCardiacMRIHelper.h
   )
ENDIF (MAF_USE_Offis)

  
# List libraries that are needed by this project.
SET(PROJECT_LIBS "mafVME" "mafInteraction")

SET(PROJECT_LIBS ${PROJECT_LIBS} vtkMAF mafCommon mafGui)

IF (MAF_USE_Offis)
  INCLUDE_DIRECTORIES(${DCMTK_INCLUDE_DIRS})
  SET(PROJECT_LIBS ${PROJECT_LIBS} ${DCMTK_LIBRARIES})
ENDIF (MAF_USE_Offis)

IF (MAF_USE_VTK)
  SET(PROJECT_LIBS ${PROJECT_LIBS} vtkMAF)
ENDIF (MAF_USE_VTK)

# Create the library.
ADD_LIBRARY(${PROJECT_NAME} ${PROJECT_SRCS})
TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${PROJECT_LIBS})
