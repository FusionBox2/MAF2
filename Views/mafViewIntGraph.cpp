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
#include <fstream>
#include <algorithm>
#include <iterator>

#include "mafViewIntGraphWindow.h"
#include "lhpPipeIntGraphLocal.h"
#include <unordered_map>

#include "mafIndent.h"

#include "mafPipe.h"
#include "mafPipeFactory.h"

#include "mafTagArray.h"
#include "mafVME.h"
//#include "mafVMERoot.h"

#include "mafSceneNode.h"
#include "mafSceneGraph.h"

#include <queue>
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
  m_RenderWindow = new mafViewIntGraphWindow(GetLabel().toWx());
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
    mafString vme_type = _R(vme->GetTypeName());
    auto it = m_PipeMap.find(vme_type);
    if(it == m_PipeMap.end())
      return status;
    if(it->second.m_Visibility == NON_VISIBLE)
    {
      status = NODE_NON_VISIBLE;
    }
    else if(it->second.m_Visibility == MUTEX)
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
  // custom visualization for the view should be considered only
  // if we are not in editing mode.
  mafString vme_type = _R(v->GetTypeName());
  auto it = m_PipeMap.find(vme_type);
  if (it != m_PipeMap.end())
  {
    // pick up the visual pipe from the view's visual pipe map
    pipe_name = it->second.m_PipeName;
  }

  if(pipe_name.empty())
  {
    // pick up the default visual pipe from the vme
    pipe_name = _R("lhpPipeIntGraph");
  }
}

//----------------------------------------------------------------------------
void mafViewIntGraph::VmeCreatePipe(mafNode *vme)
//----------------------------------------------------------------------------
{
  mafString pipe_name = _R("");
  GetVisualPipeName(vme, pipe_name);
  if (!pipe_name.empty())
  {
    m_NumberOfVisibleVme++;
    mafPipeFactory *pipe_factory  = mafPipeFactory::GetInstance();
    assert(pipe_factory!=NULL);
    mafObject *obj = NULL;
    obj = pipe_factory->CreateInstance(pipe_name.GetCStr());
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
      mafErrorMessage(_M(_L("Cannot create visual pipe object of type \"")+ pipe_name + _L("\"!")));
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

  m_Gui->Label(_R("General Features"),true);
  //m_Gui->Integer(ID_REFERENCE_FRAME, "Reference frame", &(m_ReferenceFrame), 0, nMaxFrame, "This frame will be treated as upright(reference) for all representations that require it!");
  //m_Gui->Button(ID_FIND_REFERENCE, "Autofind reference", "", "Find best reference frames for all joints (hierarchially based or not) ");

  m_Gui->Double(ID_SMOOTHING, _R("Smooth param"), &m_Smoothing, 0, 1000);

  //m_Gui->Bool(ID_FREEZE_GRAPH,"Freeze graph", &m_IsFrozen,0);

  m_Gui->Divider(2);

  m_Gui->Button(ID_SAVE_PLOT, _R("Save plot"), _R(""), _R("Save plot"));
  m_Gui->Button(ID_LOAD_PLOT, _R("Load plot"), _R(""), _R("Restore plot"));


  m_Gui->Divider(2);
  m_Gui->RollOut(ID_ROLLOUT_RENDER, _R("Plot appearance"), m_RenderWindow->GetGui(), false);

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
  os << indent << "Name: " << '\t' << GetLabel().GetCStr() << "\n";
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
  //savePlotGen();
  if(m_Sg == NULL)
    return;

  wxString ScriptFileName = ::wxFileSelector("Select script file name", "", "data", ".psc", "PSC files (*.psc)|*.psc|All files (*.*)|*.*",
    wxFD_SAVE | wxFD_OVERWRITE_PROMPT, GetFrame());
  if (ScriptFileName.IsEmpty())
    return;

  std::queue<mafNode*> trav;
  m_shown_flags.clear();
  m_pipe_config.clear();

  std::unordered_map<mafNode*, size_t> indexator;
  indexator[NULL] = 0;
  size_t idx = 1;
  trav.push(m_Sg->GetSelectedVme());
  while (!trav.empty())
  {
    mafNode *curr = trav.front();
    trav.pop();
    for (unsigned long i = 0, ie = curr->GetNumberOfChildren(); i != ie; ++i)
      trav.push(curr->GetChild(i));
    indexator[curr] = idx++;
  }


  trav.push(m_Sg->GetSelectedVme());
  //trav.push(nullptr);
  while (!trav.empty())
  {
    mafNode *curr = trav.front();
    trav.pop();
    for (unsigned long i = 0, ie = curr->GetNumberOfChildren(); i != ie; ++i)
      trav.push(curr->GetChild(i));
    lhpPipeIntGraphAbstract *pipe = static_cast<lhpPipeIntGraphAbstract*>(GetNodePipe(curr));
    m_shown_flags.push_back(pipe != nullptr);
    if (pipe)
    {
      std::ostringstream oss;
      if (lhpPipeIntGraphLocal *lcl = lhpPipeIntGraphLocal::SafeDownCast(pipe))
      {
        mafNode *nd = lcl->GetProximal();
        auto it = indexator.find(nd);
        if (it != indexator.end())
          oss << it->second << " ";
      }
      pipe->operator<<(oss);
      m_pipe_config.push_back(oss.str());
    }
    //trav.push(nullptr);
  }

//   for(mafSceneNode *n = m_Sg->GetNodeList(); n != NULL; n = n->m_Next)
//   {
//     mafNode *vme = n->m_Vme;
//     lhpPipeIntGraphAbstract *pipe = lhpPipeIntGraphAbstract::SafeDownCast(n->m_Pipe);
//     if(vme && pipe)
//     {
//       //pipe->savePlotInfo();
//     }
//   }
  std::ofstream ofs(mafWxToString(ScriptFileName).GetCStr());
  std::copy(m_shown_flags.begin(), m_shown_flags.end(), std::ostream_iterator<bool>(ofs, " "));
  ofs << '\n';
  std::copy(m_pipe_config.begin(), m_pipe_config.end(), std::ostream_iterator<std::string>(ofs, "\n"));
}

//----------------------------------------------------------------------------
void mafViewIntGraph::savePlotGen(void)
//----------------------------------------------------------------------------
{
  auto strv = m_RenderWindow->SaveSettings();

  //save general settings

  for(mafSceneNode *n = m_Sg->GetNodeList(); n != NULL; n = n->m_Next)
  {
    mafVME *vme = mafVME::SafeDownCast(n->m_Vme);
    if(vme)
    {
      vme->GetTagArray()->SetTag(mafTagItem(_R(mafINTGG_SAVEINFO_TAG), strv));
      {mafEvent evUnq(this,VME_MODIFIED, vme); mafEventMacro(evUnq);}
    }
  }
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
      if(mafTagItem *ti = vme->GetTagArray()->GetTag(_R(mafINTGG_SAVEINFO_TAG)))
      {
        m_RenderWindow->LoadSettings(ti->GetComponents());
        UpdateGui();
        break;
      }
    }
  }
}
namespace
{
  struct Line
  {
    std::string data;
    operator std::string const&() const { return data; }
  };
  std::istream& operator>>(std::istream& s, Line& dst)
  {
    return std::getline(s, dst.data);
  }
}

//----------------------------------------------------------------------------
void mafViewIntGraph::loadPlot(bool readfile)
//----------------------------------------------------------------------------
{
  if (readfile)
  {
    wxString ScriptFileName = ::wxFileSelector("Select script file name", "", "data", ".psc", "PSC files (*.psc)|*.psc|All files (*.*)|*.*",
      wxFD_OPEN | wxFD_FILE_MUST_EXIST, GetFrame());
    if (ScriptFileName.IsEmpty())
      return;
    m_shown_flags.clear();
    m_pipe_config.clear();
    std::ifstream ifs(mafWxToString(ScriptFileName).GetCStr());
    Line s;
    ifs >> s;
    std::istringstream iss(s);
    std::copy(std::istream_iterator<bool>(iss), std::istream_iterator<bool>(), std::back_inserter(m_shown_flags));
    std::copy(std::istream_iterator<Line>(ifs), std::istream_iterator<Line>(), std::back_inserter(m_pipe_config));
  }

  //loadPlotGen();
  std::queue<mafNode*> trav;
  auto it = m_shown_flags.cbegin();
  auto its = m_pipe_config.cbegin();

  std::vector<mafNode*> indexator;
  indexator.push_back(NULL);
  trav.push(m_Sg->GetSelectedVme());
  while (!trav.empty())
  {
    mafNode *curr = trav.front();
    trav.pop();
    for (unsigned long i = 0, ie = curr->GetNumberOfChildren(); i != ie; ++i)
      trav.push(curr->GetChild(i));
    indexator.push_back(curr);
  }


  trav.push(m_Sg->GetSelectedVme());
  //trav.push(nullptr);
  while (!trav.empty())
  {
    mafNode *curr = trav.front();
    trav.pop();
    for (unsigned long i = 0, ie = curr->GetNumberOfChildren(); i != ie; ++i)
      trav.push(curr->GetChild(i));
    bool show = false;
    if (it != m_shown_flags.cend())
      show = *it++;
    if (readfile)
      {mafEvent evUnq(this, VME_SHOW, curr, show); mafEventMacro(evUnq);}
    else
    {
      if (show)
        VmeCreatePipe(curr);
    }
    if(lhpPipeIntGraphAbstract *pipe = static_cast<lhpPipeIntGraphAbstract*>(GetNodePipe(curr)))
    {
      if (its != m_pipe_config.cend())
      {
        std::istringstream iss(*its++);
        if (lhpPipeIntGraphLocal *lcl = lhpPipeIntGraphLocal::SafeDownCast(pipe))
        {
          size_t ix;
          iss >> ix;
          lcl->SetProximal(mafVME::SafeDownCast(indexator[ix]));
        }
        pipe->operator>>(iss);
        if (!readfile)
          pipe->SetForcedWholeRange(true);
      }
    }

    //trav.push(nullptr);
  }

  //clean all old plots
  /*for(mafSceneNode *n = m_Sg->GetNodeList(); n != NULL; n = n->m_Next)
  {
    mafNode *vme = n->m_Vme;
    if(vme)
    {
      {mafEvent evUnq(this, VME_SHOW, vme, false); mafEventMacro(evUnq);}
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
  }*/
}
