/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafPipe.h,v $
  Language:  C++
  Date:      $Date: 2010-11-10 16:52:01 $
  Version:   $Revision: 1.18.2.3 $
  Authors:   Silvano Imboden
==========================================================================
  Copyright (c) 2002/2004
  CINECA - Interuniversity Consortium (www.cineca.it) 
=========================================================================*/
#ifndef __mafPipeVTK_H__
#define __mafPipeVTK_H__

//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "mafDecl.h" // for MINID
#include "mafPipe.h"
#include "mafVME.h"
#include "vtkMAFAssembly.h"
#include "vtkRenderer.h"
//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// mafPipe :
//----------------------------------------------------------------------------
/*
  mafPipe is the base class for all visual pipes; each pipe represents how a vme can be
  visualized inside the view, so logically a pipe has as input a vme and in output
  creates actors that will be rendered in a render view.
  It can handle a GUI, which events can be catched by OnEvent.
*/
class MAF_EXPORT mafPipeVTK : public mafPipe
{
public:
  mafTypeMacro(mafPipeVTK,mafPipe);
  
  mafPipeVTK();
  virtual ~mafPipeVTK();

protected:
};
#endif
