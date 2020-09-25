/*=========================================================================

 Program: MAF2
 Module: mafOpCreateCylinder
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


#include "mafOpCreateCylinder.h"
#include "mafDecl.h"
#include "mafEvent.h"

#include "mafVMECylinder.h"


//----------------------------------------------------------------------------
// Constants :
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
mafCxxTypeMacro(mafOpCreateCylinder);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
mafOpCreateCylinder::mafOpCreateCylinder(const mafString& label) : Superclass(label)
//----------------------------------------------------------------------------
{
  m_OpType	= OPTYPE_OP;
  m_Canundo = true;
  m_Cylinder = NULL;
}
//----------------------------------------------------------------------------
mafOpCreateCylinder::~mafOpCreateCylinder()
//----------------------------------------------------------------------------
{
	mafDEL(m_Cylinder);
}
//----------------------------------------------------------------------------
mafOp* mafOpCreateCylinder::Copy()
//----------------------------------------------------------------------------
{
	return new mafOpCreateCylinder(GetLabel());
}
//----------------------------------------------------------------------------
bool mafOpCreateCylinder::Accept(mafNode *node)
//----------------------------------------------------------------------------
{
	
  return (node && node->IsMAFType(mafVME));


}
//----------------------------------------------------------------------------
void mafOpCreateCylinder::OpRun()
//----------------------------------------------------------------------------
{
	
	mafNEW(m_Cylinder);
	
	m_Cylinder->SetName("Cylinder");
	m_Output = m_Cylinder;
  mafEventMacro(mafEvent(this,OP_RUN_OK));
  
}
//----------------------------------------------------------------------------
void mafOpCreateCylinder::OpDo()
//----------------------------------------------------------------------------
{
	
	

	
	m_Cylinder->ReparentTo(m_Input);
	

	
}
