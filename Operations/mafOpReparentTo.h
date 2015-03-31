/*=========================================================================

 Program: MAF2
 Module: mafOpReparentTo
 Authors: Paolo Quadrani
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __mafOpReparentTo_H__
#define __mafOpReparentTo_H__

//----------------------------------------------------------------------------
// Include :
//----------------------------------------------------------------------------
#include "mafOp.h"

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class mafNode;
class mafVME;
class mafEvent;
class mafGUI;

//----------------------------------------------------------------------------
// mafOpReparentTo :
//----------------------------------------------------------------------------
/** */
class MAF_EXPORT mafOpReparentTo: public mafOp
{
public:
  mafOpReparentTo(const mafString& label = "ReparentTo", bool keepGlobal = true);
 ~mafOpReparentTo(); 
  
  mafTypeMacro(mafOpReparentTo, mafOp);

  mafOp* Copy();

	/** Return true for the acceptable vme type. */
  bool Accept(mafNode *node);

	/** Builds operation's interface. */
  void OpRun();

	/** Execute the operation. */
  void OpDo();

	/** Makes the undo for the operation. */
  void OpUndo();

  /** Set the target vme (used to be called without using interface)*/
  void SetTargetVme(mafVME *target);

protected:
  bool   m_KeepGlobal;
	mafVME *m_TargetVme;
  mafVME *m_OldParent;
};
#endif
