/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: lhpPipeIntGraphLocal.cpp,v $
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

#include "lhpPipeIntGraphLocal.h"
#include "mafDecl.h"
#include "mafJointAnalysis.h"

#include "mafMatrix3x3.h"
#include "mafVME.h"

//----------------------------------------------------------------------------
// constants
//----------------------------------------------------------------------------
namespace
{
  const char *saVarDesc[]  = {"Local Pos X", "Local Pos Y", "Local Pos Z", "Local Ori X", "Local Ori Y", "Local Ori Z"};

  const char *saVarUnits[] = {"mm" ,"mm" ,"mm", "deg","deg","deg"};

  double saCoefs[] = {1.0, 1.0, 1.0, mafMatrix3x3::DegreesToRadians(), mafMatrix3x3::DegreesToRadians(), mafMatrix3x3::DegreesToRadians()};
}


mafCxxTypeMacro(lhpPipeIntGraphLocal);


const char *lhpPipeIntGraphLocal::GetVarTitle(int i) const
{
  if(i < Superclass::GDT_LAST)
    return Superclass::GetVarTitle(i);
  return saVarDesc[i - Superclass::GDT_LAST];
}

const char *lhpPipeIntGraphLocal::GetVarUnit(int i)const
{
  if(i < Superclass::GDT_LAST)
    return Superclass::GetVarUnit(i);
  return saVarUnits[i - Superclass::GDT_LAST];
}

double lhpPipeIntGraphLocal::GetVarDerivativeCoef(int i)const
{
  if(i < Superclass::GDT_LAST)
    return Superclass::GetVarDerivativeCoef(i);
  return saCoefs[i - Superclass::GDT_LAST];
}


//----------------------------------------------------------------------------
lhpPipeIntGraphLocal::lhpPipeIntGraphLocal()
//----------------------------------------------------------------------------
{
  m_vars.resize(GDT_LAST);
  InvalidateAllVars();
  m_Proximal = NULL;
  m_ProximalName = "";
}
void lhpPipeIntGraphLocal::Create(mafNode *node, mafView *view)
{
  Superclass::Create(node, view);
  SetProximal(AutoSelectProximal(mafVME::SafeDownCast(node)));
}

//----------------------------------------------------------------------------
lhpPipeIntGraphLocal::~lhpPipeIntGraphLocal()
//----------------------------------------------------------------------------
{
  SetProximal(NULL);
}
void lhpPipeIntGraphLocal::SetProximal(mafVME *proximal)
{
  if(m_Proximal)
    m_Proximal->RemoveObserver(this);
  m_Proximal = proximal;
  m_ProximalName = (m_Proximal) ? m_Proximal->GetName() : "";
  if(m_Proximal)
    m_Proximal->AddObserver(this);
  if(m_Gui)
    m_Gui->Update();
}
//----------------------------------------------------------------------------
void lhpPipeIntGraphLocal::OnEvent(mafEventBase *maf_event)
//----------------------------------------------------------------------------
{
  if(mafEvent *e = mafEvent::SafeDownCast(maf_event))
  {
    if(ID_PARENT == e->GetId())
    {
      mafString s(_("Choose cloud"));
      mafEvent e(this,VME_CHOOSE, &s, NULL/*, (long)&lhpOpRegisterLMScripted::ClosedCloudAccept*/);
      mafEventMacro(e);
      mafVME *vme = mafVME::SafeDownCast(e.GetVme());
      SetProximal(vme);
      return;
    }
    if(ID_RESETPARENT == e->GetId())
    {
      SetProximal(AutoSelectProximal(mafVME::SafeDownCast(m_Node)));
      return;
    }
  }
  if(maf_event->GetSender() == m_Proximal)
  {
    if(NODE_DESTROYED == maf_event->GetId() || NODE_DETACHED_FROM_TREE == maf_event->GetId())
      m_Proximal = NULL;
  }
  Superclass::OnEvent(maf_event);
}
/** Create the Gui for the visual pipe that allow the user to change the pipe's parameters.*/
//----------------------------------------------------------------------------
mafGUI *lhpPipeIntGraphLocal::CreateGui()
//----------------------------------------------------------------------------
{
  Superclass::CreateGui();
  m_Gui->Label(_("Parent :"),true);
  m_Gui->Label(&m_ProximalName);
  m_Gui->Button(ID_PARENT,_("parent "));
  m_Gui->Button(ID_RESETPARENT,_("reset parent"));
  return m_Gui;
}

//----------------------------------------------------------------------------
bool lhpPipeIntGraphLocal::StoreValueByIdx(int nVarID, mafTimeStamp ts, mafTimeStamp prevts)
//----------------------------------------------------------------------------
{
  if(m_Proximal == NULL)
    SetProximal(AutoSelectProximal(mafVME::SafeDownCast(m_Node)));
  if(nVarID >= GDT_LAST)
  {
    wxASSERT(false);
    return false;
  }

  if(nVarID < Superclass::GDT_LAST)
  {
    return Superclass::StoreValueByIdx(nVarID, ts, prevts);
  }

  //LTM
  /*if(GDT_LTM_POSX <= nVarID && nVarID <= GDT_LTM_ROTZ)
  {
    mafMatrix mLTM;
    V4d<double>    vPos, vRot;
    GetLocalMatrix(m_Vme, ts, mLTM, m_Proximal);
    mafTransfInverseTransformUpright(&mLTM, &vPos, &vRot);

    SetValue(GDT_LTM_POSX, vPos.x);
    SetValue(GDT_LTM_POSY, vPos.y);
    SetValue(GDT_LTM_POSZ, vPos.z);
    SetValue(GDT_LTM_ROTX, vRot.x * mafMatrix3x3::RadiansToDegrees());
    SetValue(GDT_LTM_ROTY, vRot.y * mafMatrix3x3::RadiansToDegrees());
    SetValue(GDT_LTM_ROTZ, vRot.z * mafMatrix3x3::RadiansToDegrees());
  }*/
  return true;
}

