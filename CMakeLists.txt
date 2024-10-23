cmake_minimum_required(VERSION 3.27)
project (FTK VERSION 0.1.0)

#-----------------------------------------------------------------------------
# Macro for DEBUG logging
#-----------------------------------------------------------------------------
INCLUDE (${FTK_SOURCE_DIR}/modules/DebugMacro.cmake)
MARK_AS_ADVANCED (
  DEBUG_MESSAGES
)

OPTION(DEBUG_MESSAGES "Enable debug Messages" OFF)

#-----------------------------------------------------------------------------
# where to place output binaries
#-----------------------------------------------------------------------------
set(LIBRARY_OUTPUT_PATH    ${FTK_BINARY_DIR}/bin CACHE INTERNAL "Single output directory for building all libraries.")
set(EXECUTABLE_OUTPUT_PATH ${FTK_BINARY_DIR}/bin CACHE INTERNAL "Single output directory for building all executables.")
mark_as_advanced(LIBRARY_OUTPUT_PATH)

MARK_AS_ADVANCED (
  LIBRARY_OUTPUT_PATH 
  #EXECUTABLE_OUTPUT_PATH
)

#set(CMN_PRJ_FILE ${FTK_SOURCE_DIR}/CMake/CmnPrj.cmake)

if(CMAKE_SIZEOF_VOID_P EQUAL 8)
    message( "64 bits compiler detected" )
    set(EX_PLATFORM 64)
    set(EX_PLATFORM_NAME "x64")
    set(HAVE_64_BIT 1)
else(CMAKE_SIZEOF_VOID_P EQUAL 8) 
    message( "32 bits compiler detected" )
    set(EX_PLATFORM 32)
    set(EX_PLATFORM_NAME "x86")
endif(CMAKE_SIZEOF_VOID_P EQUAL 8)

#-----------------------------------------------------------------------------
# FTK version number.
#-----------------------------------------------------------------------------
SET(FTK_VERSION "${FTK_VERSION_MAJOR}.${FTK_VERSION_MINOR}.${FTK_VERSION_PATCH}")

#-----------------------------------------------------------------------------
# This enables support for Visual Studio Edit & Continue, OpenMP, and parallel compilation
#-----------------------------------------------------------------------------
IF (${CMAKE_CXX_COMPILER_ID} STREQUAL "MSVC")
	SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /openmp")
	SET(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /ZI")
ENDIF (${CMAKE_CXX_COMPILER_ID} STREQUAL "MSVC")

#-----------------------------------------------------------------------------
# Options for FTK building
#-----------------------------------------------------------------------------
#Remove temporarily the BUILD_SHARED_LIBS because it is not possible to switch the compilation to dynamic.
#SET(BUILD_SHARED_LIBS 0)
# OPTION(BUILD_SHARED_LIBS "Build with shared libraries." OFF )
OPTION(BUILD_EXAMPLES "Build examples." OFF )
OPTION(BUILD_TESTING "Build the testing tree." OFF)
OPTION(MAF_BUILD_MAFDLL "Build mafDLL.dll." OFF)
OPTION(MAF_QA "Generate QA" OFF)

option(BUILD_SHARED_LIBS "Build in shared libraries."           OFF)
option(MAF_USE_BOOST     "Find and Link the boost library."     ON)
option(MAF_USE_EIGEN     "Find and Link the Eigen library."     ON)
option(MAF_USE_WX        "Find and Link the wxWidgets library." ON)
option(MAF_USE_QT        "Find and Link the Qt library."        OFF)
OPTION(MAF_USE_VTK       "Find and Link the VTK library."       ON)
OPTION(MAF_USE_ITK       "Find and Link the ITK library."       ON)
OPTION(FTK_USE_XercesC   "Find and Link the XercesC library."   ON)
OPTION(MAF_USE_CURL      "Find and Link the cURL library."      ON)
OPTION(MAF_USE_CRYPTO    "Find and Link the CRYPTO library."    ON)
OPTION(MAF_USE_Offis     "Find and Link the Offis library."     ON)
OPTION(MAF_USE_BTK       "Find and Link the BTK library."       ON)

mark_as_advanced(BUILD_SHARED_LIBS)
if(MSVC)
add_compile_options(/Zc:__cplusplus)
endif(MSVC)

if(MAF_USE_BOOST)
  set(Boost_DIR ${BST_DIR})
  find_package(Boost REQUIRED CONFIG)
  if(Boost_FOUND)
#  LINK_LIBRARIES(${wxWidgets_LIBRARIES})
    include_directories(${Boost_INCLUDE_DIRS})
  endif(Boost_FOUND)
endif(MAF_USE_BOOST)

if(MAF_USE_EIGEN)
  set(Eigen3_DIR ${EGN_DIR})
  find_package(Eigen3 REQUIRED CONFIG)
endif(MAF_USE_EIGEN)

if(MAF_USE_WX)
  set(wxWidgets_DIR ${WXW_DIR})
  find_package(wxWidgets REQUIRED CONFIG)#COMPONENTS adv core base aui CONFIG)
  if(wxWidgets_FOUND)
  LINK_LIBRARIES(${wxWidgets_LIBRARIES})
#    include("${wxWidgets_USE_FILE}")
#    add_subdirectory(wxApp)
#    add_subdirectory(wxGui)
#    set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT ftkWxApp)
  endif(wxWidgets_FOUND)
endif(MAF_USE_WX)

if(MAF_USE_QT)
#  set(Qt6_DIR C:/Qt/6.5.2/msvc2019_64/lib/cmake)
  find_package(Qt6 COMPONENTS Core Widgets LinguistTools)
  if(Qt6_FOUND)
#    PATHS C:/Qt/6.5.2/msvc2019_64/lib/cmake/Qt6)
#    add_subdirectory(QtApp)
#    add_subdirectory(QtGui)    
#    set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT ftkQtApp)
  endif(Qt6_FOUND)
endif(MAF_USE_QT)

if(MAF_USE_VTK)
  find_package(VTK REQUIRED CONFIG)
endif(MAF_USE_VTK)

if(MAF_USE_ITK)
  find_package(ITK REQUIRED CONFIG)
  if(ITK_FOUND)
    INCLUDE(${ITK_USE_FILE})
    LINK_DIRECTORIES(${ITK_LIBRARY_DIRS})
    LINK_LIBRARIES(${ITK_LIBRARIES})
  endif(ITK_FOUND)
endif(MAF_USE_ITK)

if(FTK_USE_XercesC)
  set(XercesC_DIR ${XRC_DIR}/cmake)
  find_package(XercesC REQUIRED CONFIG)
  if(XercesC_FOUND)
  endif(XercesC_FOUND)
endif(FTK_USE_XercesC)

if(MAF_USE_CURL)
  set(CURL_DIR ${CRL_DIR})
  find_package(CURL REQUIRED CONFIG)
  if(CURL_FOUND)
  endif(CURL_FOUND)
endif(MAF_USE_CURL)

if(MAF_USE_CRYPTO)
  set(cryptopp_DIR ${CRP_DIR})
  find_package(cryptopp REQUIRED CONFIG)
  if(cryptopp_FOUND)
  endif(cryptopp_FOUND)
endif(MAF_USE_CRYPTO)

if(MAF_USE_Offis)
  find_package(DCMTK REQUIRED)
  if(DCMTK_FOUND)
    INCLUDE(${DCMTK_CONFIG})
    LINK_DIRECTORIES(${DCMTK_DIR}/lib)
  endif(DCMTK_FOUND)
endif(MAF_USE_Offis)

if(MAF_USE_BTK)
  find_package(BTK REQUIRED)
  if(BTK_FOUND)
  INCLUDE_DIRECTORIES(${BTK_INCLUDE_DIRS})
  LINK_DIRECTORIES(${BTK_LIBRARY_DIRS})
  LINK_LIBRARIES(${BTK_LIBRARIES})
  endif(BTK_FOUND)
endif(MAF_USE_BTK)

INCLUDE_DIRECTORIES(BEFORE ".")

SET(INCLUDE_DIRS "${FTK_BINARY_DIR}" 
                 "${FTK_BINARY_DIR}/Interaction" 
                 "${FTK_BINARY_DIR}/vtkMAF" 
                 "${FTK_SOURCE_DIR}/Application" 
                 "${FTK_SOURCE_DIR}/Base" 
                 "${FTK_SOURCE_DIR}/Common" 
                 "${FTK_SOURCE_DIR}/Core" 
                 "${FTK_SOURCE_DIR}/Gui"
                 "${FTK_SOURCE_DIR}/Interaction" 
                 "${FTK_SOURCE_DIR}/Interaction/Drivers" 
                 "${FTK_SOURCE_DIR}/Operations" 
                 "${FTK_SOURCE_DIR}/Views"
                 "${FTK_SOURCE_DIR}/VME" 
                 "${FTK_SOURCE_DIR}/VME/DataPipes" 
                 "${FTK_SOURCE_DIR}/vtkMAF") 
SET(INCLUDE_DIRS ${INCLUDE_DIRS} "${FTK_SOURCE_DIR}/mafDLL")

INCLUDE_DIRECTORIES(${INCLUDE_DIRS})


	IF(MAF_BUILD_MAFDLL)
		OPTION(MED_BUILD_MEDDLL "Build medDLL.dll." OFF)
	ENDIF(MAF_BUILD_MAFDLL)

	IF(MED_BUILD_MEDDLL)
	  SET(MED_BUILD_MEDDLL 1)
	ELSE(MED_BUILD_MEDDLL)
    SET(MED_BUILD_MEDDLL 0)
	ENDIF (MED_BUILD_MEDDLL)
  
SUBDIRS(Application)
SUBDIRS(Base)
SUBDIRS(Common)
SUBDIRS(Core)
SUBDIRS(Gui)
SUBDIRS(Interaction)
SUBDIRS(ftk/IO)
SUBDIRS(Operations)
SUBDIRS(Views)
SUBDIRS(VME)
SUBDIRS(vtkMAF)
set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT lhpFusionBox)

  SET(BUILD_SHARED_LIBS 0)
  
  IF(WIN32)
  # Disable automatic manifest generation.
  STRING(REPLACE "/MANIFEST" "/MANIFEST:NO" CMAKE_EXE_LINKER_FLAGS ${CMAKE_EXE_LINKER_FLAGS})
  # Explicitly disable it since it is the default for newer versions of VS
  STRING(REGEX MATCH "MANIFEST:NO" tmp_manifest ${CMAKE_EXE_LINKER_FLAGS})
  IF(NOT tmp_manifest)
    SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /MANIFEST:NO")
  ENDIF(NOT tmp_manifest)
ENDIF(WIN32)

# Create the header to configure the classes.
CONFIGURE_FILE(${PROJECT_SOURCE_DIR}/Configure.h.in
             ${PROJECT_BINARY_DIR}/ftkConfigure.h
             @ONLY)
