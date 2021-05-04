/*=========================================================================

 Program: MAF2
 Module: mafOpCreateEllipsoid
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


#include "mafOpCreateEllipsoid.h"
#include "mafDecl.h"
#include "mafEvent.h"

#include "mafVMEEllipsoid.h"


//----------------------------------------------------------------------------
// Constants :
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
mafCxxTypeMacro(mafOpCreateEllipsoid);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
mafOpCreateEllipsoid::mafOpCreateEllipsoid(const mafString& label) : Superclass(label)
//----------------------------------------------------------------------------
{
  m_OpType	= OPTYPE_OP;
  m_Canundo = true;
  m_Ellipsoid = NULL;
}
//----------------------------------------------------------------------------
mafOpCreateEllipsoid::~mafOpCreateEllipsoid()
//----------------------------------------------------------------------------
{
	mafDEL(m_Ellipsoid);
}
//----------------------------------------------------------------------------
mafOp* mafOpCreateEllipsoid::Copy()
//----------------------------------------------------------------------------
{
	return new mafOpCreateEllipsoid(GetLabel());
}
//----------------------------------------------------------------------------
bool mafOpCreateEllipsoid::Accept(mafNode *node)
//----------------------------------------------------------------------------
{
	
  return (node && node->IsMAFType(mafVME));


}
//----------------------------------------------------------------------------
void mafOpCreateEllipsoid::OpRun()
//----------------------------------------------------------------------------
{
	
	mafNEW(m_Ellipsoid);
	
	m_Ellipsoid->SetName(_R("Ellipsoid"));
	m_Output = m_Ellipsoid;
  mafEventMacro(mafEvent(this,OP_RUN_OK));
  
}
//----------------------------------------------------------------------------
void mafOpCreateEllipsoid::OpDo()
//----------------------------------------------------------------------------
{
	
	

	
	m_Ellipsoid->ReparentTo(m_Input);
	

	
}
