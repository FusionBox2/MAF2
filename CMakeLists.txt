# =========================================================================
#   Program:   lhpBuilder
#   Module:    $RCSfile: CMakeLists.txt,v $
#   Language:  CMake
#   Date:      $Date: 2008-10-29 13:06:25 $
#   Version:   $Revision: 1.33.2.2 $
#   Authors:   Stefano Perticoni
# ==========================================================================

SET (CMAKE_BACKWARDS_COMPATIBILITY "2.0" CACHE INTERNAL "This project is for CMake 2.0")
CMAKE_MINIMUM_REQUIRED(VERSION 2.0) 

PROJECT (LHP)

# Create the header to configure the classes.
CONFIGURE_FILE(${PROJECT_SOURCE_DIR}/lhpDefines.h.in
             ${PROJECT_BINARY_DIR}/lhpDefines.h
             @ONLY)

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

# find MAF library
IF(MAF_FOUND)
	
  INCLUDE("${MAF_USE_FILE}")

  # Find the Medical library 
  INCLUDE("${PROJECT_SOURCE_DIR}/CMakeModules/FindMED.cmake")

  IF(MED_FOUND)
    INCLUDE("${MED_USE_FILE}")
  ELSE (MED_FOUND)
  	MESSAGE (FATAL_ERROR "MAF-MEDICAL library not found, cannot build application!!")
  ENDIF(MED_FOUND)
      
  # Add single dirs to inclusion path
  INCLUDE_DIRECTORIES(BEFORE ".")
  INCLUDE_DIRECTORIES("${LHP_BINARY_DIR}" "${LHP_SOURCE_DIR}/Application" "${LHP_SOURCE_DIR}/Core" "${LHP_SOURCE_DIR}/Common" "${LHP_SOURCE_DIR}/Gui"
                      "${LHP_SOURCE_DIR}/Operations" "${LHP_SOURCE_DIR}/VME" "${LHP_SOURCE_DIR}/Views" "${LHP_SOURCE_DIR}/vtkLHP" 
                      "${LHP_BINARY_DIR}/vtkLHP")
  INCLUDE_DIRECTORIES(${LHP_SOURCE_DIR}/BES_Beta/Medical_AneuFuse/VME)
  INCLUDE_DIRECTORIES(${LHP_SOURCE_DIR}/BES_Beta/openMAF_AneuFuse/vtkMAF)  
  INCLUDE_DIRECTORIES(${LHP_SOURCE_DIR}/BES_Beta/openMAF/Interaction)
  INCLUDE_DIRECTORIES(${LHP_SOURCE_DIR}/BES_Beta/openMAF/IO)
  INCLUDE_DIRECTORIES(${LHP_SOURCE_DIR}/BES_Beta/openMAF/Operators)
  INCLUDE_DIRECTORIES(${LHP_SOURCE_DIR}/BES_Beta/openMAF/VME)
  INCLUDE_DIRECTORIES(${LHP_SOURCE_DIR}/BES_Beta/openMAF/vtkMAF)                     
  
  SUBDIRS (Core)
  SUBDIRS (vtkLHP)
  SUBDIRS (Application)
  SUBDIRS (Common)
  SUBDIRS (Gui)
  SUBDIRS (Operations)
  SUBDIRS (Views)
  SUBDIRS (VME)
  ADD_SUBDIRECTORY(BES_Beta)
  
  # Configuring Doxygen documentation  
  SUBDIRS (Documentation)
  
  SUBDIRS(Aurion/C3DReader/share)
  INCLUDE("${PROJECT_SOURCE_DIR}/CMakeModules/FindC3DReaderAPI.cmake")

  IF(C3DReaderAPI_FOUND)
      INCLUDE(${C3DReaderAPI_USE_FILE})
  ELSE(C3DReaderAPI_FOUND)
		#MESSAGE(FATAL_ERROR "Cannot find C3DReaderAPI_PATH")
  ENDIF(C3DReaderAPI_FOUND)

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
    
    MESSAGE(STATUS "variable LHP_SOURCE_DIR = " ${LHP_SOURCE_DIR})
    FIND_PATH(LHP_DATA_ROOT "TestingDataRoot.txt" "${LHP_SOURCE_DIR}/Testing/UnitTestsData" "${LHP_SOURCE_DIR}/../TestingData" "${LHP_SOURCE_DIR}/../mmData/TestingData" "${LHP_SOURCE_DIR}/../../TestingData"  DOC "Test data repository")
    
    ENABLE_TESTING()
      
    # where will executable tests be written ?
    IF (EXECUTABLE_OUTPUT_PATH)
        SET (CXX_TEST_PATH ${EXECUTABLE_OUTPUT_PATH})
    ELSE (EXECUTABLE_OUTPUT_PATH)
        SET (CXX_TEST_PATH .)
    ENDIF (EXECUTABLE_OUTPUT_PATH)
      
    SUBDIRS (Testing)
    SUBDIRS (VMEUploaderDownloader)
    
  ENDIF(BUILD_TESTING)
  
ELSE (MAF_FOUND)
  MESSAGE (FATAL_ERROR "MAF library not found, cannot build application!!")
ENDIF (MAF_FOUND)



#-----------------------------------------------------------------------------
# LHPBUILDER dictionary download stuff
#-----------------------------------------------------------------------------
OPTION(DOWNLOAD_DICTIONARY "Generate XML lhpBuilder dictionary" ON)

IF(DOWNLOAD_DICTIONARY)
   
   # MASTER DICTIONARY
   
    SET(ERROR 0)    
    #  MESSAGE("variable DOWNLOAD_DICTIONARY = " ${DOWNLOAD_DICTIONARY})
    SET(LHPBUILDER_DICTIONARY_HOST "www.biomedtown.org" CACHE INTERNAL "LHPBuilder dictionary host")
    SET(LHPBUILDER_MASTER_DICTIONARY_SELECTOR "/biomed_town/LHDL/users/swclient/dictionaries/LHDL_dictionary" CACHE INTERNAL "LHPBuilder dictionary selector")
    SET(LHPBUILDER_XML_MASTER_DICTIONARY "lhpXMLDictionary" CACHE INTERNAL "LHPBuilder xml master dictionary")
    
#     MESSAGE("variable LHP_SOURCE_DIR = " ${LHP_SOURCE_DIR})

#     This line contains OUTPUT_VARIABLE OUTPUT_VALUE 
#     EXEC_PROGRAM("python.exe" "${LHP_SOURCE_DIR}/VMEUploaderDownloader" ARGS "lhpCSVDictionaryDownloader.py ${LHPBUILDER_DICTIONARY_HOST} ${LHPBUILDER_DICTIONARY_SELECTOR} ${LHP_SOURCE_DIR}/${LHPBUILDER_XML_DICTIONARY}" OUTPUT_VARIABLE OUTPUT_VALUE RETURN_VALUE RETURN)
    
    SET(RET -1)
    EXEC_PROGRAM("python.exe" "${LHP_SOURCE_DIR}/VMEUploaderDownloader" ARGS "lhpCSVDictionaryDownloader.py ${LHPBUILDER_DICTIONARY_HOST} ${LHPBUILDER_MASTER_DICTIONARY_SELECTOR} ${LHPBUILDER_XML_MASTER_DICTIONARY}"  RETURN_VALUE RET)
    
    IF(${RET} MATCHES "0")
        MESSAGE(STATUS "XML master dictionary generated correctly!")
    ELSE(${RET} MATCHES "0")
        SET(ERROR 1)
    ENDIF(${RET} MATCHES "0")
#     MESSAGE("variable LHPBUILDER_XML_DICTIONARY = " ${LHPBUILDER_XML_DICTIONARY})
#     MESSAGE("variable  OUTPUT = " ${OUTPUT_VALUE})
#     MESSAGE("variable  RET = " ${RET})

    # DICOM DICTIONARY
       
    SET(LHPBUILDER_DICOM_DICTIONARY_SELECTOR "/biomed_town/LHDL/users/swclient/dictionaries/dicomsource" CACHE INTERNAL "LHPBuilder dictionary selector")
    SET(LHPBUILDER_XML_DICOM_DICTIONARY "lhpXMLDicomSourceSubdictionary" CACHE INTERNAL "LHPBuilder xml dicom dictionary")
        
    EXEC_PROGRAM("python.exe" "${LHP_SOURCE_DIR}/VMEUploaderDownloader" ARGS "lhpCSVDictionaryDownloader.py ${LHPBUILDER_DICTIONARY_HOST} ${LHPBUILDER_DICOM_DICTIONARY_SELECTOR} ${LHPBUILDER_XML_DICOM_DICTIONARY}"  RETURN_VALUE RET)
    
    IF(${RET} MATCHES "0")
        MESSAGE(STATUS "XML dicom subdictionary generated correctly!")
    ELSE(${RET} MATCHES "0")
        SET(ERROR 1)
    ENDIF(${RET} MATCHES "0")

    # MOTIONANALYSIS DICTIONARY
    
    SET(LHPBUILDER_MOTIONANALYSIS_DICTIONARY_SELECTOR "/biomed_town/LHDL/users/swclient/dictionaries/MAsource" CACHE INTERNAL "LHPBuilder dictionary selector")
    SET(LHPBUILDER_XML_MOTIONANALYSIS_DICTIONARY "lhpXMLMotionAnalysisSourceSubdictionary" CACHE INTERNAL "LHPBuilder xml motion analysis dictionary")
    
    EXEC_PROGRAM("python.exe" "${LHP_SOURCE_DIR}/VMEUploaderDownloader" ARGS "lhpCSVDictionaryDownloader.py ${LHPBUILDER_DICTIONARY_HOST} ${LHPBUILDER_MOTIONANALYSIS_DICTIONARY_SELECTOR} ${LHPBUILDER_XML_MOTIONANALYSIS_DICTIONARY}"  RETURN_VALUE RET)

    
    IF(${RET} MATCHES "0")
        MESSAGE(STATUS "XML motion analysis dictionary generated correctly!")
    ELSE(${RET} MATCHES "0")
        SET(ERROR 1)
    ENDIF(${RET} MATCHES "0")

    # FA DICTIONARY
    
    SET(LHPBUILDER_FA_DICTIONARY_SELECTOR "/biomed_town/LHDL/users/swclient/dictionaries/FA_onto" CACHE INTERNAL "LHPBuilder dictionary selector")
    SET(LHPBUILDER_XML_FA_DICTIONARY "lhpXMLFASourceSubdictionary" CACHE INTERNAL "LHPBuilder xml FA dictionary")
    
    EXEC_PROGRAM("python.exe" "${LHP_SOURCE_DIR}/VMEUploaderDownloader" ARGS "lhpCSVDictionaryDownloader.py ${LHPBUILDER_DICTIONARY_HOST} ${LHPBUILDER_FA_DICTIONARY_SELECTOR} ${LHPBUILDER_XML_FA_DICTIONARY}"  RETURN_VALUE RET)

    
    IF(${RET} MATCHES "0")
        MESSAGE(STATUS "XML FA dictionary generated correctly!")
    ELSE(${RET} MATCHES "0")
        SET(ERROR 1)
    ENDIF(${RET} MATCHES "0")

    # MICROCT DICTIONARY
    
    SET(LHPBUILDER_MICROCT_DICTIONARY_SELECTOR "/biomed_town/LHDL/users/swclient/dictionaries/MicroCTSource" CACHE INTERNAL "LHPBuilder dictionary selector")
    SET(LHPBUILDER_XML_MICROCT_DICTIONARY "lhpXMLMicroCTSourceSubdictionary" CACHE INTERNAL "LHPBuilder xml MicroCT dictionary")
    
    EXEC_PROGRAM("python.exe" "${LHP_SOURCE_DIR}/VMEUploaderDownloader" ARGS "lhpCSVDictionaryDownloader.py ${LHPBUILDER_DICTIONARY_HOST} ${LHPBUILDER_MICROCT_DICTIONARY_SELECTOR} ${LHPBUILDER_XML_MICROCT_DICTIONARY}"  RETURN_VALUE RET)

    
    IF(${RET} MATCHES "0")
        MESSAGE(STATUS "XML MicroCT dictionary generated correctly!")
    ELSE(${RET} MATCHES "0")
        SET(ERROR 1)
    ENDIF(${RET} MATCHES "0")
        
    IF(${ERROR} MATCHES "0")
        MESSAGE(STATUS "XML dictionary generated correctly!")
    ELSE(${ERROR} MATCHES "0")
        MESSAGE("Problems generating XML dictionary!

        
In order for the generation to work you need to:
1: Be connected to the Internet
1: Have python 2.5 installed
2: Have python 2.5 python.exe in your PATH as default python version

If You don't want to this message to appear you can set DOWNLOAD_DICTIONARY
to false. Please notice that some functions ol LHPBuilder will not be available
with this setting.
")
#         MESSAGE(${OUTPUT_VALUE})
    ENDIF(${ERROR} MATCHES "0")
    
ELSE(DOWNLOAD_DICTIONARY)
         MESSAGE("You have choosed not to generate XML dictionary!
Please notice that some functions of LHPBUilder will not be available
with this setting.")
#   
ENDIF(DOWNLOAD_DICTIONARY)

#-----------------------------------------------------------------------------
# Security stuff.  
#-----------------------------------------------------------------------------

EXEC_PROGRAM("python.exe" "${LHP_SOURCE_DIR}/Security/keyczar" ARGS "CheckKeyczarDependencies.py"  RETURN_VALUE RET)

# MESSAGE("variable RET = " ${RET})

IF(${RET} MATCHES "0")
    MESSAGE(STATUS "Check Keyczar Dependencies successfull!")
    SET(ERROR 0)
ELSE(${RET} MATCHES "0")
    SET(ERROR 1)
ENDIF(${RET} MATCHES "0")
    
# MESSAGE("variable ERROR = " ${ERROR})

IF(${ERROR} MATCHES "0")
    MESSAGE(STATUS "Check Keyczar Dependencies successfull!")
ELSE(${ERROR} MATCHES "0")
    MESSAGE("Problems checking Keyczar Dependencies...
    
In order to install the security components you have to:
1: Be connected to the Internet; this is required to download additional components
2: Have python 2.5 installed
3: Have python 2.5 python.exe in your PATH as default python version
4: Your user account should be able to write in python2.5 directory

")

# MESSAGE(${OUTPUT_VALUE})
ENDIF(${ERROR} MATCHES "0")

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
  
  SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /LARGEADDRESSAWARE " )
  
ENDIF (WIN32)               