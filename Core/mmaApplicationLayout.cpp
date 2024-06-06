/*=========================================================================

 Program: MAF2
 Module: mmaApplicationLayout
 Authors: Paolo Quadrani
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/


#include "mafDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the MAF must include "mafDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------

#include "mmaApplicationLayout.h"
#include "mafDecl.h"
#include "mafEvent.h"
#include "mafView.h"

#include "mafStorageElement.h"
#include "mafIndent.h"
#include "mafNode.h"
#include "mafViewVTK.h"
#include "mafSceneGraph.h"
#include "mafSceneNode.h"
#include "mafRWIBase.h"

#include "vtkCamera.h"
#include <iterator>

//----------------------------------------------------------------------------
mafCxxTypeMacro(mmaApplicationLayout)
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
mmaApplicationLayout::mmaApplicationLayout()
//----------------------------------------------------------------------------
{  
  m_Name = _R("ApplicationLayout");
  m_LayoutName = _R("Default Name");
  m_AppMaximized = 0;
  m_AppPosition[0] = 0;
  m_AppPosition[1] = 0;
  m_AppSize[0] = 800;
  m_AppSize[1] = 600;
  m_VisibilityVme = 0;
  ClearLayout();
}
//----------------------------------------------------------------------------
mmaApplicationLayout::~mmaApplicationLayout()
//----------------------------------------------------------------------------
{
}
//-------------------------------------------------------------------------
void mmaApplicationLayout::DeepCopy(const mafAttribute *a)
//-------------------------------------------------------------------------
{ 
  Superclass::DeepCopy(a);
  mmaApplicationLayout *src_layout = (mmaApplicationLayout *)a;
  m_LayoutName = src_layout->m_LayoutName;
  m_LayoutViewList.clear();
  for (int i = 0; i < src_layout->m_LayoutViewList.size(); i++)
  {
    m_LayoutViewList.push_back(src_layout->m_LayoutViewList[i]);
  }
}
//----------------------------------------------------------------------------
bool mmaApplicationLayout::Equals(const mafAttribute *a)
//----------------------------------------------------------------------------
{
  if (Superclass::Equals(a))
  {
    mmaApplicationLayout *src_layout = (mmaApplicationLayout *)a;
    if (m_LayoutViewList.size() != src_layout->m_LayoutViewList.size())
      return false;
    if (m_LayoutName != src_layout->m_LayoutName)
      return false;
    std::vector<ViewLayoutInfo>::iterator it1 = m_LayoutViewList.begin();
    std::vector<ViewLayoutInfo>::iterator it2 = src_layout->m_LayoutViewList.begin();
    for (;it1 != m_LayoutViewList.end();it1++,it2++)
    {
      if ((*it1).m_Id != (*it2).m_Id                    ||
          (*it1).m_Mult != (*it2).m_Mult                ||
          (*it1).m_Label != (*it2).m_Label              ||
          (*it1).m_Maximized != (*it2).m_Maximized      ||
          (*it1).m_Position[0] != (*it2).m_Position[0]  ||
          (*it1).m_Position[1] != (*it2).m_Position[1]  ||
          (*it1).m_Size[0] != (*it2).m_Size[0]          ||
          (*it1).m_Size[1] != (*it2).m_Size[1])
      {
        return false;
      }
    }
    return true;
  }
  return false;
}
//-----------------------------------------------------------------------
void mmaApplicationLayout::SetLayoutName(const mafString& name)
//-----------------------------------------------------------------------
{
  m_LayoutName = name;
}
//-----------------------------------------------------------------------
void mmaApplicationLayout::GetApplicationInfo(int &maximized, int pos[2], int size[2])
//-----------------------------------------------------------------------
{
  maximized = m_AppMaximized;
  pos[0] = m_AppPosition[0];
  pos[1] = m_AppPosition[1];
  size[0] = m_AppSize[0];
  size[1] = m_AppSize[1];
}
//-----------------------------------------------------------------------
void mmaApplicationLayout::SetApplicationInfo(int maximized, int pos[2], int size[2])
//-----------------------------------------------------------------------
{
  m_AppMaximized = maximized;
  m_AppPosition[0] = pos[0];
  m_AppPosition[1] = pos[1];
  m_AppSize[0] = size[0];
  m_AppSize[1] = size[1];
}
//-----------------------------------------------------------------------
void mmaApplicationLayout::SetInterfaceElementVisibility(const mafString& panel_name, int visibility)
//-----------------------------------------------------------------------
{
  if (panel_name == _R("toolbar"))
  {
    m_ToolBarVisibility = visibility;
  }
  else if (panel_name == _R("sidebar"))
  {
    m_SideBarVisibility = visibility;
  }
  else if (panel_name == _R("logbar"))
  {
    m_LogBarVisibility = visibility;
  }
}
//-----------------------------------------------------------------------
void mmaApplicationLayout::InternalStore(mafStorageElementBuilder& parent)
//-----------------------------------------------------------------------
{  
  Superclass::InternalStore(parent);// == MAF_OK)
  parent[_R("APPLICATION_MAXIMIZED")].SetValue(m_AppMaximized);
  parent[_R("APPLICATION_SIZE")].SetValue(mafToString(m_AppSize, 2));
  parent[_R("APPLICATION_POSITION")].SetValue(mafToString(m_AppPosition, 2));

  parent[_R("TOOLBAR_VISIBILITY")].SetValue(m_ToolBarVisibility);
  parent[_R("SIDEBAR_VISIBILITY")].SetValue(m_SideBarVisibility);
  parent[_R("LOGBAR_VISIBILITY")].SetValue(m_LogBarVisibility);

  parent[_R("LAYOUT_NAME")].SetValue(m_LayoutName);
  ViewLayoutInfo info;
  int n = m_LayoutViewList.size();
  parent[_R("NUMBER_OF_VIEW")].SetValue(n);
  for (int i = 0; i < n; i++)
  {
    info = m_LayoutViewList[i];
    parent[_R("VIEW_ID_") + mafToString(i)].SetValue(info.m_Id);
    parent[_R("VIEW_MULT_") + mafToString(i)].SetValue(info.m_Mult);
    parent[_R("VIEW_LABEL_") + mafToString(i)].SetValue(info.m_Label);
    parent[_R("VIEW_MAXIMIZED_") + mafToString(i)].SetValue(info.m_Maximized);
    parent[_R("VIEW_SIZE_") + mafToString(i)].SetValue(mafToString(info.m_Size, 2));
    parent[_R("VIEW_POS_") + mafToString(i)].SetValue(mafToString(info.m_Position, 2));
    parent[_R("VME_IN_VIEW_") + mafToString(i)].SetValue(info.m_VisibleVmes.size());
    if (info.m_VisibleVmes.size() > 0)
    {
      parent[_R("VME_IDS_IN_VIEW_") + mafToString(i)].SetValue(mafToString(info.m_VisibleVmes));
      parent[_R("VIEW_CAMERA_PARAMETERS_") + mafToString(i)].SetValue(mafToString(info.m_CameraParameters, 9));
    }
  }
}
//----------------------------------------------------------------------------
void mmaApplicationLayout::InternalRestore(const mafStorageElement& node)
//----------------------------------------------------------------------------
{
  Superclass::InternalRestore(node);
  m_AppMaximized = node[_R("APPLICATION_MAXIMIZED")].As<int>();
  mafParseVector(node[_R("APPLICATION_SIZE")].As<mafString>(), m_AppSize, 2);
  mafParseVector(node[_R("APPLICATION_POSITION")].As<mafString>(), m_AppPosition, 2);

  m_ToolBarVisibility = node[_R("TOOLBAR_VISIBILITY")].As<int>();
  m_SideBarVisibility = node[_R("SIDEBAR_VISIBILITY")].As<int>();
  m_LogBarVisibility = node[_R("LOGBAR_VISIBILITY")].As<int>();

  m_LayoutName = node[_R("LAYOUT_NAME")].As<mafString>();
  ViewLayoutInfo info;
  int n = node[_R("NUMBER_OF_VIEW")].As<int>();
  for (int i = 0; i < n; i++)
  {
    info.m_Id = node[_R("VIEW_ID_") + mafToString(i)].As<int>();
    info.m_Mult = node[_R("VIEW_MULT_") + mafToString(i)].As<int>();
    info.m_Label = node[_R("VIEW_LABEL_") + mafToString(i)].As<mafString>();
    info.m_Maximized = node[_R("VIEW_MAXIMIZED_") + mafToString(i)].As<int>();
    mafParseVector(node[_R("VIEW_SIZE_") + mafToString(i)].As<mafString>(), info.m_Size, 2);
    mafParseVector(node[_R("VIEW_POS_") + mafToString(i)].As<mafString>(), info.m_Position, 2);
    int num_vme = node[_R("VME_IN_VIEW_") + mafToString(i)].As<int>();
    if (num_vme > 0)
    {
      info.m_VisibleVmes.resize(num_vme);
      mafParseVector(node[_R("VME_IDS_IN_VIEW_") + mafToString(i)].As<mafString>(), info.m_VisibleVmes);
      mafParseVector(node[_R("VIEW_CAMERA_PARAMETERS_") + mafToString(i)].As<mafString>(), info.m_CameraParameters, 9);
    }
    m_LayoutViewList.push_back(info);
  }
}
//-----------------------------------------------------------------------
void mmaApplicationLayout::AddView(mafView *v, bool vme_visibility)
//-----------------------------------------------------------------------
{
  ViewLayoutInfo info;
  info.m_Id = v->m_Id;
  info.m_Mult = v->m_Mult;
  info.m_Label = v->GetLabel();
  wxFrame *frame = v->GetFrame();
  info.m_Maximized = frame->IsMaximized();
  wxRect r = frame->GetRect();
  info.m_Position[0] = r.GetPosition().x;
  info.m_Position[1] = r.GetPosition().y;
  info.m_Size[0] = r.GetSize().GetWidth();
  info.m_Size[1] = r.GetSize().GetHeight();
  info.m_VisibleVmes.clear();
  
  if (m_VisibilityVme)
  {
    int idx;
    for(mafSceneNode *n=v->GetSceneGraph()->GetNodeList(); n; n = n->m_Next)
    {
      if(n->m_Pipe)
      {
        idx = n->m_Vme->GetId();
        info.m_VisibleVmes.push_back(idx);
      }
    }
    if(mafViewVTK *vvtk = mafViewVTK::SafeDownCast(v))
    {
      info.m_CameraParameters[0] = vvtk->GetRWI()->GetCamera()->GetViewUp()[0];
      info.m_CameraParameters[1] = vvtk->GetRWI()->GetCamera()->GetViewUp()[1];
      info.m_CameraParameters[2] = vvtk->GetRWI()->GetCamera()->GetViewUp()[2];

      info.m_CameraParameters[3] = vvtk->GetRWI()->GetCamera()->GetPosition()[0];
      info.m_CameraParameters[4] = vvtk->GetRWI()->GetCamera()->GetPosition()[1];
      info.m_CameraParameters[5] = vvtk->GetRWI()->GetCamera()->GetPosition()[2];

      info.m_CameraParameters[6] = vvtk->GetRWI()->GetCamera()->GetFocalPoint()[0];
      info.m_CameraParameters[7] = vvtk->GetRWI()->GetCamera()->GetFocalPoint()[1];
      info.m_CameraParameters[8] = vvtk->GetRWI()->GetCamera()->GetFocalPoint()[2];
    }
  }
  m_LayoutViewList.push_back(info);
}
//-----------------------------------------------------------------------
void mmaApplicationLayout::RemoveView(mafView *v)
//-----------------------------------------------------------------------
{
  for (int i = 0; i < m_LayoutViewList.size(); i++)
  {
    if (m_LayoutViewList[i].m_Id == v->m_Id && m_LayoutViewList[i].m_Mult == v->m_Mult)
    {
      m_LayoutViewList.erase(m_LayoutViewList.begin()+i);
      return;
    }
  }
}
//-----------------------------------------------------------------------
void mmaApplicationLayout::RemoveView(int view_id)
//-----------------------------------------------------------------------
{
  m_LayoutViewList.erase(m_LayoutViewList.begin()+view_id);
}
//-----------------------------------------------------------------------
void mmaApplicationLayout::ClearLayout()
//-----------------------------------------------------------------------
{
  m_LayoutViewList.clear();
  m_LayoutName = _R("Null Layout");
}
//-----------------------------------------------------------------------
void mmaApplicationLayout::Print(std::ostream& os, const int tabs) const
//-----------------------------------------------------------------------
{
  Superclass::Print(os,tabs);
  mafIndent indent(tabs);

  os << indent << "Number of stored views: " << m_LayoutViewList.size() << "\n";
  os << std::endl;
}
