/*=========================================================================

 Program: MAF2
 Module: mafOpCreateGroup
 Authors: Paolo Quadrani
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#include "mafDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the MAF must include "mafDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------


#include "mafOpCreateGroup.h"
#include "mafDecl.h"
#include "mafEvent.h"

#include "mafVMERoot.h"
#include "mafVMEGroup.h"

//----------------------------------------------------------------------------
mafCxxTypeMacro(mafOpCreateGroup);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
mafOpCreateGroup::mafOpCreateGroup(const mafString& label) : Superclass(label)
//----------------------------------------------------------------------------
{
  m_OpType	= OPTYPE_OP;
  m_Canundo = true;
}
//----------------------------------------------------------------------------
mafOpCreateGroup::~mafOpCreateGroup()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
mafOp* mafOpCreateGroup::Copy()   
//----------------------------------------------------------------------------
{
	return new mafOpCreateGroup(GetLabel());
}
//----------------------------------------------------------------------------
bool mafOpCreateGroup::Accept(mafNode *node)
//----------------------------------------------------------------------------
{
  return (node && node->IsMAFType(mafVME));
}
//----------------------------------------------------------------------------
void mafOpCreateGroup::OpRun()
//----------------------------------------------------------------------------
{
  m_Group = mafVMEGroup::NewSPtr();
  m_Group->SetName(_R("group"));
  SetOutput(m_Group);
  {mafEvent evUnq(this,OP_RUN_OK); InvokeEvent(evUnq);}
}
