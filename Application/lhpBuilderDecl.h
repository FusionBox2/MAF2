/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: lhpBuilderDecl.h,v $
Language:  C++
Date:      $Date: 2009-04-01 10:47:17 $
Version:   $Revision: 1.3.2.5 $
Authors:   Daniele Giunchi , Stefano Perticoni
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

  /** In DEBUG mode return LHPBuilder source code directory in WIN32 path format. 
  In RELEASE mode it returns the Application installation directory in WIN32 path format (same as mafGetApplicationDirectory)*/

  static wxString lhpGetApplicationDirectory();

};
#endif