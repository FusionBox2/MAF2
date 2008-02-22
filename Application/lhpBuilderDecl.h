/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: lhpBuilderDecl.h,v $
Language:  C++
Date:      $Date: 2008-02-22 10:40:51 $
Version:   $Revision: 1.2 $
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
};

#endif