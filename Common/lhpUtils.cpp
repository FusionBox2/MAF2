/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: lhpUtils.cpp,v $
Language:  C++
Date:      $Date: 2009-04-10 19:12:06 $
Version:   $Revision: 1.1.2.2 $
Authors:   Stefano Perticoni
==========================================================================
Copyright (c) 2001/2005 
CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/

#include "mafDecl.h"

#include "lhpUtils.h"
#include "mafDefines.h"

mafString lhpUtils::lhpGetApplicationDirectory()
{
#ifndef _DEBUG
             // RELEASE 
             return mafGetApplicationDirectory();

#else
             // DEBUG
             mafString sourceDir = _R(LHP_SOURCE_DIR);

             for (unsigned int i=0;i<sourceDir.Length();i++)
             {
               if (sourceDir[i]=='/')
                 sourceDir[i]='\\';
             }

             return sourceDir;

#endif
}