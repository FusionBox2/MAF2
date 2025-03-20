cmake_minimum_required(VERSION 3.27)

project (FTK VERSION 0.1.0)

set(FTK_VERSION "${FTK_VERSION_MAJOR}.${FTK_VERSION_MINOR}.${FTK_VERSION_PATCH}")

#-----------------------------------------------------------------------------
# where to place output binaries
#-----------------------------------------------------------------------------
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${FTK_BINARY_DIR}/bin CACHE INTERNAL "Single output directory for building all archives.")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${FTK_BINARY_DIR}/bin CACHE INTERNAL "Single output directory for building all libraries.")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${FTK_BINARY_DIR}/bin CACHE INTERNAL "Single output directory for building all executables.")

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
# This enables support for Visual Studio Edit & Continue and OpenMP
#-----------------------------------------------------------------------------
if(MSVC)
	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /openmp")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /openmp")
#	set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} /ZI")
#	set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /ZI")
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<CONFIG:Debug,RelWithDebInfo>,EditAndContinue,ProgramDatabase>")
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<CONFIG:Debug,RelWithDebInfo>,EditAndContinue,ProgramDatabase>")
  add_compile_options(/Zc:__cplusplus)
endif(MSVC)

#-----------------------------------------------------------------------------
# Options for FTK building
#-----------------------------------------------------------------------------
option(MAF_BUILD_MAFDLL  "Build mafDLL.dll"                    OFF)
option(BUILD_EXAMPLES    "Build examples"                      OFF)
option(BUILD_TESTING     "Build the testing tree"              OFF)
option(MAF_QA            "Generate QA"                         OFF)
option(BUILD_SHARED_LIBS "Build as shared libraries"           OFF)
option(MAF_USE_BOOST     "Find and Link the boost library"     ON)
option(MAF_USE_EIGEN     "Find and Link the Eigen library"     ON)
option(MAF_USE_WX        "Find and Link the wxWidgets library" ON)
option(MAF_USE_QT        "Find and Link the Qt library"        ON)
option(MAF_USE_VTK       "Find and Link the VTK library"       ON)
option(MAF_USE_ITK       "Find and Link the ITK library"       ON)
option(FTK_USE_XercesC   "Find and Link the XercesC library"   ON)
option(MAF_USE_CURL      "Find and Link the cURL library"      ON)
option(MAF_USE_CRYPTO    "Find and Link the CRYPTO library"    ON)
option(MAF_USE_Offis     "Find and Link the Offis library"     ON)
option(MAF_USE_BTK       "Find and Link the BTK library"       ON)

include(Dependencies/dependencies.cmake)

if(ITK_FOUND)
  include(${ITK_USE_FILE})
  link_directories(${ITK_LIBRARY_DIRS})
  link_libraries(${ITK_LIBRARIES})
endif(ITK_FOUND)

if(wxWidgets_FOUND)
  link_libraries(${wxWidgets_LIBRARIES})
endif(wxWidgets_FOUND)

add_subdirectory(Application)
add_subdirectory(Base)
add_subdirectory(Common)
add_subdirectory(Core)
add_subdirectory(Gui)
add_subdirectory(Interaction)
add_subdirectory(ftk/IO)
add_subdirectory(Operations)
add_subdirectory(Views)
add_subdirectory(VME)
add_subdirectory(vtkMAF)
add_subdirectory(libjson)
add_subdirectory(wxApp)

if(Qt6_FOUND)
  add_subdirectory(QtApp)
endif(Qt6_FOUND)

set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT lhpFusionBox)

if(WIN32)
  # Disable automatic manifest generation.
  string(REPLACE "/MANIFEST" "/MANIFEST:NO" CMAKE_EXE_LINKER_FLAGS ${CMAKE_EXE_LINKER_FLAGS})
  # Explicitly disable it since it is the default for newer versions of VS
  string(REGEX MATCH "MANIFEST:NO" tmp_manifest ${CMAKE_EXE_LINKER_FLAGS})
  if(NOT tmp_manifest)
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /MANIFEST:NO")
  endif(NOT tmp_manifest)
endif(WIN32)

# Create the header to configure the classes.
configure_file(${PROJECT_SOURCE_DIR}/Configure.h.in
             ${PROJECT_BINARY_DIR}/ftkConfigure.h
             @ONLY)

set(OUT_SPLASH_DIR ${FTK_BINARY_DIR}/Splash)
file(MAKE_DIRECTORY ${OUT_SPLASH_DIR})
file(COPY Installer/SPLASH_SCREEN.bmp DESTINATION ${OUT_SPLASH_DIR})
 
include(CPack) 