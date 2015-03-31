#.rst:
# FindCryptoPP
# -----------
#
# Find the Crpto++ headers and libraries.
#
# This module reports information about the Xerces installation in
# several variables.  General variables::
#
#   CryptoPP_FOUND - true if the Crypto++ headers and libraries were found
#   CryptoPP_VERSION - Crypto++ release version
#   CryptoPP_INCLUDE_DIRS - the directory containing the Crypto++ headers
#   CryptoPP_LIBRARIES - Crypto++ libraries to be linked
#
# The following cache variables may also be set::
#
#   CryptoPP_INCLUDE_DIR - the directory containing the Xerces headers
#   CryptoPP_LIBRARY - the Xerces library

# Written by Fedor Moiseev <fedormsv@gmail.com>

#=============================================================================
# Copyright 2014 Universite Libre de Bruxelles
#
# Distributed under the OSI-approved BSD License (the "License");
# see accompanying file Copyright.txt for details.
#
# This software is distributed WITHOUT ANY WARRANTY; without even the
# implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
# See the License for more information.
#=============================================================================
# (To distribute this file outside of CMake, substitute the full
#  License text for the above reference.)

function(_CryptoPP_GET_VERSION  version_hdr)
    file(STRINGS ${version_hdr} _contents REGEX "^[ \t]*#define CRYPTOPP_VERSION.*")
    if(_contents)
        string(REGEX REPLACE ".*#define CRYPTOPP_VERSION[ \t]+([0-9]+).*" "\\1" CryptoPP_VER "${_contents}")

        if(NOT CryptoPP_VER MATCHES "^[0-9]+$")
            message(FATAL_ERROR "Version parsing failed for CRYPTOPP_VERSION!")
        endif()

        set(CryptoPP_VERSION "${CryptoPP_VER}" PARENT_SCOPE)
    else()
        message(FATAL_ERROR "Include file ${version_hdr} does not exist or does not contain expected version information")
    endif()
endfunction()

# Find include directory
find_path(CryptoPP_INCLUDE_DIR
          NAMES "config.h"
          DOC "Crypto++ include directory")
mark_as_advanced(CryptoPP_INCLUDE_DIR)

# Find all XercesC libraries
find_library(CryptoPP_LIBRARY "cryptlib"
  DOC "Crypto++ libraries")
mark_as_advanced(CryptoPP_LIBRARY)

if(CryptoPP_INCLUDE_DIR)
  _CryptoPP_GET_VERSION("${CryptoPP_INCLUDE_DIR}/config.h")
endif()

#include(${CMAKE_CURRENT_LIST_DIR}/FindPackageHandleStandardArgs.cmake)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(CryptoPP
                                  FOUND_VAR CryptoPP_FOUND
                                  REQUIRED_VARS CryptoPP_LIBRARY
                                                CryptoPP_INCLUDE_DIR
                                                CryptoPP_VERSION
                                  VERSION_VAR CryptoPP_VERSION
                                  FAIL_MESSAGE "Failed to find Crypto++")

if(CryptoPP_FOUND)
  set(CryptoPP_INCLUDE_DIRS "${CryptoPP_INCLUDE_DIR}")
  set(CryptoPP_LIBRARIES "${CryptoPP_LIBRARY}")
endif()
