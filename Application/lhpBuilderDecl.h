/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: lhpBuilderDecl.h,v $
Language:  C++
Date:      $Date: 2009-03-31 16:43:52 $
Version:   $Revision: 1.3.2.4 $
Authors:   Daniele Giunchi
==========================================================================
Copyright (c) 2001/2005 
CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/
#ifndef __lhpBuilderDecl_H__
#define __lhpBuilderDecl_H__

#include "mafDefines.h"
#include "mafDecl.h"

#include "lhpDefines.h"

enum LHP_MAIN_EVENT_ID
{
	ID_MSF_DATA_CACHE = EVT_USER_START,
  ID_REQUEST_USER,
  ID_REQUEST_APPLICATION_NAME,
  ID_REQUEST_PYTHON_EXE_INTERPRETER,
  ID_REQUEST_PYTHONW_EXE_INTERPRETER,
};

class lhpUtils
{
public:

  /** Return LHPBuilder source code directory name full path in WIN32 file format in DEBUG mode 
  while it returns the Application directory name in RELEASE mode (same as mafGetApplicationDirectory)*/
  static wxString lhpGetApplicationDirectory()
  {
    #ifndef _DEBUG
      // RELEASE 
      wxString applicationDirectory = mafGetApplicationDirectory().c_str();
      return applicationDirectory;
    
    #else
      // DEBUG
      wxString sourceDir = LHP_SOURCE_DIR;

      for (unsigned int i=0;i<sourceDir.Length();i++)
      {
        if (sourceDir[i]=='/')
          sourceDir[i]='\\';
      }
      
      return sourceDir;

    #endif
  }

};
#endif