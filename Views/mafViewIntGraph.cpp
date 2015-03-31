/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafViewIntGraph.cpp,v $
  Language:  C++
  Date:      $Date: 2008-07-25 12:13:22 $
  Version:   $Revision: 1.3 $
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
#include "mafViewIntGraph.h"

#include "mafViewIntGraphWindow.h"

#include "mafIndent.h"

#include "mafPipe.h"
#include "mafPipeFactory.h"

#include "mafStringSet.h"
#include "mafTagArray.h"
#include "mafVME.h"
//#include "mafVMERoot.h"

#include "mafSceneNode.h"
#include "mafSceneGraph.h"


#include "mafVMERoot.h"
class lhpPlotGraph  : public mafSceneGraph
{
public:
  lhpPlotGraph(mafView *view):mafSceneGraph(view, NULL, NULL){}
  virtual ~lhpPlotGraph(){}
  virtual int  GetNodeStatus (mafNode *node);
};

//----------------------------------------------------------------------------
int lhpPlotGraph::GetNodeStatus(mafNode *node)
//----------------------------------------------------------------------------
{
  return mafSceneGraph::GetNodeStatus(node);
  if (!m_InformationPipeModality && mafVMERoot::SafeDownCast(node))
  {
    return NODE_NON_VISIBLE;
  }

  mafSceneNode *n = Vme2Node(node);
  if(!n)
    return NODE_NON_VISIBLE;

  if (!node->IsMAFType(mafVME))
  {
    return NODE_NON_VISIBLE;
  }
  mafVME *vme = (mafVME *)node;
  bool creatable = n->m_PipeCreatable && vme;
  //landmark are not creatable
  //if(vme->IsA("mafNodeLandmark")) creatable = false;

  if(!creatable)                      return NODE_NON_VISIBLE;
  if( n->m_Mutex &&  n->IsVisible())  return NODE_MUTEX_ON;
  if( n->m_Mutex && !n->IsVisible())  return NODE_MUTEX_OFF;
  if( n->IsVisible())                 return NODE_VISIBLE_ON;
  if(!n->IsVisible())                 return NODE_VISIBLE_OFF;
  return NODE_NON_VISIBLE;
}

//----------------------------------------------------------------------------
mafCxxTypeMacro(mafViewIntGraph);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
mafViewIntGraph::mafViewIntGraph(const mafString& label)
:mafView(label)
//----------------------------------------------------------------------------
{
  m_RenderWindow       = NULL;
  m_IsFrozen           = 0;
  //m_ReferenceFrame     = 0;
  m_Smoothing          = 0.0;
  m_Sg                 = NULL;
}

//----------------------------------------------------------------------------
mafViewIntGraph::~mafViewIntGraph() 
//----------------------------------------------------------------------------
{
  m_PipeMap.clear();
  cppDEL(m_Sg);
}
//----------------------------------------------------------------------------
void mafViewIntGraph::PlugVisualPipe(mafString vme_type, mafString pipe_type, long visibility)
//----------------------------------------------------------------------------
{
  mafVisualPipeInfo plugged_pipe;
  plugged_pipe.m_PipeName=pipe_type;
  plugged_pipe.m_Visibility=visibility;
  m_PipeMap[vme_type] = plugged_pipe;
}

//----------------------------------------------------------------------------
mafView *mafViewIntGraph::Copy(mafBaseEventHandler *Listener, bool lightCopyEnabled)
//----------------------------------------------------------------------------
{
  m_LightCopyEnabled = lightCopyEnabled;
  mafViewIntGraph *v = new mafViewIntGraph(GetLabel());
  v->SetListener(Listener);
  v->m_Id = m_Id;
  v->m_PipeMap = m_PipeMap;
  v->m_IsFrozen       = m_IsFrozen;
  //v->m_ReferenceFrame = m_ReferenceFrame;
  v->Create();
  return v;
}

//----------------------------------------------------------------------------
void mafViewIntGraph::Create()
//----------------------------------------------------------------------------
{
  m_RenderWindow = new mafViewIntGraphWindow(GetLabel().GetCStr());
  m_Win          = m_RenderWindow;

  m_Sg  = new lhpPlotGraph(this);
  m_Sg->SetListener(this);

  //set reference frame as first sequence frame
  //m_ReferenceFrame = 0;
}

//----------------------------------------------------------------------------
void mafViewIntGraph::VmeAdd(mafNode *vme)
//----------------------------------------------------------------------------
{
  m_Sg->VmeAdd(vme);
}
//----------------------------------------------------------------------------
void mafViewIntGraph::VmeShow(mafNode *vme, bool show)
//----------------------------------------------------------------------------
{
  assert(m_Sg); 
  m_Sg->VmeShow(vme,show);
}

//----------------------------------------------------------------------------
void mafViewIntGraph::VmeUpdateProperty(mafNode *vme, bool fromTag)
//----------------------------------------------------------------------------
{
  assert(m_Sg); 
  m_Sg->VmeUpdateProperty(vme,fromTag);
}
//----------------------------------------------------------------------------
int mafViewIntGraph::GetNodeStatus(mafNode *vme)
//----------------------------------------------------------------------------
{
  int status = m_Sg ? m_Sg->GetNodeStatus(vme) : NODE_NON_VISIBLE;
  if (!m_PipeMap.empty())
  {
    mafString vme_type = vme->GetTypeName();
    if(m_PipeMap[vme_type].m_Visibility == NON_VISIBLE)
    {
      status = NODE_NON_VISIBLE;
    }
    else if (m_PipeMap[vme_type].m_Visibility == MUTEX)
    {
      mafSceneNode *n = m_Sg->Vme2Node(vme);
      if (n != NULL)
      {
        n->m_Mutex = true;
      }
      status = m_Sg->GetNodeStatus(vme);
    }
  }
  return status;
}

//----------------------------------------------------------------------------
void mafViewIntGraph::VmeRemove(mafNode *vme)
//----------------------------------------------------------------------------
{
  m_Sg->VmeRemove(vme);
}
//----------------------------------------------------------------------------
void mafViewIntGraph::VmeSelect(mafNode *vme, bool select)
//----------------------------------------------------------------------------
{
  assert(m_Sg); 
  m_Sg->VmeSelect(vme,select);
}
//----------------------------------------------------------------------------
void mafViewIntGraph::CameraUpdate() 
//----------------------------------------------------------------------------
{
  if(m_RenderWindow != NULL)
    m_RenderWindow->Update();
}
//----------------------------------------------------------------------------
mafPipe* mafViewIntGraph::GetNodePipe(mafNode *vme)
//----------------------------------------------------------------------------
{
  assert(m_Sg);
  mafSceneNode *n = m_Sg->Vme2Node(vme);
  if(!n) return NULL;
  return n->m_Pipe;
}

//----------------------------------------------------------------------------
void mafViewIntGraph::GetVisualPipeName(mafNode *node, mafString &pipe_name)
//----------------------------------------------------------------------------
{
  mafVME *v = mafVME::SafeDownCast(node);
  assert(v);

  v->Modified();
  vtkDataSet *data = v->GetOutput()->GetVTKData();
  // custom visualization for the view should be considered only
  // if we are not in editing mode.
  mafString vme_type = v->GetTypeName();
  if (!m_PipeMap.empty())
  {
    // pick up the visual pipe from the view's visual pipe map
    pipe_name = m_PipeMap[vme_type].m_PipeName;
  }

  if(pipe_name.IsEmpty())
  {
    // pick up the default visual pipe from the vme
    pipe_name = "lhpPipeIntGraph";
  }
}

//----------------------------------------------------------------------------
void mafViewIntGraph::VmeCreatePipe(mafNode *vme)
//----------------------------------------------------------------------------
{
  mafString pipe_name = "";
  GetVisualPipeName(vme, pipe_name);
  if (pipe_name != "")
  {
    m_NumberOfVisibleVme++;
    mafPipeFactory *pipe_factory  = mafPipeFactory::GetInstance();
    assert(pipe_factory!=NULL);
    mafObject *obj = NULL;
    obj = pipe_factory->CreateInstance(pipe_name);
    lhpPipeIntGraphAbstract *pipe = lhpPipeIntGraphAbstract::SafeDownCast(obj);
    if (pipe)
    {
      pipe->SetListener(this);
      mafSceneNode *n = m_Sg->Vme2Node(vme);
      assert(n && !n->m_Pipe);
      pipe->Create(vme, this);
      pipe->SetSmoothParam(m_Smoothing);
      pipe->SetForcedWholeRange(m_IsFrozen);
      n->m_Pipe = pipe;
    }
    else
    {
      if(obj)
        cppDEL(obj);
      mafErrorMessage(_("Cannot create visual pipe object of type \"%s\"!"),pipe_name.GetCStr());
    }
  }
  return;
}

//----------------------------------------------------------------------------
void mafViewIntGraph::VmeDeletePipe(mafNode *vme)
//----------------------------------------------------------------------------
{
  m_NumberOfVisibleVme--;
  mafSceneNode *n = m_Sg->Vme2Node(vme);

  assert(n && n->m_Pipe);
  cppDEL(n->m_Pipe);
}
//-------------------------------------------------------------------------
mafGUI *mafViewIntGraph::CreateGui()
//-------------------------------------------------------------------------
{
  assert(m_Gui == NULL);
  m_Gui = mafView::CreateGui();

  //////////////////////////////////////////Plot gui
  //m_Gui = new mafGUI(this);
  m_Gui->SetListener(this);

  m_Gui->Label("General Features",true);
  //m_Gui->Integer(ID_REFERENCE_FRAME, "Reference frame", &(m_ReferenceFrame), 0, nMaxFrame, "This frame will be treated as upright(reference) for all representations that require it!");
  //m_Gui->Button(ID_FIND_REFERENCE, "Autofind reference", "", "Find best reference frames for all joints (hierarchially based or not) ");

  m_Gui->Double(ID_SMOOTHING, "Smooth param", &m_Smoothing, 0, 1000);

  //m_Gui->Bool(ID_FREEZE_GRAPH,"Freeze graph", &m_IsFrozen,0);

  m_Gui->Divider(2);

  /*m_Gui->Button(ID_SAVE_PLOT, "Save plot", "", "Save plot to VME tree");
  m_Gui->Button(ID_LOAD_PLOT, "Load plot", "", "Restore plot from VME tree");


  m_Gui->Divider(2);*/
  m_Gui->RollOut(ID_ROLLOUT_RENDER, "Plot appearance", m_RenderWindow->GetGui(), false);

  /////////////////////////////////////////DisplayList GUI
  m_Gui->Divider(2);
  //m_Gui->AddGui(m_sg->GetGui());

  //ShowGui();
  m_Gui->Update();

  return m_Gui;
}
//----------------------------------------------------------------------------
void mafViewIntGraph::OnEvent(mafEventBase *maf_event)
//----------------------------------------------------------------------------
{
  if (mafEvent *e = mafEvent::SafeDownCast(maf_event))
  {
    switch(e->GetId())
    {
    /*case VIEW_DELETE:
      {
        if(m_RenderWindow) 
          m_RenderWindow->Destroy();
        m_RenderWindow = NULL;
        mafEventMacro(*e);
        break;
      }*/
    case ID_ROLLOUT_RENDER:
      break;
    /*case ID_REFERENCE_FRAME:
      {
        m_Graph->Clean();
        if(m_IsFrozen)
        {
          CameraUpdate();
        }
        break;
      }*/
    case ID_LOAD_PLOT:
      {
        loadPlot();
        break;
      }
    case ID_SAVE_PLOT:
      {
        savePlot();
        break;
      }
    case ID_FREEZE_GRAPH:
      {
        if(m_Sg)
        {
          for(mafSceneNode *n = m_Sg->GetNodeList(); n != NULL; n = n->m_Next)
          {
            lhpPipeIntGraphAbstract *pg = lhpPipeIntGraphAbstract::SafeDownCast(n->m_Pipe);
            if(pg)
              pg->SetForcedWholeRange(m_IsFrozen);
          }
        }
      }
      break;
    case ID_SMOOTHING:
      {
        if(m_Sg)
        {
          for(mafSceneNode *n = m_Sg->GetNodeList(); n != NULL; n = n->m_Next)
          {
            lhpPipeIntGraphAbstract *pg = lhpPipeIntGraphAbstract::SafeDownCast(n->m_Pipe);
            if(pg)
              pg->SetSmoothParam(m_Smoothing);
          }
        }
        break;
      }
    default:
      Superclass::OnEvent(e);
      break;
    }

  }
  else
  {
    mafEventMacro(*maf_event);

  }

}
//----------------------------------------------------------------------------
void mafViewIntGraph::SetWindowSize(int w, int h)
//----------------------------------------------------------------------------
{
  GetRenderWindow()->SetSize(w,h);
}
//----------------------------------------------------------------------------
void mafViewIntGraph::Print(wxDC *dc, wxRect margins)
//----------------------------------------------------------------------------
{
  wxBitmap image;
  GetImage(image/*, 2*/);
  PrintBitmap(dc, margins, &image);
}
//----------------------------------------------------------------------------
void mafViewIntGraph::GetImage(wxBitmap &bmp, int magnification)
//----------------------------------------------------------------------------
{
  bmp = m_RenderWindow->GetBitmap();
}
//----------------------------------------------------------------------------
void mafViewIntGraph::OptionsUpdate()
//----------------------------------------------------------------------------
{
}

//-------------------------------------------------------------------------
void mafViewIntGraph::Print(std::ostream& os, const int tabs)// const
//-------------------------------------------------------------------------
{
  Superclass::Print(os,tabs);
  mafIndent indent(tabs);

  os << indent << "mafViewIntGraph " << '\t' << this << "\n";
  os << indent << "Name: " << '\t' << GetLabel() << "\n";
  os << indent << "View ID: " << '\t' << m_Id << "\n";
  os << indent << "View Mult: " << '\t' << m_Mult << "\n";
  os << indent << "Visible VME counter: " << '\t' << m_NumberOfVisibleVme << "\n";

  m_Sg->Print(os, 1);
  os << std::endl;
}
//----------------------------------------------------------------------------
void mafViewIntGraph::UpdateGui() 
//----------------------------------------------------------------------------
{ 
  if(m_Gui != NULL)
    m_Gui->Update();
}

//----------------------------------------------------------------------------
void mafViewIntGraph::savePlot(void)
//----------------------------------------------------------------------------
{
  savePlotGen();
  if(m_Sg == NULL)
    return;

  for(mafSceneNode *n = m_Sg->GetNodeList(); n != NULL; n = n->m_Next)
  {
    mafNode *vme = n->m_Vme;
    lhpPipeIntGraphAbstract *pipe = lhpPipeIntGraphAbstract::SafeDownCast(n->m_Pipe);
    if(vme && pipe)
    {
      pipe->savePlotInfo();
    }
  }
}

//----------------------------------------------------------------------------
void mafViewIntGraph::savePlotGen(void)
//----------------------------------------------------------------------------
{
  mafStringSet *pSave = m_RenderWindow->SaveSettings();

  //save general settings

  for(mafSceneNode *n = m_Sg->GetNodeList(); n != NULL; n = n->m_Next)
  {
    mafVME *vme = mafVME::SafeDownCast(n->m_Vme);
    if(vme)
    {
      std::vector<mafString> strv;
      char **sttr = pSave->GetData();
      int nm = pSave->GetStringNumber();
      for(int i = 0; i < nm; i++)
        strv.push_back(mafString(sttr[i]));
      vme->GetTagArray()->SetTag(mafTagItem(mafINTGG_SAVEINFO_TAG, strv));
      mafEventMacro(mafEvent(this,VME_MODIFIED, vme));
    }
  }
  cppDEL(pSave);
}
//----------------------------------------------------------------------------
void mafViewIntGraph::loadPlotGen(void)
//----------------------------------------------------------------------------
{
  //load general settings
  for(mafSceneNode *n = m_Sg->GetNodeList(); n != NULL; n = n->m_Next)
  {
    mafVME *vme = mafVME::SafeDownCast(n->m_Vme);
    if(vme)
    {
      if(mafTagItem *ti = vme->GetTagArray()->GetTag(mafINTGG_SAVEINFO_TAG))
      {
        m_RenderWindow->LoadSettings(&mafStringSet(ti->GetNumberOfComponents(), &ti->GetComponents()));
        UpdateGui();
        break;
      }
    }
  }
}

//----------------------------------------------------------------------------
void mafViewIntGraph::loadPlot(void)
//----------------------------------------------------------------------------
{
  loadPlotGen();

  //clean all old plots
  for(mafSceneNode *n = m_Sg->GetNodeList(); n != NULL; n = n->m_Next)
  {
    mafNode *vme = n->m_Vme;
    if(vme)
    {
      mafEventMacro(mafEvent(this, VME_SHOW, vme, false));
    }
  }
  for(mafSceneNode *n = m_Sg->GetNodeList(); n != NULL; n = n->m_Next)
  {
    mafNode *vme = n->m_Vme;
    lhpPipeIntGraphAbstract *pipe = lhpPipeIntGraphAbstract::SafeDownCast(n->m_Pipe);
    if(vme && pipe)
    {
      pipe->loadPlotInfo();
    }
  }
}
