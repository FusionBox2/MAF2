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


#include "mafOpCreateHyperboloid2S.h"
#include "mafDecl.h"
#include "mafEvent.h"

#include "mafVMEHyperboloid2S.h"


//----------------------------------------------------------------------------
// Constants :
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
mafCxxTypeMacro(mafOpCreateHyperboloid2S);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
mafOpCreateHyperboloid2S::mafOpCreateHyperboloid2S(const mafString& label) : Superclass(label)
//----------------------------------------------------------------------------
{
  m_OpType	= OPTYPE_OP;
  m_Canundo = true;
  m_Hyperboloid = NULL;
}
//----------------------------------------------------------------------------
mafOpCreateHyperboloid2S::~mafOpCreateHyperboloid2S()
//----------------------------------------------------------------------------
{
	mafDEL(m_Hyperboloid);
}
//----------------------------------------------------------------------------
mafOp* mafOpCreateHyperboloid2S::Copy()
//----------------------------------------------------------------------------
{
	return new mafOpCreateHyperboloid2S(GetLabel());
}
//----------------------------------------------------------------------------
bool mafOpCreateHyperboloid2S::Accept(mafNode *node)
//----------------------------------------------------------------------------
{
	
  return (node && node->IsMAFType(mafVME));


}
//----------------------------------------------------------------------------
void mafOpCreateHyperboloid2S::OpRun()
//----------------------------------------------------------------------------
{
	
	mafNEW(m_Hyperboloid);
	
	m_Hyperboloid->SetName("Hyperboloid2S");
	m_Output = m_Hyperboloid;
  mafEventMacro(mafEvent(this,OP_RUN_OK));
  
}
//----------------------------------------------------------------------------
void mafOpCreateHyperboloid2S::OpDo()
//----------------------------------------------------------------------------
{
	
	

	
	m_Hyperboloid->ReparentTo(m_Input);
	

	
}
