/*=========================================================================

 Program: MAF2
 Module: mafOpExplodeCollapse
 Authors: Paolo Quadrani
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __mafOpExplodeCollapse_H__
#define __mafOpExplodeCollapse_H__

//----------------------------------------------------------------------------
// Include :
//----------------------------------------------------------------------------
#include "mafOp.h"

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class mafNode;

//----------------------------------------------------------------------------
// mafOpExplodeCollapse :
//----------------------------------------------------------------------------
/** */
class MAF_EXPORT mafOpExplodeCollapse: public mafOp
{
public:
  mafOpExplodeCollapse(const mafString& label = _R("ExplodeCollapse"));
 ~mafOpExplodeCollapse() override; 
  
  mafTypeMacro(mafOpExplodeCollapse, mafOp);

  mafOp* Copy() override;

	/** Return true for the acceptable vme type. */
  bool Accept(mafNode *node) override;

	/** Builds operation's interface. */
  void OpRun() override;

	/** Execute the operation. */
  void OpDo() override;

	/** Makes the undo for the operation. */
  void OpUndo() override;
};
#endif
