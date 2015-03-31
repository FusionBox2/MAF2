/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: lhpPipeIntGraphAnalog.cpp,v $
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

#include "lhpPipeIntGraphAnalog.h"
#include "mafDecl.h"
#include "mafEventSender.h"
#include "mafViewIntGraph.h"
#include "mafGUICheckListBox.h"

#include "mafVME.h"
#include "medVMEAnalog.h"
#include "mafTagArray.h"
#include "mafTagItem.h"
#include "mafVMEOutputScalarMatrix.h"

//----------------------------------------------------------------------------
// constants
//----------------------------------------------------------------------------
mafCxxTypeMacro(lhpPipeIntGraphAnalog);


const char *lhpPipeIntGraphAnalog::GetVarTitle(int i) const
{
  return m_Names[i];//"";//saVarDesc[i];
}

const char *lhpPipeIntGraphAnalog::GetVarUnit(int i)const
{
  return "";//saVarUnits[i];
}

double lhpPipeIntGraphAnalog::GetVarDerivativeCoef(int i)const
{
  return 1.0;//saCoefs[i];
}


//----------------------------------------------------------------------------
lhpPipeIntGraphAnalog::lhpPipeIntGraphAnalog()
//----------------------------------------------------------------------------
{
  m_Selected     = false;
  m_PrevStamp    = -1;
  m_CheckBoxXval = NULL;
  m_CheckBoxYval = NULL;
  m_CheckBoxYder = NULL;


  m_NumberOfSignals = 0;
  m_TimeStamp = 0;
  m_EmgPlot = NULL;

  m_Limited = 0;
  m_Start   = 0.0;
  m_End     = 0.0;
}
//----------------------------------------------------------------------------
lhpPipeIntGraphAnalog::~lhpPipeIntGraphAnalog()
//----------------------------------------------------------------------------
{
}

//----------------------------------------------------------------------------
void lhpPipeIntGraphAnalog::Create(mafSceneNode *n)
//----------------------------------------------------------------------------
{
  Superclass::Create(n);

  m_EmgPlot = medVMEAnalog::SafeDownCast(m_Vme);
  m_NumberOfSignals = m_EmgPlot->GetScalarOutput()->GetScalarData().rows()-1; //1 row is for time information
  //m_DataMin = m_EmgPlot->GetScalarOutput()->GetScalarData().min_value();
  //m_DataMax = m_EmgPlot->GetScalarOutput()->GetScalarData().max_value();
  m_TimeStamp = m_EmgPlot->GetScalarOutput()->GetScalarData().columns();

  delete m_Graph;
  m_Graph = new mafGraphDataImpl(this, 8888.8888, m_TimeStamp);
  m_Graph->AddXVar(0);


  mafTagItem *tag_Signals = m_Vme->GetTagArray()->GetTag("SIGNALS_NAME");
  bool tagPresent = (tag_Signals != NULL);
  if (!tagPresent)
  {
    mafTagItem tag_Sig;
    tag_Sig.SetName("SIGNALS_NAME");
    tag_Sig.SetNumberOfComponents(m_NumberOfSignals);
    m_Vme->GetTagArray()->SetTag(tag_Sig);
    mafTagItem *tag_Signals = m_Vme->GetTagArray()->GetTag("SIGNALS_NAME");
  }
  mafString name;
  m_Names.push_back("Time");

  for (int n = 0; n < m_NumberOfSignals; n++)
  {
    if (tagPresent)
    {
      name = tag_Signals->GetValue(n);
    }
    else
    {
      name  = "analog_";
      name += wxString::Format("%d", n);
      tag_Signals->SetValue(name, n);
    }
    m_Names.push_back(name);
  }


}
//----------------------------------------------------------------------------
void lhpPipeIntGraphAnalog::Select(bool sel)
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
mafGUI *lhpPipeIntGraphAnalog::CreateGui()
//----------------------------------------------------------------------------
{
  assert(m_Gui == NULL);
  m_Gui = Superclass::CreateGui();
  wxString name;
  bool checked = false;

  m_Gui->Bool(ID_LIMITED, _("Limit"), &m_Limited);
  m_Gui->Double(ID_START,_("min time"),&m_Start);
  m_Gui->Double(ID_END,_("max time"),&m_End);

  m_CheckBoxXval = m_Gui->CheckList(ID_CHECK_BOXXVAL,_("X value"),100,_("Choose value for X axis"));
  m_CheckBoxYval = m_Gui->CheckList(ID_CHECK_BOXYVAL,_("Y values"),100,_("Choose values for Y axis"));
  m_CheckBoxYder = m_Gui->CheckList(ID_CHECK_BOXYDER,_("Y derivs"),100,_("Choose values for Y axis"));

  m_CheckBoxXval->SetCheckMode(MODE_RADIO);
  for (int n = 0; n < m_Names.size(); n++)
  {
    name = GetVarTitle(n);
    m_CheckBoxXval->AddItem(n, name, checked);
    m_CheckBoxYval->AddItem(n, name, checked);
    m_CheckBoxYder->AddItem(n, name, checked);
  }
  m_Gui->Enable(ID_START,m_Limited != 0);
  m_Gui->Enable(ID_END,m_Limited != 0);
  return m_Gui;
}
//----------------------------------------------------------------------------
void lhpPipeIntGraphAnalog::UpdateProperty(bool fromTag)
//----------------------------------------------------------------------------
{
}

//----------------------------------------------------------------------------
void lhpPipeIntGraphAnalog::UpdateGUIChecks()
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
void lhpPipeIntGraphAnalog::OnEvent(mafEventBase *maf_event)
//----------------------------------------------------------------------------
{
  int itemId;
  if (mafEvent *e = mafEvent::SafeDownCast(maf_event))
  {
    switch(e->GetId()) 
    {
    case ID_LIMITED:
      m_Gui->Enable(ID_START,m_Limited != 0);
      m_Gui->Enable(ID_END,m_Limited != 0);
    case ID_START:
    case ID_END:
      m_Graph->Clean();
      GrabData();
      mafEventMacro(mafEvent(this,CAMERA_UPDATE));
      break;
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
      GrabData();
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
        GrabData();
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
        GrabData();
        mafEventMacro(mafEvent(this,CAMERA_UPDATE));
      }
      break;
    default:
      mafEventMacro(*e);
      break;
    }
  }
  else if(maf_event->GetId() == VME_TIME_SET)
  {
    //GrabData();
  }
}

//----------------------------------------------------------------------------
void lhpPipeIntGraphAnalog::StoreValueByIdx(int nVarID, int nGraphIndex, mafTimeStamp ts, mafTimeStamp prevts)
//----------------------------------------------------------------------------
{
}

//----------------------------------------------------------------------------
void lhpPipeIntGraphAnalog::GrabData()
//----------------------------------------------------------------------------
{
  const vnl_matrix<double>& matr = m_EmgPlot->GetScalarOutput()->GetScalarData();
  if(m_Graph->GetYDim() + m_Graph->GetXDim() == 0)
    return;
  for(int i = 0; i < m_TimeStamp; i++)
  {
    double ts = matr.get(0, i);
    if(m_Limited && (m_Start > ts || m_End < ts))
      continue;
    for(unsigned j = 0; j < m_Graph->GetDim(); j++)
    {
      m_Graph->SetAddCoord(j, matr.get(m_Graph->GetID(j), i));
    }
  }
}
