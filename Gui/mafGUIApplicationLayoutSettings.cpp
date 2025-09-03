/*=========================================================================

 Program: MAF2
 Module: mafGUIApplicationLayoutSettings
 Authors: Daniele Giunchi
 
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

#include "mafGUIApplicationLayoutSettings.h"
#include "mafDecl.h"
#include "mafGUI.h"
#include "ftk/Gui/MainFrame.h"
#include "mafViewManager.h"
#include "mafViewVTK.h"

#include "ftk/Base/Object.h"
#include "mmaApplicationLayout.h"
#include "mafStorage.h"
#include "ftk/IO/StorageElement.h"
#include "mafVMERoot.h"
#include "mafNodeLayout.h"
#include "mafTagArray.h"
#include "mafNode.h"

#include "vtkCamera.h"
//#include "mafMemDbg.h"

//----------------------------------------------------------------------------
mafGUIApplicationLayoutSettings::mafGUIApplicationLayoutSettings(mafBaseEventHandler *listener, const mafString &label):
mafGUISettings(listener, label)
//----------------------------------------------------------------------------
{
  m_DefaultLayoutName = _R(" - ");

  m_ActiveLayoutName  = _R("");
  m_LayoutType        = _R(" - ");
  m_DefaultFlag       = 0;
}
//----------------------------------------------------------------------------
mafGUIApplicationLayoutSettings::~mafGUIApplicationLayoutSettings() 
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
void mafGUIApplicationLayoutSettings::CreateGui()
//----------------------------------------------------------------------------
{
  m_Gui = new mafGUI(this);

  // layout editor
  m_Gui->Label(_L("Layout Editor"), true);
  m_Gui->Divider(1);
  m_Gui->String(LAYOUT_NAME_ID,_L("name"),&m_ActiveLayoutName);
  m_Gui->Label(_L("Type"), &m_LayoutType);
  m_Gui->Divider(1);

  //application layout msf
  m_Gui->Label(_L("MSF"), true);
  //m_Gui->Label("For becoming definitive the layout saving,");
  //m_Gui->Label(" it needs to save the project");

  //m_Gui->String(LAYOUT_NAME_ID,_("name"),&m_DefaultLayout);
  m_Gui->Button(APPLY_TREE_LAYOUT_ID,_L("Apply Root Layout"));
  m_Gui->Bool(LAYOUT_VISIBILITY_VME, _L("Visibility"), &m_VisibilityVme ,0,_L("If checked the layout will be comprehensive of vme visibility"));
  m_Gui->Button(SAVE_TREE_LAYOUT_ID,_L("Save in root"));

  m_Gui->Enable(APPLY_TREE_LAYOUT_ID,false);

  //application layout 
  m_Gui->Divider(1);
  m_Gui->Label(_L("Application"), true);
  m_Gui->Label(_L("default:"), &m_DefaultLayoutName);

  m_Gui->Button(OPEN_LAYOUT_ID,_L("Load file"));
  m_Gui->Button(ADD_LAYOUT_ID,_L("Add Current layout"));
  m_Gui->Button(REMOVE_LAYOUT_ID,_L("Remove layout"));
  m_List= m_Gui->ListBox(ID_LIST_LAYOUT,_R(""),60);
  //m_Gui->Button(SAVE_APPLICATION_LAYOUT_ID,_("Save"));
  m_Gui->Button(APPLY_LAYOUT_ID,_L("Apply layout"));
  m_Gui->Bool(DEFAULT_LAYOUT_ID,_L("Default Layout"), &m_DefaultFlag, 1);
  //here lies the button for set as default

  m_Gui->FileSave(SAVE_APPLICATION_LAYOUT_ID,_L("File"), &m_LayoutFileSave,_L("All Files (*.mly)|*.mly"));
  m_Gui->Label(_R(""));

  InitializeSettings();
}
//----------------------------------------------------------------------------
void mafGUIApplicationLayoutSettings::OnEvent(mafEventBase *maf_event)
//----------------------------------------------------------------------------
{
  switch(maf_event->GetId())
  {
    case SAVE_TREE_LAYOUT_ID:
    {
      SaveTreeLayout();
    }
    break;
    case APPLY_TREE_LAYOUT_ID:
      ApplyTreeLayout();
    break;
    case LAYOUT_NAME_ID:
    break;
    case LAYOUT_VISIBILITY_VME:
      m_Gui->Enable(SAVE_APPLICATION_LAYOUT_ID, m_VisibilityVme == 0);
    break;
    case ID_LIST_LAYOUT:
    {
      m_SelectedItem = m_List->GetSelection();
      mafString sel = mafWxToString(m_List->GetStringSelection());
      m_DefaultFlag = (sel == m_DefaultLayoutName) ? 1 : 0;
      m_Gui->Update();
    }
    break;
    case OPEN_LAYOUT_ID:
      LoadLayout();
    break;
    case APPLY_LAYOUT_ID:
      ApplyLayout();
    break;
    case ADD_LAYOUT_ID:
      AddLayout();
    break;
    case REMOVE_LAYOUT_ID:
      RemoveLayout();
    break;
    case DEFAULT_LAYOUT_ID:
      SetLayoutAsDefault();
    break;
    case SAVE_APPLICATION_LAYOUT_ID:
      //save
      SaveApplicationLayout();
    break;
    default:
      InvokeEvent(*maf_event);
    break; 
  }
}
//----------------------------------------------------------------------------
void mafGUIApplicationLayoutSettings::SaveTreeLayout()
//----------------------------------------------------------------------------
{
  if (m_ViewManager)
  {
    wxFrame *frame = (wxFrame *)mafGetFrame();
    int pos[2], size[2];
    wxRect rect;
    rect = frame->GetRect();
    pos[0] = rect.GetPosition().x;
    pos[1] = rect.GetPosition().y;
    size[0] = rect.GetSize().GetWidth();
    size[1] = rect.GetSize().GetHeight();

    auto root = m_ViewManager->GetCurrentRoot();
    auto layout = mmaApplicationLayout::SafeDownCast(root->GetAttribute(_R("ApplicationLayout")));
    if (!layout)
    {
      layout = mmaApplicationLayout::NewSPtr();
      root->SetAttribute(layout);
    }
    else
    {
      layout->ClearLayout();
    }
    layout->SetVisibilityVme(m_VisibilityVme);
    layout->SetApplicationInfo(frame->IsMaximized(), pos, size);
    wxAuiPaneInfo toolbar = static_cast<mafGUIMDIFrame*>(m_Win)->GetDockManager().GetPane("toolbar");
    bool toolbar_vis = toolbar.IsShown();
    layout->SetInterfaceElementVisibility(_R("toolbar"), toolbar_vis);
    wxAuiPaneInfo sidebar = static_cast<mafGUIMDIFrame*>(m_Win)->GetDockManager().GetPane("sidebar");
    bool sidebar_vis = sidebar.IsShown();
    layout->SetInterfaceElementVisibility(_R("sidebar"), sidebar_vis);
    wxAuiPaneInfo logbar = static_cast<mafGUIMDIFrame*>(m_Win)->GetDockManager().GetPane("logbar");
    bool logbar_vis = logbar.IsShown();
    layout->SetInterfaceElementVisibility(_R("logbar"), logbar_vis);
    layout->SetLayoutName(_R("Layout")); //m_DefaultLayout.GetCStr()
    const std::list<mafView *>& v = m_ViewManager->GetList();
    for(std::list<mafView*>::const_iterator it = v.begin(); it != v.end(); ++it)
    {
      layout->AddView(*it);
    }
    m_Gui->Enable(APPLY_TREE_LAYOUT_ID,true);
  }
}
//----------------------------------------------------------------------------
void mafGUIApplicationLayoutSettings::InitializeSettings()
//----------------------------------------------------------------------------
{
  m_XMLRoot = mafVMERoot::NewSPtr();
  m_XMLRoot->SetName(_R("ApplicationLayout"));
  m_XMLRoot->Initialize();

  //reg key for application layout
  wxString layout_filename;
  if(m_Config->Read("DefaultLayoutFile", &layout_filename))
  {
    m_DefaultLayoutFile = mafWxToString(layout_filename);
  }
  else
  {
    mafString layout_dir  = mafGetApplicationDirectory();
    layout_dir += _R("\\Layout\\layout.mly");
    m_Config->Write("DefaultLayoutFile", layout_dir.toWx());
    m_Config->Flush();
    m_DefaultLayoutFile = layout_dir;
  }
}
//----------------------------------------------------------------------------
void mafGUIApplicationLayoutSettings::AddLayout()
//----------------------------------------------------------------------------
{
  mafString name;
  
  auto dlg = std::make_unique<wxTextEntryDialog>(nullptr,_("please enter a name"), _("New Layout"), name.toWx());
  dlg->SetValue(_("new layout"));
  int result = dlg->ShowModal(); 
  name = mafWxToString(dlg->GetValue());
  if(result != wxID_OK) return;

  //check for equal names
  int idx = m_List->FindString(name.toWx());
  if(idx != -1)
  {
    wxString msg = _("this name is already used, do you wanto to overwrite this layout ?");
    int res = wxMessageBox(msg,_("Save Layout"), wxOK|wxCANCEL|wxICON_QUESTION, NULL);
    if(res == wxCANCEL) return;

    // delete old child which will be substituted
    m_List->Delete(idx);
    m_XMLRoot->RemoveChild(m_XMLRoot->FindInTreeByName(name).get());
  }

  {
    wxFrame *frame = (wxFrame *)mafGetFrame();
    int pos[2], size[2];
    wxRect rect;
    rect = frame->GetRect();
    pos[0] = rect.GetPosition().x;
    pos[1] = rect.GetPosition().y;
    size[0] = rect.GetSize().GetWidth();
    size[1] = rect.GetSize().GetHeight();

    auto child = mafNodeLayout::NewSPtr();
    m_XMLRoot->AddChild(child);

    auto layout = child->GetLayout();
    if(layout)
    {}
    else
    {
      layout = mmaApplicationLayout::NewSPtr();
      child->SetAttribute(layout); //mettere application layout
    }
    layout->SetName(_R("ApplicationLayout"));
    child->SetName(name);
    
    //disable visibility (useless for application layout)
    layout->SetVisibilityVme(0);
   
    layout->SetApplicationInfo(frame->IsMaximized(), pos, size);
    wxAuiPaneInfo toolbar = static_cast<mafGUIMDIFrame*>(m_Win)->GetDockManager().GetPane("toolbar");
    bool toolbar_vis = toolbar.IsShown();
    layout->SetInterfaceElementVisibility(_R("toolbar"), toolbar_vis);
    wxAuiPaneInfo sidebar = static_cast<mafGUIMDIFrame*>(m_Win)->GetDockManager().GetPane("sidebar");
    bool sidebar_vis = sidebar.IsShown();
    layout->SetInterfaceElementVisibility(_R("sidebar"), sidebar_vis);
    wxAuiPaneInfo logbar = static_cast<mafGUIMDIFrame*>(m_Win)->GetDockManager().GetPane("logbar");
    bool logbar_vis = logbar.IsShown();
    layout->SetInterfaceElementVisibility(_R("logbar"), logbar_vis);
    layout->SetLayoutName(_R("Layout")); //m_DefaultLayout.GetCStr()

    const std::list<mafView *>& v = m_ViewManager->GetList();
    for(std::list<mafView*>::const_iterator it = v.begin(); it != v.end(); ++it)
    {
      layout->AddView(*it);
    }
    m_List->Append(name.toWx());

    //restore the original visibility
    layout->SetVisibilityVme(m_VisibilityVme);

    m_ModifiedLayouts = true;
  }
}
//----------------------------------------------------------------------------
void mafGUIApplicationLayoutSettings::RemoveLayout()
//----------------------------------------------------------------------------
{
  if(m_SelectedItem != -1)
  {   
    // delete old child which will be substituted
    mafString name = mafWxToString(m_List->GetString(m_SelectedItem));
    if(mafNodeLayout::StaticDownCast(m_XMLRoot->FindInTreeByName(name))->GetLayout()->GetLayoutName() == _R("Default"))
      m_DefaultLayoutName = _R(" - ");

    m_Gui->Update();
    m_XMLRoot->RemoveChild(m_XMLRoot->FindInTreeByName(name).get());

    m_List->Delete(m_SelectedItem);
    m_SelectedItem = -1;
    m_ModifiedLayouts = true;
  }
}
//----------------------------------------------------------------------------
void mafGUIApplicationLayoutSettings::SaveApplicationLayout()
//----------------------------------------------------------------------------
{
	io::Writer writer(_R("MLY"), _R("2.0"));
	auto root = writer.GetRoot();
	m_XMLRoot->Store(root);
    writer.Save(m_LayoutFileSave);
	m_ModifiedLayouts = false;
}
//----------------------------------------------------------------------------
void mafGUIApplicationLayoutSettings::LoadLayout(bool fileDefault)
//----------------------------------------------------------------------------
{  
  mafString file;

  if(fileDefault)
    file = m_DefaultLayoutFile;
  else
    file = mafGetOpenFile(_R(""), _L("All Files (*.*)|*.*"), _L("Open Layout File"), GetGui());

  if(file.empty())
    return;

  if(mafFileExists(file))
  {
    //clear tree
    m_XMLRoot->RemoveAllChildren();
    m_XMLRoot->RemoveAllAttributes();
    m_List->Clear();

  	io::Reader reader(_R("MLY"), _R("2.0"));
    reader.Load(m_LayoutFileSave);
    m_XMLRoot->Restore(reader.GetRoot());

    //fill listbox
    for(auto& vme : *m_XMLRoot)
    {
      if(!vme.IsMAFType(mafVMERoot))
      {
        m_List->Append(vme.GetName().toWx());
        if(mafNodeLayout::StaticDownCast(&vme)->GetLayout()->GetLayoutName() == _R("Default"))
          m_DefaultLayoutName = mafNodeLayout::StaticDownCast(&vme)->GetName();
      }
    }

    m_LayoutFileSave = file;
    
    if(m_Gui)
    {
      m_Gui->Update();
      m_List->Update();

      //apply default layout
      if(m_List->GetCount() != 0)
      {
        m_SelectedItem = m_List->FindString(m_DefaultLayoutName.toWx());
        if(m_SelectedItem != -1)
          m_List->SetSelection(m_SelectedItem, true);
        m_Gui->Update();

        if(fileDefault && m_SelectedItem == -1)
          return;

        ApplyLayout();
        mafString sel = mafWxToString(m_List->GetStringSelection());
        m_DefaultFlag = (sel == m_DefaultLayoutName) ? 1 : 0;
        m_SelectedItem = m_List->GetSelection();
        m_Gui->Update();
      }
    }
  }
}
//----------------------------------------------------------------------------
void mafGUIApplicationLayoutSettings::ApplyLayout()
//----------------------------------------------------------------------------
{
  if(m_SelectedItem == -1)
  {
    return;
  }

  mafString name = mafWxToString(m_List->GetString(m_SelectedItem));

  // Retrieve the saved layout.
  auto vme = mafNodeLayout::SafeDownCast(m_XMLRoot->FindInTreeByName(name));
  auto app_layout = vme->GetLayout(); //application layout
  m_ActiveLayoutName = vme->GetName();
  m_LayoutType       = _L("Application Layout");
  if(m_Gui) m_Gui->Update();

  if (app_layout)
  {
    m_ViewManager->ViewDeleteAll();
    int maximized, pos[2], size[2];
    app_layout->GetApplicationInfo(maximized, pos, size);
    if (maximized != 0)
    {
      m_Win->Maximize();
    }
    else
    {
      wxRect rect(pos[0],pos[1],size[0],size[1]);
      m_Win->SetSize(rect);
    }
    bool tb_vis = app_layout->GetToolBarVisibility() != 0;
    bool sb_vis = app_layout->GetSideBarVisibility() != 0;
    bool lb_vis = app_layout->GetLogBarVisibility() != 0;
    static_cast<mafGUIMDIFrame*>(m_Win)->ShowPane("toolbar", tb_vis);
    static_cast<mafGUIMDIFrame*>(m_Win)->ShowPane("logbar", lb_vis);
    static_cast<mafGUIMDIFrame*>(m_Win)->ShowPane("sidebar", sb_vis);
    int num = app_layout->GetNumberOfViewsInLayout();
    std::vector<ViewLayoutInfo>::iterator iter = app_layout->GetLayoutList();
    mafView *v = NULL;
    for (int i = 0; i < num; i++, iter++)
    {
      if(m_ViewManager)
      {
        mafView* v = m_ViewManager->ViewCreate((*iter).m_Id);
      }
      
      mafYield();
      v = m_ViewManager->GetSelectedView();
      if (v)
      {
        v->SetName((*iter).m_Label);
        pos[0] = (*iter).m_Position[0];
        pos[1] = (*iter).m_Position[1];
        size[0] = (*iter).m_Size[0];
        size[1] = (*iter).m_Size[1];
        wxRect rect(pos[0],pos[1],size[0],size[1]);
        v->GetFrame()->SetSize(rect);
      }
    }
  }
}
//----------------------------------------------------------------------------
void mafGUIApplicationLayoutSettings::ApplyTreeLayout()
//----------------------------------------------------------------------------
{
  // Retrieve the saved layout.
  auto vme = m_ViewManager->GetCurrentRoot();
  auto app_layout = mmaApplicationLayout::SafeDownCast(vme->GetAttribute(_R("ApplicationLayout"))); 

  if (app_layout)
  {
    m_ActiveLayoutName = app_layout->GetLayoutName();
    m_LayoutType       = _L("MSF Layout");
    if(m_Gui) m_Gui->Update();

    m_ViewManager->ViewDeleteAll();
    int maximized, pos[2], size[2];
    app_layout->GetApplicationInfo(maximized, pos, size);
    if (maximized != 0)
    {
      m_Win->Maximize();
    }
    else
    {
      wxRect rect(pos[0],pos[1],size[0],size[1]);
      m_Win->SetSize(rect);
    }
    bool tb_vis = app_layout->GetToolBarVisibility() != 0;
    bool sb_vis = app_layout->GetSideBarVisibility() != 0;
    bool lb_vis = app_layout->GetLogBarVisibility() != 0;
    static_cast<mafGUIMDIFrame*>(m_Win)->ShowPane("toolbar", tb_vis);
    static_cast<mafGUIMDIFrame*>(m_Win)->ShowPane("logbar", lb_vis);
    static_cast<mafGUIMDIFrame*>(m_Win)->ShowPane("sidebar", sb_vis);
    int num = app_layout->GetNumberOfViewsInLayout();
    std::vector<ViewLayoutInfo>::iterator iter = app_layout->GetLayoutList();
    mafView *v = NULL;
    for (int i = 0; i < num; i++, iter++)
    {
      if(m_ViewManager)
      {
        mafView* v = m_ViewManager->ViewCreate((*iter).m_Id);
      }
      mafYield();
      v = m_ViewManager->GetSelectedView();
      if (v)
      {
        v->SetName((*iter).m_Label);
        pos[0] = (*iter).m_Position[0];
        pos[1] = (*iter).m_Position[1];
        size[0] = (*iter).m_Size[0];
        size[1] = (*iter).m_Size[1];
        wxRect rect(pos[0],pos[1],size[0],size[1]);
        v->GetFrame()->SetSize(rect);

        if(m_VisibilityVme)
        {
          for (int i=0; i<(*iter).m_VisibleVmes.size();i++)
        {
          if (auto node_restored = m_ViewManager->GetCurrentRoot()->FindInTreeById((*iter).m_VisibleVmes[i]))
          {
            {mafEvent evUnq(this, VME_SHOW); evUnq.SetVme(node_restored.get()); evUnq.SetBool(true); InvokeEvent(evUnq);}
          }
        }

        if((*iter).m_VisibleVmes.size() > 0)
        {
          if(v->IsMAFType(mafViewVTK))
          {
            double view_up[3], position[3], focal_point[3];
            view_up[0] = (*iter).m_CameraParameters[0];
            view_up[1] = (*iter).m_CameraParameters[1];
            view_up[2] = (*iter).m_CameraParameters[2];

            position[0] = (*iter).m_CameraParameters[3];
            position[1] = (*iter).m_CameraParameters[4];
            position[2] = (*iter).m_CameraParameters[5];

            focal_point[0] = (*iter).m_CameraParameters[6];
            focal_point[1] = (*iter).m_CameraParameters[7];
            focal_point[2] = (*iter).m_CameraParameters[8];

            v->GetRWI()->GetCamera()->SetViewUp(view_up);
            v->GetRWI()->GetCamera()->SetPosition(position);
            v->GetRWI()->GetCamera()->SetFocalPoint(focal_point);
          }
          else //compound
          {;}
        }
        }
      }
    }
  }
  else
  {
    m_Gui->Enable(APPLY_TREE_LAYOUT_ID,false);
    m_Gui->Enable(SAVE_TREE_LAYOUT_ID,false);
  }
}
//----------------------------------------------------------------------------
void mafGUIApplicationLayoutSettings::SetLayoutAsDefault()
//----------------------------------------------------------------------------
{
  if(m_SelectedItem != -1)
  {   
    m_ModifiedLayouts = true;
    for(auto& vme : *m_XMLRoot)
    {
      if(!vme.IsMAFType(mafVMERoot))
        mafNodeLayout::StaticDownCast(&vme)->GetLayout()->SetLayoutName(_R("Layout"));
    }

    if(m_DefaultFlag != 0)
      m_DefaultLayoutName = mafWxToString(m_List->GetString(m_SelectedItem));
    else
    {
      m_DefaultLayoutName = _R(" - ");
      m_Gui->Update();
      return;
    }

    mafNodeLayout::StaticDownCast(m_XMLRoot->FindInTreeByName(m_DefaultLayoutName))->GetLayout()->SetLayoutName(_R("Default"));
    
    m_ModifiedLayouts = true;
    m_Gui->Update();
  }
}
