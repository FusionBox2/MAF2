/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: lhpPipeIntGraphAbstract.cpp,v $
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

#include "lhpPipeIntGraphAbstract.h"
#include "mafDecl.h"
#include "mafEventSender.h"
#include "mafViewIntGraph.h"
#include "mafSceneNode.h"
#include "mafSceneGraph.h"

#include "mafVME.h"
#include "mafGraphIDDesc.h"


mafCxxAbstractTypeMacro(lhpPipeIntGraphAbstract);

//----------------------------------------------------------------------------
lhpPipeIntGraphAbstract::lhpPipeIntGraphAbstract()
//----------------------------------------------------------------------------
{
  m_ForcedWholeRange = false;
  m_WholeRange       = false;
  m_Graph = new mafGraphDataImpl(this, 8888.8888, 1000);
  m_Graph->AddXVar(0);
}
//----------------------------------------------------------------------------
lhpPipeIntGraphAbstract::~lhpPipeIntGraphAbstract()
//----------------------------------------------------------------------------
{
  if (m_Vme)
  {
    m_Vme->RemoveObserver(this);
  }
  mafViewIntGraph *vgraph = mafViewIntGraph::SafeDownCast(m_View);
  if(vgraph && vgraph->GetRenderWindow())
  {
    vgraph->GetRenderWindow()->RemGraphData(m_Graph);
    vgraph->GetRenderWindow()->SetXParam(NULL);
  }
  cppDEL(m_Graph);
}

//----------------------------------------------------------------------------
void lhpPipeIntGraphAbstract::Create(mafNode *node, mafView *view)
//----------------------------------------------------------------------------
{
  Superclass::Create(node, view);
  mafViewIntGraph *vgraph = mafViewIntGraph::SafeDownCast(m_View);
  if(vgraph)
    vgraph->GetRenderWindow()->AddGraphData(m_Graph);
  m_Vme->AddObserver(this);
}

//----------------------------------------------------------------------------
mafGUI *lhpPipeIntGraphAbstract::CreateGui()
//----------------------------------------------------------------------------
{
  assert(m_Gui == NULL);
  m_Gui = new mafGUI(this);

  m_Gui->Bool(ID_WHOLE_RANGE, _("Whole range"), &m_WholeRange);
  return m_Gui;
}

//----------------------------------------------------------------------------
void lhpPipeIntGraphAbstract::OnEvent(mafEventBase *maf_event)
//----------------------------------------------------------------------------
{
  if (mafEvent *e = mafEvent::SafeDownCast(maf_event))
  {
    switch(e->GetId()) 
    {
    case ID_WHOLE_RANGE:
      {
        if(m_WholeRange)
        {
          GrabData();
          mafEventMacro(mafEvent(this,CAMERA_UPDATE));
        }
      }
      break;
    default:
      Superclass::OnEvent(maf_event);
      break;
    }
  }
  else if(maf_event->GetId() == VME_TIME_SET)
  {
    GrabData();
  }
  else
  {
    Superclass::OnEvent(maf_event);
  }
}

void lhpPipeIntGraphAbstract::SetForcedWholeRange(int forced)
{
  m_ForcedWholeRange = forced;
  if(m_ForcedWholeRange)
  {
    GrabData();
    mafEventMacro(mafEvent(this,CAMERA_UPDATE));
  }
}

void lhpPipeIntGraphAbstract::SetSmoothParam(double param)
{
  m_Graph->SetSmoothParam(param);
  mafEventMacro(mafEvent(this,CAMERA_UPDATE));
}


//----------------------------------------------------------------------------
void lhpPipeIntGraphAbstract::loadPlotInfo()
//----------------------------------------------------------------------------
{
  //mafTagItem        Tag;
  //wxInt32           nI;
  //wxChar const      *cpTagValue = NULL;
  //wxInt32           nValue;
  //unsigned int      der;

  return;
/*
  //if(!vme->GetTagArray()->IsTagPresent(mafINTG_SAVEINFO_TAG))
  {
    return;
  }
  // X Value have double value + GDT_LAST;
  //read a values one by one and add them to plot
  for(nI = 0; nI< Tag.GetNumberOfComponents(); nI++)
  {
    cpTagValue = Tag.GetValue(nI);
    sscanf(cpTagValue, "%d %ud", &nValue, &der);
    if(m_Descriptions == NULL)
    {
      return;
    }
    IDType setID;
    setID[1] = mafGraphDescType(nValue % GDT_LAST);
    {
      m_LoadMode = true;
      mafEventMacro(mafEvent(this, VME_SHOW, vme, true));
      mafEventMacro(mafEvent(this, CAMERA_UPDATE));

      if(nValue >  GDT_LAST)
      {
        setID[0] = m_Pipes->Count() - 1;
        m_Graph->SetXVar(0, setID, der);
      }
      else// if(nValue <= GDT_LAST)
      {
        setID[0] = m_Pipes->Count() - 1;
        m_Graph->AddYVar(setID, der);
      }
      m_LoadMode = false;
    }
  }*/
}
//----------------------------------------------------------------------------
void lhpPipeIntGraphAbstract::savePlotInfo()
//----------------------------------------------------------------------------
{/*
  mafTagItem        *pTag = NULL;
  wxInt32           nI;
  wxChar const      *cpTagValue = NULL ;
  wxInt32           nPipeIndex = -1;
  wxInt32           nNumComp= 0, nCount = 0;
  wxString          sString("");
  wxString          sNumString("");
  wxChar     const  **pEntries = NULL; 

  //for all pipes
  for(nI = 0; nI< m_Pipes->GetCount(); nI++)
  {
    if(m_Pipes->Item(nI)->m_Vme == vme)
    {
      nPipeIndex = nI;
      break;
    }
  }

  if(nPipeIndex != -1)
  {
    nNumComp = 0;
    for(nI = m_Graph->GetYDim() - 1; nI >= 0; nI--)
    {
      if(nPipeIndex == m_Graph->GetYID(nI)[0] && m_Graph->GetYID(nI)[1] != 0)
      {
        //save Y var
        nNumComp++;
      }
    }
    for(nI = m_Graph->GetXDim() - 1; nI >= 0; nI--)
    {
      if(nPipeIndex == m_Graph->GetXID(nI)[0] && m_Graph->GetYID(nI)[1] != 0)
      {
        //save X var
        nNumComp++;
      }
    }
    //actually save
    nCount = 0;
    pEntries = (wxChar const **)malloc(sizeof(wxChar *) * nNumComp);
    for(nI = m_Graph->GetYDim() - 1; nI >= 0; nI--)
    {
      if(nPipeIndex == m_Graph->GetYID(nI)[0] && m_Graph->GetYID(nI)[1] != 0)
      {
        //save Y var
        sNumString.Printf("%d %ud", m_Graph->GetYID(nI)[1], m_Graph->GetYDeriv(nI));
        pEntries[nCount] = _strdup(sNumString.GetData());
        nCount ++;
      }
    }
    for(nI = m_Graph->GetXDim() - 1; nI >= 0; nI--)
    {
      if(nPipeIndex == m_Graph->GetXID(nI)[0] && m_Graph->GetYID(nI)[1] != 0)
      {
        //save X var
        sNumString.Printf("%d %ud", m_Graph->GetYID(nI)[1] + GDT_LAST, m_Graph->GetYDeriv(nI));
        pEntries[nCount] = _strdup(sNumString.GetData());
        nCount ++;
      }
    }
  }

  wxASSERT(nNumComp == nCount);
  pTag = new mafTagItem(mafINTG_SAVEINFO_TAG, pEntries, nNumComp);
  m_ModifyMode = true;
  vme->GetTagArray()->SetTag(*pTag);
  mafEventMacro(mafEvent(this,VME_MODIFIED,vme));
  m_ModifyMode = false;
  cppDEL(pTag);
  for(nI = nNumComp - 1; nI >= 0; nI--)
  {
    free(const_cast<char *>(pEntries[nI]));
  }
  free(pEntries);*/
}
