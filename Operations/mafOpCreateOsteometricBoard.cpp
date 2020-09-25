/*=========================================================================

 Program: MAF2
 Module: mafOpCreateOsteometricBoard
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


#include "mafOpCreateOsteometricBoard.h"
#include "mafDecl.h"
#include "mafEvent.h"



#include "mafVMEOsteometricBoard.h"



//----------------------------------------------------------------------------
mafCxxTypeMacro(mafOpCreateOsteometricBoard);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
mafOpCreateOsteometricBoard::mafOpCreateOsteometricBoard(const mafString& label) : Superclass(label)
//----------------------------------------------------------------------------
{
	m_OpType = OPTYPE_OP;
	m_Canundo = true;
	m_osteometricBoard = NULL;
}
//----------------------------------------------------------------------------
mafOpCreateOsteometricBoard::~mafOpCreateOsteometricBoard()
//----------------------------------------------------------------------------
{
	mafDEL(m_osteometricBoard);
}
//----------------------------------------------------------------------------
mafOp* mafOpCreateOsteometricBoard::Copy()
//----------------------------------------------------------------------------
{
	return new mafOpCreateOsteometricBoard(GetLabel());
}
//----------------------------------------------------------------------------
bool mafOpCreateOsteometricBoard::Accept(mafNode *node)
//----------------------------------------------------------------------------
{
  return (node && node->IsMAFType(mafVME));
}
//----------------------------------------------------------------------------

void mafOpCreateOsteometricBoard::OpRun()
//----------------------------------------------------------------------------
{
	mafNEW(m_osteometricBoard);

	m_osteometricBoard->SetName("OsteometricBoard");
	m_Output = m_osteometricBoard;
	mafEventMacro(mafEvent(this, OP_RUN_OK));
}
//----------------------------------------------------------------------------





//----------------------------------------------------------------------------
void mafOpCreateOsteometricBoard::OpDo()
//----------------------------------------------------------------------------
{
	m_osteometricBoard->ReparentTo(m_Input);
  
}


