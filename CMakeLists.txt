PROJECT (lhpBuilder)

SET(PROJECT_SRCS 
  lhpBuilderApp.cpp
  lhpBuilderApp.h
)

IF (NOT EXECUTABLE_OUTPUT_PATH)
  SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin/ CACHE INTERNAL 
     "Single output directory for building all executables.")
ENDIF (NOT EXECUTABLE_OUTPUT_PATH)

INCLUDE("${PROJECT_SOURCE_DIR}/FindMAF.cmake")
INCLUDE("${MAF_BINARY_PATH}/MAFConfig.cmake")

# find MAF library
IF(MAF_FOUND)
  INCLUDE("${MAF_USE_FILE}")

  ################################################
  ## MAF found, now try to find Medical library ##
  ################################################

  INCLUDE("${PROJECT_SOURCE_DIR}/FindMED.cmake")
  INCLUDE("${MED_BINARY_PATH}/MEDConfig.cmake")

  IF(MED_FOUND)
    INCLUDE("${MED_USE_FILE}")

    # MED library
    SET(PROJECT_LIBS
      medOperations
    )
  ENDIF(MED_FOUND)
  ################################################

  IF (WIN32)
    ADD_EXECUTABLE(${PROJECT_NAME} WIN32 ${PROJECT_SRCS} "${PROJECT_SOURCE_DIR}/app.rc")
    IF(EXECUTABLE_OUTPUT_PATH)
      FOREACH(config ${CMAKE_CONFIGURATION_TYPES})
        CONFIGURE_FILE( "${MAF_BINARY_PATH}/Gui/manifest.in" "${EXECUTABLE_OUTPUT_PATH}/${config}/${PROJECT_NAME}.exe.manifest" COPYONLY)
      ENDFOREACH(config)
    ELSE (EXECUTABLE_OUTPUT_PATH)
      FOREACH(config ${CMAKE_CONFIGURATION_TYPES})
        CONFIGURE_FILE( "${MAF_BINARY_PATH}/Gui/manifest.in" "${PROJECT_BINARY_DIR}/${config}/${PROJECT_NAME}.exe.manifest" COPYONLY)
      ENDFOREACH(config)
    ENDIF (EXECUTABLE_OUTPUT_PATH)
  ELSE (WIN32)
    ADD_EXECUTABLE(${PROJECT_NAME} ${PROJECT_SRCS} )
  ENDIF (WIN32)
  
  # MAF library
  SET(PROJECT_LIBS
    ${PROJECT_LIBS} mafVME mafOperations
  )

  TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${PROJECT_LIBS})
  
  IF (MAF_USE_ITK)
    TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${ITK_LIBRARIES})
  ENDIF (MAF_USE_ITK)

ELSE (MAF_FOUND)
  MESSAGE (FATAL_ERROR "MAF library not found, cannot build application!!")
ENDIF (MAF_FOUND)


