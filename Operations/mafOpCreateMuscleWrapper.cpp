/*=========================================================================

Program: MAF2
Module: mafOpCreateMuscleWrapper
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


#include "mafOpCreateMuscleWrapper.h"
#include "mafDecl.h"
#include "mafEvent.h"

#include "mafVMEMuscleWrapper.h"

//----------------------------------------------------------------------------
// Constants :
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
mafCxxTypeMacro(mafOpCreateMuscleWrapperAQ);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
mafOpCreateMuscleWrapperAQ::mafOpCreateMuscleWrapperAQ(const mafString& label) : Superclass(label)
//----------------------------------------------------------------------------
{
	m_OpType = OPTYPE_OP;
	m_Canundo = true;
	m_MuscleWrapper = NULL;
}
//----------------------------------------------------------------------------
mafOpCreateMuscleWrapperAQ::~mafOpCreateMuscleWrapperAQ()
//----------------------------------------------------------------------------
{
	mafDEL(m_MuscleWrapper);
}
//----------------------------------------------------------------------------
mafOp* mafOpCreateMuscleWrapperAQ::Copy()
//----------------------------------------------------------------------------
{
	return new mafOpCreateMuscleWrapperAQ(GetLabel());
}
//----------------------------------------------------------------------------
bool mafOpCreateMuscleWrapperAQ::Accept(mafNode *node)
//----------------------------------------------------------------------------
{
	
	return (node && node->IsMAFType(mafVME));
}
//----------------------------------------------------------------------------
void mafOpCreateMuscleWrapperAQ::OpRun()
//----------------------------------------------------------------------------
{
	mafNEW(m_MuscleWrapper);
	m_MuscleWrapper->SetName(_R("Muscle_Wrapper"));
	m_Output = m_MuscleWrapper;
	mafEventMacro(mafEvent(this, OP_RUN_OK));
}
//----------------------------------------------------------------------------
void mafOpCreateMuscleWrapperAQ::OpDo()
//----------------------------------------------------------------------------
{
	m_MuscleWrapper->ReparentTo(m_Input);
}