PROJECT (lhpBuilder)

SET(PROJECT_SRCS 
  lhpBuilderApp.cpp
  lhpBuilderApp.h
  mafLHPBuilderViewCompound.cpp
  mafLHPBuilderViewCompound.h
  mafViewSliceLHPBuilder.cpp
  mafViewSliceLHPBuilder.h
  mafViewRXLHPBuilder.cpp
  mafViewRXLHPBuilder.h
  mafViewRXCTLHPBuilder.cpp
  mafViewRXCTLHPBuilder.h
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


#-----------------------------------------------------------------------------
# LHPBUILDER deploy stuff.  
#-----------------------------------------------------------------------------

# This is specific for NSIS Nullsoft Installer that runs currently under Windows only and Parabuild.
# An LHPBUILDER_VERSION variable is created to gather versioning information
IF (WIN32)
    
  # installer useful values
  SET(LHPBUILDER_EXECUTABLE_OUTPUT_PATH "${EXECUTABLE_OUTPUT_PATH}/Debug/" CACHE PATH "Source directory for lhpBuilder executable. This is used by the installer to find the executable.")
  
  STRING(REPLACE "/" "\\" LHPBUILDER_EXECUTABLE_OUTPUT_PATH_WIN ${LHPBUILDER_EXECUTABLE_OUTPUT_PATH})
  SET(LHPBUILDER_EXECUTABLE_OUTPUT_PATH_WIN ${LHPBUILDER_EXECUTABLE_OUTPUT_PATH_WIN} CACHE INTERNAL "get the executable from this directory")
  
  # MESSAGE("variable value = " ${LHPBUILDER_EXECUTABLE_OUTPUT_PATH})
  # MESSAGE("variable value = " ${LHPBUILDER_EXECUTABLE_OUTPUT_PATH_WIN})
   
  # this values are set by hand when major or minor is released
  SET(LHPBUILDER_MAJOR_VERSION 1 CACHE INTERNAL "major version")
  SET(LHPBUILDER_MINOR_VERSION 0 CACHE INTERNAL "minor version")
  
  # test for environment variable reading
  # MESSAG# "$ENV{PATH}")
  # MESSAG# "$ENV{PIPPO}")
  
  # automatic values from parabuild created temporary environment variable
  SET(LHPBUILDER_BUILD_TIMESTAMP $ENV{PARABUILD_BUILD_TIMESTAMP} CACHE INTERNAL "build timestamp")
  SET(LHPBUILDER_BUILD_NUMBER $ENV{PARABUILD_BUILD_NUMBER} CACHE INTERNAL "build number")
  
  # create the main version variable:
  SET(LHPBUILDER_VERSION "${LHPBUILDER_MAJOR_VERSION}_${LHPBUILDER_MINOR_VERSION}_TS_${PARABUILD_BUILD_TIMESTAMP}_BUILD_${PARABUILD_BUILD_NUMBER}" CACHE INTERNAL "big version identifier concatenating other identifier")
  
  # MESSAGE("variable value = " ${LHPBUILDER_VERSION})
    
  # config installer file
  # set the installer directory variable
  SET(LHPBUILDER_INSTALLER_DIR ${PROJECT_SOURCE_DIR}/Installer)
  # convert it to Windows format
  STRING (REPLACE "/" "\\" LHPBUILDER_INSTALLER_DIR_WIN ${LHPBUILDER_INSTALLER_DIR} )
  # MESSAGE("variable LHPBUILDER_INSTALLER_DIR_WIN = " ${LHPBUILDER_INSTALLER_DIR_WIN})
  
  # installer executable will be written to this directory
  SET(LHPBUILDER_INSTALLER_EXECUTABLE_OUTPUT_DIR ${LHPBUILDER_EXECUTABLE_OUTPUT_PATH} CACHE PATH "the installer executable will be written to this directory ")
  # convert it to Windows format  
  STRING(REPLACE "/" "\\" LHPBUILDER_INSTALLER_EXECUTABLE_OUTPUT_DIR_WIN ${LHPBUILDER_INSTALLER_EXECUTABLE_OUTPUT_DIR})
  SET(LHPBUILDER_INSTALLER_EXECUTABLE_OUTPUT_DIR_WIN ${LHPBUILDER_INSTALLER_EXECUTABLE_OUTPUT_DIR_WIN} CACHE INTERNAL "get the executable  from this directory")
  # MESSAGE("variable LHPBUILDER_INSTALLER_EXECUTABLE_OUTPUT_DIR_WIN = " ${LHPBUILDER_INSTALLER_EXECUTABLE_OUTPUT_DIR_WIN})
  
  CONFIGURE_FILE(${LHPBUILDER_INSTALLER_DIR}/LHPBuilderInstallerScript.nsi.in  ${LHPBUILDER_EXECUTABLE_OUTPUT_PATH}/LHPBuilderInstallerScript.nsi @ONLY)
  
ENDIF (WIN32)               