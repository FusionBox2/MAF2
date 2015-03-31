/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: lhpPipeIntGraphFixed.cpp,v $
  Language:  C++
  Date:      $Date: 2008-04-14 12:03:18 $
  Version:   $Revision: 1.6 $
  Authors:   Fedor Moiseev
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

#include "lhpPipeIntGraphFixed.h"
#include "mafDecl.h"
#include "mafEventSender.h"
#include "mafViewIntGraph.h"
#include "mafGUICheckListBox.h"

#include "mafVME.h"

//----------------------------------------------------------------------------
// constants
//----------------------------------------------------------------------------

mafCxxTypeMacro(lhpPipeIntGraphFixed);

const char *lhpPipeIntGraphFixed::GetVarTitle(int i) const
{
  wxASSERT(false);
  return "";
}

const char *lhpPipeIntGraphFixed::GetVarUnit(int i)const
{
  wxASSERT(false);
  return "";
}

double lhpPipeIntGraphFixed::GetVarDerivativeCoef(int i)const
{
  wxASSERT(false);
  return 1.0;
}


//----------------------------------------------------------------------------
lhpPipeIntGraphFixed::lhpPipeIntGraphFixed()
//----------------------------------------------------------------------------
{
  m_vars.resize(GDT_LAST);
  m_Selected     = false;
  m_PrevStamp    = -1;
  m_CheckBoxXval = NULL;
  m_CheckBoxYval = NULL;
  m_CheckBoxYder = NULL;
  m_InGrabData   = 0;
  InvalidateAllVars();
}
//----------------------------------------------------------------------------
lhpPipeIntGraphFixed::~lhpPipeIntGraphFixed()
//----------------------------------------------------------------------------
{
}

//----------------------------------------------------------------------------
void lhpPipeIntGraphFixed::Create(mafSceneNode *n)
//----------------------------------------------------------------------------
{
  Superclass::Create(n);
}
//----------------------------------------------------------------------------
void lhpPipeIntGraphFixed::Select(bool sel)
//----------------------------------------------------------------------------
{
  m_Selected = sel;
  if(m_Selected)
  {
    GetGui();//assure that GUI is created
    UpdateGUIChecks();
  }
}
//----------------------------------------------------------------------------
mafGUI *lhpPipeIntGraphFixed::CreateGui()
//----------------------------------------------------------------------------
{
  assert(m_Gui == NULL);
  m_Gui = Superclass::CreateGui();
  wxString name;
  bool checked = false;

  m_CheckBoxXval = m_Gui->CheckList(ID_CHECK_BOXXVAL,_("X value"),100,_("Choose value for X axis"));
  m_CheckBoxYval = m_Gui->CheckList(ID_CHECK_BOXYVAL,_("Y values"),100,_("Choose values for Y axis"));
  m_CheckBoxYder = m_Gui->CheckList(ID_CHECK_BOXYDER,_("Y derivs"),100,_("Choose values for Y axis"));

  m_CheckBoxXval->SetCheckMode(MODE_RADIO);
  for (int n = 0; n < m_vars.size(); n++)
  {
    name = GetVarTitle(n);
    m_CheckBoxXval->AddItem(n, name, checked);
    m_CheckBoxYval->AddItem(n, name, checked);
    m_CheckBoxYder->AddItem(n, name, checked);
  }
  return m_Gui;
}
//----------------------------------------------------------------------------
void lhpPipeIntGraphFixed::UpdateProperty(bool fromTag)
//----------------------------------------------------------------------------
{
}

//----------------------------------------------------------------------------
void lhpPipeIntGraphFixed::UpdateGUIChecks()
//----------------------------------------------------------------------------
{
  int id;
  for(int i = 0; i < m_CheckBoxXval->GetNumberOfItems(); i++)
    m_CheckBoxXval->CheckItem(i, false);
  for(int i = 0; i < m_CheckBoxYval->GetNumberOfItems(); i++)
    m_CheckBoxYval->CheckItem(i, false);
  for(int i = 0; i < m_CheckBoxYder->GetNumberOfItems(); i++)
    m_CheckBoxYder->CheckItem(i, false);
  for(unsigned int i = 0; i < m_Graph->GetXDim(); i++)
  {
    const mafGraphData *pg = m_View->GetRenderWindow()->GetXParam();
    if(pg == m_Graph)
      m_CheckBoxXval->CheckItem(m_Graph->GetXID(0), true);
    else if(pg == NULL)
      m_CheckBoxXval->CheckItem(0, true);
  }
  for(unsigned int i = 0; i < m_Graph->GetYDim(); i++)
  {
    id = m_Graph->GetYID(i);
    mafGUICheckListBox *lst = (m_Graph->GetYDeriv(i) == 0) ? m_CheckBoxYval : m_CheckBoxYder;
    lst->CheckItem(id, true);
  }
}

//----------------------------------------------------------------------------
void lhpPipeIntGraphFixed::OnEvent(mafEventBase *maf_event)
//----------------------------------------------------------------------------
{
  int itemId;
  if (mafEvent *e = mafEvent::SafeDownCast(maf_event))
  {
    switch(e->GetId()) 
    {
    case ID_CHECK_BOXXVAL:
      {
        itemId = e->GetArg();
        m_Gui->Update();
        if(m_CheckBoxXval->IsItemChecked(itemId))
        {
          m_Graph->SetXVar(0, itemId);
          if(itemId != 0)
            m_View->GetRenderWindow()->SetXParam(m_Graph);
          else 
            m_View->GetRenderWindow()->SetXParam(NULL);
        }
        mafEventMacro(mafEvent(this,CAMERA_UPDATE));
      }
      break;
    case ID_CHECK_BOXYVAL:
      {
        itemId = e->GetArg();
        m_Gui->Update();
        if(m_CheckBoxYval->IsItemChecked(itemId))
        {
          m_Graph->AddYVar(itemId, 0);
        }
        else
        {
          for(unsigned i = 0; i < m_Graph->GetYDim(); i++)
          {
            if(m_Graph->GetYID(i) == itemId && m_Graph->GetYDeriv(i) == 0)
              m_Graph->RemYVar(i);
          }
        }
        mafEventMacro(mafEvent(this,CAMERA_UPDATE));
      }
      break;
    case ID_CHECK_BOXYDER:
      {
        itemId = e->GetArg();
        m_Gui->Update();
        if(m_CheckBoxYder->IsItemChecked(itemId))
        {
          m_Graph->AddYVar(itemId, 1);
        }
        else
        {
          for(unsigned i = 0; i < m_Graph->GetYDim(); i++)
          {
            if(m_Graph->GetYID(i) == itemId && m_Graph->GetYDeriv(i) > 0)
              m_Graph->RemYVar(i);
          }
        }
        mafEventMacro(mafEvent(this,CAMERA_UPDATE));
      }
      break;
    default:
      Superclass::OnEvent(maf_event);
      break;
    }
  }
  else if(maf_event->GetId() == VME_TIME_SET)
  {
    if(!m_InGrabData)
    {
      m_InGrabData = true;
      GrabData();
      m_InGrabData = false;
    }
  }
  else
  {
    Superclass::OnEvent(maf_event);
  }
}

void lhpPipeIntGraphFixed::InvalidateAllVars()
{
  for(unsigned int i = 0; i < m_vars.size(); i++)
    m_vars[i].second = false;
}

//----------------------------------------------------------------------------
bool lhpPipeIntGraphFixed::StoreValueByIdx(int nVarID, mafTimeStamp ts, mafTimeStamp prevts)
//----------------------------------------------------------------------------
{
  wxASSERT(false);
  return false;
}

//----------------------------------------------------------------------------
void lhpPipeIntGraphFixed::GrabData()
//----------------------------------------------------------------------------
{
  if(m_ForcedWholeRange || m_WholeRange)
  {
    m_PrevStamp = -1;
    m_Vme->Update();
    mafTimeStamp ts = m_Vme->GetOutput()->GetTimeStamp();
    std::vector<mafTimeStamp> stamps;
    m_Vme->GetAbsTimeStamps(stamps);
    for(unsigned i = 0; i < stamps.size(); i++)
    {
      //m_Vme->SetTimeStamp(stamps[i]);
      //m_Vme->Update();
      //m_Vme->GetOutput()->Update();
      InvalidateAllVars();
      for(unsigned j = 0; j < m_Graph->GetDim(); j++)
      {
        int ID = m_Graph->GetID(j);
        if(!IsValueStored(ID))
          StoreValueByIdx(ID, stamps[i], (i > 0)? stamps[i - 1] : stamps[i]);
        m_Graph->SetAddCoord(j, GetValue(ID));
      }
    }
    return;
  }

  m_Vme->Update();
  mafTimeStamp ts = m_Vme->GetOutput()->GetTimeStamp();

  if(ts != m_PrevStamp)
  {
    mafTimeStamp prevts = m_PrevStamp;//store previous value as we are going to change it, but we will need it just after changing
    m_PrevStamp = (m_PrevStamp < 0) ? ts : m_PrevStamp;//Use current time stamp as previous if array is empty
    if(fabs(ts - prevts) > 1e-6)//invalidate variables if ts is updated
    {
      InvalidateAllVars();
      if(m_Graph->GetYDim() + m_Graph->GetXDim() == 0)
        return;
      for(unsigned i = 0; i < m_Graph->GetDim(); i++)
      {
        int ID = m_Graph->GetID(i);
        if(!IsValueStored(ID))
          StoreValueByIdx(ID, ts, m_PrevStamp);
        m_Graph->SetAddCoord(i, GetValue(ID));
      }
      m_PrevStamp = ts;
    }
  }
}
