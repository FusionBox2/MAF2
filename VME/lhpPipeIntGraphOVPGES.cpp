/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: lhpPipeIntGraphOVPGES.cpp,v $
  Language:  C++
  Date:      $Date: 2008-04-14 12:03:18 $
  Version:   $Revision: 1.6 $
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

#include "lhpPipeIntGraphOVPGES.h"
#include "mafDecl.h"
#include "mafJointAnalysis.h"

#include "mafMatrix3x3.h"
#include "mafVME.h"

//----------------------------------------------------------------------------
// constants
//----------------------------------------------------------------------------
namespace
{
  const mafString saVarDesc[]  = {_R("OVP Pos X"), _R("OVP Pos Y"), _R("OVP Pos Z"), _R("OVP Ori X"), _R("OVP Ori Y"), _R("OVP Ori Z"), _R("GES Pos X"), _R("GES Pos Y"), _R("GES Pos Z"), _R("GES Ori X"), _R("GES Ori Y"), _R("GES Ori Z")};
  const mafString saVarUnits[] = {_R("mm"), _R("mm"), _R("mm"), _R("deg"), _R("deg"), _R("deg"), _R("mm"), _R("mm"), _R("mm"), _R("deg"), _R("deg"), _R("deg")};
  double saCoefs[] = {1.0, 1.0, 1.0, mafMatrix3x3::DegreesToRadians(), mafMatrix3x3::DegreesToRadians(), mafMatrix3x3::DegreesToRadians(), 1.0, 1.0, 1.0, mafMatrix3x3::DegreesToRadians(), mafMatrix3x3::DegreesToRadians(), mafMatrix3x3::DegreesToRadians()};
}


mafCxxTypeMacro(lhpPipeIntGraphOVPGES);


const mafString& lhpPipeIntGraphOVPGES::GetVarTitle(int i) const
{
  if(i < Superclass::GDT_LAST)
    return Superclass::GetVarTitle(i);
  return saVarDesc[i - Superclass::GDT_LAST];
}

const mafString& lhpPipeIntGraphOVPGES::GetVarUnit(int i)const
{
  if(i < Superclass::GDT_LAST)
    return Superclass::GetVarUnit(i);
  return saVarUnits[i - Superclass::GDT_LAST];
}

double lhpPipeIntGraphOVPGES::GetVarDerivativeCoef(int i)const
{
  if(i < Superclass::GDT_LAST)
    return Superclass::GetVarDerivativeCoef(i);
  return saCoefs[i - Superclass::GDT_LAST];
}


//----------------------------------------------------------------------------
lhpPipeIntGraphOVPGES::lhpPipeIntGraphOVPGES()
//----------------------------------------------------------------------------
{
  m_vars.resize(GDT_LAST);
  InvalidateAllVars();
  m_RefStamp = 0.0;
}
//----------------------------------------------------------------------------
lhpPipeIntGraphOVPGES::~lhpPipeIntGraphOVPGES()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
mafGUI *lhpPipeIntGraphOVPGES::CreateGui()
//----------------------------------------------------------------------------
{
  assert(m_Gui == NULL);
  m_Gui = Superclass::CreateGui();
  m_Gui->Double(ID_REF_STAMP, _L("RefStamp"), &m_RefStamp);
  return m_Gui;
}

//----------------------------------------------------------------------------
void lhpPipeIntGraphOVPGES::OnEvent(mafEventBase *maf_event)
//----------------------------------------------------------------------------
{
  if (mafEvent *e = mafEvent::SafeDownCast(maf_event))
  {
    switch(e->GetId()) 
    {
    case ID_REF_STAMP:
      break;
    default:
      Superclass::OnEvent(maf_event);
      break;
    }
  }
  else
  {
    Superclass::OnEvent(maf_event);
  }
}


//----------------------------------------------------------------------------
bool lhpPipeIntGraphOVPGES::StoreValueByIdx(int nVarID, mafTimeStamp ts, mafTimeStamp prevts)
//----------------------------------------------------------------------------
{
  if(nVarID >= GDT_LAST)
  {
    wxASSERT(false);
    return false;
  }

  if(nVarID < Superclass::GDT_LAST)
  {
    return Superclass::StoreValueByIdx(nVarID, ts, prevts);
  }
  //OVP and GES
  if(GDT_OVP_POSX <= nVarID && nVarID <= GDT_GES_ROTZ)
  {
    V4d<double> vOVPRot, vOVPPos;
    V4d<double> vGESRot, vGESPos;
    OVP_GES(mafVME::SafeDownCast(m_Node), ts, m_RefStamp, &vOVPPos, &vOVPRot, &vGESPos, &vGESRot, m_Proximal);
    SetValue(GDT_OVP_ROTX, vOVPRot.x * mafMatrix3x3::RadiansToDegrees());
    SetValue(GDT_OVP_ROTY, vOVPRot.y * mafMatrix3x3::RadiansToDegrees());
    SetValue(GDT_OVP_ROTZ, vOVPRot.z * mafMatrix3x3::RadiansToDegrees());
    SetValue(GDT_OVP_POSX, vOVPPos.x);
    SetValue(GDT_OVP_POSY, vOVPPos.y);
    SetValue(GDT_OVP_POSZ, vOVPPos.z);

    SetValue(GDT_GES_ROTX, vGESRot.x * mafMatrix3x3::RadiansToDegrees());
    SetValue(GDT_GES_ROTY, vGESRot.y * mafMatrix3x3::RadiansToDegrees());
    SetValue(GDT_GES_ROTZ, vGESRot.z * mafMatrix3x3::RadiansToDegrees());
    SetValue(GDT_GES_POSX, vGESPos.x);
    SetValue(GDT_GES_POSY, vGESPos.y);
    SetValue(GDT_GES_POSZ, vGESPos.z);
  }
  return true;
}

