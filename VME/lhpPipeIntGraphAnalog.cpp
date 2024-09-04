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
namespace
{
    const mafString emptyDesc = _R("");
}
mafCxxTypeMacro(lhpPipeIntGraphAnalog);


const mafString& lhpPipeIntGraphAnalog::GetVarTitle(int i) const
{
  return m_Names[i];//"";//saVarDesc[i];
}

const mafString& lhpPipeIntGraphAnalog::GetVarUnit(int i)const
{
  return emptyDesc;//saVarUnits[i];
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
  m_InGrabData   = 0;

}
//----------------------------------------------------------------------------
lhpPipeIntGraphAnalog::~lhpPipeIntGraphAnalog()
//----------------------------------------------------------------------------
{
}

//----------------------------------------------------------------------------
void lhpPipeIntGraphAnalog::Create(mafNode *node, mafView *view)
//----------------------------------------------------------------------------
{
  Superclass::Create(node, view);

  medVMEAnalog *emgPlot = medVMEAnalog::SafeDownCast(m_Node);
  if(emgPlot)
  {
    m_NumberOfSignals = emgPlot->GetScalarOutput()->GetScalarData().rows()-1; //1 row is for time information
    //m_DataMin = emgPlot->GetScalarOutput()->GetScalarData().min_value();
    //m_DataMax = emgPlot->GetScalarOutput()->GetScalarData().max_value();
    m_TimeStamp = emgPlot->GetScalarOutput()->GetScalarData().columns();
  }

  delete m_Graph;
  m_Graph = new mafGraphDataImpl(this, 8888.8888, m_TimeStamp);
  m_Graph->AddXVar(0);


  mafTagItem *tag_Signals = m_Node->GetTagArray()->GetTag(_R("SIGNALS_NAME"));
  bool tagPresent = (tag_Signals != NULL);
  if (!tagPresent)
  {
    mafTagItem tag_Sig;
    tag_Sig.SetName(_R("SIGNALS_NAME"));
    tag_Sig.SetNumberOfComponents(m_NumberOfSignals);
    m_Node->GetTagArray()->SetTag(tag_Sig);
    mafTagItem *tag_Signals = m_Node->GetTagArray()->GetTag(_R("SIGNALS_NAME"));
  }
  mafString name;
  m_Names.push_back(_R("Time"));

  for (int n = 0; n < m_NumberOfSignals; n++)
  {
    if (tagPresent)
    {
      name = tag_Signals->GetValue(n);
    }
    else
    {
      name  = _R("analog_") + mafToString(n);
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
  bool checked = false;

  m_CheckBoxXval = m_Gui->CheckList(ID_CHECK_BOXXVAL,_L("X value"),100,_L("Choose value for X axis"));
  m_CheckBoxYval = m_Gui->CheckList(ID_CHECK_BOXYVAL,_L("Y values"),100,_L("Choose values for Y axis"));
  m_CheckBoxYder = m_Gui->CheckList(ID_CHECK_BOXYDER,_L("Y derivs"),100,_L("Choose values for Y axis"));

  m_CheckBoxXval->SetCheckMode(MODE_RADIO);
  for (int n = 0; n < m_Names.size(); n++)
  {
    wxString name = GetVarTitle(n).toWx();
    m_CheckBoxXval->AddItem(n, name, checked);
    m_CheckBoxYval->AddItem(n, name, checked);
    m_CheckBoxYder->AddItem(n, name, checked);
  }
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
    const mafGraphData *pg = NULL;
    if(mafViewIntGraph *vgraph = mafViewIntGraph::SafeDownCast(m_View))
      pg = vgraph->GetRenderWindow()->GetXParam();
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
std::istream& lhpPipeIntGraphAnalog::operator>>(std::istream& is)
{
  unsigned dim;
  is >> dim;
  for (unsigned i = 0; i < dim; ++i)
  {
    int id;
    unsigned yd;
    is >> id >> yd;
    m_Graph->AddYVar(id, yd);
  }
  if (m_Gui)
    UpdateGUIChecks();
  return is;
}
std::ostream& lhpPipeIntGraphAnalog::operator<<(std::ostream& os) const
{
  os << m_Graph->GetYDim() << " ";
  for (unsigned int i = 0; i < m_Graph->GetYDim(); i++)
  {
    auto id = m_Graph->GetYID(i);
    auto yd = m_Graph->GetYDer(i);
    os << id << " " << yd << " ";
  }
  return os;
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
    case ID_CHECK_BOXXVAL:
      {
        itemId = e->GetArg();
        m_Gui->Update();
        if(m_CheckBoxXval->IsItemChecked(itemId))
        {
          if(mafViewIntGraph *vgraph = mafViewIntGraph::SafeDownCast(m_View))
          {
            m_Graph->SetXVar(0, itemId);
            if(itemId != 0)
              vgraph->GetRenderWindow()->SetXParam(m_Graph);
            else 
              vgraph->GetRenderWindow()->SetXParam(NULL);
          }
        }
        if(m_ForcedWholeRange || m_WholeRange)
          GrabData();
        {mafEvent evUnq(this,CAMERA_UPDATE); mafEventMacro(evUnq);}
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
        if(m_ForcedWholeRange || m_WholeRange)
          GrabData();
        {mafEvent evUnq(this,CAMERA_UPDATE); mafEventMacro(evUnq);}
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
        if(m_ForcedWholeRange || m_WholeRange)
          GrabData();
        {mafEvent evUnq(this,CAMERA_UPDATE); mafEventMacro(evUnq);}
      }
      break;
    default:
      mafEventMacro(*e);
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

//----------------------------------------------------------------------------
void lhpPipeIntGraphAnalog::GrabData()
//----------------------------------------------------------------------------
{
  medVMEAnalog *emgPlot = medVMEAnalog::SafeDownCast(m_Node);
  if(!emgPlot)
    return;
  const vnl_matrix<double>& matr = emgPlot->GetScalarOutput()->GetScalarData();
  if(m_Graph->GetYDim() + m_Graph->GetXDim() == 0)
    return;
  
  if(m_ForcedWholeRange || m_WholeRange)
  {
    m_PrevStamp = -1;
    for(int i = 0; i < m_TimeStamp; i++)
    {
      double ts = matr.get(0, i);
      for(unsigned j = 0; j < m_Graph->GetDim(); j++)
      {
        m_Graph->SetAddCoord(j, matr.get(m_Graph->GetID(j), i));
      }
    }
    return;
  }

  mafVME::SafeDownCast(m_Node)->Update();
  mafTimeStamp ts = mafVME::SafeDownCast(m_Node)->GetOutput()->GetTimeStamp();

  if(ts != m_PrevStamp)
  {
    mafTimeStamp prevts = m_PrevStamp;//store previous value as we are going to change it, but we will need it just after changing
    m_PrevStamp = (m_PrevStamp < 0) ? ts : m_PrevStamp;//Use current time stamp as previous if array is empty
    if(fabs(ts - prevts) > 1e-6)//invalidate variables if ts is updated
    {
      if(m_TimeStamp <= 0)
        return;
      int i = 0;
      for(i = 0; i < m_TimeStamp - 1; i++)
      {
        if(ts < matr.get(0, i + 1))
          break;
      }
      for(unsigned j = 0; j < m_Graph->GetDim(); j++)
      {
        if(m_Graph->GetID(j) != 0)
          m_Graph->SetAddCoord(j, matr.get(m_Graph->GetID(j), i));
        else
          m_Graph->SetAddCoord(j, ts);
      }
      m_PrevStamp = ts;
    }
  }
}
