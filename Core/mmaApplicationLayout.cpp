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
  m_Name = "ApplicationLayout";
  m_LayoutName = "Default Name";
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
void mmaApplicationLayout::SetLayoutName(const char *name)
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
void mmaApplicationLayout::SetInterfaceElementVisibility(mafString panel_name, int visibility)
//-----------------------------------------------------------------------
{
  if (panel_name.Equals("toolbar"))
  {
    m_ToolBarVisibility = visibility;
  }
  else if (panel_name.Equals("sidebar"))
  {
    m_SideBarVisibility = visibility;
  }
  else if (panel_name.Equals("logbar"))
  {
    m_LogBarVisibility = visibility;
  }
}
//-----------------------------------------------------------------------
int mmaApplicationLayout::InternalStore(mafStorageElement *parent)
//-----------------------------------------------------------------------
{  
  if (Superclass::InternalStore(parent)==MAF_OK)
  {
    parent->StoreInteger("APPLICATION_MAXIMIZED", m_AppMaximized);
    parent->StoreVectorN("APPLICATION_SIZE",m_AppSize,2);
    parent->StoreVectorN("APPLICATION_POSITION",m_AppPosition,2);

    parent->StoreInteger("TOOLBAR_VISIBILITY", m_ToolBarVisibility);
    parent->StoreInteger("SIDEBAR_VISIBILITY", m_SideBarVisibility);
    parent->StoreInteger("LOGBAR_VISIBILITY", m_LogBarVisibility);

    parent->StoreText("LAYOUT_NAME",m_LayoutName.GetCStr());
    mafString view_id;
    mafString view_mult;
    mafString view_label;
    mafString view_max;
    mafString view_size;
    mafString view_pos;
    mafString vme_in_view;
    mafString vme_ids_in_view;
    mafString view_camera_parameters;
    ViewLayoutInfo info;
    int n = m_LayoutViewList.size();
    parent->StoreInteger("NUMBER_OF_VIEW", n);
    for (int i = 0; i < n; i++)
    {
      info = m_LayoutViewList[i];
      view_id = "VIEW_ID_";
      view_id << i;
      parent->StoreInteger(view_id, info.m_Id);
      view_mult = "VIEW_MULT_";
      view_mult << i;
      parent->StoreInteger(view_mult, info.m_Mult);
      view_label = "VIEW_LABEL_";
      view_label << i;
      parent->StoreText(view_label.GetCStr(), info.m_Label.GetCStr());
      view_max = "VIEW_MAXIMIZED_";
      view_max << i;
      parent->StoreInteger(view_max.GetCStr(),info.m_Maximized);
      view_size = "VIEW_SIZE_";
      view_size << i;
      view_pos = "VIEW_POS_";
      view_pos << i;
      parent->StoreVectorN(view_size.GetCStr(),info.m_Size,2);
      parent->StoreVectorN(view_pos.GetCStr(),info.m_Position,2);
      vme_in_view = "VME_IN_VIEW_";
      vme_in_view << i;
      parent->StoreInteger(vme_in_view.GetCStr(),info.m_VisibleVmes.size());
      if (info.m_VisibleVmes.size() > 0)
      {
        vme_ids_in_view = "VME_IDS_IN_VIEW_";
        vme_ids_in_view << i;
        parent->StoreVectorN(vme_ids_in_view.GetCStr(),info.m_VisibleVmes, info.m_VisibleVmes.size());

        view_camera_parameters = "VIEW_CAMERA_PARAMETERS_";
        view_camera_parameters << i;
        parent->StoreVectorN(view_camera_parameters.GetCStr(),info.m_CameraParameters, 9);
      }
    }
    return MAF_OK;
  }
  return MAF_ERROR;
}
//----------------------------------------------------------------------------
int mmaApplicationLayout::InternalRestore(mafStorageElement *node)
//----------------------------------------------------------------------------
{
  if (Superclass::InternalRestore(node) == MAF_OK)
  {
    node->RestoreInteger("APPLICATION_MAXIMIZED", m_AppMaximized);
    node->RestoreVectorN("APPLICATION_SIZE",m_AppSize,2);
    node->RestoreVectorN("APPLICATION_POSITION",m_AppPosition,2);

    node->RestoreInteger("TOOLBAR_VISIBILITY", m_ToolBarVisibility);
    node->RestoreInteger("SIDEBAR_VISIBILITY", m_SideBarVisibility);
    node->RestoreInteger("LOGBAR_VISIBILITY", m_LogBarVisibility);

    node->RestoreText("LAYOUT_NAME",m_LayoutName);
    mafString view_id;
    mafString view_mult;
    mafString view_label;
    mafString view_max;
    mafString view_size;
    mafString view_pos;
    mafString vme_in_view;
    mafString vme_ids_in_view;
    mafString view_camera_parameters;
    ViewLayoutInfo info;
    int n;
    node->RestoreInteger("NUMBER_OF_VIEW", n);
    for (int i = 0; i < n; i++)
    {
      view_id = "VIEW_ID_";
      view_id << i;
      node->RestoreInteger(view_id, info.m_Id);
      view_mult = "VIEW_MULT_";
      view_mult << i;
      node->RestoreInteger(view_mult, info.m_Mult);
      view_label = "VIEW_LABEL_";
      view_label << i;
      node->RestoreText(view_label.GetCStr(), info.m_Label);
      view_max = "VIEW_MAXIMIZED_";
      view_max << i;
      node->RestoreInteger(view_max, info.m_Maximized);
      view_size = "VIEW_SIZE_";
      view_size << i;
      view_pos = "VIEW_POS_";
      view_pos << i;
      node->RestoreVectorN(view_size.GetCStr(),info.m_Size,2);
      node->RestoreVectorN(view_pos.GetCStr(),info.m_Position,2);
      vme_in_view = "VME_IN_VIEW_";
      vme_in_view << i;
      int num_vme = 0;
      node->RestoreInteger(vme_in_view.GetCStr(),num_vme);
      if (num_vme > 0)
      {
        vme_ids_in_view = "VME_IDS_IN_VIEW_";
        vme_ids_in_view << i;
        info.m_VisibleVmes.resize(num_vme);
        node->RestoreVectorN(vme_ids_in_view.GetCStr(),info.m_VisibleVmes, num_vme);

        view_camera_parameters = "VIEW_CAMERA_PARAMETERS_";
        view_camera_parameters << i;
        node->RestoreVectorN(view_camera_parameters.GetCStr(),info.m_CameraParameters, 9);
      }
      m_LayoutViewList.push_back(info);
    }
    return MAF_OK;
  }
  return MAF_ERROR;
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
    info.m_CameraParameters[0] = v->GetRWI()->GetCamera()->GetViewUp()[0];
    info.m_CameraParameters[1] = v->GetRWI()->GetCamera()->GetViewUp()[1];
    info.m_CameraParameters[2] = v->GetRWI()->GetCamera()->GetViewUp()[2];

    info.m_CameraParameters[3] = v->GetRWI()->GetCamera()->GetPosition()[0];
    info.m_CameraParameters[4] = v->GetRWI()->GetCamera()->GetPosition()[1];
    info.m_CameraParameters[5] = v->GetRWI()->GetCamera()->GetPosition()[2];

    info.m_CameraParameters[6] = v->GetRWI()->GetCamera()->GetFocalPoint()[0];
    info.m_CameraParameters[7] = v->GetRWI()->GetCamera()->GetFocalPoint()[1];
    info.m_CameraParameters[8] = v->GetRWI()->GetCamera()->GetFocalPoint()[2];
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
  m_LayoutName = "Null Layout";
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
