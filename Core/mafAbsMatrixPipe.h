/*=========================================================================

 Program: MAF2
 Module: mafAbsMatrixPipe
 Authors: Marco Petrone
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#ifndef __mafAbsMatrixPipe_h
#define __mafAbsMatrixPipe_h
//----------------------------------------------------------------------------
// includes :
//----------------------------------------------------------------------------
#include "mafMatrixPipe.h"
#include "mafMatrix.h"
//------------------------------------------------------------------------------
// Forward declarations
//------------------------------------------------------------------------------
class mafTransformFrame;

/** mafAbsMatrixPipe - this class implements VME tree direct cinematic (AbsMatrix).
  mafAbsMatrixPipe is a MatrixPipe implementing the VME tree direct cinematic of 
  absolute matrices. It supports efficient on demand update of the output matrix for the
  current time being coincident with the VME's one. Also it supports computing of the abs-matrix
  for arbitrary time with a slightly less efficient mechanism, by temporary switching current time of 
  the pointed VME and its parent to a diffent time. In the later case (i.e. when this class CurrentTime
  differs from VME current time) no MatriUpdateEvent is rised! 
 
  @sa mafMatrixPipe mafVME mafMatrixVector

  @todo
  - check if it's possible to remove the m_Updating flag!
*/ 
class MAF_EXPORT mafAbsMatrixPipe: public mafMatrixPipe
{
public:
  mafAbsMatrixPipe();
  virtual ~mafAbsMatrixPipe();
  
  mafTypeMacro(mafAbsMatrixPipe,mafMatrixPipe);

  /** Set the VME to be used as input for this PIPE */
  virtual int SetVME(mafVME *vme);

  /** 
    Overridden to take into consideration the internal transform
    object MTime. */
  virtual unsigned long GetMTime();
  
protected:
  
  virtual void InternalUpdate();

  mafTransformFrame *m_Transform; ///< internal transform used to compute the local to ABS frame transformation

private:
  mafAbsMatrixPipe(const mafAbsMatrixPipe&); // Not implemented
  void operator=(const mafAbsMatrixPipe&); // Not implemented
  
};

#endif /* __mafAbsMatrixPipe_h */
 
