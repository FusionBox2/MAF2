project(vtkMAF)
cmake_minimum_required(VERSION 2.8)

# Find and load VTK settings.
if(NOT VTK_BINARY_DIR)
  find_package(VTK 6.0 REQUIRED NO_MODULE)
  include(${VTK_USE_FILE})
endif()

include(${VTK_CMAKE_DIR}/vtkExternalModuleMacros.cmake)

vtk_module(vtkMAF
  DEPENDS
  vtkCommonCore
)

include_directories(SYSTEM
  ${VTK_INCLUDE_DIRS}
)

# Source files.
SET(vtkMAF_SRCS

  vtkMAFOrientationMarkerWidget.h
  vtkMAFOrientationMarkerWidget.cpp
  
  vtkMAFAnnotatedCubeActor.h
  vtkMAFAnnotatedCubeActor.cpp
  
  vtkMAFGlobalAxesHeadActor.h
  vtkMAFGlobalAxesHeadActor.cpp
  
  vtkMAFCellsFilter.cxx
  vtkMAFCellsFilter.h  
  vtkMAFRemoveCellsFilter.cxx
  vtkMAFRemoveCellsFilter.h
  vtkMAFClipSurfaceBoundingBox.cxx
  vtkMAFClipSurfaceBoundingBox.h
  vtkMAFContourVolumeMapper.cxx
  vtkMAFContourVolumeMapper.h
  vtkMAFContourVolumeMapperAdv.cxx
  vtkMAFContourVolumeMapperAdv.h
  vtkMAFDistanceFilter.cxx
  vtkMAFDistanceFilter.h
  vtkMAFDOFMatrix.cxx
  vtkMAFDOFMatrix.h
  vtkMAFExtendedGlyph3D.cxx
  vtkMAFExtendedGlyph3D.h
  vtkMAFFixedCutter.cxx
  vtkMAFFixedCutter.h
  vtkMAFGlobalAxisCoordinate.cxx
  vtkMAFGlobalAxisCoordinate.h
  vtkMAFGridActor.cxx
  vtkMAFGridActor.h
   vtkMAFHistogram.cxx
   vtkMAFHistogram.h
   vtkMAFImplicitPolyData.cxx
   vtkMAFImplicitPolyData.h
  vtkMAFLocalAxisCoordinate.cxx
  vtkMAFLocalAxisCoordinate.h
  vtkMAFAdaptiveVolumeMapper.cxx
  vtkMAFAdaptiveVolumeMapper.h
  vtkMAFImageMapToWidgetColors.cxx
  vtkMAFImageMapToWidgetColors.h
  vtkMAFMeshCutter.cxx
  vtkMAFMeshCutter.h
  vtkMAFPolyDataSingleSourceShortestPath.cxx
  vtkMAFPolyDataSingleSourceShortestPath.h
  vtkMAFProjectRG.cxx
  vtkMAFProjectRG.h
  vtkMAFProjectSP.cxx
  vtkMAFProjectSP.h
  vtkMAFRayCast3DPicker.cxx
  vtkMAFRayCast3DPicker.h
 vtkMAFRectilinearGridToRectilinearGridFilter.cxx
 vtkMAFRectilinearGridToRectilinearGridFilter.h
 vtkMAFRGSliceAccumulate.cxx
 vtkMAFRGSliceAccumulate.h
 vtkMAFRulerActor2D.cxx
 vtkMAFRulerActor2D.h
 vtkMAFSimpleRulerActor2D.cxx
 vtkMAFSimpleRulerActor2D.h
 vtkMAFTextActorMeter.cxx
 vtkMAFTextActorMeter.h
 vtkMAFTransferFunction2D.cxx
 vtkMAFTransferFunction2D.h
 vtkMAFVolumeResample.h
 vtkMAFVolumeResample.cxx
 vtkMAFVolumeSlicer.cxx
  vtkMAFVolumeSlicer.h
  vtkMAFPolyDataToSinglePolyLine.cxx
  vtkMAFPolyDataToSinglePolyLine.h
  vtkMAFTextOrientator.cxx
  vtkMAFTextOrientator.h
  vtkMAFProfilingActor.cxx
  vtkMAFProfilingActor.h
  vtkMAFXYPlotActor.cxx
  vtkMAFXYPlotActor.h
  vtkMAFPolyDataNormals.cxx
  vtkMAFPolyDataNormals.h
)

IF(WIN32)
SET(vtkMAF_SRCS
	${vtkMAF_SRCS}
  vtkMAFContourVolumeMapperGPU.cxx
  vtkMAFContourVolumeMapperGPU.h
  )
ENDIF(WIN32)

# Build and link library.
vtk_module_library(vtkMAF ${vtkMAF_SRCS})
