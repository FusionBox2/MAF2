/*=========================================================================

 Program: MAF2
 Module: lhpOpCreateRefSysLM
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


#include "lhpOpCreateRefSysLM.h"
#include "mafDecl.h"
#include "mafEvent.h"

#include "mafVMERefSysAbstract.h"
#include "mafVMELandmarkCloud.h"

//----------------------------------------------------------------------------
mafCxxTypeMacro(lhpOpCreateRefSysLM);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
lhpOpCreateRefSysLM::lhpOpCreateRefSysLM(const mafString& label) : Superclass(label)
//----------------------------------------------------------------------------
{
  m_OpType	= OPTYPE_OP;
  m_Canundo = true;
  m_Cloud   = NULL;
}
//----------------------------------------------------------------------------
lhpOpCreateRefSysLM::~lhpOpCreateRefSysLM()
//----------------------------------------------------------------------------
{
  mafDEL(m_Cloud);
}
//----------------------------------------------------------------------------
mafOp* lhpOpCreateRefSysLM::Copy()   
//----------------------------------------------------------------------------
{
	return new lhpOpCreateRefSysLM(GetLabel());
}
//----------------------------------------------------------------------------
bool lhpOpCreateRefSysLM::Accept(mafNode *node)
//----------------------------------------------------------------------------
{
  return (node && node->IsMAFType(mafVMERefSysAbstract));
}
//----------------------------------------------------------------------------
void lhpOpCreateRefSysLM::OpRun()
//----------------------------------------------------------------------------
{
  mafNEW(m_Cloud);
  m_Cloud->SetName(_R("points"));
  m_Cloud->SetRadius(5.0);
  m_Cloud->AppendLandmark(_R("O"));
  m_Cloud->SetLandmark(_R("O"), 0.0, 0.0, 0.0);
  m_Cloud->AppendLandmark(_R("X"));
  m_Cloud->SetLandmark(_R("X"), 20.0, 0.0, 0.0);
  m_Cloud->AppendLandmark(_R("Y"));
  m_Cloud->SetLandmark(_R("Y"), 0.0, 20.0, 0.0);
  m_Cloud->AppendLandmark(_R("Z"));
  m_Cloud->SetLandmark(_R("Z"), 0.5, 0.0, 20.0);
  m_Output = m_Cloud;
  mafEventMacro(mafEvent(this,OP_RUN_OK));
}
//----------------------------------------------------------------------------
void lhpOpCreateRefSysLM::OpDo()
//----------------------------------------------------------------------------
{
  m_Cloud->ReparentTo(m_Input);
}
