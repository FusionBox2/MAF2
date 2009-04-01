PROJECT(lhpOperations)

# Set your list of sources here.
SET(PROJECT_SRCS
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
	mmoAFSys.cpp
	mmoAFSys.h
	mmoAverageLM.cpp
	mmoAverageLM.h
	mmoBuildHierarchy.cpp
	mmoBuildHierarchy.h
	mmoHelAxis.cpp
	mmoHelAxis.h
	mmoINPExporter.cpp
	mmoINPExporter.h
	mmoINPImporter.cpp
	mmoINPImporter.h
	mmoMTRExporter.cpp
	mmoMTRExporter.h
	mmoMTRImporter.cpp
	mmoMTRImporter.h
	mmoStickPalpation.cpp
	mmoStickPalpation.h
	mmoTimeReduce.cpp
	mmoTimeReduce.h
	mmoLnSurf.cpp
	mmoLnSurf.h
	
	lhpOpImporterC3D.cpp
	lhpOpImporterC3D.h

	lhpOpComputeTensor.cpp
	lhpOpComputeTensor.h
	lhpCOmputeTensor.cpp
	lhpComputeTensor.h
        
)

IF (MAF_USE_ITK)
SET(PROJECT_SRCS  ${PROJECT_SRCS}
	lhpOpCreateSurfaceScalar.cpp
	lhpOpCreateSurfaceScalar.h
)
ENDIF (MAF_USE_ITK)

# List libraries that are needed by this project.
# SET(PROJECT_LIBS "mafVME" "mafInteraction")

# SET(PROJECT_LIBS ${PROJECT_LIBS} vtkMED medCommon)

# Create the library.
ADD_LIBRARY(${PROJECT_NAME} ${PROJECT_SRCS})
TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${PROJECT_LIBS})
