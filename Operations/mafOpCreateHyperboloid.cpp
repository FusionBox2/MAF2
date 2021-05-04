/*=========================================================================

 Program: MAF2
 Module: mafOpCreateHyperboloid
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


#include "mafOpCreateHyperboloid.h"
#include "mafDecl.h"
#include "mafEvent.h"

#include "mafVMEHyperboloid.h"


//----------------------------------------------------------------------------
// Constants :
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
mafCxxTypeMacro(mafOpCreateHyperboloid);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
mafOpCreateHyperboloid::mafOpCreateHyperboloid(const mafString& label) : Superclass(label)
//----------------------------------------------------------------------------
{
  m_OpType	= OPTYPE_OP;
  m_Canundo = true;
  m_Hyperboloid = NULL;
}
//----------------------------------------------------------------------------
mafOpCreateHyperboloid::~mafOpCreateHyperboloid()
//----------------------------------------------------------------------------
{
	mafDEL(m_Hyperboloid);
}
//----------------------------------------------------------------------------
mafOp* mafOpCreateHyperboloid::Copy()
//----------------------------------------------------------------------------
{
	return new mafOpCreateHyperboloid(GetLabel());
}
//----------------------------------------------------------------------------
bool mafOpCreateHyperboloid::Accept(mafNode *node)
//----------------------------------------------------------------------------
{
	
  return (node && node->IsMAFType(mafVME));


}
//----------------------------------------------------------------------------
void mafOpCreateHyperboloid::OpRun()
//----------------------------------------------------------------------------
{
	
	mafNEW(m_Hyperboloid);
	
	m_Hyperboloid->SetName(_R("Hyperboloid"));
	m_Output = m_Hyperboloid;
  mafEventMacro(mafEvent(this,OP_RUN_OK));
  
}
//----------------------------------------------------------------------------
void mafOpCreateHyperboloid::OpDo()
//----------------------------------------------------------------------------
{
	
	

	
	m_Hyperboloid->ReparentTo(m_Input);
	

	
}
