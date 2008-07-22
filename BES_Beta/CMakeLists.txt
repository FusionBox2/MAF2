PROJECT(BES_Beta)

# Set your list of sources here.
SET(PROJECT_SRCS
	Medical_AneuFuse/VME/medPipeVolumeMIP_BES.cpp
	Medical_AneuFuse/VME/medPipeVolumeMIP_BES.h
	Medical_AneuFuse/VME/medPipeVolumeVR_BES.cpp
	Medical_AneuFuse/VME/medPipeVolumeVR_BES.h
	Medical/Operations/medOpImporterRAWImages_BES.cpp
	Medical/Operations/medOpImporterRAWImages_BES.h
	Medical/Operations/mmoDICOMImporter_BES.cpp
	Medical/Operations/mmoDICOMImporter_BES.h
	openMAF_AneuFuse/vtkMAF/vtkMAFVolumeRayCastMapper.cxx
	openMAF_AneuFuse/vtkMAF/vtkMAFVolumeRayCastMapper.h
	openMAF_AneuFuse/vtkMAF/vtkMAFVolumeTextureMapper2D.cxx 
	openMAF_AneuFuse/vtkMAF/vtkMAFVolumeTextureMapper2D.h
	openMAF/Interaction/mafGizmoROI_BES.cpp
	openMAF/Interaction/mafGizmoROI_BES.h     
	openMAF/IO/mafBrickedFile.cpp
	openMAF/IO/mafBrickedFile.h
	openMAF/IO/mafBrickedFileReader.cpp
	openMAF/IO/mafBrickedFileReader.h
	openMAF/IO/mafBrickedFileWriter.cpp
	openMAF/IO/mafBrickedFileWriter.h
	openMAF/IO/mafVolumeLargeReader.cpp
	openMAF/IO/mafVolumeLargeReader.h
	openMAF/IO/mafVolumeLargeWriter.cpp
	openMAF/IO/mafVolumeLargeWriter.h  
	openMAF/Operators/mafOpImporterRAWVolume_BES.cpp
	openMAF/Operators/mafOpImporterRAWVolume_BES.h       
	openMAF/VME/mafVMEVolumeLarge.cpp
	openMAF/VME/mafVMEVolumeLarge.h
	openMAF/VME/mafVMEVolumeLargeUtils.cpp
	openMAF/VME/mafVMEVolumeLargeUtils.h
	openMAF/vtkMAF/vtkMAFDataArrayDescriptor.cxx
	openMAF/vtkMAF/vtkMAFDataArrayDescriptor.h	
	openMAF/vtkMAF/vtkMAFFileDataProvider.cxx
	openMAF/vtkMAF/vtkMAFFileDataProvider.h
	openMAF/vtkMAF/vtkMAFIdType64.h
	openMAF/vtkMAF/vtkMAFLargeDataProvider.cxx
	openMAF/vtkMAF/vtkMAFLargeDataProvider.h
	openMAF/vtkMAF/vtkMAFLargeDataSet.cxx
	openMAF/vtkMAF/vtkMAFLargeDataSet.h
	openMAF/vtkMAF/vtkMAFLargeDataSetCallback.cxx
	openMAF/vtkMAF/vtkMAFLargeDataSetCallback.h
	openMAF/vtkMAF/vtkMAFLargeImageData.cxx
	openMAF/vtkMAF/vtkMAFLargeImageData.h
	openMAF/vtkMAF/vtkMAFLargeImageReader.cxx
	openMAF/vtkMAF/vtkMAFLargeImageReader.h
	openMAF/vtkMAF/vtkMAFLargeImageSource.cxx
	openMAF/vtkMAF/vtkMAFLargeImageSource.h
	openMAF/vtkMAF/vtkMAFFile.cxx
	openMAF/vtkMAF/vtkMAFFile.h	
	openMAF/vtkMAF/vtkMAFDataArrayLayout.cpp
	openMAF/vtkMAF/vtkMAFDataArrayLayout.h
	openMAF/vtkMAF/vtkMAFMultiFileDataProvider.cpp
	openMAF/vtkMAF/vtkMAFMultiFileDataProvider.h
)

# List libraries that are needed by this project.
# SET(PROJECT_LIBS "mafVME" "mafInteraction" "vtkMAF")

# SET(PROJECT_LIBS ${PROJECT_LIBS} vtkMED)

# Create the library.
ADD_LIBRARY(${PROJECT_NAME} ${PROJECT_SRCS})
# TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${PROJECT_LIBS})

