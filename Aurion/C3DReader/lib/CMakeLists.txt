#
# Program:   MULTIMOD APPLICATION FRAMEWORK (MAF)
# Module:    $RCSfile: CMakeLists.txt,v $
# Language:  CMake 1.2
# Date:      $Date: 2008-01-15 10:48:20 $
# Version:   $Revision: 1.1 $
#
# Description: MAF_DRIVERS Cmake project file.
#

PROJECT(C3DReaderDLL)

# 
# Copy drivers libs to library and execute path
#
      FOREACH(config ${CMAKE_CONFIGURATION_TYPES})
          CONFIGURE_FILE(C3D_Reader.dll ${EXECUTABLE_OUTPUT_PATH}/${config}/C3D_Reader.dll COPYONLY IMMEDIATE)
          CONFIGURE_FILE(LicenseAurion.dll ${EXECUTABLE_OUTPUT_PATH}/${config}/LicenseAurion.dll COPYONLY IMMEDIATE)
      ENDFOREACH(config)
