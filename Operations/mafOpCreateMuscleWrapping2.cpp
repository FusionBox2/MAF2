/*=========================================================================

Program: MAF2
Module: mafOpCreateQuadricSurfaceFitting
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


#include "mafOpCreateMuscleWrapping2.h"
#include "mafDecl.h"
#include "mafEvent.h"

#include "mafVMEMuscleWrapping.h"

//----------------------------------------------------------------------------
// Constants :
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
mafCxxTypeMacro(mafOpCreateMuscleWrapping2);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
mafOpCreateMuscleWrapping2::mafOpCreateMuscleWrapping2(const mafString& label) : Superclass(label)
//----------------------------------------------------------------------------
{
	m_OpType = OPTYPE_OP;
	m_Canundo = true;
	m_QuadricSurfaceFitting = NULL;
}
//----------------------------------------------------------------------------
mafOpCreateMuscleWrapping2::~mafOpCreateMuscleWrapping2()
//----------------------------------------------------------------------------
{
	mafDEL(m_QuadricSurfaceFitting);
}
//----------------------------------------------------------------------------
mafOp* mafOpCreateMuscleWrapping2::Copy()
//----------------------------------------------------------------------------
{
	return new mafOpCreateMuscleWrapping2(GetLabel());
}
//----------------------------------------------------------------------------
bool mafOpCreateMuscleWrapping2::Accept(mafNode *node)
//----------------------------------------------------------------------------
{
	return (node && node->IsMAFType(mafVME));
}
//----------------------------------------------------------------------------
void mafOpCreateMuscleWrapping2::OpRun()
//----------------------------------------------------------------------------
{
	mafNEW(m_QuadricSurfaceFitting);
	m_QuadricSurfaceFitting->SetName("Quadric_Surface_Wrapping");
	m_Output = m_QuadricSurfaceFitting;
	mafEventMacro(mafEvent(this, OP_RUN_OK));
}
//----------------------------------------------------------------------------
void mafOpCreateMuscleWrapping2::OpDo()
//----------------------------------------------------------------------------
{
	m_QuadricSurfaceFitting->ReparentTo(m_Input);
}
