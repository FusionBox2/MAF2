/*=========================================================================

 Program: MAF2
 Module: mafOpCreatePlane
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


#include "mafOpCreatePlane.h"
#include "mafDecl.h"
#include "mafEvent.h"



#include "mafVMEPlane.h"



//----------------------------------------------------------------------------
mafCxxTypeMacro(mafOpCreatePlane);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
mafOpCreatePlane::mafOpCreatePlane(const mafString& label) : Superclass(label)
//----------------------------------------------------------------------------
{
	m_OpType = OPTYPE_OP;
	m_Canundo = true;
	m_Plane = NULL;
}
//----------------------------------------------------------------------------
mafOpCreatePlane::~mafOpCreatePlane()
//----------------------------------------------------------------------------
{
	mafDEL(m_Plane);
}
//----------------------------------------------------------------------------
mafOp* mafOpCreatePlane::Copy()
//----------------------------------------------------------------------------
{
	return new mafOpCreatePlane(GetLabel());
}
//----------------------------------------------------------------------------
bool mafOpCreatePlane::Accept(mafNode *node)
//----------------------------------------------------------------------------
{
  return (node && node->IsMAFType(mafVME));
}
//----------------------------------------------------------------------------

void mafOpCreatePlane::OpRun()
//----------------------------------------------------------------------------
{
	mafNEW(m_Plane);

	m_Plane->SetName("Plane");
	m_Output = m_Plane;
	mafEventMacro(mafEvent(this, OP_RUN_OK));
}
//----------------------------------------------------------------------------





//----------------------------------------------------------------------------
void mafOpCreatePlane::OpDo()
//----------------------------------------------------------------------------
{
	m_Plane->ReparentTo(m_Input);
  
}


