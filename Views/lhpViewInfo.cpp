/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: lhpViewInfo.cpp,v $
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

#include "lhpPipeInfo.h"
#include "lhpViewInfo.h"
#include "lhpVMEKMInfo.h"

#include "lhpViewInfoWnd.h"

#include "mafIndent.h"

#include "mafPipe.h"
#include "mafPipeFactory.h"

#include "mafStringSet.h"
#include "mafTagArray.h"
#include "mafVME.h"

#include "mafSceneNode.h"
#include "mafSceneGraph.h"


#include "mafVMERoot.h"
class lhpViewInfoGraph  : public mafSceneGraph
{
public:
  lhpViewInfoGraph(mafView *view):mafSceneGraph(view, NULL, NULL){}
  virtual ~lhpViewInfoGraph(){}
  virtual int  GetNodeStatus (mafNode *node);
};

//----------------------------------------------------------------------------
int lhpViewInfoGraph::GetNodeStatus(mafNode *node)
//----------------------------------------------------------------------------
{
  if (!m_InformationPipeModality && mafVMERoot::SafeDownCast(node))
  {
    return NODE_NON_VISIBLE;
  }

  mafSceneNode *n = Vme2Node(node);
  if(!n)
    return NODE_NON_VISIBLE;

  if (!node->IsMAFType(lhpVMEKMInfo))
  {
    return NODE_NON_VISIBLE;
  }
  lhpVMEKMInfo *vme = (lhpVMEKMInfo*)node;
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
mafCxxTypeMacro(lhpViewInfo);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
lhpViewInfo::lhpViewInfo(const mafString& label)
:mafView(label)
//----------------------------------------------------------------------------
{
  m_RenderWindow       = NULL;
  m_Sg                 = NULL;
}

//----------------------------------------------------------------------------
lhpViewInfo::~lhpViewInfo() 
//----------------------------------------------------------------------------
{
  m_PipeMap.clear();
  cppDEL(m_Sg);
}
//----------------------------------------------------------------------------
void lhpViewInfo::PlugVisualPipe(mafString vme_type, mafString pipe_type, long visibility)
//----------------------------------------------------------------------------
{
  mafVisualPipeInfo plugged_pipe;
  plugged_pipe.m_PipeName=pipe_type;
  plugged_pipe.m_Visibility=visibility;
  m_PipeMap[vme_type] = plugged_pipe;
}

//----------------------------------------------------------------------------
mafView *lhpViewInfo::Copy(mafBaseEventHandler *Listener, bool lightCopyEnabled)
//----------------------------------------------------------------------------
{
  m_LightCopyEnabled = lightCopyEnabled;
  lhpViewInfo *v = new lhpViewInfo(GetLabel());
  v->SetListener(Listener);
  v->m_Id = m_Id;
  v->m_PipeMap = m_PipeMap;
  v->Create();
  return v;
}

//----------------------------------------------------------------------------
void lhpViewInfo::Create()
//----------------------------------------------------------------------------
{
  m_RenderWindow = new wxHtmlWindow(mafGetFrame(), -1, wxDefaultPosition, wxDefaultSize, 0, GetLabel().toWx());

  m_Win          = m_RenderWindow;

  m_Sg  = new lhpViewInfoGraph(this);
  m_Sg->SetListener(this);
}

//----------------------------------------------------------------------------
void lhpViewInfo::VmeAdd(mafNode *vme)
//----------------------------------------------------------------------------
{
  m_Sg->VmeAdd(vme);
}
//----------------------------------------------------------------------------
void lhpViewInfo::VmeShow(mafNode *vme, bool show)
//----------------------------------------------------------------------------
{
  assert(m_Sg); 
  m_Sg->VmeShow(vme,show);
}

//----------------------------------------------------------------------------
void lhpViewInfo::VmeUpdateProperty(mafNode *vme, bool fromTag)
//----------------------------------------------------------------------------
{
  assert(m_Sg); 
  m_Sg->VmeUpdateProperty(vme,fromTag);
}
//----------------------------------------------------------------------------
int lhpViewInfo::GetNodeStatus(mafNode *vme)
//----------------------------------------------------------------------------
{
  int status = m_Sg ? m_Sg->GetNodeStatus(vme) : NODE_NON_VISIBLE;
  if (!m_PipeMap.empty())
  {
    mafString vme_type = _R(vme->GetTypeName());
    if(m_PipeMap.find(vme_type) == m_PipeMap.end())
      return status;
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
void lhpViewInfo::VmeRemove(mafNode *vme)
//----------------------------------------------------------------------------
{
  m_Sg->VmeRemove(vme);
}
//----------------------------------------------------------------------------
void lhpViewInfo::VmeSelect(mafNode *vme, bool select)
//----------------------------------------------------------------------------
{
  assert(m_Sg); 
  m_Sg->VmeSelect(vme,select);
}
//----------------------------------------------------------------------------
void lhpViewInfo::CameraUpdate() 
//----------------------------------------------------------------------------
{
  if(m_RenderWindow != NULL)
    m_RenderWindow->Update();
}
void lhpViewInfo::CameraReset(mafNode *node)
{
  CameraUpdate();
}

//----------------------------------------------------------------------------
mafPipe* lhpViewInfo::GetNodePipe(mafNode *vme)
//----------------------------------------------------------------------------
{
  assert(m_Sg);
  mafSceneNode *n = m_Sg->Vme2Node(vme);
  if(!n) return NULL;
  return n->m_Pipe;
}

//----------------------------------------------------------------------------
void lhpViewInfo::GetVisualPipeName(mafNode *node, mafString &pipe_name)
//----------------------------------------------------------------------------
{
  mafVME *v = mafVME::SafeDownCast(node);
  assert(v);

  v->Modified();
  vtkDataSet *data = v->GetOutput()->GetVTKData();
  // custom visualization for the view should be considered only
  // if we are not in editing mode.
  mafString vme_type = _R(v->GetTypeName());
  if (!m_PipeMap.empty())
  {
    // pick up the visual pipe from the view's visual pipe map
    pipe_name = m_PipeMap[vme_type].m_PipeName;
  }

  if(pipe_name.IsEmpty())
  {
    // pick up the default visual pipe from the vme
    pipe_name = _R("lhpPipeIntGraph");
  }
}

//----------------------------------------------------------------------------
void lhpViewInfo::VmeCreatePipe(mafNode *vme)
//----------------------------------------------------------------------------
{
  mafString pipe_name = _R("");
  GetVisualPipeName(vme, pipe_name);
  if (!pipe_name.IsEmpty())
  {
    m_NumberOfVisibleVme++;
    mafPipeFactory *pipe_factory  = mafPipeFactory::GetInstance();
    assert(pipe_factory!=NULL);
    mafObject *obj = NULL;
    obj = pipe_factory->CreateInstance(pipe_name.GetCStr());
    lhpPipeInfo *pipe = lhpPipeInfo::SafeDownCast(obj);
    if (pipe)
    {
      pipe->SetListener(this);
      mafSceneNode *n = m_Sg->Vme2Node(vme);
      assert(n && !n->m_Pipe);
      pipe->Create(vme, this);
      n->m_Pipe = pipe;
      m_VNodes.push_back(pipe);
      UpdatePage();
    }
    else
    {
      if(obj)
        cppDEL(obj);
      mafErrorMessage(_M(_L("Cannot create visual pipe object of type \"") + pipe_name + _L("\"!")));
    }
  }
  return;
}

//----------------------------------------------------------------------------
void lhpViewInfo::VmeDeletePipe(mafNode *vme)
//----------------------------------------------------------------------------
{
  m_NumberOfVisibleVme--;
  mafSceneNode *n = m_Sg->Vme2Node(vme);



  assert(n && n->m_Pipe);
  for(std::vector<mafPipe *>::iterator it = m_VNodes.begin(); it != m_VNodes.end(); ++it)
  {
    if((*it) == n->m_Pipe)
    {
      m_VNodes.erase(it);
      break;
    }
  }
  UpdatePage();
  cppDEL(n->m_Pipe);
}
void lhpViewInfo::UpdatePage()
{
  mafString pageText;
  for(std::vector<mafPipe *>::iterator it = m_VNodes.begin(); it != m_VNodes.end(); ++it)
  {
    if(lhpPipeInfo *pi = lhpPipeInfo::SafeDownCast(*it))
    {
      pageText += pi->GetPageText();
      pageText += _R("\n");
    }
  }
  m_RenderWindow->SetPage(pageText.toWx());

}

//-------------------------------------------------------------------------
mafGUI *lhpViewInfo::CreateGui()
//-------------------------------------------------------------------------
{
  assert(m_Gui == NULL);
  m_Gui = mafView::CreateGui();

  m_Gui->SetListener(this);

  m_Gui->Label(_R("General Features"),true);
  m_Gui->Divider(2);

  //m_Gui->RollOut(ID_ROLLOUT_RENDER, "Plot appearance", m_RenderWindow->GetGui(), false);

  m_Gui->Divider(2);

  m_Gui->Update();

  return m_Gui;
}
//----------------------------------------------------------------------------
void lhpViewInfo::OnEvent(mafEventBase *maf_event)
//----------------------------------------------------------------------------
{
  if (mafEvent *e = mafEvent::SafeDownCast(maf_event))
  {
    switch(e->GetId())
    {
    case ID_ROLLOUT_RENDER:
      break;
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
void lhpViewInfo::SetWindowSize(int w, int h)
//----------------------------------------------------------------------------
{
  GetRenderWindow()->SetSize(w,h);
}
//----------------------------------------------------------------------------
void lhpViewInfo::Print(wxDC *dc, wxRect margins)
//----------------------------------------------------------------------------
{
  wxBitmap image;
  GetImage(image/*, 2*/);
  PrintBitmap(dc, margins, &image);
}
//----------------------------------------------------------------------------
void lhpViewInfo::GetImage(wxBitmap &bmp, int magnification)
//----------------------------------------------------------------------------
{
  //bmp = m_RenderWindow->GetBitmap();
}
//----------------------------------------------------------------------------
void lhpViewInfo::OptionsUpdate()
//----------------------------------------------------------------------------
{
}

//-------------------------------------------------------------------------
void lhpViewInfo::Print(std::ostream& os, const int tabs)// const
//-------------------------------------------------------------------------
{
  Superclass::Print(os,tabs);
  mafIndent indent(tabs);

  os << indent << "lhpViewInfo " << '\t' << this << "\n";
  os << indent << "Name: " << '\t' << GetLabel().GetCStr() << "\n";
  os << indent << "View ID: " << '\t' << m_Id << "\n";
  os << indent << "View Mult: " << '\t' << m_Mult << "\n";
  os << indent << "Visible VME counter: " << '\t' << m_NumberOfVisibleVme << "\n";

  m_Sg->Print(os, 1);
  os << std::endl;
}
//----------------------------------------------------------------------------
void lhpViewInfo::UpdateGui() 
//----------------------------------------------------------------------------
{ 
  if(m_Gui != NULL)
    m_Gui->Update();
}
