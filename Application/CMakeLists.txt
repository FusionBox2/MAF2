cmake_minimum_required(VERSION 3.27)
PROJECT (lhpFusionBox)

SET(PROJECT_SRCS 
	lhpFusionBoxApp.cpp
	lhpFusionBoxApp.h
)

IF (NOT EXECUTABLE_OUTPUT_PATH)
  SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin/ CACHE INTERNAL 
  "Single output directory for building all executables.")
ENDIF (NOT EXECUTABLE_OUTPUT_PATH)

if(WIN32)
  SET(NO_MANIFEST 1)
  CONFIGURE_FILE(${PROJECT_SOURCE_DIR}/LHPFusionBoxApplication.rc.in ${PROJECT_BINARY_DIR}/LHPFusionBoxApplication.rc @ONLY)
  set(PROJECT_SRCS ${PROJECT_SRCS}
      "${PROJECT_BINARY_DIR}/LHPFusionBoxApplication.rc"
  )
endif(WIN32)

add_executable(${PROJECT_NAME} WIN32 MACOSX_BUNDLE ${PROJECT_SRCS})

if(WIN32)
  if(NO_MANIFEST EQUAL 1)
    set_target_properties(${PROJECT_NAME} PROPERTIES LINK_FLAGS "/MANIFEST")
  else(NO_MANIFEST EQUAL 1)
    set_target_properties(${PROJECT_NAME} PROPERTIES LINK_FLAGS "/MANIFEST:NO")
  endif(NO_MANIFEST EQUAL 1)
endif(WIN32)

SET(PROJECT_LIBS
  mafCommon mafCore mafGui mafOperations mafViews mafVME vtkMAF
)
TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${PROJECT_LIBS})
TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${VTK_LIBRARIES} )

FOREACH(config ${CMAKE_CONFIGURATION_TYPES})
  SET(OUT_SHADER_DIR ${EXECUTABLE_OUTPUT_PATH}/${config}/Shaders/iso-surface)
  FILE(MAKE_DIRECTORY ${OUT_SHADER_DIR})
		
  FILE(GLOB cg_shading_file_list "${MAF_SOURCE_DIR}/vtkMAF/iso-surface/*.cg")
  FILE(GLOB glshader_shading_file_list "${MAF_SOURCE_DIR}/vtkMAF/iso-surface/*.glshader")
		
  FOREACH(file ${cg_shading_file_list})
    FILE(RELATIVE_PATH file_name "${MAF_SOURCE_DIR}/vtkMAF/iso-surface/" "${file}")
    CONFIGURE_FILE("${file}" "${OUT_SHADER_DIR}/${file_name}" @ONLY)
  ENDFOREACH(file ${cg_shading_file_list})
		
  FOREACH(file ${glshader_shading_file_list})
    FILE(RELATIVE_PATH file_name "${MAF_SOURCE_DIR}/vtkMAF/iso-surface/" "${file}")
    CONFIGURE_FILE("${file}" "${OUT_SHADER_DIR}/${file_name}" @ONLY)
  ENDFOREACH(file ${glshader_shading_file_list})
ENDFOREACH(config)
