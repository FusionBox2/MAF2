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

#include "mafDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the MAF must include "mafDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------

#include "mafOpExplodeCollapse.h"
#include "mafDecl.h"

#include "mafVMELandmarkCloud.h" 
#include "mafVMELandmark.h"

//----------------------------------------------------------------------------
mafCxxTypeMacro(mafOpExplodeCollapse);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
mafOpExplodeCollapse::mafOpExplodeCollapse(const mafString& label) : Superclass(label)
//----------------------------------------------------------------------------
{
  m_OpType = OPTYPE_OP;
  m_Canundo = true;
}
//----------------------------------------------------------------------------
mafOpExplodeCollapse::~mafOpExplodeCollapse()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
bool mafOpExplodeCollapse::Accept(mafNode *node)
//----------------------------------------------------------------------------
{
  return (node && node->IsMAFType(mafVMELandmarkCloud));
}
//----------------------------------------------------------------------------
mafOp* mafOpExplodeCollapse::Copy()   
//----------------------------------------------------------------------------
{
  mafOpExplodeCollapse *cp = new mafOpExplodeCollapse(GetLabel());
  return cp;
}
//----------------------------------------------------------------------------
void mafOpExplodeCollapse::OpRun()   
//----------------------------------------------------------------------------
{
  {mafEvent evUnq(this,OP_RUN_OK); mafEventMacro(evUnq);} 
}

//----------------------------------------------------------------------------
void mafOpExplodeCollapse::OpDo()
//----------------------------------------------------------------------------
{
  mafVMELandmarkCloud *cloud = mafVMELandmarkCloud::SafeDownCast(m_Input);

  if (cloud->IsOpen())
  {
	  cloud->Close(); 
	}
  else
  {
    cloud->Open();  
  }
	{mafEvent evUnq(this,VME_MODIFIED,cloud); mafEventMacro(evUnq);} //update the icon in the tree
	{mafEvent evUnq(this,CAMERA_UPDATE); mafEventMacro(evUnq);}
}
//----------------------------------------------------------------------------
void mafOpExplodeCollapse::OpUndo()
//----------------------------------------------------------------------------
{
  OpDo();
}
