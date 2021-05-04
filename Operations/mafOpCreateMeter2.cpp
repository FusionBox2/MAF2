/*=========================================================================

 Program: MAF2
 Module: mafOpCreateMeter
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


#include "mafOpCreateMeter2.h"
#include "mafDecl.h"
#include "mafEvent.h"
#include "mafVMEMuscleWrapper.h"
#include "mafVMEMeter.h"

//----------------------------------------------------------------------------
// Constants :
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
mafCxxTypeMacro(mafOpCreateMeter2);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
mafOpCreateMeter2::mafOpCreateMeter2(const mafString& label) : Superclass(label)
//----------------------------------------------------------------------------
{
  m_OpType	= OPTYPE_OP;
  m_Canundo = true;
  m_Meter   = NULL;
}
//----------------------------------------------------------------------------
mafOpCreateMeter2::~mafOpCreateMeter2( ) 
//----------------------------------------------------------------------------
{
  mafDEL(m_Meter);
}
//----------------------------------------------------------------------------
mafOp* mafOpCreateMeter2::Copy()   
//----------------------------------------------------------------------------
{
	return new mafOpCreateMeter2(GetLabel());
}
//----------------------------------------------------------------------------
bool mafOpCreateMeter2::Accept(mafNode *node)
//----------------------------------------------------------------------------
{
  return (node && node->IsMAFType(mafVME));
}
//----------------------------------------------------------------------------
void mafOpCreateMeter2::OpRun()   
//----------------------------------------------------------------------------
{
  mafNEW(m_Meter);
  m_Meter->SetName(_R("meter"));
  m_Output = m_Meter;
  mafEventMacro(mafEvent(this,OP_RUN_OK));
}
//----------------------------------------------------------------------------
void mafOpCreateMeter2::OpDo()
//----------------------------------------------------------------------------
{
  m_Meter->ReparentTo(m_Input);
}
