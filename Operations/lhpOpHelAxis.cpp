/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: lhpOpHelAxis.cpp,v $
  Language:  C++
  Date:      $Date: 2008-10-21 15:29:11 $
  Version:   $Revision: 1.4 $
  Authors:   Fedor Moiseev / Vladik Aranov
==========================================================================
  Copyright (c) 2001/2007 
  ULB - Universite Libre de Bruxelles (www.ulb.ac.be)
=========================================================================*/

#include "mafDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the MAF must include "mafDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------

#include "mafDecl.h"
#include "lhpOpHelAxis.h"
#include "mafVMEHelAxis.h"
#include "mafVMELandmarkCloud.h"


//----------------------------------------------------------------------------
// Constants :
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
mafCxxTypeMacro(lhpOpHelAxis);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Forward Refs
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
lhpOpHelAxis::lhpOpHelAxis(const mafString& label) : Superclass(label)
//----------------------------------------------------------------------------
{
  m_OpType            = OPTYPE_OP;
  m_Canundo           = true;
  m_HelicalSys        = NULL;
}

//----------------------------------------------------------------------------
lhpOpHelAxis::~lhpOpHelAxis()
//----------------------------------------------------------------------------
{
  mafDEL(m_HelicalSys);
}

//----------------------------------------------------------------------------
mafOp* lhpOpHelAxis::Copy()   
//----------------------------------------------------------------------------
{
  return new lhpOpHelAxis(GetLabel());
}

//----------------------------------------------------------------------------
bool lhpOpHelAxis::Accept(mafNode* vme)
//----------------------------------------------------------------------------
{
  if(!vme) return false;
  return true;
}


//----------------------------------------------------------------------------
void lhpOpHelAxis::OpRun()   
//----------------------------------------------------------------------------
{
  mafNEW(m_HelicalSys);
  m_HelicalSys->SetName("Helical_axis");
  mafVMELandmarkCloud *lmc;
  mafNEW(lmc);
  lmc->SetName("HA_Points");
  lmc->AppendLandmark(0, 0,    0, "Pivot");
  lmc->AppendLandmark(0, 0,  100, "PntFrt");
  lmc->AppendLandmark(0, 0, -100, "PntBck");
  lmc->ReparentTo(m_HelicalSys);
  mafDEL(lmc);
  mafEventMacro(mafEvent(this,OP_RUN_OK)); 
}


//----------------------------------------------------------------------------
void lhpOpHelAxis::OpDo()
//----------------------------------------------------------------------------
{
  assert(m_HelicalSys);
  m_HelicalSys->ReparentTo(m_Input);
  m_HelicalSys->SetLengthFactor(100.0);
  m_HelicalSys->SetRadiusFactor(100.0);
  m_HelicalSys->Update();
  m_Output = m_HelicalSys;
}

