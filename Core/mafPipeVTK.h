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
#include "vtkMAFAssembly.h"
#include "mafVME.h"
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

  /** The real setup must be performed here - not in the ctor */
  virtual void Create(mafNode *node, mafView *view);

  /** Get assembly front/back */
  virtual vtkMAFAssembly *GetAssemblyFront(){return m_AssemblyFront;};
  virtual vtkMAFAssembly *GetAssemblyBack(){return m_AssemblyBack;};

  mafVME         *m_Vme;//it is for compatibility purposes, will be removed later;
	vtkMAFAssembly *m_AssemblyFront; ///< Assembly used to contain the actor in the front layer
	vtkMAFAssembly *m_AssemblyBack;  ///< Assembly used to contain the actor in the background layer
	vtkMAFAssembly *m_AlwaysVisibleAssembly;
  bool            m_Selected; ///< Flag used to say if the rendered VME is selected.
  vtkRenderer    *m_RenFront; ///< Renderer associated to the front layer
  vtkRenderer    *m_RenBack;  ///< Renderer associated to the background layer
  vtkRenderer		 *m_AlwaysVisibleRenderer; /// < Renderer used to superimpose utility stuff to main render window

protected:
};
#endif
