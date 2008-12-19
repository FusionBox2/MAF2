/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: lhpBuilderDecl.h,v $
Language:  C++
Date:      $Date: 2008-12-19 17:16:27 $
Version:   $Revision: 1.3.2.2 $
Authors:   Daniele Giunchi
==========================================================================
Copyright (c) 2001/2005 
CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/
#ifndef __lhpBuilderDecl_H__
#define __lhpBuilderDecl_H__

#include "mafDefines.h"
#include "mafDecl.h"


enum LHP_MAIN_EVENT_ID
{
	ID_MSF_DATA_CACHE = EVT_USER_START,
  ID_REQUEST_PROXY,
  ID_REQUEST_USER,
  ID_REQUEST_APPLICATION_NAME,
  ID_REQUEST_PYTHON_EXE_INTERPRETER,
  ID_REQUEST_PYTHONW_EXE_INTERPRETER,
};

#endif