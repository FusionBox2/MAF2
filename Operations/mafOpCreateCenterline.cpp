/*=========================================================================

 Program: MAF2
 Module: mafOpCreateCenterLine
 Authors: Taha Jerbi
 
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


#include "mafOpCreateCenterline.h"
#include "mafDecl.h"
#include "mafEvent.h"

#include "mafVMECenterLine.h"

//----------------------------------------------------------------------------
// Constants :
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
mafCxxTypeMacro(mafOpCreateCenterLine);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
mafOpCreateCenterLine::mafOpCreateCenterLine(const mafString& label) : Superclass(label)
//----------------------------------------------------------------------------
{
  m_OpType	= OPTYPE_OP;
  m_Canundo = true;
  m_Meter   = NULL;
}
//----------------------------------------------------------------------------
mafOpCreateCenterLine::~mafOpCreateCenterLine()
//----------------------------------------------------------------------------
{
  mafDEL(m_Meter);
}
//----------------------------------------------------------------------------
mafOp* mafOpCreateCenterLine::Copy()
//----------------------------------------------------------------------------
{
	return new mafOpCreateCenterLine(GetLabel());
}
//----------------------------------------------------------------------------
bool mafOpCreateCenterLine::Accept(mafNode *node)
//----------------------------------------------------------------------------
{

  return (node && node->IsMAFType(mafVME));
}
//----------------------------------------------------------------------------
void mafOpCreateCenterLine::OpRun()
//----------------------------------------------------------------------------
{
  mafNEW(m_Meter);
  m_Meter->SetName("center_line");
  m_Output = m_Meter;
  mafEventMacro(mafEvent(this,OP_RUN_OK));
}
//----------------------------------------------------------------------------
void mafOpCreateCenterLine::OpDo()
//----------------------------------------------------------------------------
{
  m_Meter->ReparentTo(m_Input);
}
