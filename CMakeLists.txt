# =========================================================================
#   Program:   lhpBuilder
#   Module:    $RCSfile: CMakeLists.txt,v $
#   Language:  CMake
#   Date:      $Date: 2007-12-04 12:56:28 $
#   Version:   $Revision: 1.15 $
#   Authors:   Stefano Perticoni
# ==========================================================================

SET (CMAKE_BACKWARDS_COMPATIBILITY "2.0" CACHE INTERNAL "This project is for CMake 2.0")
CMAKE_MINIMUM_REQUIRED(VERSION 2.0) 

PROJECT (LHP)

#-----------------------------------------------------------------------------
# this define the LHP_SUBPROJECT macro
#-----------------------------------------------------------------------------
INCLUDE (${LHP_SOURCE_DIR}/CMakeModules/LHPConfigMacro.cmake)

#-----------------------------------------------------------------------------
# where to place output binaries
#-----------------------------------------------------------------------------
SET (LIBRARY_OUTPUT_PATH ${LHP_BINARY_DIR}/bin/ CACHE INTERNAL
     "Single output directory for building all libraries.")

SET (EXECUTABLE_OUTPUT_PATH ${LHP_BINARY_DIR}/bin/ CACHE INTERNAL 
     "Single output directory for building all executables.")

MARK_AS_ADVANCED (
  LIBRARY_OUTPUT_PATH 
)

#-----------------------------------------------------------------------------
# Options for lhpBuilder building
#-----------------------------------------------------------------------------
OPTION(BUILD_SHARED_LIBS "Build with shared libraries." 0)
MARK_AS_ADVANCED (
  BUILD_SHARED_LIBS 
)

OPTION(BUILD_TESTING "Build the testing tree." OFF)

#-----------------------------------------------------------------------------
# find required libraries
#-----------------------------------------------------------------------------

INCLUDE("${PROJECT_SOURCE_DIR}/CMakeModules/FindMAF.cmake")
INCLUDE("${MAF_BINARY_PATH}/MAFConfig.cmake")

# find MAF library
IF(MAF_FOUND)
	
  INCLUDE("${MAF_USE_FILE}")

  # Find the Medical library 
  INCLUDE("${PROJECT_SOURCE_DIR}/CMakeModules/FindMED.cmake")
  INCLUDE("${MED_BINARY_PATH}/MEDConfig.cmake")

  IF(MED_FOUND)
    INCLUDE("${MED_USE_FILE}")
  ELSE (MED_FOUND)
  	MESSAGE (FATAL_ERROR "MAF-MEDICAL library not found, cannot build application!!")
  ENDIF(MED_FOUND)
      
  # Add single dirs to inclusion path
  INCLUDE_DIRECTORIES(BEFORE ".")
  INCLUDE_DIRECTORIES(${LHP_BINARY_DIR})
  INCLUDE_DIRECTORIES(${LHP_SOURCE_DIR}/Application)
  INCLUDE_DIRECTORIES(${LHP_BINARY_DIR}/Core)
  INCLUDE_DIRECTORIES(${LHP_SOURCE_DIR}/Common)
  INCLUDE_DIRECTORIES(${LHP_SOURCE_DIR}/Operations)
  INCLUDE_DIRECTORIES(${LHP_SOURCE_DIR}/VME)
  INCLUDE_DIRECTORIES(${LHP_SOURCE_DIR}/Views)
  INCLUDE_DIRECTORIES(${LHP_SOURCE_DIR}/vtkLHP)
  INCLUDE_DIRECTORIES(${LHP_BINARY_DIR}/vtkLHP)
  
  SUBDIRS (Core)
  SUBDIRS (vtkLHP)
  SUBDIRS (Application)
  SUBDIRS (Common)
  SUBDIRS (Operations)
  SUBDIRS (Views)
  SUBDIRS (VME)
  
  # Configuring Doxygen documentation  
  SUBDIRS (Documentation)
  
  # Export Build settings and library dependencies
  INCLUDE(CMakeExportBuildSettings)
  
  CMAKE_EXPORT_BUILD_SETTINGS(
    ${LHP_BINARY_DIR}/LHPBuildSettings.cmake
  )
  
  EXPORT_LIBRARY_DEPENDENCIES(
    ${LHP_BINARY_DIR}/LHPLibraryDepends.cmake
  )

  CONFIGURE_FILE(
    ${LHP_SOURCE_DIR}/CMakeInput/LHPConfig.cmake.in
    ${LHP_BINARY_DIR}/LHPConfig.cmake
  @ONLY IMMEDIATE
  )
  
  CONFIGURE_FILE(
    ${LHP_SOURCE_DIR}/CMakeInput/LHPUse.cmake.in
    ${LHP_BINARY_DIR}/LHPUse.cmake
  @ONLY IMMEDIATE
  )
  
  #-----------------------------------------------------------------------------
  # Testing stuff
  #-----------------------------------------------------------------------------
  IF(BUILD_TESTING)
    
    MESSAGE("variable LHP_SOURCE_DIR = " ${LHP_SOURCE_DIR})
    FIND_PATH(LHP_DATA_ROOT "TestingDataRoot.txt" "${LHP_SOURCE_DIR}/Testing/UnitTestsData" "${LHP_SOURCE_DIR}/../TestingData" "${LHP_SOURCE_DIR}/../mmData/TestingData" "${LHP_SOURCE_DIR}/../../TestingData"  DOC "Test data repository")
    ENABLE_TESTING()
      
    # where will executable tests be written ?
    IF (EXECUTABLE_OUTPUT_PATH)
        SET (CXX_TEST_PATH ${EXECUTABLE_OUTPUT_PATH})
    ELSE (EXECUTABLE_OUTPUT_PATH)
        SET (CXX_TEST_PATH .)
    ENDIF (EXECUTABLE_OUTPUT_PATH)
      
    SUBDIRS (Testing)
    
  ENDIF(BUILD_TESTING)
  
ELSE (MAF_FOUND)
  MESSAGE (FATAL_ERROR "MAF library not found, cannot build application!!")
ENDIF (MAF_FOUND)



#-----------------------------------------------------------------------------
# LHPBUILDER stuff
#-----------------------------------------------------------------------------
OPTION(DOWNLOAD_DICTIONARY "Download the lhpBuilder dictionary" ON)

IF(DOWNLOAD_DICTIONARY)
    
    SET(LHPBUILDER_DICTIONARY_HOST "www.biomedtown.org" CACHE PATH "LHPBuilder dictionary host")
    SET(LHPBUILDER_DICTIONARY_SELECTOR "/biomed_town/LHDL/lhdl-management/Consortium-room/lhdl-repository/WP5/Dictionaries/LHDL_dictionary.csv" CACHE PATH "LHPBuilder dictionary selector")
    SET(LHPBUILDER_XML_DICTIONARY "lhpXMLDictionary.xml" CACHE PATH "LHPBuilder xml dictionary")
    
#     MESSAGE("variable LHP_SOURCE_DIR = " ${LHP_SOURCE_DIR})

#     This line contains OUTPUT_VARIABLE OUTPUT_VALUE 
#     EXEC_PROGRAM("python.exe" "${LHP_SOURCE_DIR}/VMEUploaderDownloader" ARGS "lhpCSVDictionaryDownloader.py ${LHPBUILDER_DICTIONARY_HOST} ${LHPBUILDER_DICTIONARY_SELECTOR} ${LHP_SOURCE_DIR}/${LHPBUILDER_XML_DICTIONARY}" OUTPUT_VARIABLE OUTPUT_VALUE RETURN_VALUE RETURN)
    
    SET(RET -1)
    EXEC_PROGRAM("python.exe" "${LHP_SOURCE_DIR}/VMEUploaderDownloader" ARGS "lhpCSVDictionaryDownloader.py ${LHPBUILDER_DICTIONARY_HOST} ${LHPBUILDER_DICTIONARY_SELECTOR} ${LHPBUILDER_XML_DICTIONARY}"  RETURN_VALUE RET)

#     MESSAGE("variable LHPBUILDER_XML_DICTIONARY = " ${LHPBUILDER_XML_DICTIONARY})
#     MESSAGE("variable  OUTPUT = " ${OUTPUT_VALUE})
#     MESSAGE("variable  RET = " ${RET})
    
    IF(${RET} EQUAL 0)
        MESSAGE(STATUS "XML dictionary generated correctly!")
    ELSE(${RET} EQUAL 0)
        MESSAGE(SEND_ERROR "Problems generating XML dictionary!")
#         MESSAGE(${OUTPUT_VALUE})
    ENDIF(${RET} EQUAL 0)
    
ENDIF(DOWNLOAD_DICTIONARY)

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
  SET(LHPBUILDER_MINOR_VERSION 2 CACHE INTERNAL "minor version")
  
  # test for environment variable reading
  # MESSAG# "$ENV{PATH}")
  # MESSAG# "$ENV{PIPPO}")
  
  # automatic values from parabuild created temporary environment variable
  SET(LHPBUILDER_BUILD_TIMESTAMP $ENV{PARABUILD_BUILD_TIMESTAMP} CACHE INTERNAL "build timestamp")
  SET(LHPBUILDER_BUILD_NUMBER $ENV{PARABUILD_BUILD_NUMBER} CACHE INTERNAL "build number")
  
  # create the main version variable:
  SET(LHPBUILDER_VERSION "${LHPBUILDER_MAJOR_VERSION}_${LHPBUILDER_MINOR_VERSION}_TS_${LHPBUILDER_BUILD_TIMESTAMP}_BUILD_${LHPBUILDER_BUILD_NUMBER}" CACHE INTERNAL "big version identifier concatenating other identifier")
  
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